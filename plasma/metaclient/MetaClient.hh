#ifndef PLASMA_METCLIENT_METACLIENT_HH
# define PLASMA_METCLIENT_METACLIENT_HH

# include <functional>
# include <list>
# include <map>
# include <stdexcept> // XXX

# include <QCoreApplication>
# include <QApplication>
# include <QNetworkAccessManager>
# include <QNetworkReply>
# include <QVariantMap>

# include <elle/log.hh>
# include <elle/print.hh>

# include <plasma/common/resources.hh>

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

    struct LoginResponse : Response
    {
      std::string  token;
      std::string  fullname;
      std::string  email;
      std::string  identity;
    };

    struct RegisterResponse : Response
    {};

    struct NetworksResponse : Response
    {
      std::list<std::string> networks;
    };

    struct NetworkResponse : Response
    {
      std::string              _id;
      std::string              name;
      std::string              model;
      std::string              root_block;
      std::string              root_address;
      std::string              descriptor;
      std::list<std::string>   devices;
      std::list<std::string>   users;
    };

    struct CreateDeviceResponse : Response
    {
      std::string             created_device_id;
      std::string             passport;
    };

    struct UpdateDeviceResponse : Response
    {
      std::string             updated_device_id;
      std::string             passport;
    };

    struct UpdateNetworkResponse : Response
    {
      std::string             updated_network_id;
      std::string             descriptor;
      std::string             root_block;
      std::string             root_address;
    };

    struct NetworkNodesResponse : Response
    {
      std::string             network_id;
      std::list<std::string>  nodes;
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
        no_error = 0,
        ConnectionFailure,
        InvalidContent,
        ServerError,
        CallbackError,
      };

      /// Callbacks for API calls
      typedef std::function<void(LoginResponse const&)> LoginCallback;
      typedef std::function<void(RegisterResponse const&)> RegisterCallback;
      typedef std::function<void(NetworksResponse const&)> NetworksCallback;
      typedef std::function<void(NetworkResponse const&)> NetworkCallback;
      typedef std::function<void(CreateDeviceResponse const&)> CreateDeviceCallback;
      typedef std::function<void(UpdateDeviceResponse const&)> UpdateDeviceCallback;
      typedef std::function<void(UpdateNetworkResponse const&)> UpdateNetworkCallback;
      typedef std::function<void(NetworkNodesResponse const&)> NetworkNodesCallback;

      typedef std::function<void(Error, std::string const&)> Errback;
    public:
      struct RequestHandler;
    protected:
      typedef std::map<QNetworkReply*, RequestHandler*> HandlerMap;

    private:
      QCoreApplication*     _app;
      // Network loop
      QNetworkAccessManager _network;
      // Connection token
      QByteArray            _token;
      // Current requests handlers
      HandlerMap            _handlers;
      // server url
      std::string           _server;


    public:

      /// ctor & dtor
      MetaClient(QCoreApplication& app, std::string const& server = INFINIT_META_URL);
      MetaClient(QApplication& app, std::string const& server = INFINIT_META_URL);
      MetaClient(std::string const& server = INFINIT_META_URL);
      ~MetaClient();

      /// Each method represent an API call
      void Login(std::string const& email,
                 std::string const& password,
                 LoginCallback callback,
                 Errback errback = nullptr);

      void Register(std::string const& email,
                    std::string const& fullname,
                    std::string const& password,
                    RegisterCallback callback,
                    Errback errback = nullptr);


      void CreateDevice(std::string const& name,
                        std::string const& endpoint,
                        short port,
                        CreateDeviceCallback callback,
                        Errback errback = nullptr);

      void UpdateDevice(std::string const& _id,
                        char const* name,
                        char const* endpoint,
                        short port,
                        UpdateDeviceCallback callback,
                        Errback errback = nullptr);

      void Networks(NetworksCallback callback,
                       Errback errback = nullptr);

      void Network(std::string const& id,
                   NetworkCallback callback,
                   Errback errback = nullptr);

      void UpdateNetwork(std::string const& id,
                         std::string const* name,
                         std::list<std::string> const* users,
                         std::list<std::string> const* devices,
                         std::string const* rootBlock,
                         std::string const* rootAddress,
                         UpdateNetworkCallback callback,
                         Errback errback = nullptr);

      void NetworkNodes(std::string const& id,
                        NetworkNodesCallback cb,
                        Errback errback = nullptr);

      /// properties
      QByteArray const& token() const { return this->_token; }
      void token(QByteArray const& token) { this->_token = token; }

#define __SYNC_CALL(call)                                                     \
      template<typename... Args>                                              \
      call##Response sync_##call(Args... args)                                \
      {                                                                       \
        call##Response response;                                              \
        struct {                                                              \
            call##Response& response;                                         \
            void operator ()(call##Response const& response)                  \
            { this->response = response; }                                    \
        } callback{response};                                                 \
                                                                              \
        std::string error;                                                    \
                                                                              \
        struct {                                                              \
            std::string& string;                                              \
            void operator ()(Error error, std::string const string)           \
            {                                                                 \
              assert(string.size() > 0);                                      \
              this->string = elle::sprint(string, "code =", (int) error);     \
            }                                                                 \
        } errback{error};                                                     \
                                                                              \
        this->call(args..., callback, errback);                               \
        QObject::connect(this, SIGNAL(finished()), _app, SLOT(quit()));       \
        _app->exec();                                                         \
                                                                              \
        if (error.size())                                                     \
          {                                                                   \
            throw std::runtime_error(                                         \
              elle::sprint("Got callback error:", error)                      \
            );                                                                \
          }                                                                   \
                                                                              \
        return response;                                                      \
      }                                                                       \
  /**/
      __SYNC_CALL(Login);
      __SYNC_CALL(Register);
      __SYNC_CALL(CreateDevice);
      __SYNC_CALL(UpdateDevice);
      __SYNC_CALL(Networks);
      __SYNC_CALL(Network);
      __SYNC_CALL(UpdateNetwork);
      __SYNC_CALL(NetworkNodes);
#undef __SYNC_CALL

    private:
      void _Post(std::string const& url,
                 QVariantMap const& data,
                 RequestHandler* handler);
      void _Get(std::string const& url,
                RequestHandler* handler);

    private Q_SLOTS:
      void _OnRequestFinished(QNetworkReply* reply);

    Q_SIGNALS:
      void finished();
    };

  } // !metaclient
} // !plasma

#endif /* ! METACLIENT_HH */


