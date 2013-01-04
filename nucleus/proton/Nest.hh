#ifndef NUCLEUS_PROTON_NEST_HH
# define NUCLEUS_PROTON_NEST_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Limits.hh>

# include <elle/idiom/Close.hh>
#  include <functional>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    /// XXX
    class Nest
    {
      // XXX
    public:
      Nest(Limits const& limits);
      virtual
      ~Nest();

      //
      // methods
      //
    public:
      /// XXX: ownership is transferred
      virtual
      Handle
      attach(Contents* block) = 0;
      /// XXX
      virtual
      void
      detach(Handle& handle) = 0;
      /// XXX
      virtual
      void
      load(Handle& handle) = 0;
      /// XXX
      virtual
      void
      unload(Handle& handle) = 0;
      /// XXX
      virtual
      Limits const&
      limits() const;

      //
      // attributes
      //
    private:
      nucleus::proton::Limits _limits;
    };
  }
}

#endif
