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
// updated       julien quintard   [wed jun  1 10:19:50 2011]
//

#ifndef ELLE_UTILITY_PARSER_HH
#define ELLE_UTILITY_PARSER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Character.hh>
#include <elle/core/String.hh>

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
      enum Type
	{
	  TypeNone,
	  TypeOptional,
	  TypeRequired
	};

      //
      // structures
      //
      struct Option:
	public Entity
      {
      public:
	//
	// attributes
	//
	Character	character;
	String		string;
	String		description;
	Type		type;
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

      Status		Register(const Character&,
				 const String&,
				 const String&,
				 const Type&);

      Status		Parse(Character&);

      Void		Usage();

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

#endif
