#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HXX
# define ELLE_CRYPTOGRAPHY_ONEWAY_HXX

# include <elle/log.hh>

# include <elle/cryptography/Plain.hh>
# include <elle/cryptography/Digest.hh>

# include <elle/Buffer.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

    template <typename T>
    Status
    OneWay::Hash(T const&     value,
                 Digest&      digest)
    {
      ELLE_LOG_COMPONENT("elle.cryptography.OneWay");
      ELLE_TRACE_FUNCTION(value);

      try
        {
          elle::Buffer buffer;
          buffer.writer() << value;
          Plain plain_text(buffer.contents(), buffer.size());
          return OneWay::Hash(plain_text, digest);
        }
      catch (std::exception const& err)
        {
          escape("%s", err.what());
        }

      // never reached
      throw false;
    }

  }
}

#endif
