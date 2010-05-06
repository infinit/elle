//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Phrase.cc
//
// created       julien quintard   [tue may  4 20:49:38 2010]
// updated       julien quintard   [wed may  5 00:08:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Phrase.hh>

namespace lune
{

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a phrase.
  ///
  elle::Status		Phrase::Create(const elle::String&	string)
  {
    enter();

    // assign the string.
    this->string = string;

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Phrase, _(), _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the object.
  ///
  elle::Status		Phrase::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Phrase] " << this->string << std::endl;

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status		Phrase::Serialize(elle::Archive&	archive) const
  {
    enter();

    // serialize the attributes.
    if (archive.Serialize(this->string) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Phrase::Extract(elle::Archive&		archive)
  {
    enter();

    // extract the attributes.
    if (archive.Extract(this->string) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

}
