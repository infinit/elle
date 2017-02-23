#ifndef ELLE_FORMAT_BASE64URL_HXX
# define ELLE_FORMAT_BASE64URL_HXX

# include <elle/log.hh>

namespace elle
{
  namespace format
  {
    namespace base64url
    {
      template <typename T>
      T
      encode(ConstWeakBuffer input)
      {
        ELLE_LOG_COMPONENT("elle.format.base64url")
        ELLE_TRACE_SCOPE("encode %s", input);
        size_t size = base64::encoded_size(input);
        ELLE_DEBUG("previsional size: %s", size);
        T res;
        res.capacity(size);
        IOStream stream(res.ostreambuf());
        Stream base64_stream(stream);
        base64_stream.write(reinterpret_cast<char const*>(input.contents()),
                            input.size());
        return res;
      }

      template <>
      inline
      elle::String
      encode(ConstWeakBuffer input)
      {
        ELLE_LOG_COMPONENT("elle.format.base64url")
        ELLE_TRACE_SCOPE("encode %s", input);
        std::stringstream stream;
        {
          Stream base64_stream(stream);
          base64_stream.write(reinterpret_cast<char const*>(input.contents()),
                              input.size());
        }
        return stream.str();
      }

      template <typename T>
      Buffer
      decode(T input)
      {
        ELLE_LOG_COMPONENT("elle.format.base64url")
        ELLE_TRACE_SCOPE("decode %s", input);
        size_t size = base64::decoded_size(input);
        ELLE_DEBUG("previsional size: %s", size);
        Buffer res(size);
        IOStream stream(input.istreambuf());
        Stream base64_stream(stream);
        base64_stream.read(reinterpret_cast<char*>(res.mutable_contents()),
                           size);
        return res;
      }

      template <>
      inline
      Buffer
      decode(elle::String const& input)
      {
        ConstWeakBuffer buffer(input.c_str(), input.size());
        return decode(buffer);
      }
    }
  }
}

#endif
