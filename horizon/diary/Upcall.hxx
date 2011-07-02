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
// updated       julien quintard   [fri jul  1 07:35:27 2011]
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
