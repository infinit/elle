#include <cassert>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <QSslConfiguration>
#include <QJson/Parser>
#include <QJson/Serializer>

#include <elle/log.hh>

#include <plasma/common/resources.hh>

#include "MetaClient.hh"

using namespace plasma::metaclient;

///
/// Handler for network responses
///
struct MetaClient::RequestHandler
{
  virtual void OnReply(QByteArray const&) = 0;
  virtual void OnNetworkError(QNetworkReply::NetworkError) = 0;
  virtual ~RequestHandler() {}
};

namespace {

    ///
    /// Generic response handler
    /// will call bool Deserializer::Fill(QVariantMap const&, ResponseType&)
    /// In charge of calling callback or errback
    ///
    template<typename ResponseType, typename Deserializer>
    struct RequestHandler : MetaClient::RequestHandler
    {
    public:
      typedef std::function<void(ResponseType const&)> Callback;
      typedef MetaClient::Errback Errback;

    public:
      Callback callback;
      Errback errback;

    public:
      RequestHandler(Callback callback, Errback errback = nullptr) :
        callback(callback), errback(errback)
      {
        assert(callback != nullptr && "Null callbacks not allowed");
      }

      virtual void OnReply(QByteArray const& data)
      {
        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(data, &ok).toMap();

        if (!ok)
          {
            if (this->errback != nullptr)
              this->errback(
                  MetaClient::Error::InvalidContent,
                  "Cannot parse json data"
              );
            else
              std::cerr << "Cannot parse json data\n";
            return;
          }

        ResponseType response;
        // Deserialize the response
        try
          {
            Deserializer::Fill(result, response);
          }
        catch (std::exception const& error)
          {
            if (this->errback != nullptr)
              this->errback(
                  MetaClient::Error::InvalidContent,
                  "Malformed response: " + std::string(error.what())
              );
            else
              std::cerr << "Missing errback (" << error.what() << ")\n";
            return;
          }

        if (!response.success && this->errback != nullptr)
          {
            return this->errback(
                MetaClient::Error::ServerError,
                response.error
            );
          }

        // Calling the callback with the response
        try
          {
            this->callback(response);
          }
        catch (std::exception const& error)
          {
            if (this->errback != nullptr)
              this->errback(
                  MetaClient::Error::CallbackError,
                  std::string("CallbackError: ") + error.what()
              );
            else
              std::cerr << "Callback thrown: " << error.what() << std::endl;
          }
      }

      virtual void OnNetworkError(QNetworkReply::NetworkError err)
      {
        std::stringstream ss;
        ss << (int) err;

        // XXX fine grained network errors
        if (this->errback != nullptr)
          this->errback(MetaClient::Error::ConnectionFailure, "Network error: " + ss.str());
      }
    };

    /// Make life easer for fillers (tools box)
    class _BasicFiller
    {
    protected:
      static std::string _GetString(QVariantMap const& map,
                                    char const* key,
                                    char const* default_value)
      {
        if (!map.contains(key))
          return default_value;
        return _GetString(map, key);
      }

      static std::string _GetString(QVariantMap const& map, char const* key)
      {
        if (!map.contains(key))
          throw std::runtime_error("KeyError: " + std::string(key));
        return map[key].toString().toStdString();
      }

      static std::list<std::string> _GetStringList(QVariantMap const& map,
                                                   char const* key)
      {
        auto list = map[key].toList();
        auto it = list.begin(), end = list.end();
        std::list<std::string> res;
        for (; it != end; ++it)
          res.push_back((*it).toString().toStdString());
        return res;
      }

      static std::string _GetNonEmptyString(QVariantMap const& map,
                                            char const* key)
      {
        std::string val{_GetString(map, key)};
        if (val.empty())
          throw std::runtime_error(
              "ValueError: map[\"" + std::string(key) + "\"] is emtpy"
          );
        return val;
      }
    };
///
/// Helper macro to focus on deserializing the response
/// It handles the server error.
/// Note: it declares the class cls##Handler, with is the handler
/// for the response.
///
#define FILLER(cls)                                                         \
    struct cls##Filler : _BasicFiller                                       \
    {                                                                       \
      static void Fill(QVariantMap const& map, cls& response)               \
      {                                                                     \
        if (map.contains("success"))                                        \
          {                                                                 \
            response.success = map["success"].toBool();                     \
            if (!response.success)                                          \
              {                                                             \
                response.error = _GetString(map, "error", "(no error)");    \
                return ;                                                    \
              }                                                             \
          }                                                                 \
        return _Fill(map, response);                                        \
      }                                                                     \
      static void _Fill(QVariantMap const&, cls&);                          \
    };                                                                      \
    typedef RequestHandler<cls, cls##Filler> cls##Handler;                  \
    void cls##Filler::_Fill(QVariantMap const& map, cls& response)          \

/////////////////////////////////////////////////////////////////////////////
// Response deserializers defined here

    FILLER(LoginResponse)
    {
      response.token    =        _GetNonEmptyString(map, "token");
      response.fullname =     _GetNonEmptyString(map, "fullname");
      response.email    =        _GetNonEmptyString(map, "email");
      response.identity =     _GetNonEmptyString(map, "identity");
    }

    FILLER(RegisterResponse)
    {}

    FILLER(NetworksResponse)
    {
      response.networks =         _GetStringList(map, "networks");
    }

    FILLER(NetworkResponse)
    {
      response._id          =      _GetNonEmptyString(map, "_id");
      response.name         =     _GetNonEmptyString(map, "name");
      response.model        =    _GetNonEmptyString(map, "model");
      response.root_block   =       _GetString(map, "root_block");
      response.descriptor   =       _GetString(map, "descriptor");
      response.devices      =      _GetStringList(map, "devices");
      response.users        =        _GetStringList(map, "users");
    }

    FILLER(CreateDeviceResponse)
    {
      response.created_device_id = _GetNonEmptyString(map, "created_device_id");
      response.passport          =          _GetNonEmptyString(map, "passport");
    }

    FILLER(UpdateDeviceResponse)
    {
      response.updated_device_id = _GetNonEmptyString(map, "updated_device_id");
      response.passport          = _GetNonEmptyString(map, "passport");
    }

    FILLER(UpdateNetworkResponse)
    {
      response.updated_network_id = _GetNonEmptyString(map, "updated_network_id");
      response.descriptor         =             _GetString(map, "descriptor", "");
      response.root_block         =             _GetString(map, "root_block", "");
      response.root_address       =           _GetString(map, "root_address", "");
    }


    FILLER(NetworkNodesResponse)
    {
      response.network_id         =   _GetNonEmptyString(map, "network_id");
      response.nodes              =            _GetStringList(map, "nodes");
    }

#undef FILLER

} // !ns anonymous

//
// ---------- contructors & descructors ---------------------------------------
//

MetaClient::MetaClient(QCoreApplication& app, std::string const& server)
  : _app(nullptr)
  , _network(&app)
  , _token()
  , _handlers()
  , _server(server)
{
  this->connect(
      &this->_network, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(_OnRequestFinished(QNetworkReply*))
  );
}

MetaClient::MetaClient(QApplication& app, std::string const& server)
  : _app(nullptr)
  , _network(&app)
  , _token()
  , _handlers()
  , _server(server)
{
  QSslConfiguration sslconf(QSslConfiguration::defaultConfiguration());
  QSslConfiguration::setDefaultConfiguration(sslconf);
  this->connect(
      &this->_network, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(_OnRequestFinished(QNetworkReply*))
  );
}

MetaClient::MetaClient(std::string const& server)
  : _app(QCoreApplication::instance())
  , _network(_app)
  , _token()
  , _handlers()
  , _server(server)
{
  if (_app == nullptr)
    {
      static int dummy_argc = 0;
      static char* dummy_argv[] = {};
      _app = new QCoreApplication(dummy_argc, dummy_argv);
    }
  this->connect(
      &this->_network, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(_OnRequestFinished(QNetworkReply*))
  );
}

MetaClient::~MetaClient()
{
  if (this->_handlers.size() > 0)
    std::cerr << "WARNING: Client closed while there are "
              << this->_handlers.size()
              << " request pending.\n";
  // XXX _app is not deleted
}

//
// ---------- methods  --------------------------------------------------------
//

void MetaClient::Login(std::string const& email,
                       std::string const& password,
                       LoginCallback callback,
                       Errback errback)
{
  QVariantMap req;
  req.insert("email", email.c_str());
  req.insert("password", password.c_str());
  this->_Post("/user/login", req, new LoginResponseHandler(callback, errback));
}

void MetaClient::Register(std::string const& email,
                          std::string const& fullname,
                          std::string const& password,
                          RegisterCallback callback,
                          Errback errback)
{
  QVariantMap req;
  req.insert("email", email.c_str());
  req.insert("fullname", fullname.c_str());
  req.insert("password", password.c_str());
  this->_Post("/user/register", req,
              new RegisterResponseHandler(callback, errback));
}

void MetaClient::Networks(NetworksCallback callback,
                          Errback errback)
{
  this->_Get("/networks", new NetworksResponseHandler(callback, errback));
}

void MetaClient::Network(std::string const& id,
                         NetworkCallback callback,
                         Errback errback)
{
  this->_Get("/network/" + id, new NetworkResponseHandler(callback, errback));
}

void MetaClient::CreateDevice(std::string const& name,
                              std::string const& endpoint,
                              short port,
                              CreateDeviceCallback callback,
                              Errback errback)
{
  QVariantMap req;
  req.insert("name", name.c_str());
  req.insert("ip", endpoint.c_str());
  std::stringstream ss;
  ss << port;
  req.insert("port", ss.str().c_str());
  this->_Post("/devices", req, new CreateDeviceResponseHandler(callback, errback));
}

void MetaClient::UpdateDevice(std::string const& _id,
                              char const* name,
                              char const* endpoint,
                              short port,
                              UpdateDeviceCallback callback,
                              Errback errback)
{
  QVariantMap req;
  req.insert("_id", _id.c_str());
  if (name != nullptr)
    req.insert("name", name);
  if (endpoint != nullptr)
    req.insert("ip", endpoint);
  if (port != 0)
    {
      std::stringstream ss;
      ss << port;
      req.insert("port", ss.str().c_str());
    }
  this->_Post(
      "/devices",
      req,
      new UpdateDeviceResponseHandler(callback, errback)
  );
}

void MetaClient::UpdateNetwork(std::string const& id,
                               std::string const* name,
                               std::list<std::string> const* users,
                               std::list<std::string> const* devices,
                               std::string const* rootBlock,
                               std::string const* rootAddress,
                               UpdateNetworkCallback callback,
                               Errback errback)
{
  QVariantMap req;

  assert(id.size() && "You have to give an id to update a network");
  req.insert("id", id.c_str());

  if (name != nullptr)
      req.insert("name", name->c_str());

  assert(users == nullptr && "XXX TODO");

  if (devices != nullptr)
    {
      QVariantList l;

      auto it = devices->begin(),
           end = devices->end();
      for (; it != end; ++it)
        l.append(QString(it->c_str()));
      req.insert("devices", l);
    }

  assert(((rootBlock == nullptr && rootAddress == nullptr) ||
          (rootBlock != nullptr && rootAddress != nullptr)) &&
         "Give both rootBlock and rootAddress or none of them");
  if (rootBlock != nullptr)
    req.insert("root_block", rootBlock->c_str());
  if (rootAddress != nullptr)
    req.insert("root_address", rootAddress->c_str());
  this->_Post("/network/" + id, req, new UpdateNetworkResponseHandler(callback, errback));
}


void MetaClient::NetworkNodes(std::string const& id,
                              NetworkNodesCallback callback,
                              Errback errback)
{
  this->_Get(
      "/network/" + id + "/nodes",
      new NetworkNodesResponseHandler(callback, errback)
  );
}


///
/// -------- private methods --------------------------------------------------
///

void MetaClient::_Post(std::string const& url,
                       QVariantMap const& data,
                       RequestHandler* handler)
{
  QString uri((this->_server + url).c_str());

  QByteArray json = QJson::Serializer().serialize(data);
  QNetworkRequest request{QUrl{uri}};
  if (this->_token.size() > 0)
    request.setRawHeader("Authorization", this->_token);
  request.setRawHeader("Content-Type", "application/json");
  std::cout << "POST " << uri.toStdString() << "\n";
  auto reply = this->_network.post(request, json);
  if (reply != nullptr)
    {
      this->_handlers[reply] = handler;
    }
  else
    {
      std::cerr << "Cannot create the POST request !\n";
      delete handler;
    }
}

void MetaClient::_Get(std::string const& url, RequestHandler* handler)
{
  QString uri((this->_server + url).c_str());
  QNetworkRequest request{QUrl{uri}};
  if (this->_token.size() > 0)
    request.setRawHeader("Authorization", this->_token);
  request.setRawHeader("Content-Type", "application/json");
  auto reply = this->_network.get(request);
  if (reply != nullptr)
    {
      this->_handlers[reply] = handler;
    }
  else
    {
      std::cerr << "Cannot create the GET request !\n";
      delete handler;
    }
}

void MetaClient::_OnRequestFinished(QNetworkReply* reply)
{
  elle::log::debug("Request finished");
  assert(reply != nullptr);
  auto it = this->_handlers.find(reply);
  if (it != this->_handlers.end())
    {
      RequestHandler* handler = it->second;
      assert(handler != nullptr && "Corrupted map");
      this->_handlers.erase(it); // WARNING do not use 'it' from here
      try
        {
          if (reply->error() == QNetworkReply::NoError)
            handler->OnReply(reply->readAll());
          else
            handler->OnNetworkError(reply->error());
        }
      catch (std::exception const& err)
        {
          delete handler;
          reply->deleteLater();
          Q_EMIT finished();
          elle::log::error("Caught handler exception:", err.what());
        }
      delete handler;
    }
  else
    {
      std::cerr << "Cannot find any registered handler for this reply\n";
    }
  reply->deleteLater();
  Q_EMIT finished();
}
