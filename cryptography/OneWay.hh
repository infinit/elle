#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HH
# define ELLE_CRYPTOGRAPHY_ONEWAY_HH

# include <elle/types.hh>

# include <elle/cryptography/fwd.hh>

namespace elle
{
  namespace cryptography
  {

    ///
    /// this class provides one-way functions.
    ///
    /// note that, although static, these methods are thread-safe.
    ///
    class OneWay
    {
    public:
      /// Hashes a plain text buffer.
      static Status Hash(Plain const&       plain,
                         Digest&            digest);

      /// Hashes an serializable object.
      template <typename T>
      static Status Hash(T const&           value,
                         Digest&            digest);

    };

  }
}

#include <elle/cryptography/OneWay.hxx>

#endif
