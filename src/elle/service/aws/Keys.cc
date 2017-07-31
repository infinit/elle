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
          return stream << "aws4_request";
        }
        elle::unreachable();
      }

      std::ostream&
      operator << (std::ostream& stream, Service service)
      {
        switch (service)
        {
        case Service::iam:
          return stream << "iam";
        case Service::s3:
          return stream << "s3";
        case Service::sts:
          return stream << "sts";
        }
        elle::unreachable();
      }

      std::ostream&
      operator << (std::ostream& stream, SigningMethod signing_method)
      {
        switch (signing_method)
        {
        case SigningMethod::aws4_hmac_sha256:
          return stream << "AWS4-HMAC-SHA256";
        }
        elle::unreachable();
      }
    }
  }
}
