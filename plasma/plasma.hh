#ifndef PLASMA_HH
# define PLASMA_HH

# include <functional>
# include <elle/serialize/ListSerializer.hxx>
# include <stdexcept>
# include <elle/HttpClient.hh>

namespace elle
{
  namespace serialize
  {
    template<typename T>
    struct ResponseSerializer
    {
      template<typename Archive, typename Value>
      static void serialize(Archive&, Value&);
    };

    template<typename T>
    struct NotificationSerializer
    {
      template<typename Archive, typename Value>
      static void serialize(Archive&, Value&);
    };
  }
}

namespace plasma
{
  /// Base class for every response
  struct Response
  {
    bool _success;
    int response_code;
    std::string response_details;

    bool success() const
    {
      return _success;//response_code < (int) elle::ResponseCode::error;
    }
  };

  struct Notification
  {
    int notification_id; // The type of notification, according to enum.
  };
}

  /**/

#endif
