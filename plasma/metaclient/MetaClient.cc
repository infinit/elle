//
// ---------- header ----------------------------------------------------------
//
// project       plasma/metaclient
//
// license       infinit
//
// author        Raphaël Londeix   [Tue 14 Feb 2012 04:17:19 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <cassert>
#include <iostream>
#include <stdexcept>

#include <QJson/Parser>
#include <QJson/Serializer>

#include "plasma/common/resources.hh"

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
              this->errback(MetaClient::Error::InvalidContent, "Cannot parse json data");
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
          }

        // Calling the callback with the response
        this->callback(response);
      }

      virtual void OnNetworkError(QNetworkReply::NetworkError)
      {
        // XXX fine grained network errors
        if (this->errback != nullptr)
          this->errback(MetaClient::Error::ConnectionFailure, "Network error");
      }
    };

///
/// Helper macro to focus on deserialize response
/// It handles the server error
///
#define FILLER(cls)                                                         \
    struct cls##Filler                                                      \
    {                                                                       \
      static void Fill(QVariantMap const& map, cls& response)               \
      {                                                                     \
        if (map.contains("success"))                                        \
          {                                                                 \
            response.success = map["success"].toBool();                     \
            if (!response.success)                                          \
              {                                                             \
                response.error = _GetString(map, "error");                  \
                return ;                                                    \
              }                                                             \
          }                                                                 \
        return _Fill(map, response);                                        \
      }                                                                     \
                                                                            \
    protected:                                                              \
      static std::string _GetString(QVariantMap const& map,                 \
                                    char const* key,                        \
                                    char const* default_value)              \
      {                                                                     \
        if (!map.contains(key))                                             \
          return default_value;                                             \
        return _GetString(map, key);                                        \
      }                                                                     \
                                                                            \
      static std::string _GetString(QVariantMap const& map, char const* key)\
      {                                                                     \
        if (!map.contains(key))                                             \
          throw std::runtime_error("KeyError: " + std::string(key));        \
        return map[key].toString().toStdString();                           \
      }                                                                     \
                                                                            \
      static std::string _GetNonEmptyString(QVariantMap const& map,         \
                                            char const* key)                \
        {                                                                   \
          std::string val{_GetString(map, key)};                            \
          if (val.empty())                                                  \
            throw std::runtime_error(                                       \
                "ValueError: map[\"" + std::string(key) + "\"] is emtpy"    \
            );                                                              \
          return val;                                                       \
        }                                                                   \
      static void _Fill(QVariantMap const&, cls&);                          \
    };                                                                      \
    void cls##Filler::_Fill(QVariantMap const& map, cls& response)          \

/////////////////////////////////////////////////////////////////////////////
// Response deserializers defined here

    FILLER(LoginResponse)
    {
      response.token =    _GetNonEmptyString(map, "token");
      response.fullname = _GetNonEmptyString(map, "fullname");
      response.email =    _GetNonEmptyString(map, "email");
      response.identity = _GetNonEmptyString(map, "identity");
    }

    FILLER(NetworksResponse)
    {
      auto networks = map["networks"].toList();
      auto it = networks.begin(), end = networks.end();
      for (; it != end; ++it)
        {
          response.networks.push_back((*it).toString().toStdString());
        }
    }

#undef FILLER

///
/// Typedef for specific request handler
///
#define HANDLER_TYPEDEF(cls)                                                 \
    typedef RequestHandler<cls, cls##Filler> cls##Handler

//////////////////////////////////////////////////////////////////////////////
// Requests handlers defined here

    HANDLER_TYPEDEF(LoginResponse);
    HANDLER_TYPEDEF(NetworksResponse);

#undef HANDLER_TYPEDEF
}

//
// ---------- contructors & descructors ---------------------------------------
//

MetaClient::MetaClient(QApplication& app) :
  _network(&app)
{
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
  this->_Post("/login", req, new LoginResponseHandler(callback, errback));
}

void MetaClient::GetNetworks(NetworksCallback callback,
                             Errback errback)
{
  this->_Get("/networks", new NetworksResponseHandler(callback, errback));
}

void MetaClient::_Post(std::string const& url,
                       QVariantMap const& data,
                       RequestHandler* handler)
{
  QString uri((INFINIT_META_URL + url).c_str());

  QByteArray json = QJson::Serializer().serialize(data);
  QNetworkRequest request{QUrl{uri}};
  if (this->_token.size() > 0)
    request.setRawHeader("Authorization", this->_token);
  request.setRawHeader("Content-Type", "application/json");
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
  QString uri((INFINIT_META_URL + url).c_str());
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
  assert(reply != nullptr);
  auto it = this->_handlers.find(reply);
  if (it != this->_handlers.end())
    {
      RequestHandler* handler = it->second;
      assert(handler != nullptr && "Corrupter map");
      this->_handlers.erase(it); // WARNING do not use it from here
      if (reply->error() == QNetworkReply::NoError)
        handler->OnReply(reply->readAll());
      else
        handler->OnNetworkError(reply->error());
      delete handler;
    }
  else
    {
      std::cerr << "Cannot find any registered handler for this reply\n";
    }
  reply->deleteLater();
}
