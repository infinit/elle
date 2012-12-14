#include <elle/log.hh>
#include <elle/serialize/JSONArchive.hh>
#include <elle/format/json/Dictionary.hxx>
#include <elle/format/json/Parser.hh>
#include <elle/serialize/ListSerializer.hxx>
#include <elle/serialize/Serializer.hh>
#include <elle/serialize/NamedValue.hh>

#include <surface/gap/gap.h>

#include "Client.hh"

#include <iostream>
#include <fstream>

#include <elle/idiom/Close.hh>

#include <elle/print.hh>
#include <elle/assert.hh>

#include <fcntl.h>

ELLE_LOG_COMPONENT("infinit.plasma.trophonius.Client");

//- Notification serializers --------------------------------------------------

#define XXX_UGLY_SERIALIZATION_FOR_NOTIFICATION_TYPE()      \
  int* n = (int*) &value;                                   \
  ar & named("notification_type", *n)                       \
  /**/

ELLE_SERIALIZE_NO_FORMAT(plasma::trophonius::Notification);
ELLE_SERIALIZE_SIMPLE(plasma::trophonius::Notification, ar, value, version)
{
  enforce(version == 0);

  XXX_UGLY_SERIALIZATION_FOR_NOTIFICATION_TYPE();
}

ELLE_SERIALIZE_NO_FORMAT(plasma::trophonius::UserStatusNotification);
ELLE_SERIALIZE_SIMPLE(plasma::trophonius::UserStatusNotification, ar, value, version)
{
  enforce(version == 0);

  //ar & base_class<plasma::trophonius::Notification>(value);
  XXX_UGLY_SERIALIZATION_FOR_NOTIFICATION_TYPE();
  ar & named("user_id", value.user_id);
  ar & named("status", value.status);
}

ELLE_SERIALIZE_NO_FORMAT(plasma::trophonius::TransactionNotification);
ELLE_SERIALIZE_SIMPLE(plasma::trophonius::TransactionNotification, ar, value, version)
{
  enforce(version == 0);

  //ar & base_class<plasma::trophonius::Notification>(value);
  XXX_UGLY_SERIALIZATION_FOR_NOTIFICATION_TYPE();
  ar & named("transaction", value.transaction);
}

ELLE_SERIALIZE_NO_FORMAT(plasma::trophonius::TransactionStatusNotification);
ELLE_SERIALIZE_SIMPLE(plasma::trophonius::TransactionStatusNotification, ar, value, version)
{
  enforce(version == 0);

  //ar & base_class<plasma::trophonius::Notification>(value);
  XXX_UGLY_SERIALIZATION_FOR_NOTIFICATION_TYPE();
  ar & named("transaction_id", value.transaction_id);
  ar & named("status", value.status);
}

ELLE_SERIALIZE_NO_FORMAT(plasma::trophonius::MessageNotification);
ELLE_SERIALIZE_SIMPLE(plasma::trophonius::MessageNotification, ar, value, version)
{
  enforce(version == 0);

  //ar & base_class<plasma::trophonius::Notification>(value);
  XXX_UGLY_SERIALIZATION_FOR_NOTIFICATION_TYPE();
  ar & named("sender_id", value.sender_id);
  ar & named("message", value.message);
}

namespace plasma
{
  namespace trophonius
  {

    //- Implementation --------------------------------------------------------
    struct Client::Impl
    {
      boost::asio::io_service io_service;

      /// The connexion to the server
      boost::asio::ip::tcp::socket socket;
      std::string             server;
      uint16_t                port;
      bool                    check_errors;
      boost::asio::streambuf request;
      boost::asio::streambuf response;

      Impl(std::string const& server,
           uint16_t port,
           bool check_errors)
        : io_service{}
        , socket{io_service}
        , server{server}
        , port{port}
        , check_errors{check_errors}
        , request{}  // Use once to initiate connection.
        , response{}
      {}
    };

    Client::Client(std::string const& server,
                   uint16_t port,
                   bool check_errors)
      : _impl{new Impl{server, port, check_errors}}
    {
      typedef boost::asio::ip::tcp tcp;
      // Resolve the host name into an IP address.
      tcp::resolver resolver(_impl->io_service);
      tcp::resolver::query query(_impl->server, elle::sprint(_impl->port));
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

      // Start connect operation.
      _impl->socket.connect(*endpoint_iterator);
      //_impl->socket.non_blocking(true);
      ::fcntl(_impl->socket.native_handle(), F_SETFD, 1);
    }

    Client::~Client()
    {
      delete _impl;
      _impl = nullptr;
    }

    void Client::_on_read_socket(boost::system::error_code const& err,
                                 size_t bytes_transferred)
    {
      ELLE_DEBUG_SCOPE("Read %s bytes from the socket (%s available)",
                       bytes_transferred,
                       _impl->response.in_avail());
      if (err || bytes_transferred == 0)
        {
          ELLE_WARN("Something went wrong while reading from socket: %s", err);
          return;
        }

      try
        {
          ELLE_DEBUG("Recieved stream from trophonius.");

          // Bind stream to response.
          std::istream is(&(_impl->response));

          // Transfer socket stream to stringstream that ensure there are no
          // encoding troubles (and make the stream human readable).
          std::unique_ptr<char[]> data{new char[bytes_transferred]};
          if (!data)
            throw std::bad_alloc{};
          is.read(data.get(), bytes_transferred);
          std::string msg{data.get(), bytes_transferred};
          ELLE_DEBUG("Got message: %s", msg);

          plasma::trophonius::NotificationType notification_type =
            plasma::trophonius::NotificationType::none; // Invalid notification type.

          {
            std::stringstream ss{msg};

            Notification notification;
            elle::serialize::InputJSONArchive ar(ss, notification);
            notification_type = notification.notification_type;
          }

          std::unique_ptr<Notification> notification;
          {
            std::stringstream ss{msg};
            elle::serialize::InputJSONArchive ar{ss};
            switch (notification_type)
              {
              case NotificationType::user_status:
                notification = std::move(ar.Construct<UserStatusNotification>());
                break;
              case NotificationType::transaction:
                notification = std::move(ar.Construct<TransactionNotification>());
                break;
              case NotificationType::transaction_status:
                notification = std::move(ar.Construct<TransactionStatusNotification>());
                break;
              case NotificationType::message:
                notification = std::move(ar.Construct<MessageNotification>());
                break;
              case NotificationType::connection_enabled:
                notification = std::move(ar.Construct<Notification>());
                break;
              default:
                throw elle::Exception{
                    "Unknown notification type %s", notification_type
                };
              }
          }

          ELLE_ASSERT(notification != nullptr);

          this->_notifications.push(std::move(notification));

          this->_read_socket();
        }
      catch (std::runtime_error const& err)
        {
          throw elle::HTTPException{
            elle::ResponseCode::bad_content, err.what()
          };
        }
    }

    void
    Client::_read_socket()
    {
      boost::asio::async_read_until(
        _impl->socket, _impl->response, "\n",
        std::bind(
          &Client::_on_read_socket, this,
          std::placeholders::_1, std::placeholders::_2
        )
      );
    }

    bool
    Client::connect(std::string const& _id,
                    std::string const& token)
    {
      json::Dictionary connection_request{std::map<std::string, std::string>{
        {"_id", _id},
        {"token", token},
      }};

      std::ostream request_stream(&_impl->request);

      // May raise an exception.
      elle::serialize::OutputJSONArchive(request_stream, connection_request);

      // Add '\n' to request.
      request_stream << std::endl;

      boost::system::error_code err;

      boost::asio::write(
        _impl->socket,
        _impl->request,
        err
      );

      if (err)
        throw elle::HTTPException(elle::ResponseCode::error, "Writting socket error");

      this->_read_socket();
      return true;
    }

    std::unique_ptr<Notification>
    Client::poll()
    {
      // Check socket and insert notification dictionary in the queue if any.
      //_read_socket();

      if (_notifications.empty())
        _impl->io_service.poll_one();

      std::unique_ptr<Notification> ret;

      if (!_notifications.empty())
        {
          ELLE_TRACE("Pop notification dictionnary to be handle.");

          // Fill dictionary.
          ret.reset(_notifications.front().release());
          _notifications.pop();
        }

      return ret;
    }

    bool
    Client::has_notification(void)
    {
      return !(_notifications.empty());
    }

  }
}
