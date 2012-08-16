# include "hexadecimal.hh"

namespace elle
{
  namespace format
  {
    namespace hexadecimal
    {

      std::string
      encode(char const* binary_data,
             size_t size)
      {
        std::string result;
        encode(binary_data, size, result);
        return result;
      }

      void
      encode(char const* binary_data,
             size_t size,
             std::string& hexadecimal_string)
      {
        static constexpr char const* chars = "0123456789abcdef";
        if (size == 0)
          return;
        assert(binary_data != nullptr);
        size_t i = hexadecimal_string.size();
        hexadecimal_string.resize(i + size * 2);
        Byte const* ptr = reinterpret_cast<Byte const*>(binary_data);
        Byte const* end = ptr + size;
        while (ptr != end)
          {
            Byte c = *ptr++;
            hexadecimal_string[i++] = chars[(c >> 4) & 0xf];
            hexadecimal_string[i++] = chars[c & 0xf];
          }
        assert(hexadecimal_string.size() == i);
      }

#define HEX2VAL(c)                                                            \
(c >= '0' && c <= '9' ? c - '0' : (c >= 'a' && c <= 'f' ? c - 'a' + 10 : -1)) \
  /**/
      void
      decode(std::string const& hexadecimal_string,
             elle::utility::Buffer& binary_data)
      {
        char const* src = hexadecimal_string.c_str();
        size_t src_size = hexadecimal_string.size();
        size_t dst_size = src_size / 2;
        assert(src_size % 2 == 0);
        Byte* dst = nullptr;
          {
            size_t old_size = binary_data.size();
            binary_data.size(old_size + dst_size);
            dst = binary_data.mutable_contents() + old_size;
          }
        assert(dst != nullptr);

        size_t i = 0, j = 0;
        while (i < src_size)
          {
            char c1 = src[i++];
            char c2 = src[i++];

            char v1 = HEX2VAL(c1);
            char v2 = HEX2VAL(c2);
            if (v1 == -1 || v2 == -1)
              throw std::runtime_error{
                  "Invalid char found in hexadecimal stream"
              };
            dst[j++] = ((v1 << 4) & 0xf0) + (v2 & 0xf);
          }
      }
#undef HEX2VAL

    }
  }
}
