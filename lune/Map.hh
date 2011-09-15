//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [sun apr 18 10:49:35 2010]
//

#ifndef LUNE_MAP_HH
#define LUNE_MAP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents an map of links between local user/group
  /// names and Infinit identifiers.
  ///
  template <typename T>
  class Map:
    public elle::Object
  {
  public:
    //
    // constants
    //
    struct			Trash
    {
      static elle::String*	Name;
      static T*			Value;
    };

    //
    // structures
    //
    struct Entry
    {
      //
      // attributes
      //
      elle::String	name;
      T			value;
    };

    //
    // types
    //
    typedef std::list<Entry*>				Container;
    typedef typename Container::iterator		Iterator;
    typedef typename Container::const_iterator		Scoutor;

    //
    // constructors & destructors
    //
    ~Map();

    //
    // methods
    //
    elle::Status	Add(const elle::String&,
			    const T&);
    elle::Status	Lookup(const elle::String&,
			       T*& = Trash::Value);
    elle::Status	Lookup(const T&,
			       elle::String*& = Trash::Name);
    elle::Status	Remove(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Map);

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    //
    // attributes
    //
    Container		container;
  };

}

//
// ---------- templates -------------------------------------------------------
//

#include <lune/Map.hxx>

#endif
