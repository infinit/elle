#pragma once

#include <iostream>

#include <elle/Duration.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      using RequestTime = elle::Time;
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
