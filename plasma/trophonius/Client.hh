#ifndef PLASMA_TROPHONIUS_CLIENT_HH
# define PLASMA_TROPHONIUS_CLIENT_HH

# include <plasma/plasma.hh>

# include <elle/HttpClient.hh>
# include <elle/serialize/JSONArchive.hh>

# include <boost/system/error_code.hpp>

# include <functional>
# include <queue>

namespace plasma
{
  namespace trophonius
  {
    enum class NotificationType: int
    {
# define NOTIFICATION_TYPE(name, value)         \
      name = value,
# include <oracle/disciples/meta/notification_type.hh.inc>
# undef NOTIFICATION_TYPE
    };

    enum class NetworkUpdate: int
    {
# define NETWORK_UPDATE(name, value)         \
      name = value,
# include <oracle/disciples/meta/resources/network_update.hh.inc>
# undef NETWORK_UPDATE
    };

    /// Base class for all notifications.
    struct Notification
    {
      NotificationType notification_type;
    };

    namespace json = elle::format::json;

    struct UserStatusNotification:
      public Notification
    {
      std::string user_id;
      int         status;
    };

    struct TransactionNotification:
      public Notification
    {
      Transaction transaction;
    };

    struct TransactionStatusNotification:
      public Notification
    {
      std::string transaction_id;
      int         status;
    };

    struct NetworkUpdateNotification:
      public Notification
    {
      std::string network_id;
      /* NetworkUpdate */ int what;
    };

    struct MessageNotification:
      public Notification
    {
      std::string sender_id;
      std::string message;
    };

    class Client:
      private boost::noncopyable
    {
    public:
      struct Impl;
      Impl* _impl;

      Client(std::string const& server,
             uint16_t port,
             bool check_error = true);

      ~Client();

    public:
      bool
      connect(std::string const& _id,
              std::string const& token);

      //GenericNotification
      std::unique_ptr<Notification>
      poll();

      bool
      has_notification(void);

    private:
      std::queue<std::unique_ptr<Notification>> _notifications;

      void
      _read_socket();

      void
      _on_read_socket(boost::system::error_code const& err,
                      size_t bytes_transferred);
    };
  }
}

#undef _PLASMA_TROPHONIUS_GENERATE_HANDLERS

#endif
