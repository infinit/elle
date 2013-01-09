#include <cstdlib>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <thread>
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
#include <elle/os/getenv.hh>
#include <elle/Passport.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/utility/Time.hh>
#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>
#include <elle/HttpClient.hh>
#include <elle/CrashReporter.hh>

#include <common/common.hh>
#include <elle/os/path.hh>

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
    namespace fs = boost::filesystem;
    namespace json = elle::format::json;

    // - Exception ------------------------------------------------------------

    Exception::Exception(gap_Status code, std::string const& msg)
      : std::runtime_error(msg)
      , code(code)
    {}

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
      , _logged{false}
      , _swaggers_dirty{true}
      , _output_dir{common::system::download_directory()}
      , _files_infos{}
      , _networks{}
      , _networks_dirty{true}
      , _networks_status{}
      , _networks_status_dirty{true}
    {
      // XXX degeu !
      static std::ofstream* out = new std::ofstream{
          elle::os::path::join(common::infinit::home(), "state.log"),
          std::fstream::app | std::fstream::out
      };
      elle::log::logger("infinit.surface.gap.State").output(*out);
      elle::log::logger("infinit.surface.gap.State").level(
          elle::log::Logger::Level::debug
      );
      elle::log::logger("elle.HttpClient").output(*out);
      elle::log::logger("elle.HttpClient").level(
          elle::log::Logger::Level::debug
      );
      ELLE_LOG("Creating a new State");

      namespace p = std::placeholders;
      this->transaction_callback(
        std::bind(&State::_on_transaction, this, p::_1, p::_2)
      );
      this->transaction_status_callback(
        std::bind(&State::_on_transaction_status, this, p::_1)
      );

      std::string user = elle::os::getenv("INFINIT_USER", "");

      if (user.length() > 0)
      {
        std::string identity_path = common::watchdog::identity_path(user);

        if (identity_path.length() > 0 && fs::exists(identity_path))
        {
          std::ifstream identity;
          identity.open(identity_path);

          if (!identity.good())
            return;

          std::string token;
          std::getline(identity, token);

          std::string ident;
          std::getline(identity, ident);

          std::string mail;
          std::getline(identity, mail);

          std::string id;
          std::getline(identity, id);

          this->_meta->token(token);
          this->_meta->identity(ident);
          this->_meta->email(mail);

          this->_me = static_cast<User const&>(this->_meta->self());

          this->_logged = true;
        }
      }
    }

    State::State(std::string const& token):
      State{}
    {
      this->_meta->token(token);
      auto res = this->_meta->self();
      this->_meta->identity(res.identity);
      this->_meta->email(res.email);
      //XXX factorize that shit
      this->_me = static_cast<User const&>(res);

      this->_logged = true;
    }


    State::~State()
    {
      for (auto& it: this->_networks)
        delete it.second;
      this->_networks.clear();
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
      ELLE_TRACE("Send watchdog command");
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
          response.status,
      }};

      this->_users[response._id] = user.get();
      return *(user.release());
    }

    elle::Buffer
    State::user_icon(std::string const& id)
    {
      return this->_meta->user_icon(id);
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
          response.status,
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

    State::SwaggersMap const&
    State::swaggers()
    {
      if (this->_swaggers_dirty)
        {
          auto response = this->_meta->get_swaggers();
          for (auto const& swagger_id: response.swaggers)
            {
              if (this->_swaggers.find(swagger_id) == this->_swaggers.end())
                {
                  auto response = this->_meta->user(swagger_id);
                  this->_swaggers[swagger_id] = new User{
                      response._id,
                      response.fullname,
                      response.email,
                      response.public_key,
                      response.status,
                  };
                }
            }
          this->_swaggers_dirty = false;
        }
      return this->_swaggers;
    }

    User const&
    State::swagger(std::string const& id)
    {
      auto it = this->swaggers().find(id);
      if (it == this->swaggers() .end())
        throw Exception{
            gap_error,
            "Cannot find any swagger for id '" + id + "'"
        };
      return *(it->second);
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

        this->_logged = true;
    }

    void
    State::logout()
    {
      this->_meta->logout();
    }

    static
    std::unique_ptr<Notification>
    _xxx_dict_to_notification(json::Dictionary const& d)
    {
      std::unique_ptr<Notification> res;
      NotificationType notification_type = (NotificationType) d["notification_type"].as_integer().value();

      std::unique_ptr<UserStatusNotification> user_status{
          new UserStatusNotification
      };

      std::unique_ptr<TransactionNotification> transaction{
          new TransactionNotification
      };
      std::unique_ptr<TransactionStatusNotification> transaction_status{
          new TransactionStatusNotification
      };
      std::unique_ptr<MessageNotification> message{
          new MessageNotification
      };

      switch (notification_type)
        {
        case NotificationType::user_status:
          user_status->user_id = d["user_id"].as_string();
          user_status->status = d["status"].as_integer();
          res = std::move(user_status);
          break;

        case NotificationType::transaction:
          transaction->transaction.transaction_id = d["transaction"]["transaction_id"].as_string();
          transaction->transaction.sender_id = d["transaction"]["sender_id"].as_string();
          transaction->transaction.sender_fullname = d["transaction"]["sender_fullname"].as_string();
          transaction->transaction.sender_device_id = d["transaction"]["sender_device_id"].as_string();
          transaction->transaction.recipient_id = d["transaction"]["recipient_id"].as_string();
          transaction->transaction.recipient_fullname = d["transaction"]["recipient_fullname"].as_string();
          transaction->transaction.recipient_device_id = d["transaction"]["recipient_device_id"].as_string();
          transaction->transaction.recipient_device_name = d["transaction"]["recipient_device_name"].as_string();
          transaction->transaction.network_id = d["transaction"]["network_id"].as_string();
          transaction->transaction.message = d["transaction"]["message"].as_string();
          transaction->transaction.first_filename = d["transaction"]["first_filename"].as_string();
          transaction->transaction.files_count = d["transaction"]["files_count"].as_integer();
          transaction->transaction.total_size = d["transaction"]["total_size"].as_integer();
          transaction->transaction.is_directory = d["transaction"]["is_directory"].as_integer();
          transaction->transaction.status = d["transaction"]["status"].as_integer();
          res = std::move(transaction);
          break;

        case NotificationType::transaction_status:
          transaction_status->transaction_id = d["transaction_id"].as_string();
          transaction_status->status = d["status"].as_integer();
          res = std::move(transaction_status);
          break;

        case NotificationType::message:
          message->sender_id = d["sender_id"].as_string();
          message->message = d["message"].as_string();
          res = std::move(message);
          break;

        case NotificationType::connection_enabled:
          res.reset(new Notification);
          break;

        default:
          throw elle::Exception{
              "Unknown notification type %s", notification_type
          };
        }
      res->notification_type = notification_type;
      return res;
    }


    void
    State::pull_notifications(int count, int offset)
    {
      if (count < 1)
        return;

      if (offset < 0)
        return;

      auto res = this->_meta->pull_notifications(count, offset);

      // Handle old notif first to act like a queue.
      for (auto& dict : res.old_notifs)
        this->_handle_notification(*_xxx_dict_to_notification(dict), false);

      for (auto& dict : res.notifs)
        this->_handle_notification(*_xxx_dict_to_notification(dict), true);
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

    //- Network process -------------------------------------------------------

    struct State::Process
    {
      typedef std::function<void(void)> Callback;

      Callback            callback;
      std::string         name;
      bool                done;
      bool                success;
      std::exception_ptr  exception;
      std::thread         thread;

      Process(std::string const& name,
              Callback const& cb)
        : callback{cb}
        , name{name}
        , done{false}
        , success{false}
        , exception{}
        , thread{&Process::_run, this}
      {
        ELLE_LOG("Creating long operation: %s", this->name);
      }

    private:
      void _run()
      {
        try
          {
            ELLE_LOG("Running long operation: %s", this->name);
            (this->callback)();
            success = true;
          }
        catch (...)
          {
            this->exception = std::current_exception();
          }
        done = true;
      }
    };

    State::ProcessStatus
    State::process_status(ProcessId const id) const
    {
      auto it = _processes.find(id);
      if (it == _processes.end())
        throw elle::Exception{
            "Couldn't find any process with id " + std::to_string(id)
        };
      if (!it->second->done)
        return ProcessStatus::running;
      // the process is terminated.
      if (it->second->success)
        return ProcessStatus::success;
      return ProcessStatus::failure;
    }

    void
    State::process_finalize(ProcessId const id)
    {
      auto it = _processes.find(id);
      if (it == _processes.end())
        throw elle::Exception{
            "Couldn't find any process with id " + std::to_string(id)
        };
      if (!it->second->done)
        throw elle::Exception{"Process not finished"};
      ProcessPtr ptr{it->second.release()};
      _processes.erase(it);
      if (!ptr->success)
        std::rethrow_exception(ptr->exception); // XXX exception_ptr deleted !
    }

    State::ProcessId
    State::_add_process(std::string const& name,
                        std::function<void(void)> const& cb)
    {
      //auto process = elle::make_unique<Process>(*this, name, cb);
      // XXX leak if emplace() fails.
      static ProcessId id = 0;
      _processes[id].reset(new Process{name, cb});
      return id++;
    }

    State::ProcessId
    State::send_files(std::string const& recipient_id_or_email,
                      std::unordered_set<std::string> const& files)
    {
      ELLE_TRACE("Sending files to %s", recipient_id_or_email);
      return this->_add_process(
        "send_files",
        std::bind(&State::_send_files, this, recipient_id_or_email, files)
      );
    }

    void
    State::_send_files(std::string const& recipient_id_or_email,
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

      std::string network_name = elle::sprintf("%s-%s",
                                               recipient_id_or_email,
                                               time.nanoseconds);

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
          ELLE_DEBUG("Waiting for portal file");
          ::sleep(1);
        }

      ELLE_DEBUG("Portal file found? %s", fs::exists(portal_path));
      if (!fs::exists(portal_path))
          throw Exception{gap_error, "Couldn't find portal to infinit instance"};

      ELLE_DEBUG("Retrieving 8transfert binary path...");
      std::string const& transfer_binary = common::infinit::binary_path("8transfer");
      ELLE_DEBUG("Using 8transfert binary '%s'", transfer_binary);

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

      try
      {
        this->_meta->create_transaction(recipient_id_or_email,
                                        first_filename,
                                        files.size(),
                                        size,
                                        fs::is_directory(first_filename),
                                        network_id,
                                        this->device_id());
      }
      catch (elle::Exception const&)
      {
        // Something went wrong, we need to destroy the network.
        this->delete_network(network_id, false);
        throw;
      }
    }

    //- Transactions ----------------------------------------------------------

    void
    State::_download_files(std::string const& transaction_id)
    {
      ELLE_ASSERT(this->_output_dir.length() != 0);

      auto pair = State::transactions().find(transaction_id);

      ELLE_ASSERT(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      Transaction const& trans = pair->second;

      (void) this->refresh_networks();

      // Ensure the network status is available
      (void) this->network_status(trans.network_id);

      std::string const& transfer_binary = common::infinit::binary_path("8transfer");

      QStringList arguments;
      arguments << "-n" << trans.network_id.c_str()
                << "-u" << this->_me._id.c_str()
                << "--path" << this->_output_dir.c_str()
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
                this->_output_dir.c_str());

      update_transaction(transaction_id,
                         gap_TransactionStatus::gap_transaction_status_finished);

    }

    void
    State::output_dir(std::string const& dir)
    {
      printf("%s\n", dir.c_str());

      if (!fs::exists(dir))
        throw Exception(gap_error,
                        "directory doesn't exist.");

      if (!fs::is_directory(dir))
        throw Exception(gap_error,
                        "not a directroy.");

      this->_output_dir = dir;
    }

    std::string
    State::output_dir()
    {
      return this->_output_dir;
    }

    void
    State::update_transaction(std::string const& transaction_id,
                              gap_TransactionStatus status)
    {
      typedef
        std::map<gap_TransactionStatus, std::set<gap_TransactionStatus>>
        StatusMap;

      static StatusMap _sender_status_update{
        {gap_transaction_status_pending,
          {
            gap_transaction_status_canceled
          }
        },
        {gap_transaction_status_accepted,
          {
            gap_transaction_status_started,
            gap_transaction_status_canceled
          }
        },
        {gap_transaction_status_started,
          {
            gap_transaction_status_canceled
          }
        },
        // {gap_transaction_status_canceled,
        //   {
        //   }
        // },
        // {gap_transaction_status_finished,
        //   {
        //   }
        //}
      };

      static StatusMap _recipient_status_update{
        {gap_transaction_status_pending,
          {
            gap_transaction_status_accepted,
            gap_transaction_status_canceled
          }
        },
        {gap_transaction_status_accepted,
          {
            gap_transaction_status_canceled
          }
        },
        {gap_transaction_status_started,
          {
            gap_transaction_status_canceled,
            gap_transaction_status_finished
          }
        },
        // {gap_transaction_status_canceled,
        //   {
        //   }
        // },
        // {gap_transaction_status_finished,
        //   {
        //   }
        // }
      };

      ELLE_DEBUG("Update transaction '%s': '%s'", transaction_id, status);

      auto pair = State::transactions().find(transaction_id);

      ELLE_ASSERT(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      Transaction const& transaction = pair->second;

      if (this->_me._id != transaction.recipient_id &&
          this->_me._id != transaction.sender_id)
      {
        throw Exception{
          gap_error,
            "You are neither recipient nor the sender."
        };
      }

      if (this->_me._id == transaction.recipient_id)
      {
        auto const& status_list = _recipient_status_update.find(
          (gap_TransactionStatus) transaction.status);

        if (status_list == _recipient_status_update.end() ||
            status_list->second.find((gap_TransactionStatus) status) == status_list->second.end())
        {
          ELLE_WARN("You are not allowed to change status from %s to %s",
                    transaction.status, status);
          return;
        }
      }
      else if (this->_me._id == transaction.sender_id)
      {
         auto const& status_list = _sender_status_update.find(
           (gap_TransactionStatus) transaction.status);

        if (status_list == _sender_status_update.end() ||
            status_list->second.find((gap_TransactionStatus)status) == status_list->second.end())
        {
          ELLE_WARN("You are not allowed to change status from %s to %s",
                    transaction.status, status);
          return;
        }
      }

      switch ((gap_TransactionStatus) status)
      {
        case gap_transaction_status_accepted:
          this->_accept_transaction(transaction);
          break;
        case gap_transaction_status_started:
          this->_start_transaction(transaction);
          break;
        case gap_transaction_status_canceled:
          this->_cancel_transaction(transaction);
          break;
        case gap_transaction_status_finished:
          this->_close_transaction(transaction);
          break;
        default:
          ELLE_WARN("You are not able to change transaction status to '%i'.",
                    status);
          return;
      }
    } // !update_transaction()

    void
    State::_accept_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Accept transaction '%s'", transaction.transaction_id);

      if (transaction.recipient_id != this->_me._id)
      {
        throw Exception{gap_error,
            "Only recipient can accept transaction."};
      }

      this->_meta->update_transaction(transaction.transaction_id,
                                      gap_transaction_status_accepted,
                                      this->device_id(),
                                      this->device_name());

      // Could be improve.
      _swaggers_dirty = true;
    }

    void
    State::_start_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Start transaction '%s'", transaction.transaction_id);

      if (transaction.sender_id != this->_me._id)
      {
        throw Exception{gap_error,
            "Only sender can start transaction."};
      }

      this->_meta->update_transaction(transaction.transaction_id,
                                      gap_transaction_status_started);

    }

    void
    State::_cancel_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Cancel transaction '%s'", transaction.transaction_id);

      //XXX: If download has started, cancel it, delete files, ...
      if (transaction.sender_id == this->_me._id)
      {
        //XXX
        this->_meta->update_transaction(transaction.transaction_id,
                                        gap_transaction_status_canceled);

      }
      else
      {
        //XXX
        this->_meta->update_transaction(transaction.transaction_id,
                                        gap_transaction_status_canceled);
      }
    }

    void
    State::_close_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Close transaction '%s'", transaction.transaction_id);

      if(transaction.recipient_id != this->_me._id)
      {
        throw Exception{gap_error,
            "Only recipient can close transaction."};
      }

      this->_meta->update_transaction(transaction.transaction_id,
                                      gap_TransactionStatus::gap_transaction_status_finished);

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

    State::TransactionsMap const&
    State::transactions()
    {
      if (_transactions != nullptr)
        return *_transactions;

      _transactions.reset(new TransactionsMap{});

      auto response = this->_meta->transactions();
      for (auto const& transaction_id: response.transactions)
        {
          auto transaction = this->_meta->transaction(transaction_id);
          (*this->_transactions)[transaction_id] = transaction;
        }

      return *(this->_transactions);
    }

    Transaction const&
    State::transaction(std::string const& id)
    {
      auto it = this->transactions().find(id);
      if (it == this->transactions().end())
        throw Exception{
            gap_error,
            "Cannot find any transaction for id '" + id + "'"
        };
      return it->second;
    }

    // Thoose callbacks are used to keep the client and the server as the same
    // state.
    // Both recipient and sender recieve notifications, so some notification
    // will only trigger a visual effect.
    // Some of this notification will produce automatic actions such as
    // launching 8transfer, 8acces, 8group.

    void
    State::_on_transaction_accepted(Transaction const& transaction)
    {
      ELLE_DEBUG("On transaction accepted '%s'", transaction.transaction_id);


      if (transaction.sender_id != this->_me._id)
        return;

      ELLE_DEBUG("Giving '%s' acces to the network '%s'.",
                transaction.recipient_id,
                transaction.network_id);

      this->network_add_user(transaction.network_id,
                             transaction.recipient_id);

      ELLE_DEBUG("Giving '%s' permissions on the network to '%s'.",
                transaction.recipient_id,
                transaction.network_id);

      this->set_permissions(transaction.recipient_id,
                            transaction.network_id,
                            nucleus::neutron::permissions::write);

      // When recipient has rights, allow him to start download.
      this->update_transaction(transaction.transaction_id,
                               gap_transaction_status_started);

      // XXX Could be improved.
      _swaggers_dirty = true;
    }

    void
    State::_on_transaction_started(Transaction const& transaction)
    {
      ELLE_DEBUG("Started transaction '%s'", transaction.transaction_id);

      if (transaction.recipient_id != this->_me._id)
        return;

      _download_files(transaction.transaction_id);
    }

    void
    State::_on_transaction_canceled(Transaction const& transaction)
    {
      ELLE_DEBUG("Canceled transaction '%s'", transaction.transaction_id);

      // XXX: If some process are launch, such as 8transfer, 8progess for the
      // current transaction, cancel them.

      // Delete networks.
      (void) this->delete_network(transaction.network_id, true);

      (void) this->refresh_networks();
    }

    void
    State::_on_transaction_closed(Transaction const& transaction)
    {
      ELLE_DEBUG("Closed transaction '%s'", transaction.transaction_id);

      // Delete networks.
      (void) this->delete_network(transaction.network_id);

      (void) this->refresh_networks();
    }

    size_t
    State::poll(size_t max)
    {
      if (!this->_trophonius)
        throw Exception{gap_error, "Trophonius is not connected"};

      size_t count = 0;
      while (count < max)
        {
          std::unique_ptr<Notification> notif{
              this->_trophonius->poll()
          };

          if (!notif)
            break;

          this->_handle_notification(*notif);
          ++count;
        }

      return count;
    }

    void
    State::_handle_notification(Notification const& notif,
                                bool new_)
    {
      // Connexion established.
      if (notif.notification_type == NotificationType::connection_enabled)
        // XXX set _connection_enabled to true
        return;

      auto handler_list = _notification_handlers.find(notif.notification_type);

      if (handler_list == _notification_handlers.end())
        {
          ELLE_WARN("Handler missing for notification '%u'",
                    notif.notification_type);
          return;
        }

      for (auto& handler : handler_list->second)
        {
          ELLE_ASSERT(handler != nullptr);
          handler(notif, new_);
        }
    }

    bool
    State::has_device() const
    {
      ELLE_ASSERT(this->_me._id.size() > 0 && "not properly initialized");
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

    std::string
    State::delete_network(std::string const& network_id, bool force)
    {
      auto const& net = this->_networks.find(network_id);

      if (net != this->_networks.end())
      {
        delete net->second;
        this->_networks.erase(net);
      }

      auto response = this->_meta->delete_network(network_id, force);
      this->_networks_dirty = true;
      this->_networks_status_dirty = true;
      this->refresh_networks(); //XXX not optimal
      return response.deleted_network_id;
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
      ELLE_ASSERT(it != this->networks().end());
      Network* network = it->second;
      ELLE_ASSERT(network != nullptr);

      std::string const& group_binary = common::infinit::binary_path("8group");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
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
              ELLE_ASSERT(status != nullptr);
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
      arguments << "--user" << this->_me._id.c_str()
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

    void
    State::deprecated_set_permissions(std::string const& user_id,
                                      std::string const& abspath,
                                      nucleus::neutron::Permissions permissions,
                                      bool recursive)
    {
      FileInfos const& infos = this->file_infos(abspath);
      auto it = this->networks().find(infos.network_id);

      if (it == this->networks().end())
        throw Exception(gap_error, "Wrong network id");

      Network* network = it->second;

      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network->_id.c_str()
                << "--path" << ("/" + infos.relative_path).c_str()
                << "--identity" << this->user(user_id).public_key.c_str()
                ;
      if (permissions & nucleus::neutron::permissions::read)
        arguments << "--read";
      if (permissions & nucleus::neutron::permissions::write)
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
              this->deprecated_set_permissions(user_id,
                                               it->path().string(),
                                               permissions,
                                               true);
            }
        }
    }

    void
    State::set_permissions(std::string const& user_id,
                           std::string const& network_id,
                           nucleus::neutron::Permissions permissions)
    {
      std::string const& access_binary = common::infinit::binary_path("8access");

      QStringList arguments;
      arguments << "--user" << this->_me._id.c_str()
                << "--type" << "user"
                << "--grant"
                << "--network" << network_id.c_str()
                << "--path" << "/"
                << "--identity" << this->user(user_id).public_key.c_str()
        ;
      if (permissions & nucleus::neutron::permissions::read)
        arguments << "--read";
      if (permissions & nucleus::neutron::permissions::write)
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
      if (p.exitCode())
        throw Exception(gap_internal_error, "8access binary exited with errors");
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

    void
    State::user_status_callback(UserStatusNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool) -> void {
        return cb(static_cast<UserStatusNotification const&>(notif));
      };

      this->_notification_handlers[NotificationType::user_status].push_back(fn);
    }

    void
    State::transaction_callback(TransactionNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool is_new) -> void {
        return cb(static_cast<TransactionNotification const&>(notif), is_new);
      };

      this->_notification_handlers[NotificationType::transaction].push_back(fn);
    }

    void
    State::transaction_status_callback(TransactionStatusNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool is_new) -> void {
        return cb(static_cast<TransactionStatusNotification const&>(notif), is_new);
      };

      _notification_handlers[NotificationType::transaction_status].push_back(fn);
    }

    void
    State::message_callback(MessageNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool) -> void {
        return cb(static_cast<MessageNotification const&>(notif));
      };

      this->_notification_handlers[NotificationType::message].push_back(fn);
    }

    void
    State::_on_transaction(TransactionNotification const& notif,
                           bool is_new)
    {
      ELLE_TRACE("_on_notification: gap_TransactionNotification");

      if (!is_new)
        return;

      auto it = this->transactions().find(notif.transaction.transaction_id);

      if (it != this->transactions().end())
        return;

      // Normal case, this is a new transaction, store it to match server.
      (*this->_transactions)[notif.transaction.transaction_id] = notif.transaction;
    }

    void
    State::_on_transaction_status(TransactionStatusNotification const& notif)
    {
      ELLE_TRACE("_on_notification: gap_TransactionStatusNotification");

      auto const pair = State::transactions().find(notif.transaction_id);

      if (pair == State::transactions().end())
      {
        // Something went wrong.
        auto transaction = this->_meta->transaction(notif.transaction_id);

        (*this->_transactions)[notif.transaction_id] = transaction;
      }

      (*this->_transactions)[notif.transaction_id].status = notif.status;

      auto const& transaction = this->transaction(notif.transaction_id);

      switch(notif.status)
      {
        case gap_transaction_status_accepted:
          // We update the transaction from meta.
          (*_transactions)[notif.transaction_id] = this->_meta->transaction(
              notif.transaction_id
          );
          this->_on_transaction_accepted(transaction);
          break;
        case gap_transaction_status_started:
          this->_on_transaction_started(transaction);
          break;
        case gap_transaction_status_canceled:
          this->_on_transaction_canceled(transaction);
          break;
        case gap_transaction_status_finished:
          this->_on_transaction_closed(transaction);
          break;
        default:
          ELLE_WARN("The status '%s' is unknown.", notif.status);
          return;
      }
    }
  }
}
