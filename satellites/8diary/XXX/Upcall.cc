//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [tue jun 28 15:02:54 2011]
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

      // set the operation.
      this->operation = operation;

      leave();
    }

    ///
    /// this method specifies the upcall's result and serializes it
    /// right away.
    ///
    elle::Status	Upcall::Result(const elle::Integer32	result)
    {
      enter();

      // set the result;
      this->result = result;

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

      std::cout << alignment << "[Upcall]" << std::endl;

      // display the operation.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Operation] " << this->operation
		<< std::endl;

      // display the inputs.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Inputs]" << std::endl;

      if (this->inputs.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the archive");

      // display the outputs.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Inputs]" << std::endl;

      if (this->inputs.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the archive");

      // display the result.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Result] " << this->operation << std::endl;

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
      if (archive.Serialize(static_cast<elle::Natural32>(this->operation),
			    this->inputs,
			    this->outputs,
			    this->result) == elle::StatusError)
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
      if (archive.Extract(reinterpret_cast<elle::Natural32&>(this->operation),
			  this->inputs,
			  this->outputs,
			  this->result) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
