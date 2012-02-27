//
// ---------- header ----------------------------------------------------------
//
// project       plasma/metaclient
//
// license       infinit
//
// author        Raphaël Londeix   [Tue 14 Feb 2012 04:15:18 PM CET]
//

#ifndef PLASMA_METCLIENT_METACLIENT_HH
# define PLASMA_METCLIENT_METACLIENT_HH

# include <functional>
# include <map>

# include <QApplication>
# include <QNetworkAccessManager>
# include <QNetworkReply>
# include <QVariantMap>

namespace plasma
{
  namespace metaclient
  {

    /// Base class for every response
    struct Response
    {
      bool success;
      std::string error;
    };

    /// Login response
    struct LoginResponse : Response
    {
      std::string token;
      std::string fullname;
      std::string email;
      std::string identity;
    };

    ///
    /// Convenient interface to the meta server
    ///
    class MetaClient : public QObject
    {
      Q_OBJECT

    public:
      enum class Error
      {
        ConnectionFailure,
        InvalidContent,
        ServerError,
      };
      typedef std::function<void(LoginResponse const&)> LoginCallback;
      typedef std::function<void(Error, std::string const&)> Errback;
    private:
      struct RequestHandler;
      typedef std::map<QNetworkReply*, RequestHandler*> HandlerMap;

    private:
      /// Network loop
      QNetworkAccessManager _network;

      /// Connection token
      std::string           _token;

      /// Current requests handlers
      HandlerMap            _handlers;

    public:
      MetaClient(QApplication& app);
      ~MetaClient();

      void Login(std::string const& email,
                 std::string const& password,
                 LoginCallback callback,
                 Errback errback = nullptr);

    private:
      void _Post(std::string const& url,
                 QVariantMap const& data,
                 RequestHandler* handler);
      void _Get(std::string const& url,
                RequestHandler* handler);

    private slots:
      void _OnRequestFinished(QNetworkReply* reply);

    /// properties
    public:
      std::string const& token() { return this->_token; }
      void token(std::string const& token) { this->_token = token; }
    };

  } // !metaclient
} // !plasma

#endif /* ! METACLIENT_HH */


