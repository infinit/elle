#include "../State.hh"
#include "../MetricReporter.hh"

#include <common/common.hh>

#include <lune/Dictionary.hh>
#include <lune/Identity.hh>

#include <elle/serialize/HexadecimalArchive.hh>

#include <boost/filesystem.hpp>

#include <openssl/sha.h>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {

    namespace fs = boost::filesystem;

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
    State::send_message(std::string const& recipient_id,
                        std::string const& message)
    {
      this->_meta->send_message(recipient_id,
                                this->_me._id,
                                message);
    }

    std::string
    State::hash_password(std::string const& email,
                         std::string const& password)
    {
      std::string lower_email = email;

      std::transform(lower_email.begin(),
                     lower_email.end(),
                     lower_email.begin(),
                     ::tolower);

      unsigned char hash[SHA256_DIGEST_LENGTH];
      SHA256_CTX context;
      std::string to_hash = lower_email + "MEGABIET" + password + lower_email + "MEGABIET";

      if (SHA256_Init(&context) == 0 ||
          SHA256_Update(&context, to_hash.c_str(), to_hash.size()) == 0 ||
          SHA256_Final(hash, &context) == 0)
        throw Exception(gap_internal_error, "Cannot hash login/password");

      std::ostringstream out;
      elle::serialize::OutputHexadecimalArchive ar(out);

      ar.SaveBinary(hash, SHA256_DIGEST_LENGTH);

      return out.str();
    }
    void
    State::login(std::string const& email, std::string const& password)
    {
      this->_meta->token("");

      std::string lower_email = email;

      std::transform(lower_email.begin(),
                     lower_email.end(),
                     lower_email.begin(),
                     ::tolower);

      metrics::google::server().store("user:login:attempt");

      auto res = this->_meta->login(lower_email, password);

      metrics::google::update_id(res._id);
      metrics::google::server().store("user:login:succeed", "cs", "start");


      ELLE_DEBUG("Logged in as %s token = %s", email, res.token);

      this->_me._id = res._id;
      this->_me.fullname = res.fullname;
      this->_me.email = res.email;
      this->_me.public_key = "";

      ELLE_DEBUG("id: '%s' - fullname: '%s' - lower_email: '%s'.",
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
    }

    void
    State::logout()
    {
      // End session the session.
      metrics::google::server().store("user:logout::attempt", "cs", "end");

      if (this->_meta->token().length())
        this->_meta->logout();

      // End session the session.
      metrics::google::server().store("user:logout::succeed");
    }

    void
    State::register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password,
                     std::string const& activation_code)
    {
      // Send file request successful.
      metrics::google::server().store("user:register:attempt");

      std::string lower_email = email;

      std::transform(lower_email.begin(),
                     lower_email.end(),
                     lower_email.begin(),
                     ::tolower);

      // Logout first, and ignore errors.
      try { this->logout(); } catch (elle::HTTPException const&) {}

      this->_meta->register_(lower_email, fullname, password, activation_code);

      // Send file request successful.
      metrics::google::server().store("user:register:succeed");

      ELLE_DEBUG("Registered new user %s <%s>", fullname, lower_email);
      this->login(lower_email, password);
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

    User const&
    State::user(std::string const& id)
    {
      auto it = this->_users.find(id);
      if (it != this->_users.end())
      {
        // Search user.
        // metrics::google::server().store("Search-user", {{"cd1", "local"}, {"cd2", id}});

        return *(it->second);
      }
      auto response = this->_meta->user(id);
      std::unique_ptr<User> user{new User{
          response._id,
          response.fullname,
          response.email,
          response.public_key,
          response.status}};

      // metrics::google::server().store("Search-user", {{"cd1", "server"}, {"cd2", id}});

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

    void
    State::output_dir(std::string const& dir)
    {
      if (!fs::exists(dir))
        throw Exception(gap_error,
                        "directory doesn't exist.");

      if (!fs::is_directory(dir))
        throw Exception(gap_error,
                        "not a directroy.");

      this->_output_dir = dir;
    }

    std::string
    State::invite_user(std::string const& email)
    {
      auto response = this->_meta->invite_user(email);

      return response._id;
    }

    std::string const&
    State::output_dir() const
    {
      return this->_output_dir;
    }

  }
}
