#include "base64.hh"

#include <elle/assert.hh>
#include <elle/Exception.hh>

namespace elle
{
  namespace format
  {
    namespace base64
    {
      static char const base64_chars[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

      Buffer
      decode(std::string const& base64_string)
      {
        Buffer b;
        decode(base64_string, b);
        return b;
      }

      void
      decode(std::string const& base64_string,
             Buffer& binary_data)
      {
        unsigned char const* src =
          reinterpret_cast<unsigned char const*>(base64_string.data());
        size_t size = base64_string.size();

        size_t i = binary_data.size();
        binary_data.size(i + size * 3 / 4 + 1);
        unsigned char* dst = binary_data.mutable_contents();
        unsigned char from[4];
        while (size >= 3)
        {
          from[0] = detail::decode_char(*src++);
          from[1] = detail::decode_char(*src++);
          from[2] = detail::decode_char(*src++);
          from[3] = detail::decode_char(*src++);

          dst[i++] = (from[0] << 2) + ((from[1] & 0x30) >> 4);
          dst[i++] = ((from[1] & 0xf) << 4) + ((from[2] & 0x3c) >> 2);
          dst[i++] = ((from[2] & 0x3) << 6) + from[3];
          size -= 3;
          ELLE_ASSERT_LT(i, binary_data.size());
        }
        ELLE_ASSERT_EQ(size, 0u);
      }

      namespace detail
      {
        void
        encode(unsigned char const* in,
               size_t size,
               std::string& out)
        {
          ELLE_ASSERT(in != nullptr);

          if (size == 0)
            return;

          out.resize(out.size() + size + (size / 2) + 4);
          size_t i = out.size();

          unsigned char c1;
          unsigned char c2;
          unsigned char c3;

          while (size >= 3)
          {
            c1 = *in++;
            c2 = *in++;
            c3 = *in++;
            size -= 3;
            out[i++] = base64_chars[(c1 & 0xfc) >> 2];
            out[i++] = base64_chars[((c1 & 0x03) << 4) + ((c2 & 0xf0) >> 4)];
            out[i++] = base64_chars[((c2 & 0x0f) << 2) + ((c3 & 0xc0) >> 6)];
            out[i++] = base64_chars[c3 & 0x3f];
          }

          switch (size)
          {
          case 2:
            c1 = *in++;
            c2 = *in++;
            out[i++] = base64_chars[(c1 & 0xfc) >> 2];
            out[i++] = base64_chars[((c1 & 0x03) << 4) + ((c2 & 0xf0) >> 4)];
            out[i++] = base64_chars[(c2 & 0x0f) << 2];
            out[i++] = '=';
            break;
          case 1:
            c1 = *in++;
            out[i++] = base64_chars[(c1 & 0xfc) >> 2];
            out[i++] = base64_chars[(c1 & 0x03) << 4];
            out[i++] = '=';
            out[i++] = '=';
            break;
          case 0:
            return;
          default:
            elle::unreachable();
          }
        }

        char
        decode_char(unsigned char c)
        {
          static int8_t const reverse[256] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54,
            55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3,
            4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
            22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32,
            33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
            50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
          };

          if (c == '=')
            return 0;
          char res = reverse[c];
          if (res < 0)
            throw elle::Exception{"unexpected base64 char"};
          return res;
        }
      }
    }
  }
}
