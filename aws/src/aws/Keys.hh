#ifndef AWS_KEYS_HH
# define AWS_KEYS_HH

# include <iostream>

# include <boost/date_time/posix_time/posix_time.hpp>

namespace aws
{
  typedef boost::posix_time::ptime RequestTime;
  typedef std::map<std::string, std::string> RequestHeaders;
  typedef std::map<std::string, std::string> RequestQuery;

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

  /// AWS regions.
  enum class Region
  {
    us_east_1,
  };

  std::ostream&
  operator << (std::ostream& stream, Region region);

  /// AWS signing method.
  enum class SigningMethod
  {
    aws4_hmac_sha256,
  };

  std::ostream&
  operator << (std::ostream& stream, SigningMethod signing_method);
}

#endif
