//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Associat.hh
//
// created       julien quintard   [sun apr 18 10:49:35 2010]
// updated       julien quintard   [mon may  3 11:18:28 2010]
//

#ifndef ELLE_LUNE_ASSOCIAT_HH
#define ELLE_LUNE_ASSOCIAT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace elle
{
  namespace lune
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a associat of links between local user/group
    /// names and Infinit identifiers.
    ///
    template <typename T>
    class Associat:
      Object<>
    {
    public:
      //
      // constants
      //
      static const T*		Trash;

      //
      // types
      //
      typedef std::pair<String, T>			Value;
      typedef std::list<Value>				Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // static methods
      //
      static Status	Load(const String&,
			     Associat<T>&);
      static Status	Store(const Associat<T>&,
			      const String&);

      //
      // methods
      //
      Status		Add(const String&,
			    const T&);
      Status		Lookup(const String&,
			       const T*& = Trash);
      Status		Lookup(const T&,
			       const String*& = Trash);
      Status		Remove(const String&);

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
// ---------- templates -------------------------------------------------------
//

#include <elle/lune/Associat.hxx>

#endif
