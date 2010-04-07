//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Set.hh
//
// created       julien quintard   [wed mar 31 23:32:06 2010]
// updated       julien quintard   [tue apr  6 15:44:19 2010]
//

#ifndef ETOILE_KERNEL_SET_HH
#define ETOILE_KERNEL_SET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Entry.hh>
#include <etoile/kernel/Size.hh>

#include <etoile/path/Slice.hh>

#include <list>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a subset of access control list.
    ///
    class Set:
      public Dumpable, public Archivable
    {
    public:
      typedef std::list<Entry*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      ~Set();

      //
      // methods
      //
      Status		Add(Entry*);
      Status		Exist(const path::Slice&);
      Status		Lookup(const path::Slice&,
			       Entry*&);
      Status		Remove(const path::Slice&);
      Status		Capacity(Size&) const;

      Status		Locate(const path::Slice&,
			       Iterator* = NULL);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Container		container;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Index.hh>

#endif
