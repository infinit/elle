#pragma once

#include <string>

#include <elle/attribute.hh>
#include <elle/Printable.hh>

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      class EscapedString
        : public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        EscapedString(std::string const& str);

        ELLE_ATTRIBUTE_R(std::string, string);

      private:
        std::string
        _curl_format(std::string const& str);

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
