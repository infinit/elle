//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 15:00:55 2011]
//

#ifndef DIARY_UNIX_UPCALL_HXX
#define DIARY_UNIX_UPCALL_HXX

namespace satellite
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
    elle::Status        Upcall::Inputs(const T&...              inputs)
    {
      // create the archive.
      if (this->inputs.Create() == elle::Status::Error)
        escape("unable to create the archive");

      // serialize the inputs.
      if (this->inputs.Serialize(inputs...) == elle::Status::Error)
        escape("unable to serialize the inputs");

      return elle::Status::Ok;
    }

    ///
    /// this method takes the upcall's outputs and serializes them
    /// right away.
    ///
    template <typename... T>
    elle::Status        Upcall::Outputs(const T&...             outputs)
    {
      // create the archive.
      if (this->outputs.Create() == elle::Status::Error)
        escape("unable to create the archive");

      // serialize the outputs.
      if (this->outputs.Serialize(outputs...) == elle::Status::Error)
        escape("unable to serialize the outputs");

      return elle::Status::Ok;
    }

  }
}

#endif
