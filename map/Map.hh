//
// ---------- header ----------------------------------------------------------
//
// project       map
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/map/Map.hh
//
// created       julien quintard   [sun apr 18 10:49:35 2010]
// updated       julien quintard   [mon apr 19 09:47:04 2010]
//

#ifndef MAP_MAP_HH
#define MAP_MAP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace map
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a map of links between local user/group names
  /// and Infinit identifiers.
  ///
  template <typename T>
  class Map:
    public Dumpable, public Archivable
  {
  public:
    //
    // constants
    //
    static const T*		Trash;

    //
    // types
    //
    typedef std::pair<String, T>		Value;
    typedef std::list<Value>			Container;
    typedef typename Container::iterator	Iterator;
    typedef typename Container::const_iterator	Scoutor;

    //
    // static methods
    //
    static Status	Load(const String&,
			     Map&);
    static Status	Store(const String&,
			      const Map&);

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

//
// ---------- templates -------------------------------------------------------
//

#include <libraries/map/Map.hxx>

#endif
