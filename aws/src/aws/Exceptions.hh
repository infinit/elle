#ifndef AWS_EXCEPTIONS_HH
# define AWS_EXCEPTIONS_HH

#include <boost/optional.hpp>

#include <elle/Exception.hh>

#include <reactor/http/StatusCode.hh>

namespace aws
{

  /** Error on an individual AWS request.
   * Message is set with error payload if any.
   */
  class RequestError:
    public elle::Exception
  {
  public:
    RequestError(std::string const& message,
                 boost::optional<reactor::http::StatusCode> http_status = {},
                 boost::optional<std::string> const& error_code = {});

    ELLE_ATTRIBUTE_R(boost::optional<reactor::http::StatusCode>, http_status);
    ELLE_ATTRIBUTE_R(boost::optional<std::string>, error_code); // aws ErrorCode if available
  };

  /// Credentials have expired.
  class CredentialsExpired:
    public RequestError
  {
  public:
    CredentialsExpired(std::string const& message,
                       boost::optional<reactor::http::StatusCode> http_status = {},
                       boost::optional<std::string> const& error_code = {});
  };

  /// Error for an operation that can be retried.
  class TransientError:
    public RequestError
  {
  public:
    TransientError(std::string const& message,
                   boost::optional<reactor::http::StatusCode> http_status = {},
                   boost::optional<std::string> const& error_code = {}
                   );
  };

  /// Credentials not valid for action.
  class CredentialsNotValid:
    public RequestError
  {
  public:
    CredentialsNotValid(std::string const& message,
                        boost::optional<reactor::http::StatusCode> http_status = {},
                        boost::optional<std::string> const& error_code = {});
  };

  /// S3 data corrupted.
  class CorruptedData:
    public RequestError
  {
  public:
    CorruptedData(std::string const& message,
                  boost::optional<reactor::http::StatusCode> http_status = {},
                  boost::optional<std::string> const& error_code = {});
  };

  // S3 file not found.
  class FileNotFound:
    public RequestError
  {
  public:
    FileNotFound(std::string const& message,
                 boost::optional<reactor::http::StatusCode> http_status = {},
                 boost::optional<std::string> const& error_code = {});
  };

  /** Error when trying to perform an AWS high-level operation.
  * May contain an inner exception of type aws::RequestError, or
  * reactor::http::RequestError.
  */
  class AWSException:
    public elle::Exception
  {
  public:
    typedef elle::Exception Super;
    AWSException(std::string const& operation,
                 std::string const& url,
                 unsigned int attempt,
                 std::unique_ptr<elle::Exception> inner);
  private:
    // Attempt number
    ELLE_ATTRIBUTE_R(unsigned int, attempt);
    // high-level operation associated with this error
    ELLE_ATTRIBUTE_R(std::string, operation);
    // S3 url of the operation
    ELLE_ATTRIBUTE_R(std::string, url);
  };
}

#endif
