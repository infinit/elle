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

#include <Qjson/Parser>

#include "plasma/common/resources.hh"

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
    template<typename ResponseType, Deserializer>
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

        QVariantMap result = parser.parse(data, ok);

        if (!ok)
          {
            if (this->errback != nullptr)
              this->errback(MetaClient::Error::InvalidContent, "Cannot parse json data");
            return;
          }

        ResponseType response;
        if (Deserializer::Fill(result, response))
          this->callback(reponse);
        else if (this->errback != nullptr)
          this->errback(MetaClient::Error::InvalidContent, "Malformed response");
      }

      virtual void OnNetworkError(QNetworkReply::NetworkError)
      {
        // XXX fine grained network errors
        if (this->errback != nullptr)
          this->errback(MetaClient::Error::ConnectionFailure);
      }
    };

///
/// Helper macro to focus on deserialize response
/// It handles the server error
///
#define FILLER(cls)                                                         \
    struct cls#Filler                                                       \
    {                                                                       \
      static bool Fill(QVariantMap const& map, ResponseType& response)      \
      {                                                                     \
        if (!map.contains("success"))                                       \
          return false;                                                     \
        response.success = map["success"].toBool();                         \
        if (!response.success)                                              \
          {                                                                 \
            response.error = getstr(map, "error");                          \
            return true;                                                    \
          }                                                                 \
        return _Fill(map, response);                                        \
      }                                                                     \
                                                                            \
      static std::string getstr(QVariantMap const& map, char const* key)    \
      { return map[key].toString().toStdString(); }                         \
                                                                            \
      static bool _Fill(QVariantMap const&, ResponseType&);                 \
    };                                                                      \
    bool cls#Filler::_Fill(QVariantMap const& map, ResponseType& response)  \

/////////////////////////////////////////////////////////////////////////////
// Response deserializers defined here

    FILLER(LoginResponse)
    {
      if (map.contains("token") && map.contains("fullname"))
        {
          response.token = getstr(map, "token");
          response.fullname = getstr(map, "token");
          return response.token.size() > 0 && response.fullname.size() > 0;
        }
      return false;
    }


#undef FILLER

///
/// Typedef for specific request handler
///
#define HANDLER_TYPEDEF(cls)                                                 \
    typedef RequestHandler<cls#Response, cls#ResponseFiller> cls#Handler

//////////////////////////////////////////////////////////////////////////////
// Requests handlers defined here

    HANDLER_TYPEDEF(Login);

#undef HANDLER_TYPEDEF
}

//
// ---------- contructors & descructors ---------------------------------------
//

MetaClient::MetaClient(QApplication& app) :
  _network(app)
{
  this->connect(
      this->_network, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(_OnRequestFinished(QNetworkReply*))
  );
}

MetaClient::~MetaClient()
{

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
  this->_Post("/login", req, new LoginHandler(callback, errback));
}

void MetaClient::_Post(std::string const& url,
                       QVariantMap const& data,
                       RequestHandler* handler)
{
  QString uri((INFINIT_META_URL + url).c_str());

  QByteArray json = QJson::Serializer().serialize(data);
  auto reply = this->_network->post(QNetworkRequest(QUrl(uri)), json);
  this->handlers[reply] = handler;
}

void MetaClient::_Get(std::string const& url, RequestHandler* handler);
{
  QString uri((INFINIT_META_URL + url).c_str());
  auto reply = this->_network->get(QNetworkRequest(QUrl(uri)));
  this->handlers[reply] = handler;
}

void MetaClient::_OnRequestFinished(QNetworkReply* reply)
{
  assert(reply != nullptr);
  auto it = this->_handlers.find(reply);
  if (it != this->_handlers.end())
    {
      RequestHandler handler = it->second;
      assert(handler != nullptr && "Corrupter map");
      this->_handlers.remove(reply); // WARNING do not use it from here
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

