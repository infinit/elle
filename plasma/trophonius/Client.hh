#ifndef PLASMA_TROPHONIUS_CLIENT_HH
# define PLASMA_TROPHONIUS_CLIENT_HH

# include <functional>

# include <plasma/plasma.hh>

# include <elle/HttpClient.hxx>

# include <boost/asio.hpp>

# include <elle/serialize/JSONArchive.hh>

# include <surface/gap/gap.h>

# include <queue>

// Declare a struct object_nameHandler.
// Define a type callbackPrototype.
// Override call from BasicHandler.
# define _PLASMA_TROPHONIUS_GENERATE_HANDLERS(object_name, type)             \
  public :                                                              \
    struct object_name ## Handler : plasma::trophonius::BasicHandler    \
    {                                                                   \
      struct object_name ## Notification : plasma::Notification, type   \
        {                                                               \
        };                                                              \
        typedef std::function<void (type const*)> callbackPrototype;     \
                                                                        \
        object_name ## Handler(callbackPrototype const& callback)       \
        {                                                               \
          this->callback = callback;                                    \
        }                                                               \
                                                                        \
        void                                                          \
        call(elle::format::json::Dictionary const* dic);            \
    private:                                                            \
      callbackPrototype callback;                                       \
  };                                                                    \

namespace plasma
{
  namespace trophonius
  {
    struct BasicHandler
    {
      virtual
      void
      call(elle::format::json::Dictionary const* dic) = 0;
    };

    namespace json = elle::format::json;

    class Client
    {
    public :
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(Bite, gap_Bite)

      // // Generate handler: FriendRequestHandler.
      // _PLASMA_TROPHONIUS_GENERATE_HANDLERS(FriendRequest, register_friend_struct)

      // // Generate handler: FriendRequestStatus.
      // _PLASMA_TROPHONIUS_GENERATE_HANDLERS(FriendRequestStatus, register_friend_status_struct)

      // Generate handler: FileTransferHandler.
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(FileTransfer, gap_FileTransfer)

      // Generate handler: FileTransferStatusHandler
      _PLASMA_TROPHONIUS_GENERATE_HANDLERS(FileTransferStatus, gap_FileTransferStatus)

      // // Generate handler: RefreshFriendsHandler.
      // _PLASMA_TROPHONIUS_GENERATE_HANDLERS(RefreshFriends)

    public:
      struct Impl;
      Impl *_impl;

      Client(std::string const& server,
             uint16_t port,
             bool check_error = true);

      private:
      void
      handle_connect(const boost::system::error_code& e,
                     boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

      void
      handle_read(const boost::system::error_code& e);

      void
      handle_write(const boost::system::error_code& e);

    public:
      bool
      connect(std::string const& _id,
              std::string const& token);

      //GenericNotification
      std::unique_ptr<json::Dictionary>
      poll();

      bool
      has_notification(void);

    private:
      std::queue<json::Dictionary*> _notifications;

      void
      _read_socket();
    };
  }
}

#undef _PLASMA_TROPHONIUS_GENERATE_HANDLERS

#endif
