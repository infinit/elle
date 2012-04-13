//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon aug  8 12:59:07 2011]
//

#ifndef ETOILE_SHRUB_RIFFLE_HH
#define ETOILE_SHRUB_RIFFLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Slab.hh>

#include <elle/idiom/Close.hh>
# include <map>
# include <utility>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a riffle i.e a component of the hierarchical
    /// file system structure.
    ///
    /// riffles store the slab and corresponding location for a path's element
    /// but also a pointer to the parent directory.
    ///
    /// although it would have been easier to rely on class inheritance
    /// to prevent riffles from storing a pointer to a useless hierarchy
    /// variable, it would have been too much burden for no benefit.
    ///
    class Riffle:
      public elle::Entity
    {
    public:
      //
      // types
      //
      typedef std::pair<path::Slab, Riffle*>    Value;
      typedef std::map<path::Slab, Riffle*>     Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // constructors & destructors
      //
      Riffle();

      //
      // methods
      //
      elle::Status              Create(const path::Slab&,
                                       const nucleus::Location&,
                                       Riffle* = NULL);

      elle::Status              Resolve(const path::Slab&,
                                        Riffle*&);
      elle::Status              Update(const path::Slab&,
                                       const nucleus::Location&);
      elle::Status              Destroy(const path::Slab&);

      elle::Status              Flush();

      //
      // interfaces
      //

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0);

      //
      // attributes
      //
      path::Slab                slab;
      nucleus::Location         location;

      elle::Time                timestamp;

      Riffle*                   parent;
      Container                 children;
    };

  }
}

#endif
