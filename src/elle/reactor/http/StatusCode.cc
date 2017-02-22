#include <ostream>

#include <elle/reactor/http/StatusCode.hh>

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      std::ostream&
      operator <<(std::ostream& output,
                  StatusCode status)
      {
        switch (status)
        {
          case StatusCode::Continue:
            output << "Continue";
            break;
          case StatusCode::Switching_Protocols:
            output << "Switching Protocols";
            break;
          case StatusCode::OK:
            output << "OK";
            break;
          case StatusCode::Created:
            output << "Created";
            break;
          case StatusCode::Accepted:
            output << "Accepted";
            break;
          case StatusCode::Non_Authoritative_Information:
            output << "Non Authoritative Information";
            break;
          case StatusCode::No_Content:
            output << "No Content";
            break;
          case StatusCode::Reset_Content:
            output << "Reset Content";
            break;
          case StatusCode::Partial_Content:
            output << "Partial Content";
            break;
          case StatusCode::Multiple_Choices:
            output << "Multiple Choices";
            break;
          case StatusCode::Moved_Permanently:
            output << "Moved Permanently";
            break;
          case StatusCode::Found:
            output << "Found";
            break;
          case StatusCode::See_Other:
            output << "See Other";
            break;
          case StatusCode::Not_Modified:
            output << "Not Modified";
            break;
          case StatusCode::Use_Proxy:
            output << "Use Proxy";
            break;
          case StatusCode::Unused:
            output << "Unused";
            break;
          case StatusCode::Temporary_Redirect:
            output << "Temporary Redirect";
            break;
          case StatusCode::Bad_Request:
            output << "Bad Request";
            break;
          case StatusCode::Unauthorized:
            output << "Unauthorized";
            break;
          case StatusCode::Payment_Required:
            output << "Payment Required";
            break;
          case StatusCode::Forbidden:
            output << "Forbidden";
            break;
          case StatusCode::Not_Found:
            output << "Not Found";
            break;
          case StatusCode::Method_Not_Allowed:
            output << "Method Not Allowed";
            break;
          case StatusCode::Not_Acceptable:
            output << "Not Acceptable";
            break;
          case StatusCode::Proxy_Authentication_Required:
            output << "Proxy Authentication Required";
            break;
          case StatusCode::Request_Timeout:
            output << "Request Timeout";
            break;
          case StatusCode::Conflict:
            output << "Conflict";
            break;
          case StatusCode::Gone:
            output << "Gone";
            break;
          case StatusCode::Length_Required:
            output << "Length Required";
            break;
          case StatusCode::Precondition_Failed:
            output << "Precondition Failed";
            break;
          case StatusCode::Request_Entity_Too_Large:
            output << "Request Entity Too Large";
            break;
          case StatusCode::Request_URI_Too_Long:
            output << "Request-URI Too Long";
            break;
          case StatusCode::Unsupported_Media_Type:
            output << "Unsupported Media Type";
            break;
          case StatusCode::Requested_Range_Not_Satisfiable:
            output << "Requested Range Not Satisfiable";
            break;
          case StatusCode::Expectation_Failed:
            output << "Expectation Failed";
            break;
          case StatusCode::Internal_Server_Error:
            output << "Internal Server Error";
            break;
          case StatusCode::Not_Implemented:
            output << "Not Implemented";
            break;
          case StatusCode::Bad_Gateway:
            output << "Bad Gateway";
            break;
          case StatusCode::Service_Unavailable:
            output << "Service Unavailable";
            break;
          case StatusCode::Gateway_Timeout:
            output << "Gateway Timeout";
            break;
          case StatusCode::HTTP_Version_Not_Supported:
            output << "HTTP Version Not Supported";
            break;
          default:
            output << "Unknown(" << (int)status << ")";
        }
        return output;
      }
    }
  }
}

namespace std
{
  std::size_t
  hash<elle::reactor::http::StatusCode>::operator()(
    elle::reactor::http::StatusCode const& code) const
  {
    return (std::size_t) code;
  }
}
