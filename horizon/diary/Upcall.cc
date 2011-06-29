//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Upcall.cc
//
// created       julien quintard   [tue jun 28 15:02:54 2011]
// updated       julien quintard   [wed jun 29 18:05:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Upcall.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the upcall by initializing the archive.
    ///
    elle::Status	Upcall::Create(const Operation		operation)
    {
      enter();

      printf("Upcall: %u\n", operation);

      // create the archive.
      if (this->archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the operation code.
      if (this->archive.Serialize((const elle::Natural32&)operation) ==
	  elle::StatusError)
	escape("unable to serialize the operation");

      leave();
    }

    ///
    /// this method specifies the upcall's result and serializes it
    /// right away.
    ///
    elle::Status	Upcall::Result(const elle::Natural32	result)
    {
      enter();

      // serializes the result.
      if (this->archive.Serialize(result) == elle::StatusError)
	escape("unable to serialize the result");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Upcall, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an diary object.
    ///
    elle::Status	Upcall::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      // display the name.
      std::cout << alignment << "[Upcall]" << std::endl;

      // dump the archive.
      if (this->archive.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the archive");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the diary object.
    ///
    elle::Status	Upcall::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->archive) == elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the diary object.
    ///
    elle::Status	Upcall::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->archive) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
