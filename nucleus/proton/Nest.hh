#ifndef NUCLEUS_PROTON_NEST_HH
# define NUCLEUS_PROTON_NEST_HH

# include <elle/types.hh>
# include <elle/io/Dumpable.hh>

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
    class Nest:
      public elle::io::Dumpable
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
      Handle const
      attach(Contents* block) = 0;
      /// XXX
      virtual
      void
      detach(Handle& handle) = 0;
      /// XXX
      virtual
      std::shared_ptr<Contents>
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
      // interfaces
      //
    public:
      //
      // attributes
      //
    private:
      nucleus::proton::Limits _limits;
    };

  }
}

#endif
