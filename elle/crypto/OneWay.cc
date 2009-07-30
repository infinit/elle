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
// updated       julien quintard   [thu jul 30 18:17:46 2009]
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

//
// ---------- variadic methods ------------------------------------------------
//

    ///
    /// these methods make it easier to hash multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the code is replicated in order to provide optimisation.
    /// Indeed, otherwise, everytime a single item is hashed, the whole 9-step
    /// ifs would be executed, testing if there are more than one item
    /// to hash.
    ///

    ///
    /// this method hashes an archivable object.
    ///
    Status		OneWay::Hash(const Archivable&		o,
				     Digest&			digest)
    {
      Archive		archive;

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o) == StatusError)
	escape("unable to serialize the object");

      // hash the archive.
      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the object's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     const Archivable&		o4,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     const Archivable&		o4,
				     const Archivable&		o5,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     const Archivable&		o4,
				     const Archivable&		o5,
				     const Archivable&		o6,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     const Archivable&		o4,
				     const Archivable&		o5,
				     const Archivable&		o6,
				     const Archivable&		o7,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     const Archivable&		o4,
				     const Archivable&		o5,
				     const Archivable&		o6,
				     const Archivable&		o7,
				     const Archivable&		o8,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

    Status		OneWay::Hash(const Archivable&		o1,
				     const Archivable&		o2,
				     const Archivable&		o3,
				     const Archivable&		o4,
				     const Archivable&		o5,
				     const Archivable&		o6,
				     const Archivable&		o7,
				     const Archivable&		o8,
				     const Archivable&		o9,
				     Digest&			digest)
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the items");

      if (OneWay::Hash(archive, digest) == StatusError)
	escape("unable to hash the item's archive");

      leave();
    }

  }
}
