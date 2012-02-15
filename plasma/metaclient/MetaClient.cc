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
struct MetaClient::Request
{
  virtual void OnReply(QString const&) = 0;
  virtual void OnNetworkError(QNetworkReply::NetworkError) = 0;
};

namespace {

    ///
    /// Generic response handler
    /// will call bool Deserializer::Fill(QVariantMap const&, ResponseType&)
    /// In charge of calling callback or errback
    ///
    template<typename ResponseType, Deserializer>
    struct BaseRequest : MetaClient::Request
    {
    public:
      typedef std::function<void(ResponseType const&)> Callback;
      typedef std::function<void(MetaClient::Error)> Errback;

    public:
      Callback callback;
      Errback errback;

    public:
      BaseRequest(Callback callback, Errback errback = nullptr) :
        callback(callback), errback(errback)
      { assert(callback != nullptr && "Null callbacks not allowed"); }

      virtual void OnReply(QString const& data)
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
}

//
// ---------- contructors & descructors ---------------------------------------
//

MetaClient::MetaClient(QApplication& app) :
  _network(app)
{

}

MetaClient::~MetaClient()
{

}

//
// ---------- methods  --------------------------------------------------------
//

void MetaClient::_Get(std::string const& url, Request* req, Errback = nullptr);
{
  QString uri((INFINIT_META_URL + url).c_str());
  auto reply = this->_network_access_manager->get(QNetworkRequest(QUrl(uri)));
  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );
}


