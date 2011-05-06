//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Record.hh
//
// created       julien quintard   [thu apr  1 19:48:42 2010]
// updated       julien quintard   [thu may  5 13:59:31 2011]
//

#ifndef NUCLEUS_NEUTRON_RECORD_HH
#define NUCLEUS_NEUTRON_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
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
      public elle::Object
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
      declare(Record);
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
