//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/OneWay.cc
//
// created       julien quintard   [mon oct 29 13:26:52 2007]
// updated       julien quintard   [sun aug 23 17:28:35 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/OneWay.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace crypto
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant represents the algorithm used by the OneWay::Hash()
    /// static method.
    ///
    const ::EVP_MD*		OneWay::Algorithm = ::EVP_sha256();

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method hashes the given plain, returning a digest object.
    ///
    Status		OneWay::Hash(const Plain&		plain,
				     Digest&			digest)
    {
      ::EVP_MD_CTX	context;

      // allocate the digest's contents.
      if (digest.region.Prepare(EVP_MD_size(OneWay::Algorithm)) == StatusError)
	escape("unable to allocate memory for the digest");

      // initialise the context.
      ::EVP_MD_CTX_init(&context);

      // initialise the digest.
      if (::EVP_DigestInit_ex(&context, OneWay::Algorithm, NULL) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the digest with the given data.
      if (::EVP_DigestUpdate(&context,
			     plain.contents,
			     plain.size) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // finalise the digest.
      if (::EVP_DigestFinal_ex(&context,
			       (unsigned char*)digest.region.contents,
			       (unsigned int*)&digest.region.size) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // clean the context.
      if (::EVP_MD_CTX_cleanup(&context) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

  }
}
