//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Upcall.hxx
//
// created       julien quintard   [tue jun 28 15:00:55 2011]
// updated       julien quintard   [wed jun 29 17:55:04 2011]
//

#ifndef PIG_DIARY_UPCALL_HXX
#define PIG_DIARY_UPCALL_HXX

namespace pig
{
  namespace diary
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes the upcall's inputs and serializes them
    /// right away.
    ///
    template <typename... T>
    elle::Status	Upcall::Inputs(const T&...		inputs)
    {
      elle::Archive	archive;

      enter();

      // create the archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the inputs in a specific archive.
      //
      // this way, since the number and size of the inputs differ depending
      // on the operation, the diary will still be able to access to
      // the operation code and result.
      if (archive.Serialize(inputs...) == elle::StatusError)
	escape("unable to serialize the inputs");

      // then serialize the archive within the upcall's.
      if (this->archive.Serialize(archive) == elle::StatusError)
	escape("unable to serialize the archive");

      leave();
    }

    ///
    /// this method takes the upcall's outputs and serializes them
    /// right away.
    ///
    template <typename... T>
    elle::Status	Upcall::Outputs(const T&...		outputs)
    {
      elle::Archive	archive;

      enter();

      // create the archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the outputs in a specific archive.
      //
      // this way, since the number and size of the outputs differ depending
      // on the operation, the diary will still be able to access to
      // the operation code and result.
      if (archive.Serialize(outputs...) == elle::StatusError)
	escape("unable to serialize the outputs");

      // then serialize the archive within the upcall's.
      if (this->archive.Serialize(archive) == elle::StatusError)
	escape("unable to serialize the archive");

      leave();
    }

  }
}

#endif
