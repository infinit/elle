#ifndef ETOILE_NEST_NEST_HH
# define ETOILE_NEST_NEST_HH

# include <elle/types.hh>

# include <etoile/gear/fwd.hh>
# include <etoile/nest/Pod.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Placement.hh>
# include <nucleus/proton/Nest.hh>

# include <elle/idiom/Close.hh>
#  include <map>
# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace nest
  {

    ///
    /// XXX les adresses gardees (si presentes) sont celles des blocks loaded du network. si un block est cree (orphan) et sealed, il n'aura tjs pas d'adresse ici.
    ///
    class Nest:
      public nucleus::proton::Nest
    {
      //
      // types
      //
    public:
      struct P
      {
        typedef std::map<nucleus::proton::Placement const, Pod*> Container;
        typedef typename Container::iterator Iterator;
        typedef typename Container::const_iterator Scoutor;
      };

      struct A
      {
        typedef std::map<nucleus::proton::Address const, Pod*> Container;
        typedef typename Container::iterator Iterator;
        typedef typename Container::const_iterator Scoutor;
      };

      //
      // constructors & destructors
      //
      /// XXX
      Nest(nucleus::proton::Limits const& limits);
      /// XXX
      ~Nest();

      //
      // methods
      //
    public:
      /// XXX
      elle::Boolean
      exists(nucleus::proton::Placement const& placememt) const;
      /// XXX
      elle::Boolean
      exists(nucleus::proton::Address const& address) const;
      /// XXX
      void
      clear();
      /// XXX
      void
      record(gear::Transcript& transcript);

    private:
      /// XXX
      void
      _insert(nucleus::proton::Placement const& placement,
              Pod* pod);
      /// XXX
      void
      _insert(nucleus::proton::Placement const& placement,
              nucleus::proton::Address const& address,
              Pod* pod);
      /// XXX
      Pod*
      _retrieve(nucleus::proton::Placement const& placement) const;
      /// XXX
      Pod*
      _retrieve(nucleus::proton::Address const& address) const;
      /// XXX
      void
      _delete(nucleus::proton::Placement const& placement);
      /// XXX
      void
      _delete(nucleus::proton::Address const& address);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // nest
      nucleus::proton::Handle const
      attach(nucleus::proton::Contents* block);
      void
      detach(nucleus::proton::Handle& handle);
      std::shared_ptr<nucleus::proton::Contents>
      load(nucleus::proton::Handle& handle);
      void
      unload(nucleus::proton::Handle& handle);

      //
      // attributes
      //
    private:
      P::Container _placements;
      A::Container _addresses;
    };

  }
}

#endif
