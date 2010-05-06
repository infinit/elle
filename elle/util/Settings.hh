//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/util/Settings.hh
//
// created       julien quintard   [sun apr 25 20:56:02 2010]
// updated       julien quintard   [tue may  4 12:47:50 2010]
//

#ifndef ELLE_UTIL_SETTINGS_HH
#define ELLE_UTIL_SETTINGS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fstream>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for human-readable, hence editable,
    /// configuration files.
    ///
    class Settings:
      public Object<FormatBase64, FormatCustom>
    {
    public:
      //
      // classes
      //

      ///
      /// this class represents an assignment.
      ///
      class Assignment:
	public Entity
      {
      public:
	//
	// constructors & destructors
	//
	Assignment(const String&,
		   const String&);

	//
	// attributes
	//
	String		name;
	String		value;
      };

      ///
      /// this class represents a section.
      ///
      class Section:
	public Entity
      {
      public:
	//
	// types
	//
	typedef std::list<Assignment*>		Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	Section(const String&);
	~Section();

	//
	// methods
	//
	Status		Exist(const String&);
	Status		Add(const String&,
			    const String&);
	Status		Lookup(const String&,
			       String&);
	Status		Remove(const String&);
	Status		Locate(const String&,
			       Iterator* = NULL);

	//
	// attributes
	//
	String		identifier;

	Container	assignments;
      };

      //
      // types
      //
      typedef std::list<Section*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Trim(const String&,
			     String&,
			     const String& = "\n\t ");

      //
      // constructors & destructors
      //
      ~Settings();

      //
      // methods
      //
      Status		Exist(const String&);
      Status		Add(const String&);
      Status		Lookup(const String&,
			       Section*&);
      Status		Remove(const String&);
      Status		Locate(const String&,
			       Iterator* = NULL);

      Status		Set(const String&,
			    const String&,
			    const String&);
      Status		Get(const String&,
			    const String&,
			    String&);

      //
      // interfaces
      //

      // object
      declare(Settings, _(FormatBase64, FormatCustom));

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // fileable
      Status		Load(const String&);
      Status		Store(const String&) const;

      //
      // attributes
      //
      Container		sections;
    };

  }
}

#endif
