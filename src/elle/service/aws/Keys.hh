#pragma once

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      using RequestTime = boost::posix_time::ptime;
      using RequestHeaders = std::map<std::string, std::string>;
      using RequestQuery = std::map<std::string, std::string>;

      /// AWS request type.
      enum class RequestType
      {
        aws4,
      };

      std::ostream&
      operator << (std::ostream& stream, RequestType request_type);

      /// AWS services.
      enum class Service
      {
        iam,
        s3,
        sts,
      };

      std::ostream&
      operator << (std::ostream& stream, Service service);

      /// AWS signing method.
      enum class SigningMethod
      {
        aws4_hmac_sha256,
      };

      std::ostream&
      operator << (std::ostream& stream, SigningMethod signing_method);
    }
  }
}
