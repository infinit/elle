#ifndef ETOILE_NEST_NEST_HH
# define ETOILE_NEST_NEST_HH

#include <nucleus/Nucleus.hh> // XXX fwd.hh
#include <XXX/Handle.hh> // XXX fwd.hh

#include <etoile/nest/Pod.hh>

#include <map>

namespace etoile
{
  namespace nest
  {

    ///
    /// XXX
    ///
    class Nest
    {
    public:
      //
      // types
      //
      struct P
      {
        typedef std::map<const nucleus::Placement, Pod*>        Container;
        typedef typename Container::iterator                    Iterator;
      };

      struct A
      {
        typedef std::map<const nucleus::Address, Pod*>          Container;
        typedef typename Container::iterator                    Iterator;
      };

      //
      // static methods
      //
      static elle::Status       Exist(const nucleus::Placement&);
      static elle::Status       Exist(const nucleus::Address&);
      static elle::Status       Insert(const nucleus::Placement&,
                                       Pod*);
      static elle::Status       Insert(const nucleus::Placement&,
                                       const nucleus::Address&,
                                       Pod*);
      static elle::Status       Retrieve(const nucleus::Placement&,
                                         Pod*&);
      static elle::Status       Retrieve(const nucleus::Address&,
                                         Pod*&);
      static elle::Status       Delete(const nucleus::Placement&);
      static elle::Status       Delete(const nucleus::Address&);

      static elle::Status       Attach(nucleus::Block*,
                                       nucleus::Handle&);
      static elle::Status       Detach(nucleus::Handle&);
      static elle::Status       Load(nucleus::Handle&);
      static elle::Status       Unload(nucleus::Handle&);

      static elle::Status       Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static P::Container       Placements;
      static A::Container       Addresses;
    };

  }
}

#endif
