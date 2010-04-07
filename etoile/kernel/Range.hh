//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Range.hh
//
// created       julien quintard   [wed mar 31 23:32:06 2010]
// updated       julien quintard   [mon apr  5 23:05:50 2010]
//

#ifndef ETOILE_KERNEL_RANGE_HH
#define ETOILE_KERNEL_RANGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Record.hh>
#include <etoile/kernel/Size.hh>
#include <etoile/kernel/Index.hh>

#include <list>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a subset of directory i.e catalog entries.
    ///
    class Range:
      public Dumpable, public Archivable
    {
    public:
      typedef std::list<Record*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      ~Range();

      //
      // methods
      //
      Status		Add(Record*);
      Status		Exist(const Subject&);
      Status		Lookup(const Subject&,
			       Record*&);
      Status		Remove(const Subject&);
      Status		Capacity(Size&) const;

      Status		Locate(const Subject&,
			       Index&);
      Status		Locate(const Subject&,
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
