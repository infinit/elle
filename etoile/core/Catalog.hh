//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Catalog.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [thu jan 28 22:07:32 2010]
//

#ifndef ETOILE_CORE_CATALOG_HH
#define ETOILE_CORE_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Contents.hh>
#include <etoile/core/State.hh>

#include <etoile/hole/Hole.hh>

#include <list>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the content of a directory.
    ///
    /// \todo XXX note that, for now, all the entries are stored in a single
    /// block until an advanced, multi-block-based, data structure is
    /// developed.
    ///
    class Catalog:
      public Contents
    {
    public:
      //
      // identifier
      //
      static const String	Identifier;

      //
      // constants
      //
      struct		Index
      {
	static const Contents::Offset	First;
	static const Contents::Offset	Last;
      };

      //
      // structures
      //

      ///
      /// this structure defines a catalog entry.
      ///
      struct		Entry
      {
	String		name;
	hole::Address	address;
      };

      //
      // types
      //
      typedef std::list<Entry*>		Container;

      typedef Container::iterator	Iterator;
      typedef Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      Catalog();
      ~Catalog();

      //
      // methods
      //
      Status		Prepare(const SecretKey&);

      Status		Add(const String&,
			    const hole::Address&);
      Status		Remove(const String&);
      Status		Update(const String&);
      Status		Rename(const String&,
			       const String&);
      Status		Lookup(const String&,
			       hole::Address&);

      Status		Search(const String&,
			       Catalog::Iterator* = NULL);

      Status		Size(Contents::Offset&) const;

      //
      // interfaces
      //

      // entity
      Embed(Entity, Catalog);
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

      Container		entries;

      SecretKey		key;
    };

  }
}

#endif
