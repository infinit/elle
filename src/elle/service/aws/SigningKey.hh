#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/attribute.hh>
#include <elle/Printable.hh>
#include <elle/Buffer.hh>

#include <elle/service/aws/Keys.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      class SigningKey:
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        SigningKey(std::string const& aws_secret,
                   RequestTime const& request_time,
                   std::string const& aws_region,
                   Service const& aws_service);

        std::string
        sign_message(std::string const& message);

        ELLE_ATTRIBUTE_R(elle::Buffer, key);

        /*----------.
        | Printable |
        `----------*/
      public:
        virtual
        void
        print(std::ostream& stream) const;
      };
    }
  }
}
