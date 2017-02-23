#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/attribute.hh>
#include <elle/Printable.hh>
#include <elle/service/aws/Keys.hh>

namespace elle
{
  namespace service
  {
    namespace aws
    {
      class CredentialScope:
        public elle::Printable
      {
      public:
        CredentialScope(RequestTime const& request_time,
                        Service const& aws_service,
                        std::string const& aws_region);

        ELLE_ATTRIBUTE_R(std::string, credential_scope_str);

      public:
        virtual
        void
        print(std::ostream& stream) const;
      };

      class StringToSign:
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        /// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-string-to-sign.html
        StringToSign(RequestTime const& request_time,
                     CredentialScope const& credential_scope,
                     std::string const& hashed_canonical_request,
                     SigningMethod const& algorithm);

        ELLE_ATTRIBUTE_R(std::string, string);

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
