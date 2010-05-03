//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Access.hh
//
// created       julien quintard   [thu mar  5 20:17:45 2009]
// updated       julien quintard   [mon may  3 22:58:39 2010]
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
#include <etoile/kernel/Record.hh>

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
      // constructors & destructors
      //
      Access();

      //
      // methods
      //
      elle::Status	Add(Record*);
      elle::Status	Exist(const Subject&);
      elle::Status	Lookup(const Subject&,
			       Record*&);
      elle::Status	Consult(const Index&,
				const Size&,
				Range<Record>&) const;
      elle::Status	Upgrade(const elle::SecretKey&);
      elle::Status	Remove(const Subject&);
      elle::Status	Capacity(Size&) const;
      elle::Status	Locate(const Subject&,
			       Index&);
      elle::Status	Fingerprint(elle::Digest&) const;

      //
      // interfaces
      //

      // object
      declare(Access, _());

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      State		state;

      Range<Record>	range;
    };

  }
}

#endif
