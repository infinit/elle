#ifndef ELLE_FORMAT_HEXADECIMAL_HXX
# define ELLE_FORMAT_HEXADECIMAL_HXX

# include <elle/assert.hh>

namespace elle
{
  namespace format
  {
    namespace hexadecimal
    {
      /*----------.
      | Functions |
      `----------*/

      template <typename T>
      String
      encode(T const& buffer)
      {
        String string;

        encode(buffer, string);

        return (string);
      }

      template <typename T>
      void
      encode(T const& buffer,
             String& string)
      {
        static char const* chars = "0123456789abcdef";
        if (buffer.size() == 0)
          return;
        Size i = string.size();
        string.resize(i + buffer.size() * 2);
        Byte const* ptr = buffer.contents();
        Byte const* end = ptr + buffer.size();
        while (ptr != end)
          {
            Byte c = *ptr++;
            string[i++] = chars[(c >> 4) & 0xf];
            string[i++] = chars[c & 0xf];
          }
        ELLE_ASSERT(string.length() == i);
      }
    }
  }
}

#endif
