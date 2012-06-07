#ifndef DIARY_UNIX_UPCALL_HXX
# define DIARY_UNIX_UPCALL_HXX

# include <elle/serialize/TupleSerializer.hxx>
# include <elle/serialize/BufferArchive.hh>
# include <elle/utility/Buffer.hh>

# include <elle/idiom/Open.hh>

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
          this->_inputs.Writer() << std::tuple<T const&...>(inputs...);
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
          this->_outputs.Writer() << std::tuple<T const&...>(outputs...);
        }
      catch (std::exception const& err)
        {
          escape("Cannot serialize outputs: %s", err.what());
        }
      return elle::Status::Ok;
    }

    template<typename... T>
      elle::Status    Upcall::ExtractInputs(T&... inputs)
      {
        try
          {
            auto tuple = std::tuple<T&...>(inputs...);
            this->_inputs.Reader() >> tuple;
          }
        catch (std::exception const& err)
          {
            escape("Cannot extract inputs: %s", err.what());
          }

        return elle::Status::Ok;
      }

    template<typename... T>
      elle::Status    Upcall::ExtractOutputs(T&... outputs)
      {
        try
          {
            auto tuple = std::tuple<T&...>(outputs...);
            this->_outputs.Reader() >> tuple;
          }
        catch (std::exception const& err)
          {
            escape("Cannot extract outputs: %s", err.what());
          }

        return elle::Status::Ok;
      }
  }
}

#endif
#ifndef  SATELLITES_DIARY_UNIX_UPCALLSERIALIZER_HXX
# define SATELLITES_DIARY_UNIX_UPCALLSERIALIZER_HXX

# include <cassert>

# include <elle/utility/Buffer.hh>

# include <satellites/diary/unix/Upcall.hh>

ELLE_SERIALIZE_SIMPLE(satellite::unix::Upcall,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.operation;
  archive & value.result;
  archive & value._inputs;
  archive & value._outputs;
}

#endif
