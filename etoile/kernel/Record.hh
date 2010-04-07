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
// updated       julien quintard   [sun apr  4 18:47:10 2010]
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
      public Dumpable, public Archivable
    {
    public:
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
      Subject		subject;
      Permissions	permissions;
      Token		token;
    };

  }
}

#endif
