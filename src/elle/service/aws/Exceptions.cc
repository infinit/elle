#include <elle/printf.hh>
#include <elle/service/aws/Exceptions.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      AWSException::AWSException(std::string const& operation,
                                 std::string const& url,
                                 unsigned int attempt,
                                 std::exception_ptr inner)
        : Super(elle::sprintf("AWS error: '%s' on '%s' (attempt %s): %s",
                              operation, url, attempt,
                              elle::exception_string(inner)))
        , _attempt(attempt)
        , _operation(operation)
        , _url(url)
      {
        inner_exception(inner);
      };

      RequestError::RequestError(
        std::string const& message,
        boost::optional<reactor::http::StatusCode> http_status,
        boost::optional<std::string> const& error_code)
        : elle::Exception(message)
        , _http_status(http_status)
        , _error_code(error_code)
      {}

      TransientError::TransientError(
        std::string const& message,
        boost::optional<reactor::http::StatusCode> http_status,
        boost::optional<std::string> const& error_code
        )
      : RequestError(message, http_status, error_code)
      {}

      CredentialsExpired::CredentialsExpired(
          std::string const& message,
          boost::optional<reactor::http::StatusCode> http_status,
          boost::optional<std::string> const& error_code
          )
        : RequestError(message, http_status, error_code)
      {}

      CredentialsNotValid::CredentialsNotValid(
          std::string const& message,
          boost::optional<reactor::http::StatusCode> http_status,
          boost::optional<std::string> const& error_code
          )
        : RequestError(message, http_status, error_code)
      {}

      CorruptedData::CorruptedData(
          std::string const& message,
          boost::optional<reactor::http::StatusCode> http_status,
          boost::optional<std::string> const& error_code
          )
        : RequestError(message, http_status, error_code)
      {}

      FileNotFound::FileNotFound(
          std::string const& message,
          boost::optional<reactor::http::StatusCode> http_status,
          boost::optional<std::string> const& error_code
          )
        : RequestError(message, http_status, error_code)
      {}

      TemporaryRedirect::TemporaryRedirect(
          std::string const& message,
          std::string const& redirect_host,
          boost::optional<reactor::http::StatusCode> http_status,
          boost::optional<std::string> const& error_code)
        : RequestError(message, http_status, error_code)
        , _redirect_host(redirect_host)
      {}
    }
  }
}
