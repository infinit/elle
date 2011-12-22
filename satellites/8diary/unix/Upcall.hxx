//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 15:00:55 2011]
//

#ifndef DIARY_UNIX_UPCALL_HXX
#define DIARY_UNIX_UPCALL_HXX

namespace application
{
#undef unix
  namespace unix
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
      enter();

      // create the archive.
      if (this->inputs.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the inputs.
      if (this->inputs.Serialize(inputs...) == elle::StatusError)
	escape("unable to serialize the inputs");

      leave();
    }

    ///
    /// this method takes the upcall's outputs and serializes them
    /// right away.
    ///
    template <typename... T>
    elle::Status	Upcall::Outputs(const T&...		outputs)
    {
      enter();

      // create the archive.
      if (this->outputs.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the outputs.
      if (this->outputs.Serialize(outputs...) == elle::StatusError)
	escape("unable to serialize the outputs");

      leave();
    }

  }
}

#endif
