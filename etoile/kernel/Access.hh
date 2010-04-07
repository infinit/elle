//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Access.hh
//
// created       julien quintard   [thu mar  5 20:17:45 2009]
// updated       julien quintard   [tue apr  6 13:27:48 2010]
//

#ifndef ETOILE_KERNEL_ACCESS_HH
#define ETOILE_KERNEL_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/ContentHashBlock.hh>
#include <etoile/kernel/State.hh>
#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Size.hh>
#include <etoile/kernel/Range.hh>
#include <etoile/kernel/Subject.hh>

#include <list>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an access control list.
    ///
    /// XXX \todo note that, for now, such a list is implemented through a
    /// single range. however the structure will be extended to an optimised
    /// general tree structure in a near future.
    ///
    class Access:
      public ContentHashBlock
    {
    public:
      //
      // methods
      //
      Status		Add(Record*);
      Status		Exist(const Subject&);
      Status		Lookup(const Subject&,
			       Record*&);
      Status		Consult(const Index&,
				const Size&,
				Range&) const;
      Status		Remove(const Subject&);
      Status		Upgrade(const SecretKey&);
      Status		Capacity(Size&) const;
      Status		Locate(const Subject&,
			       Index&);
      Status		Fingerprint(Digest&) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Access);
      // XXX operator==

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      State		state;

      Range		range;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Address.hh>

#endif
