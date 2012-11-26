#include <cstdlib>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include <boost/algorithm/string.hpp>

#include <openssl/sha.h>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

#include <elle/format/json.hh>
#include <elle/log.hh>
#include <elle/network/Host.hh>
#include <elle/os/path.hh>
#include <elle/Passport.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/utility/Time.hh>
#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>

#include <common/common.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>
#include <lune/Lune.hh>

#include <nucleus/neutron/Permissions.hh>

#include <elle/idiom/Close.hh>

#include "State.hh"

#include <signal.h>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

#define _REGISTER_CALLBACK_HANDLER(data, indice, handler)                      \
  void                                                                         \
  State::attach_callback(std::function<void (data const*)> callback)           \
  {                                                                            \
    _notification_handlers[indice].push_back(new handler(callback));           \
  }                                                                            \
/**/

namespace surface
{
  namespace gap
  {
    void
    print_transaction(plasma::meta::TransactionResponse const& t)
    {
      printf("transaction_id: %s\n", t.transaction_id.c_str());
      printf("first_filename: %s\n", t.first_filename.c_str());
      printf("files_count: %i\n", t.files_count);
      printf("total_size: %i\n", t.total_size);
      printf("is_directory: %i\n", (int)t.is_directory);
      printf("network_id: %s\n", t.network_id.c_str());
      printf("sender_id: %s\n", t.sender_id.c_str());
      printf("sender_fullname: %s\n", t.sender_fullname.c_str());
      printf("sender_device_id: %s\n", t.sender_device_id.c_str());
      printf("recipient_id: %s\n", t.recipient_id.c_str());
      printf("recipient_fullname: %s\n", t.recipient_fullname.c_str());
      printf("recipient_device_id: %s\n", t.recipient_device_id.c_str());
      printf("status: %i\n", t.status);
    }

    namespace fs = boost::filesystem;
    namespace json = elle::format::json;

    // - Exception ------------------------------------------------------------

    Exception::Exception(gap_Status code, std::string const& msg)
      : std::runtime_error(msg)
      , code(code)
    {
    }

    // - State ----------------------------------------------------------------

    State::State()
      : _meta{
      new plasma::meta::Client{
        common::meta::host(),
          common::meta::port(),
          true,
        }}
      , _trophonius{nullptr}
      , _users{}
      , _files_infos{}
      , _networks{}
      , _networks_dirty{true}
      , _networks_status{}
      , _networks_status_dirty{true}
    {
      this->attach_callback(
        std::function<void (gap_TransactionNotification const*)>(
          std::bind(
            static_cast<void (State::*)(gap_TransactionNotification const*)>(
              &State::_on_notification),
            this,
            std::placeholders::_1
          )
        )
      );

      this->attach_callback(
        std::function<void (gap_TransactionStatusNotification const*)>(
          std::bind(
            static_cast<void (State::*)(gap_TransactionStatusNotification const*)>(
              &State::_on_notification),
            this,
            std::placeholders::_1
          )
        )
      );
    }

    State::State(std::string const& token):
      State{}
    {
   //   std::ifstream identity_file{common::watchdog::identity_path(user)};

   //   if (identity_file.good())
   //     {
   //       std::string str;
   //       std::getline(identity_file, str);
   //       this->_meta->token(str);
   //       std::getline(identity_file, str);
   //       this->_meta->identity(str);
   //       std::getline(identity_file, str);
   //       this->_meta->email(str);
   //     }
      this->_meta->token(token);
      auto res = this->_meta->self();
      this->_meta->identity(res.identity);
      this->_meta->email(res.email);
      //XXX factorize that shit
      this->_me = static_cast<User const&>(res);
    }


    State::~State()
    {
      for (auto& it: this->_networks)
        delete it.second;
      this->_networks.clear();

      if (this->_transactions)
        {
          for (auto& it: *this->_transactions)
            delete it.second;
          this->_transactions->clear();
        }
    }

    void
    State::debug()
    {
      this->_meta->debug();
    }

    void State::refresh_networks()
    {
      this->_send_watchdog_cmd("refresh_networks");
      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
    }

    void State::_reload_networks()
    {
    }

    std::string State::_watchdog_id() const // XXX should be cached
    {
      std::string watchdog_id_file = common::watchdog::id_path(_me._id);
      std::ifstream file(watchdog_id_file);
      if (!file.good())
        throw std::runtime_error("Cannot open '" + watchdog_id_file + "'");

      char wtg_id[4096];
      file.read(wtg_id, 4096);
      if (!file.eof())
        throw Exception(gap_internal_error, "Watchdog id is too long!");
      std::string id(wtg_id, file.gcount());
      file.close();
      return id;
    }

    void
    State::_send_watchdog_cmd(std::string const& cmd,
                              elle::format::json::Dictionary const* kwargs,
                              elle::format::json::Dictionary* response)
    {
      QLocalSocket conn;

      conn.connectToServer(common::watchdog::server_name(this->_me._id).c_str());
      if (!conn.waitForConnected(2000))
        throw Exception{
            gap_internal_error,
              "Couldn't connect to the watchdog:" + std::to_string(conn.error())
        };

      json::Dictionary req;
      req["_id"] = this->_watchdog_id();
      req["command"] = cmd;
      if (kwargs != nullptr)
        req.update(*kwargs);
      ELLE_DEBUG("Send watchdog command: %s", req.repr());
      conn.write(req.repr().c_str());
      conn.write("\n");
      if (!conn.waitForBytesWritten(2000))
          throw Exception(gap_internal_error,
                          "Couldn't send the command '" + cmd + "'");

      ELLE_DEBUG("Command '%s' sent.", cmd);

      if (response == nullptr)
        {
          ELLE_DEBUG("Watchdog response is ignored for call %s.", cmd);
          return;
        }
      if (!conn.waitForReadyRead(-1)) // Infinit is maybe too long, or not.
        throw Exception{
            gap_internal_error,
            "Couldn't read response of '" + cmd + "' command"
        };
      QByteArray response_data = conn.readLine();
      std::stringstream ss{
          std::string{
              response_data.data(),
              static_cast<size_t>(response_data.size()),
          },
      };
      auto ptr = json::parse(ss);
      response->update(ptr->as_dictionary());
    }

    User const& State::user(std::string const& id)
    {
      auto it = this->_users.find(id);
      if (it != this->_users.end())
        return *(it->second);

      auto response = this->_meta->user(id);
      std::unique_ptr<User> user{new User{
          response._id,
          response.fullname,
          response.email,
          response.public_key,
      }};

      this->_users[response._id] = user.get();
      return *(user.release());
    }

    User const&
    State::get_me()
    {
      return this->_me;
    }

    std::string const&
    State::get_token()
    {
      return this->_meta->token();
    }

    User const&
    State::user_from_public_key(std::string const& public_key)
    {
      for (auto const& pair : this->_users)
        {
          if (pair.second->public_key == public_key)
            return *(pair.second);
        }
      auto response = this->_meta->user_from_public_key(public_key);
      std::unique_ptr<User> user{new User{
          response._id,
          response.fullname,
          response.email,
          response.public_key,
      }};

      this->_users[response._id] = user.get();
      return *(user.release());
    }

    std::map<std::string, User const*>
    State::search_users(std::string const& text)
    {
      std::map<std::string, User const*> result;
      auto res = this->_meta->search_users(text);
      for (auto const& user_id : res.users)
        {
          result[user_id] = &this->user(user_id);
        }
      return result;
    }

    std::string State::hash_password(std::string const& email,
                                     std::string const& password)
    {
      unsigned char hash[SHA256_DIGEST_LENGTH];
      SHA256_CTX context;
      std::string to_hash = email + "MEGABIET" + password + email + "MEGABIET";

      if (SHA256_Init(&context) == 0 ||
          SHA256_Update(&context, to_hash.c_str(), to_hash.size()) == 0 ||
          SHA256_Final(hash, &context) == 0)
        throw Exception(gap_internal_error, "Cannot hash login/password");

      std::ostringstream out;
      elle::serialize::OutputHexadecimalArchive ar(out);

      ar.SaveBinary(hash, SHA256_DIGEST_LENGTH);

      return out.str();
    }

    void State::login(std::string const& email, std::string const& password)
    {
      this->_meta->token("");
      auto res = this->_meta->login(email, password);

      ELLE_DEBUG("Logged in as %s token = %s", email, res.token);

      this->_me._id = res._id.c_str();
      this->_me.fullname = res.fullname.c_str();
      this->_me.email = res.email.c_str();
      this->_me.public_key = "";

      ELLE_DEBUG("id: '%s' - fullname: '%s' - email: '%s'.",
                 this->_me._id,
                 this->_me.fullname,
                 this->_me.email);

      std::string identity_clear;

      lune::Identity      identity;

      // Decrypt the identity
        {
          if (identity.Restore(res.identity)    == elle::Status::Error ||
              identity.Decrypt(password)        == elle::Status::Error ||
              identity.Clear()                  == elle::Status::Error ||
              identity.Save(identity_clear)     == elle::Status::Error)
            throw Exception(gap_internal_error,
                            "Couldn't decrypt the identity file !");
        }

      // Store the identity
        {
          if (identity.Restore(identity_clear)  == elle::Status::Error)
            throw Exception(gap_internal_error,
                            "Cannot save the identity file.");

          identity.store();

          // user.dic
          lune::Dictionary dictionary;

          dictionary.store(res._id);
        }

        transactions();
    }

    void
    State::logout()
    {
      this->_meta->logout();
    }

    void
    State::pull_notifications(int count, int offset)
    {
      if (count < 1)
        return;

      if (offset < 0)
        return;

      auto res = this->_meta->pull_notifications(count, offset);

      for (auto dict : res.notifs)
        this->_handle_dictionnary(dict, true);

      for (auto dict : res.old_notifs)
        this->_handle_dictionnary(dict, false);
    }

    void
    State::notifications_read()
    {
      this->_meta->notification_read();
    }

    std::string
    State::invite_user(std::string const& email)
    {
      auto response = this->_meta->invite_user(email);

      return response._id;
    }

    void
    State::send_files(std::string const& recipient_id_or_email,
                      std::unordered_set<std::string> const& files)
    {
      ELLE_DEBUG("Sending file to '%s'.", recipient_id_or_email);

      if (files.empty())
            throw Exception(gap_no_file,
                            "no files to send");

      int size = 0;
      for (auto const& path : files)
        {
          if (!fs::exists(path))
            throw Exception(gap_file_not_found,
                            "file doesn't exist.");

          size += get_size(path);
        }

      std::string first_filename = fs::path(*(files.cbegin())).filename().string();

      ELLE_DEBUG("First filename '%s'.", first_filename);

      // Build timestamp.
      elle::utility::Time time;
      time.Current();

      // Create an ostream to convert timestamp to string.
      std::ostringstream oss;
      oss << time.nanoseconds;

      // FIXME: How to compute network name ?
      /// XXX: Something fucked my id.
      std::string network_name =
        std::string(recipient_id_or_email)
        + "-"
        + oss.str();

      ELLE_DEBUG("Creating temporary network '%s'.", network_name);

      std::string network_id = this->create_network(network_name);

      // Ensure the network status is available
      (void) this->network_status(network_id);

      auto portal_path = common::infinit::portal_path(
        this->_me._id,
        network_id
      );
      for (int i = 0; i < 10; ++i)
        {
          if (fs::exists(portal_path))
            break;
          ::sleep(1);
        }

      if (!fs::exists(portal_path))
          throw Exception{gap_error, "Couldn't find portal to infinit instance"};

      std::string const& transfer_binary = common::infinit::binary_path("8transfer");

      QStringList arguments;
      arguments << "-n" << network_id.c_str()
                << "-u" << this->_me._id.c_str()
                << "--path" << files.cbegin()->c_str()
                << "--to"
      ;
      ELLE_DEBUG("LAUNCH: %s %s",
                      transfer_binary,
                      arguments.join(" ").toStdString());

      QProcess p;
      p.start(transfer_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8transfer binary failed");
      if (p.exitCode())
        throw Exception(gap_internal_error, "8transfer binary exited with errors");

      this->_meta->create_transaction(recipient_id_or_email,
                                      first_filename,
                                      files.size(),
                                      size,
                                      fs::is_directory(first_filename),
                                      network_id,
                                      this->device_id());
    }

    void
    State::download_files(std::string const& transaction_id,
                   std::string const& path)
    {
      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      // Ensure the network status is available
      (void) this->network_status(trans->network_id);

      std::string const& transfer_binary = common::infinit::binary_path("8transfer");

      QStringList arguments;
      arguments << "-n" << trans->network_id.c_str()
                << "-u" << trans->sender_id.c_str()
                << "--path" << path.c_str()
                << "--from"
      ;
      ELLE_DEBUG("LAUNCH: %s %s",
                      transfer_binary,
                      arguments.join(" ").toStdString());

      QProcess p;
      p.start(transfer_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8transfer binary failed");
      if (p.exitCode())
        throw Exception(gap_internal_error, "8transfer binary exited with errors");

      ELLE_WARN("TRANSFER IS FUCKING COMPLETE MOTHER FUCKER!! Your file is at '%s'.",
                path.c_str());

      update_transaction(transaction_id,
                         gap_TransactionStatus::gap_transaction_status_finished);

    }

    void
    State::update_transaction(std::string const& transaction_id,
                              gap_TransactionStatus status)
    {
      ELLE_DEBUG("Update transaction '%s': '%s'", transaction_id, status);

      switch(status)
      {
        case gap_TransactionStatus::gap_transaction_status_accepted:
          this->_accept_transaction(transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_rejected:
          this->_deny_transaction(transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_started:
          this->_start_transaction(transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_canceled:
          this->_cancel_transaction(transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_finished:
          this->_close_transaction(transaction_id);
          break;
        default:
          ELLE_WARN("You are not able to change transaction status to '%i'.",
            status);
          return;
      }
    }

    void
    State::_accept_transaction(std::string const& transaction_id)
    {
      ELLE_DEBUG("Accept transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->recipient_id != this->_me._id)
        return;

      this->_meta->update_transaction(transaction_id,
                                      gap_TransactionStatus::gap_transaction_status_accepted,
                                      this->device_id(),
                                      this->device_name());
    }

    void
    State::_deny_transaction(std::string const& transaction_id)
    {
      ELLE_DEBUG("Deny transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->recipient_id != this->_me._id)
        return;

      this->_meta->update_transaction(transaction_id,
                                      gap_TransactionStatus::gap_transaction_status_rejected,
                                      this->device_id(),
                                      this->device_name());
    }

    void
    State::_start_transaction(std::string const& transaction_id)
    {
      ELLE_DEBUG("Start transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->sender_id == this->_me._id)
      {
        this->_meta->start_transaction(transaction_id);
      }
    }

    void
    State::_cancel_transaction(std::string const& transaction_id)
    {
      ELLE_DEBUG("Cancel transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->recipient_id != this->_me._id)
        return;

      //XXX: If download has started, cancel it, delete files, ...
      if (trans->sender_id == this->_me._id)
      {
        //XXX
        this->_meta->cancel_transaction(transaction_id);
      }
      else
      {
        //XXX
        this->_meta->cancel_transaction(transaction_id);
      }
    }

    void
    State::_close_transaction(std::string const& transaction_id)
    {
      ELLE_DEBUG("Close transaction '%s'", transaction_id);


      auto const& pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->recipient_id == this->_me._id)
      {
        this->_meta->finish_transaction(transaction_id);
      }
    }

    void
    State::send_message(std::string const& recipient_id,
                        std::string const& message)
    {
      this->_meta->send_message(recipient_id,
                                this->_me._id,
                                message);
    }


    void
    State::register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password,
                     std::string const& activation_code)
    {
      // Logout first, and ignore errors.
      try { this->logout(); } catch (elle::HTTPException const&) {}

      this->_meta->register_(email, fullname, password, activation_code);

      ELLE_DEBUG("Registered new user %s <%s>", fullname, email);
      this->login(email, password);
    }


    _REGISTER_CALLBACK_HANDLER(gap_UserStatusNotification,
                               gap_Notification::gap_notification_user_status,
                               plasma::trophonius::Client::UserStatusHandler)

    _REGISTER_CALLBACK_HANDLER(gap_TransactionNotification,
                               gap_Notification::gap_notification_transaction_request,
                               plasma::trophonius::Client::TransactionHandler)

    _REGISTER_CALLBACK_HANDLER(gap_TransactionStatusNotification,
                               gap_Notification::gap_notification_transaction_status,
                               plasma::trophonius::Client::TransactionStatusHandler)

    _REGISTER_CALLBACK_HANDLER(gap_MessageNotification,
                               gap_Notification::gap_notification_message,
                               plasma::trophonius::Client::MessageHandler)

    _REGISTER_CALLBACK_HANDLER(gap_BiteNotification,
                               gap_Notification::gap_notification_debug,
                               plasma::trophonius::Client::BiteHandler)

    State::TransactionsMap const&
    State::transactions()
    {
      if (_transactions != nullptr)
        return *_transactions;

      _transactions.reset(new TransactionsMap{});

      auto response = this->_meta->transactions();
      for (auto const& transaction_id: response.transactions)
        {
          auto response = this->_meta->transaction(transaction_id);
          (*this->_transactions)[transaction_id] =
            new plasma::meta::TransactionResponse{response};
        }

      return *(this->_transactions);
    }

    plasma::meta::TransactionResponse const&
    State::transaction(std::string const& id)
    {
      auto it = this->transactions().find(id);
      if (it == this->transactions().end())
        throw Exception{
            gap_error,
            "Cannot find any transaction for id '" + id + "'"
        };
      return *(it->second);
    }

    void
    State::_on_notification(gap_TransactionNotification const* notif)
    {
      ELLE_ASSERT(notif != nullptr);

      ELLE_TRACE("_on_notification(gap_TransactionNotification\n");

      if (!notif->is_new)
        return;

      ELLE_ASSERT(notif->transaction_id != nullptr);

      auto const pair = State::transactions().find(notif->transaction_id);

      if (pair != State::transactions().end())
      {

#ifdef DEBUG
        plasma::meta::TransactionResponse *trans = pair->second;
#endif

        // Compare notif and see if everything match.
        ELLE_ASSERT(notif->transaction_id != nullptr);
        ELLE_ASSERT(notif->sender_id != nullptr);
        ELLE_ASSERT(notif->sender_device_id != nullptr);
        ELLE_ASSERT(notif->sender_fullname != nullptr);
        ELLE_ASSERT(notif->recipient_id != nullptr);
        ELLE_ASSERT(notif->recipient_fullname != nullptr);
        ELLE_ASSERT(notif->network_id != nullptr);
        ELLE_ASSERT(notif->first_filename != nullptr);

        ELLE_ASSERT(notif->transaction_id == trans->transaction_id);
        ELLE_ASSERT(notif->sender_id == trans->sender_id);
        ELLE_ASSERT(notif->sender_device_id == trans->sender_device_id);
        ELLE_ASSERT(notif->sender_fullname == trans->sender_fullname);
        ELLE_ASSERT(notif->recipient_id == trans->recipient_id);
        ELLE_ASSERT(notif->recipient_fullname == trans->recipient_fullname);
        ELLE_ASSERT(notif->network_id == trans->network_id);
        ELLE_ASSERT(notif->first_filename == trans->first_filename);
        ELLE_ASSERT(trans->files_count == notif->files_count);
        ELLE_ASSERT(trans->total_size == notif->total_size);
        ELLE_ASSERT(trans->is_directory == notif->is_directory);

        return;
      }

      // // Normal case, this is a new transaction, store it to match server.
      auto trans = new plasma::meta::TransactionResponse();

      trans->transaction_id = notif->transaction_id;
      trans->sender_id = notif->sender_id;
      trans->sender_device_id = notif->sender_device_id;
      trans->sender_fullname = notif->sender_fullname;
      trans->recipient_id = notif->recipient_id;
      trans->recipient_fullname = notif->recipient_fullname;
      trans->network_id = notif->network_id;
      trans->first_filename = notif->first_filename;
      trans->files_count = notif->files_count;
      trans->total_size = notif->total_size;
      trans->is_directory = notif->is_directory;
      trans->status = gap_TransactionStatus::gap_transaction_status_pending;

      print_transaction(*trans);

      (*this->_transactions)[trans->transaction_id] = trans;
    }

    void
    State::_on_notification(gap_TransactionStatusNotification const* notif)
    {
      ELLE_TRACE("_on_notification(gap_TransactionStatusNotification\n");

      ELLE_ASSERT(notif != nullptr);

      if (!notif->is_new)
        return;

      ELLE_ASSERT(notif->transaction_id != nullptr);

      auto const pair = State::transactions().find(notif->transaction_id);

      if (pair == State::transactions().end())
      {
        // Something went wrong.
        auto response = this->_meta->transaction(notif->transaction_id);
        (*this->_transactions)[notif->transaction_id] =
          new plasma::meta::TransactionResponse{response};

        return;
      }

      ELLE_ASSERT(notif->sender_id != nullptr);
      ELLE_ASSERT(notif->sender_device_id != nullptr);
      ELLE_ASSERT(notif->recipient_id != nullptr);
      ELLE_ASSERT(notif->recipient_fullname != nullptr);
      ELLE_ASSERT(notif->recipient_device_id != nullptr);
      ELLE_ASSERT(notif->recipient_device_name != nullptr);
      ELLE_ASSERT(notif->network_id != nullptr);

      auto trans = pair->second;

      ELLE_ASSERT(trans != nullptr);

      trans->recipient_fullname = notif->recipient_fullname;
      trans->recipient_device_id = notif->recipient_device_id;
      trans->recipient_device_name = notif->recipient_device_name;

      trans->status = notif->status;

      switch(trans->status)
      {
        case gap_TransactionStatus::gap_transaction_status_accepted:
          this->_on_transaction_accepted(trans->transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_rejected:
          this->_on_transaction_denied(trans->transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_started:
          this->_on_transaction_started(trans->transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_canceled:
          this->_on_transaction_canceled(trans->transaction_id);
          break;
        case gap_TransactionStatus::gap_transaction_status_finished:
          this->_on_transaction_closed(trans->transaction_id);
          break;
        default:
          ELLE_WARN("The status '%i' is unknown.",
                    trans->status);
          return;
      }
    }

    void
    State::_on_transaction_accepted(std::string const& transaction_id)
    {
      ELLE_DEBUG("On transaction accepted '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->sender_id != this->_me._id)
        return;

      // Give rights.
      network_add_user(trans->network_id,
                       trans->recipient_id);

      // When recipient has rights, allow him to start download.
      this->update_transaction(transaction_id,
                               gap_TransactionStatus::gap_transaction_status_started);

    }

    void
    State::_on_transaction_denied(std::string const& transaction_id)
    {
      ELLE_DEBUG("Deny transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->sender_id != this->_me._id)
        return;

      //XXX:
    }

    void
    State::_on_transaction_started(std::string const& transaction_id)
    {
      ELLE_DEBUG("Start transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->recipient_id != this->_me._id)
        return;

      // Waiting for "download files".
    }

    void
    State::_on_transaction_canceled(std::string const& transaction_id)
    {
      ELLE_DEBUG("Cancel transaction '%s'", transaction_id);

      auto pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      plasma::meta::TransactionResponse *trans = pair->second;

      if (trans->recipient_id != this->_me._id)
        return;

      //XXX: If download has started, cancel it, delete files, ...
    }

    void
    State::_on_transaction_closed(std::string const& transaction_id)
    {
      ELLE_DEBUG("Close transaction '%s'", transaction_id);


      auto const& pair = State::transactions().find(transaction_id);

      assert(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;
    }



    bool
    State::poll()
    {
      if (!this->_trophonius)
        throw Exception{gap_error, "Trophonius is not connected"};
      bool continue_ = false;
      do
        {
          std::unique_ptr<json::Dictionary> dict_ptr{this->_trophonius->poll()};

          if (!dict_ptr)
            return continue_;

          json::Dictionary const& dict = *dict_ptr;
          continue_ = this->_handle_dictionnary(dict);
        } while (continue_);
      return true;
    }

    bool
    State::_handle_dictionnary(json::Dictionary const& dict, bool new_)
    {
      ELLE_DEBUG("Dictionnary '%s'.", dict.repr());

      if(!dict.contains("notification_id"))
        {
          ELLE_WARN("Dictionnary doesn't contains 'notification_id' field.");
          return false;
        }

      int notification_id = dict["notification_id"].as_integer();

      // XXX: Value of shit written in hard coded.
      // Connexion established.
      if (notification_id == gap_Notification::gap_notificaiton_connection_enabled)
        return false;

      auto handler_list = _notification_handlers.find(notification_id);

      if (handler_list == _notification_handlers.end())
        {
          ELLE_WARN("Handler missing for notification '%u'", notification_id);
          return false;
        }

      for (auto& handler : handler_list->second)
        {
          ELLE_ASSERT(handler != nullptr);
          handler->call(dict, new_);
        }

      return true;
    }

    bool
    State::has_device() const
    {
      assert(this->_me._id.size() > 0 && "not properly initialized");
      ELLE_DEBUG("Check for '%s' device existence at '%s'",
                 this->_me._id,
                 common::passport_path(this->_me._id));
      return fs::exists(common::passport_path(this->_me._id));
    }

    std::string const&
    State::device_id()
    {
      if (this->_device_id.size() == 0)
        {
          elle::Passport passport;
          passport.load(common::passport_path(this->_me._id));
          this->_device_id = passport.id();
          this->_device_name = passport.name();
        }
      return this->_device_id;
    }

    std::string const&
    State::device_name()
    {
      if (this->_device_name.size() == 0)
        {
          elle::Passport passport;
          passport.load(common::passport_path(this->_me._id));
          this->_device_id = passport.id();
          this->_device_name = passport.name();
        }
      return this->_device_name;
    }

    void
    State::update_device(std::string const& name, bool force_create)
    {
      std::string passport_string;

      elle::io::Path passport_path(lune::Lune::Passport);

      ELLE_WARN("ID: '%s'", this->_me._id);
      passport_path.Complete(elle::io::Piece{"%USER%", this->_me._id});

      this->_device_name = name;
      ELLE_DEBUG("Device to update: '%s'", this->_device_name.c_str());

      if (force_create || !this->has_device())
        {
          auto res = this->_meta->create_device(name);
          passport_string = res.passport;
          this->_device_id = res.created_device_id;
          ELLE_DEBUG("Created device id: %s", this->_device_id.c_str());
        }
      else
        {
          ELLE_DEBUG("Loading passport from '%s'.", passport_path);
          elle::Passport passport;
          passport.load(passport_path);

          ELLE_DEBUG("Passport id: %s", passport.id());
          auto res = this->_meta->update_device(passport.id(), name);
          this->_device_id = res.updated_device_id;
          passport_string = res.passport;
        }

      elle::Passport passport;
      if (passport.Restore(passport_string) == elle::Status::Error)
        throw Exception(gap_wrong_passport, "Cannot load the passport");

      passport.store(passport_path);
    }

    //- Network management ----------------------------------------------------

    std::string
    State::create_network(std::string const& name)
    {
      auto response = this->_meta->create_network(name);
      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
      this->refresh_networks(); //XXX not optimal
      return response.created_network_id;
    }

    std::map<std::string, Network*> const&
    State::networks()
    {
      if (this->_networks_dirty)
        {
          auto response = this->_meta->networks();
          for (auto const& network_id: response.networks)
            {
              if (this->_networks.find(network_id) == this->_networks.end())
                {
                  auto response = this->_meta->network(network_id);
                  this->_networks[network_id] = new Network{response};
                }
            }
          this->_networks_dirty = false;
        }
      return this->_networks;
    }

    Network const&
    State::network(std::string const& id)
    {
      auto it = this->networks().find(id);
      if (it == this->networks().end())
        throw Exception{
            gap_error,
            "Cannot find any network for id '" + id + "'"
        };
      return *(it->second);
    }

    void
    State::network_add_user(std::string const& network_id,
                            std::string const& user)
    {
      // makes user we have an id
      std::string user_id = this->user(user)._id;
      auto it = this->networks().find(network_id);
      assert(it != this->networks().end());
      Network* network = it->second;
      assert(network != nullptr);

      std::string const& group_binary = common::infinit::binary_path("8group");

      QStringList arguments;
      arguments << "--user" << user_id.c_str()
                << "--type" << "user"
                << "--add"
                << "--network" << network->_id.c_str()
                << "--identity" << this->user(user_id).public_key.c_str()
      ;
      ELLE_DEBUG("LAUNCH: %s %s",
                      group_binary,
                      arguments.join(" ").toStdString());
      QProcess p;
      p.start(group_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8group binary failed");
      if (p.exitCode())
        throw Exception(gap_internal_error, "8group binary exited with errors");
      auto res = this->_meta->network_add_user(network_id, user_id);
      if (std::find(network->users.begin(),
                    network->users.end(),
                    user_id) == network->users.end())
        network->users.push_back(user_id);
    }

    std::map<std::string, NetworkStatus*> const&
    State::networks_status()
    {
      if (this->_networks_status_dirty)
        {
          json::Dictionary response;
          this->_send_watchdog_cmd("status", nullptr, &response);

          auto& networks = response["networks"].as_array();

          for (size_t i = 0; i < networks.size(); ++i)
            {
              ELLE_DEBUG("network '%i'.", i);
              auto& network = networks[i].as_dictionary();
              ELLE_DEBUG("> '%s'.", network["_id"].as_string());
              std::string mount_point = network["mount_point"].as_string();
              std::string network_id = network["_id"].as_string();

              NetworkStatus* status = nullptr;
              auto it = this->_networks_status.find(network_id);
              if (it == this->_networks_status.end())
                {
                  status = this->_networks_status[network_id]
                         = new NetworkStatus;
                }
              else
                {
                  status = it->second;
                }
              assert(status != nullptr);
              *status = NetworkStatus{
                  network_id,
                  mount_point,
              };
            }
          this->_networks_status_dirty = false;
        }
      else
        {
          ELLE_DEBUG("Networks are clean.");
        }
      return this->_networks_status;
    }

    NetworkStatus const&
    State::network_status(std::string const& id)
    {
      auto it = this->networks_status().find(id);
      if (it == this->networks_status().end())
        throw Exception{
            gap_error,
            "Unknown network id '" + id + "'",
        };
      return *(it->second);
    }

    //- Watchdog --------------------------------------------------------------

    void
    State::stop_watchdog()
    {
      this->_send_watchdog_cmd("stop");
      // Waiting for the old server to be stopped
      int tries = 1;
      int sleep_time = 2;
      do {
            {
              QLocalSocket conn;
              conn.connectToServer(
                  common::watchdog::server_name(this->_me._id).c_str()
              );
              if (!conn.waitForConnected(2000))
                break;
              conn.disconnectFromServer();
            }
            ELLE_DEBUG("Waiting %s secs for the old watchdog to be stopped (%s / 10 )",
                       sleep_time, tries);
          ::sleep(sleep_time);
          sleep_time += 2;
      } while (++tries < 10);

      if (tries >= 10)
        throw Exception(gap_internal_error,
                        "The old watchdog instance does not stop !");
      this->_networks_status_dirty = true;
    }

    void
    State::launch_watchdog()
    {
      if (!this->has_device())
        throw Exception(gap_no_device_error,
                        "Cannot start infinit without any local device");

      if (this->_meta->token().size() == 0 ||
          this->_meta->identity().size() == 0 ||
          this->_meta->email().size() == 0)
        throw Exception(gap_not_logged_in,
                        "Cannot start infinit anonymously");

      std::string old_watchdog_id;
      try
        {
          old_watchdog_id = this->_watchdog_id();
          this->stop_watchdog();
        }
      catch (std::exception const& err)
        {
          ELLE_WARN("Couldn't stop the watchdog: %s", err.what());
        }

      ELLE_ASSERT(this->_me._id.size() != 0);

      elle::os::path::make_path(
          common::infinit::user_directory(this->_me._id)
      );
      std::string watchdog_binary = common::infinit::binary_path("8watchdog");

      QStringList arguments;
      arguments << this->_me._id.c_str();

      ELLE_DEBUG("Launching binary: %s with id: %s", watchdog_binary, this->_me._id);
      if (QProcess::execute(watchdog_binary.c_str(), arguments) < 0)
        throw Exception(gap_internal_error, "Cannot start the watchdog");

      // Connect to the new watchdog instance
      QLocalSocket conn;
      int tries = 0;
      while (tries++ < 10)
        {
          conn.connectToServer(common::watchdog::server_name(this->_me._id).c_str());
          ELLE_DEBUG("Trying to connect to the new watchdog");
          if (conn.waitForConnected(2000))
            break;
          ELLE_DEBUG("Retrying to connect (%s)", tries);
          ::sleep(1);
        }
      if (!conn.isValid())
        throw Exception(gap_internal_error, "Couldn't connect to the new watchdog instance");

      ELLE_DEBUG("Connected to the watchdog");

      // Getting the new watchdog id
      // When connected, the watchdog id file should exists
      std::string new_watchdog_id;
      tries = 0;
      do {
          if (tries > 0) ::sleep(1);
          try { new_watchdog_id = this->_watchdog_id(); }
          catch (std::exception const& err)
            {
              ELLE_WARN("Cannot read the new watchdog id: %s", err.what());
            }
          if (new_watchdog_id.size() && old_watchdog_id != new_watchdog_id)
            {
              ELLE_DEBUG("Found new watchdog id: %s", new_watchdog_id);
              break;
            }
      } while (++tries < 10);

      if (tries == 10)
        throw Exception(gap_internal_error, "Couldn't open infinit watchdog id file");

      // calling watchdog run command (which gives the meta token)
        {
          json::Dictionary args;
          args["token"] = this->_meta->token();
          args["identity"] = this->_meta->identity();
          args["user"] = this->_meta->email();
          args["user_id"] = this->_me._id;
          this->_send_watchdog_cmd("run", &args);
        }
    }

    /// - File level ----------------------------------------------------------

    FileInfos const&
    State::file_infos(std::string const& path)
    {
      std::string abspath = elle::os::path::absolute(path, true);
      ELLE_DEBUG("Get file infos of %s", abspath);
      auto it = this->_files_infos.find(abspath);
      if (it != this->_files_infos.end())
        return *(it->second);

      std::unique_ptr<FileInfos> infos;

      for (auto const& pair : this->networks_status())
        {
          std::string mount_point = pair.second->mount_point;

          if (boost::algorithm::starts_with(abspath, mount_point))
            {
              std::string relpath;
              if (abspath == mount_point)
                relpath = "";
              else
                relpath = abspath.substr(mount_point.size() + 1);

              infos.reset(new FileInfos{
                  mount_point,
                  pair.first,
                  abspath,
                  relpath,
                  {},
              });
              break;
            }
        }

      if (infos.get() == nullptr)
        throw Exception(gap_error,
                        "Cannot find network for path '" + abspath + "'");

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_meta->email().c_str()
                << "--type" << "user"
                << "--network" << this->network(infos->network_id)._id.c_str()
                << "--path" << ("/" + infos->relative_path).c_str()
                << "--consult"
                ;

      QProcess p;
      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished()) // .8 sec
        throw Exception(gap_internal_error, "8access binary failed");
      std::stringstream ss{p.readAllStandardOutput().constData()};

      std::string line;
      while(std::getline(ss, line))
        {
          if (!line.size())
            continue;
          std::stringstream line_stream{line};
          std::string public_key;
          int permissions;
          line_stream >> public_key /* eat "User" */
                      >> public_key
                      >> permissions;
          int gap_perm = (int) gap_none;
          if (permissions & nucleus::neutron::permissions::read)
            gap_perm |= gap_read;
          if (permissions & nucleus::neutron::permissions::write)
            gap_perm |= gap_write;
          std::string const& user_id = this->user_from_public_key(public_key)._id;
          infos->accesses[user_id] = gap_perm;
        }

      this->_files_infos[abspath] = infos.get();
      return *(infos.release());
    }

    void State::set_permissions(std::string const& user_id,
                                std::string const& abspath,
                                int permissions,
                                bool recursive)
    {
      FileInfos const& infos = this->file_infos(abspath);
      auto it = this->networks().find(infos.network_id);

      if (it == this->networks().end())
        throw Exception(gap_error, "Wrong network id");

      Network* network = it->second;

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << _meta->email().c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network->_id.c_str()
                << "--path" << ("/" + infos.relative_path).c_str()
                << "--identifier" << this->user(user_id).public_key.c_str()
                ;
      if (permissions & gap_read)
        arguments << "--read";
      if (permissions & gap_write)
        arguments << "--write";

      ELLE_DEBUG("LAUNCH: %s %s", access_binary, arguments.join(" ").toStdString());

      if (permissions & gap_exec)
        {
          ELLE_WARN("XXX: setting executable permissions not yet implemented");
        }

      QProcess p;
      p.start(access_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception(gap_internal_error, "8access binary failed");

      if (recursive && elle::os::path::is_directory(abspath))
        {
          boost::filesystem::directory_iterator it{abspath},
                                                end{};
          for (; it != end; ++it)
            {
              this->set_permissions(user_id,
                                    it->path().string(),
                                    permissions,
                                    true);
            }
        }
    }

    size_t
    State::get_size(fs::path const& path)
    {
      if (!fs::exists(path))
        return 0;

      if (!fs::is_directory(path))
        return fs::file_size(path);

      size_t size = 0;
      fs::directory_iterator end_itr;

      for (fs::directory_iterator itr(path);
           itr != end_itr;
           ++itr)
        {
          if (fs::is_directory(itr->status()))
            {
              size += get_size(itr->path());
            }
          else
            {
              size += fs::file_size(itr->path());
            }
        }
      return size;
    }

    // - TROPHONIUS ----------------------------------------------------
    /// Connect to trophonius
    void
    State::connect()
    {
      if (this->_trophonius)
        throw Exception{gap_error, "trophonius is already connected"};

      try
        {
          this->_trophonius.reset(new plasma::trophonius::Client{
            common::trophonius::host(),
            common::trophonius::port(),
            true,
          });
        }
      catch (std::runtime_error const& err)
        {
          throw Exception{gap_error, "Couldn't connect to trophonius"};
        }
      this->_trophonius->connect(this->_me._id,
                                 this->_meta->token());

      ELLE_DEBUG("Connect to trophonius with 'id': %s and 'token':  %s",
                 this->_meta->identity(), this->_meta->token());
    }
  }
}
