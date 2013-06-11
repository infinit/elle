#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <elle/Exception.hh>
#include <elle/assert.hh>
#include <elle/format/base64.hh>

namespace elle
{
  namespace format
  {
    namespace base64
    {
      using namespace boost::archive::iterators;

      template <typename Iterator>
      void
      _encode(Buffer const& input, Iterator output)
      {
        typedef base64_from_binary<transform_width<const char *, 6, 8>> encoder;
        auto const size = input.size();
        output = std::copy(encoder(input.mutable_contents()),
                           encoder(input.mutable_contents() + size),
                           output);
        // Unfortunately, Boost does not handle base64 padding. Pad manually.
        for (signed i = 0; i < (3 - (signed(size) % 3)) % 3; ++i)
        {
          *output = '=';
          ++output;
        }
      }

      void
      encode(Buffer const& input, std::ostream& output)
      {
        return _encode(input, std::ostream_iterator<char>(output));
      }

      Buffer
      encode(Buffer const& input)
      {
        size_t size = (signed(input.size()) + 2) / 3 * 4;
        Buffer::ContentPtr content(
          reinterpret_cast<unsigned char*>(malloc(size)));
        _encode<unsigned char*>(input, content.get());
        return Buffer(Buffer::ContentPair(std::move(content), size));
      }

      static
      size_t
      _decoded_size(Buffer const& encoded)
      {
        size_t size = encoded.size();
        int padding =
          encoded.mutable_contents()[size - 2] == '=' ? 2 :
          encoded.mutable_contents()[size - 1] == '=' ? 1 : 0;
        return size / 4 * 3 - padding;
      }

      template <typename Iterator>
      void
      _decode(Buffer const& input, Iterator output)
      {
        typedef transform_width<binary_from_base64<char const *>, 8, 6> decoder;
        size_t decoded_size = _decoded_size(input);
        auto begin = decoder(input.mutable_contents());
        // Boost doesn't handle padding, we have to stop at the exact right
        // character.
        for (unsigned i = 0; i < decoded_size; ++i)
        {
          *output = *(begin++);
          ++output;
        }
      }

      void
      decode(Buffer const& input, std::ostream& output)
      {
        return _decode(input, std::ostream_iterator<char>(output));
      }

      Buffer
      decode(Buffer const& input)
      {
        Buffer res(_decoded_size(input));
        _decode(input, res.mutable_contents());
        return res;
      }
    }
  }
}
