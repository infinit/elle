#ifndef DIARY_UNIX_UPCALL_HXX
# define DIARY_UNIX_UPCALL_HXX

# include <elle/serialize/TupleSerializer.hxx>
# include <elle/serialize/BufferArchive.hh>
# include <elle/utility/Buffer.hh>

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
      try
        {
          this->inputs.Writer() << std::make_tuple(inputs...);
        }
      catch (std::exception const& err)
        {
          escape("Cannot serialize inputs: %s", err.what());
        }

      return elle::Status::Ok;
    }

    ///
    /// this method takes the upcall's outputs and serializes them
    /// right away.
    ///
    template <typename... T>
    elle::Status        Upcall::Outputs(const T&...             outputs)
    {
      try
        {
          this->outputs.Writer() << std::make_tuple(outputs...);
        }
      catch (std::exception const& err)
        {
          escape("Cannot serialize outputs: %s", err.what());
        }
      return elle::Status::Ok;
    }

  }
}

#endif
