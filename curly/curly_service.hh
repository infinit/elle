#ifndef CURL_SERVICE_HH
# define CURL_SERVICE_HH

# include <curly.hh>
# include <boost/asio.hpp>
# include <set>

#define throw_if_mcode(code) _throw_if_mcode(__func__, __LINE__, (code))

namespace curly
{
  class asio_request;

  class curl_multi_deleter
  {
  public:
    curl_multi_deleter() = default;
    void 
    operator ()(CURLM* ptr);
  };

  class curl_service : public boost::asio::io_service::service
  {
  private:
    std::set<asio_request*> _requests;
    std::unique_ptr<CURLM, curl_multi_deleter> _multi;
    boost::asio::deadline_timer _timer;
    int _concurrent = 0;

    static
    int
    timeout_helper(CURLM *multi,
                   long timeout_ms,
                   void *userptr);
  
    static
    int
    poll_helper(CURL *easy,
                curl_socket_t sockfd,
                int what,
                void *userptr,
                void *socket_userptr);
  
    void
    check_multi_info();
  public:
    static boost::asio::io_service::id id;
  
    template <typename T>
    void
    option(CURLMoption opt, T&& param);
  
    curl_service(boost::asio::io_service& io);

    ~curl_service() = default;
  
    void
    add(asio_request* ptr);
  
    void
    shutdown_service();
  };

}

#include <curly_service.hxx>

#endif /* end of include guard: CURL_SERVICE_HH */
