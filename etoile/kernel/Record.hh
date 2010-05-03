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
// updated       julien quintard   [mon may  3 22:58:57 2010]
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
      public elle::Object<>
    {
    public:
      //
      // constants
      //
      static const Record		Null;

      //
      // types
      //
      typedef Subject			S;

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
      elle::Status	Update(const Subject&,
			       const Permissions&,
			       const elle::SecretKey&);
      elle::Status	Update(const Subject&,
			       const Permissions&,
			       const Token&);

      //
      // interfaces
      //

      // object
      declare(Record, _());
      elle::Boolean	operator==(const Record&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // rangeable
      Subject&		Symbol();

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
