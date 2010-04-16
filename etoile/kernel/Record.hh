//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Record.hh
//
// created       julien quintard   [thu apr  1 19:48:42 2010]
// updated       julien quintard   [thu apr 15 15:24:55 2010]
//

#ifndef ETOILE_KERNEL_RECORD_HH
#define ETOILE_KERNEL_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Subject.hh>
#include <etoile/kernel/Permissions.hh>
#include <etoile/kernel/Token.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an access control record, composed
    /// of the subject, its permissions and the token allowing the subject
    /// to access the data.
    ///
    class Record:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Record		Null;

      //
      // constructors & destructors
      //
      Record();
      Record(const Subject&,
	     const Permissions&,
	     const Token&);

      //
      // methods
      //
      Status		Update(const Subject&,
			       const Permissions&,
			       const SecretKey&);
      Status		Update(const Subject&,
			       const Permissions&,
			       const Token&);

      //
      // interfaces
      //

      // entity
      declare(Entity, Record);
      Boolean		operator==(const Record&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Subject		subject;
      Permissions	permissions;
      Token		token;
    };

  }
}

#endif
