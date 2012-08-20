#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HXX
# define ELLE_CRYPTOGRAPHY_ONEWAY_HXX

# include <elle/log.hh>

# include <elle/cryptography/Plain.hh>
# include <elle/cryptography/Digest.hh>

# include <elle/utility/Buffer.hh>
# include <elle/utility/BufferStream.hh>

# include <elle/serialize/BufferArchive.hh>
# include <elle/serialize/BaseArchive.hxx>

# include <elle/standalone/Report.hh>

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
      try
        {
          elle::utility::Buffer buffer;
          buffer.Writer() << value;
          Plain plain_text(buffer.Contents(), buffer.Size());
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
