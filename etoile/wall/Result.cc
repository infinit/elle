//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Result.cc
//
// created       julien quintard   [sat mar 20 13:03:02 2010]
// updated       julien quintard   [sat mar 20 14:32:28 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Result.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable represents a positive result.
    ///
    const Result		Result::Ok(StatusOk);

    ///
    /// this variable represents a negative result.
    ///
    const Result		Result::Error(StatusError);

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Result::Result():
      status(StatusUnknown)
    {
    }

    ///
    /// specific constructor.
    ///
    Result::Result(const Status					status):
      status(status)
    {
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares if two objects are identical.
    ///
    /// note that for result objects, only the status is compared as
    /// the report is considered as an attachment.
    ///
    Boolean		Result::operator==(const Result&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the status.
      if (this->status != element.status)
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Result);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the result.
    ///
    Status		Result::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Result]" << std::endl;

      // dump the status.
      std::cout << alignment << shift << "[Status] "
		<< this->status << std::endl;

      // dump the report.
      if (this->report.Dump(margin + 2) == StatusError)
	escape("unable to dump the report");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the result.
    ///
    Status		Result::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize((Byte&)this->status,
			    this->report) == StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the result.
    ///
    Status		Result::Extract(Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract((Byte&)this->status,
			  this->report) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
