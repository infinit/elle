#ifndef AWS_EXCEPTIONS_HH
# define AWS_EXCEPTIONS_HH

#include <elle/Exception.hh>

namespace aws
{
  /// Fatal error when trying to perform an AWS operation.
  class AWSException:
    public elle::Exception
  {
  public:
    typedef elle::Exception Super;
    AWSException(std::string const& error);
  private:
    ELLE_ATTRIBUTE_R(std::string, error);
  };

  /// Request was not completed successfully.
  class RequestError:
    public AWSException
  {
  public:
    RequestError(std::string const& error);
  };

  /// Credentials invalid.
  class CredentialsError:
    public AWSException
  {
  public:
    CredentialsError(std::string const& error);
  };
}

#endif
