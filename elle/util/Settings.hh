//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/util/Settings.hh
//
// created       julien quintard   [sun apr 25 20:56:02 2010]
// updated       julien quintard   [mon apr 26 19:05:29 2010]
//

#ifndef ELLE_UTIL_SETTINGS_HH
#define ELLE_UTIL_SETTINGS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>
#include <elle/core/Meta.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/io/Dumpable.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fstream>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;
  using namespace io;

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
      public Meta,
      public Dumpable
    {
    public:
      //
      // classes
      //

      ///
      /// this class represents an assignment.
      ///
      class Assignment:
	public Meta,
	public Dumpable
      {
      public:
	//
	// constructors & destructors
	//
	Assignment(const String&,
		   const String&);

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

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
	public Meta,
	public Dumpable
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
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

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

      static Status	Load(const String&,
			     Settings&);
      static Status	Store(const Settings&,
			      const String&);

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

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container		sections;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Maid.hh>

#endif
