//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Directory.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [fri jun 17 16:24:19 2011]
//

#ifndef ETOILE_GEAR_DIRECTORY_HH
#define ETOILE_GEAR_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>

#include <etoile/path/Slice.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this context represents a directory object as it embeds
    /// a catalog contents along with inherited object-related stuff.
    ///
    class Directory:
      public Object
    {
    public:
      //
      // constructors & destructors
      //
      Directory();
      ~Directory();

      //
      // methods
      //
      elle::Status	Create();
      elle::Status	Load(const nucleus::Location&);
      elle::Status	Open();
      elle::Status	Add(const path::Slice&,
			    const nucleus::Address&);
      elle::Status	Lookup(const path::Slice&,
			       nucleus::Entry*&);
      elle::Status	Consult(const nucleus::Index&,
				const nucleus::Size&,
				nucleus::Range<nucleus::Entry>&);
      elle::Status	Rename(const path::Slice&,
			       const path::Slice&);
      elle::Status	Remove(const path::Slice&);
      elle::Status	Destroy();
      elle::Status	Close();
      elle::Status	Store();

      //
      // interfaces
      //

      // XXX object

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // XXX archivable

      // transcribable
      elle::Status	Transcribe(journal::Transcript&) const;

      //
      // attributes
      //
      nucleus::Contents<nucleus::Catalog>*	contents;
    };

  }
}

#endif
