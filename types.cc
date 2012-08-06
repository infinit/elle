#include <cstring>
#include <iostream>

#include <elle/types.hh>

namespace elle
{
  Null const none = Null::Nil;


  std::ostream& operator <<(std::ostream& stream, elle::Null)
  {
    return stream << "(nil)";
  }

  std::ostream& operator <<(std::ostream& stream, elle::Large const& element)
  {
    static const elle::Natural32        Length = 50;
    char*                               hexadecimal;

    // transform the number into hexadecimal.
    hexadecimal = ::BN_bn2hex(&element);

    // display the string, depending on its length.
    if (::strlen(hexadecimal) < Length)
      {
        // if the string is short enough, display it in its entirety.
        stream << hexadecimal;
      }
    else
      {
        elle::String string(hexadecimal);

        // otherwise chop it and display the begining and the end only.
        stream << string.substr(0, Length / 2) << "..."
               << string.substr(string.length() - (Length / 2));
      }

    // free the hexadecimal.
    ::OPENSSL_free(hexadecimal);

    return (stream);
  }
}
