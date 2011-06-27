//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Settings.hh
//
// created       julien quintard   [sun apr 25 20:56:02 2010]
// updated       julien quintard   [sun jun 26 21:12:32 2011]
//

#ifndef ELLE_UTILITY_SETTINGS_HH
#define ELLE_UTILITY_SETTINGS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>
#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Entity.hh>

#include <elle/io/Fileable.hh>
#include <elle/io/Format.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fstream>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace io;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for human-readable, hence editable,
    /// configuration files.
    ///
    class Settings:
      public Object,
      public virtual Fileable<FormatCustom>
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
      Status		Add(const String&);
      Status		Lookup(const String&,
			       Section*&);
      Status		Remove(const String&);
      Status		Locate(const String&,
			       Iterator* = NULL);
      Status		Find(const String&,
			     const String&);

      Status		Write(const String&,
			      const String&,
			      const String&);
      Status		Read(const String&,
			     const String&,
			     String&);

      template <typename T>
      Status		Set(const String&,
			    const String&,
			    const T&);
      template <typename T>
      Status		Get(const String&,
			    const String&,
			    T&,
			    const T = Type<T>::Default);

      //
      // interfaces
      //

      // object
      declare(Settings);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // fileable
      Status		Load(const Path&);
      Status		Store(const Path&) const;
      Status		Erase(const Path&) const;
      Status		Exist(const Path&) const;

      //
      // attributes
      //
      Container		sections;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/utility/Settings.hxx>

#endif
