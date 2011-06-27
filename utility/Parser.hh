//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Parser.hh
//
// created       julien quintard   [wed apr 28 11:11:05 2010]
// updated       julien quintard   [mon jun 27 08:10:58 2011]
//

#ifndef ELLE_UTILITY_PARSER_HH
#define ELLE_UTILITY_PARSER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Character.hh>
#include <elle/core/String.hh>
#include <elle/core/Type.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Close.hh>
# include <vector>
# include <unistd.h>
# include <getopt.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace utility
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class wraps the command line parser.
    ///
    class Parser:
      public Entity
    {
    public:
      //
      // constants
      //
      static const Natural32		Alignment;

      //
      // enumeration
      //
      enum Format
	{
	  FormatNone,
	  FormatOptional,
	  FormatRequired
	};

      enum State
	{
	  StateDeactivated,
	  StateActivated
	};

      //
      // classes
      //
      class Option:
	public Entity
      {
      public:
	//
	// constructors & destructors
	//
	Option(const String&,
	       const Character&,
	       const String&,
	       const String&,
	       const Format&);

	//
	// attributes
	//
	String		name;

	Character	character;
	String		string;
	String		description;
	Format		format;

	State		state;
	String*		value;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;
      };

      //
      // types
      //
      typedef std::vector<Option*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Parser(Natural32,
	     Character**);
      ~Parser();

      //
      // methods
      //
      Status		Description(const String&);

      Status		Register(const String&,
				 const Character&,
				 const String&,
				 const String&,
				 const Format);

      Status		Locate(const String&,
			       Option*&);
      Status		Locate(const Character&,
			       Option*&);

      Status		Parse();

      Status		Test(const String&);
      Status		Argument(const String&);
      template <typename T>
      Status		Value(const String&,
			      T&);
      template <typename T>
      Status		Default(const String&,
				T&,
				const T = Type<T>::Default);

      Void		Usage();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural32		argc;
      Character**	argv;

      String		description;

      Container		options;

      String		shorts;
      struct ::option*	longs;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/utility/Parser.hxx>

#endif
