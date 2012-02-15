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
# include <memory>

# include <QApplication>
# include <QNetworkAccessManager>

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
    };

    ///
    /// Convinient interface to the meta server
    ///
    class MetaClient
    {
    public:
      typedef std::function<void(LoginResponse const&)> LoginCallback;
      enum class Error
      {
        ConnectionFailure,
        InvalidContent,
        ServerError,
      };
      typedef std::function<void(Error, std::string const&)> Errback;
    private:
      struct Request;

    private:
      /// Network loop
      QNetworkAccessManager _network;

      /// Connection token
      std::string           _token;

      /// Current request
      Request*              _request;

    public:
      MetaClient(QApplication& app);
      ~MetaClient();

      void Login(std::string const& email,
                 std::string const& password,
                 LoginCallback cb
                 Errback = nullptr);

    private:
      void _Get(std::string const& url, Request* req, Errback = nullptr);

    private slot:
      void _OnDownloadError(QNetworkReply::NetworkError error);

    /// properties
    public:
      std::string const& token() { return this->_token; }
      void token(std::string const& token) { this->_token = token; }
    };

  } // !metaclient
} // !plasma

#endif /* ! METACLIENT_HH */


