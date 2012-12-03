#ifndef PLASMA_TROPHONIUS_CLIENT_HH
# define PLASMA_TROPHONIUS_CLIENT_HH

# include <plasma/plasma.hh>

# include <elle/HttpClient.hh>
# include <elle/serialize/JSONArchive.hh>

# include <boost/system/error_code.hpp>

# include <functional>
# include <queue>


// Declare a struct object_nameHandler.
// Define a type callbackPrototype.
// Override call from BasicHandler.
//# define _PLASMA_TROPHONIUS_GENERATE_HANDLERS(__name)                   \
//  public :                                                              \
//    struct __name ## Handler:                                           \
//      public plasma::trophonius::BasicHandler                           \
//    {                                                                   \
//    public:                                                             \
//      typedef std::function<void(type_t const&)> CallbackPrototype;     \
//                                                                        \
//    public:                                                             \
//      struct Notification:                                              \
//        public plasma::Notification,                                    \
//        public type_t                                                   \
//      {};                                                               \
//                                                                        \
//    private:                                                            \
//      CallbackPrototype _callback;                                      \
//                                                                        \
//      void callback(Notification const* n)                              \
//      {                                                                 \
//        assert(this->_callback);                                        \
//        this->_callback(n);                                             \
//      }                                                                 \
//                                                                        \
//    public:                                                             \
//      __name ## Handler(CallbackPrototype const& callback)              \
//      {                                                                 \
//        this->_callback = callback;                                     \
//      }                                                                 \
//                                                                        \
//      void                                                              \
//      call(elle::format::json::Dictionary const& dic, bool _new)        \
//      {                                                                 \
//        _call(dic,                                                      \
//              std::unique_ptr<Notification>(new Notification),          \
//              _new);                                                    \
//      }                                                                 \
//                                                                        \
//    private:                                                            \
//      void                                                              \
//      _call(elle::format::json::Dictionary const& dic,                  \
//            std::unique_ptr<Notification>&& notification,               \
//            bool _new);                                                 \
//    }                                                                   \
///**/
//


namespace plasma
{
  namespace trophonius
  {
    struct BasicHandler
    {
      virtual
      void
      call(elle::format::json::Dictionary const& dic, bool _new = true) = 0;

      virtual
      ~BasicHandler()
      {}
    };

    namespace json = elle::format::json;

    /// Base class for all notifications.
    struct Notification
    {
      int notification_type;
    };

    struct UserStatusNotification:
      public Notification
    {
      std::string user_id;
      int         status;
    };

    struct TransactionNotification:
      public Notification
    {
      meta::Transaction transaction;
    };

    struct TransactionStatusNotification:
      public Notification
    {
      std::string transaction_id;
      int         status;
    };

    struct MessageNotification:
      public Notification
    {
      std::string sender_id;
      std::string message;
    };

    class Client
    {
    public :
      // Generate: LogoutHandler and LogoutNotification.
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(UserStatus);

      // Generate: FileTransferRequestHandler and FileTransferRequestNotification.
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(Transaction);

      // Generate: FileTransferStatusHandler and FileTransferStatusNotification.
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(TransactionStatus);

      // Generate: MessageHandler and MessageNotification.
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(Message);

    public:
      struct Impl;
      std::unique_ptr<Impl> _impl;

      Client(std::string const& server,
             uint16_t port,
             bool check_error = true);

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
