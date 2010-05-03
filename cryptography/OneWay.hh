//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/OneWay.hh
//
// created       julien quintard   [mon oct 29 13:19:49 2007]
// updated       julien quintard   [mon may  3 21:18:34 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HH
#define ELLE_CRYPTOGRAPHY_ONEWAY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Digest.hh>

#include <elle/idiom/Close.hh>
# include <openssl/evp.h>
# include <openssl/err.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace radix;

  namespace cryptography
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this class provides one-way functions.
    ///
    /// note that, although static, these methods are thread-safe.
    ///
    class OneWay
    {
    public:
      //
      // constants
      //
      static const ::EVP_MD*		Algorithm;

      //
      // static methods
      //
      static Status	Hash(const Plain&,
			     Digest&);

      //
      // forward methods
      //

      ///
      /// this method is required because the compiler, given an Archive
      /// object will call a template-based method instead of the Plain one.
      ///
      /// we do not want this especially because the template-based methods
      /// build archives and we are already receiving an archive.
      ///
      static Status	Hash(const Archive&		archive,
			     Digest&			digest)
      {
	return (OneWay::Hash((Plain&)archive, digest));
      }

      //
      // variadic templates
      //
      template <typename T1>
      static Status	Hash(const T1&,
			     Digest&);
      template <typename T1,
		typename T2>
      static Status	Hash(const T1&,
			     const T2&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     Digest&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      static Status	Hash(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     const T9&,
			     Digest&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/cryptography/OneWay.hxx>

#endif
