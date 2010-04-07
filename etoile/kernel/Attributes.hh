//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Attributes.hh
//
// created       julien quintard   [wed mar 31 23:32:06 2010]
// updated       julien quintard   [tue apr  6 16:33:02 2010]
//

#ifndef ETOILE_KERNEL_ATTRIBUTES_HH
#define ETOILE_KERNEL_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Trait.hh>
#include <etoile/kernel/Index.hh>
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
    /// this class contains a list of (key, data) tuples that can be
    /// used for storing object-specific extra information.
    ///
    class Attributes:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // types
      //
      typedef std::list<Trait*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      ~Attributes();

      //
      // methods
      //
      Status		Add(Trait*);
      Status		Exist(const String&);
      Status		Lookup(const String&,
			       Trait*&);
      /*
      Status		Consult(const Index&,
				const Size&,
				T&) const; // XXX */
      Status		Remove(const String&);
      Status		Capacity(Size&) const;

      Status		Locate(const String&,
			       Iterator* = NULL);

      //
      // interfaces
      //

      // entity
      declare(Entity, Attributes);
      Boolean		operator==(const Attributes&) const;

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

#endif
