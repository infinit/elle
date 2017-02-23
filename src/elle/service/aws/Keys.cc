#include <elle/service/aws/Keys.hh>
#include <elle/assert.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {

      std::ostream&
      operator << (std::ostream& stream, RequestType request_type)
      {
        switch (request_type)
        {
          case RequestType::aws4:
            stream << "aws4_request";
            break;

          default:
            stream << "unknown AWS request type";
            elle::unreachable();
        }
        return stream;
      }

      std::ostream&
      operator << (std::ostream& stream, Service service)
      {
        switch (service)
        {
          case Service::iam:
            stream << "iam";
            break;
          case Service::s3:
            stream << "s3";
            break;
          case Service::sts:
            stream << "sts";
            break;

          default:
            stream << "unknown AWS service";
            elle::unreachable();
        }
        return stream;
      }

      std::ostream&
      operator << (std::ostream& stream, SigningMethod signing_method)
      {
        switch (signing_method)
        {
          case SigningMethod::aws4_hmac_sha256:
            stream << "AWS4-HMAC-SHA256";
            break;

          default:
            stream << "unknown AWS signing method";
            elle::unreachable();
        }
        return stream;
      }
    }
  }
}
