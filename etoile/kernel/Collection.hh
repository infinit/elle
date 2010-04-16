//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Collection.hh
//
// created       julien quintard   [wed mar 31 23:32:06 2010]
// updated       julien quintard   [thu apr 15 16:27:36 2010]
//

#ifndef ETOILE_KERNEL_COLLECTION_HH
#define ETOILE_KERNEL_COLLECTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Trait.hh>
#include <etoile/kernel/Size.hh>

#include <list>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a subcollection of access control list.
    ///
    class Collection:
      public Dumpable, public Archivable
    {
    public:
      typedef std::list<Trait*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      ~Collection();

      //
      // methods
      //
      Status		Add(Trait*);
      Status		Exist(const String&);
      Status		Lookup(const String&,
			       Trait*&);
      Status		Remove(const String&);
      Status		Capacity(Size&) const;

      Status		Locate(const String&,
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
