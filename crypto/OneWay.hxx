//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/OneWay.hxx
//
// created       julien quintard   [sun aug 23 17:11:22 2009]
// updated       julien quintard   [sun aug 23 17:28:28 2009]
//

#ifndef ELLE_CRYPTO_ONEWAY_HXX
#define ELLE_CRYPTO_ONEWAY_HXX

namespace elle
{
  namespace crypto
  {

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
    template <typename T1>
    Status		OneWay::Hash(const T1&			o,
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

    template <typename T1,
	      typename T2>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
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

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
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

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
				     const T4&			o4,
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

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
				     const T4&			o4,
				     const T5&			o5,
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

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
				     const T4&			o4,
				     const T5&			o5,
				     const T6&			o6,
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

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
				     const T4&			o4,
				     const T5&			o5,
				     const T6&			o6,
				     const T7&			o7,
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

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
				     const T4&			o4,
				     const T5&			o5,
				     const T6&			o6,
				     const T7&			o7,
				     const T8&			o8,
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

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		OneWay::Hash(const T1&			o1,
				     const T2&			o2,
				     const T3&			o3,
				     const T4&			o4,
				     const T5&			o5,
				     const T6&			o6,
				     const T7&			o7,
				     const T8&			o8,
				     const T9&			o9,
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

#endif
