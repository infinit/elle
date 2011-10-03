//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [sun jun 19 23:13:28 2011]
//

#ifndef LUNE_CONFIGURATION_HH
#define LUNE_CONFIGURATION_HH

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
  /// this class represents a configuration file which can be used to
  /// alterate the Infinit behaviour dynamically.
  ///
  class Configuration:
    public elle::Settings
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    struct				Default
    {
      struct				Shrub
      {
	static const elle::Boolean	Status;
	static const elle::Natural32	Capacity;
	static const elle::Natural32	Lifespan;
	static const elle::Natural32	Frequency;
      };

      struct				Cache
      {
	static const elle::Natural32	Capacity;
      };

      struct				Reserve
      {
	static const elle::Natural32	Capacity;
      };

      struct				Debug
      {
#if INFINIT_UNIX
	static const elle::Boolean	PIG;
#endif

#if INFINIT_WIN32
        static const elle::Boolean      IIG;
#endif

	static const elle::Boolean	Etoile;
	static const elle::Boolean	Nucleus;
	static const elle::Boolean	Hole;
      };

      struct				History
      {
	static const elle::Boolean	Status;

	struct				Indicator
	{
	  static const elle::Character	Root;
	  static const elle::Character	Slab;
	};
      };

      struct				FUSE
      {
	static const elle::Natural32	FUker;
      };
    };

    //
    // methods
    //
    elle::Status		Push();
    elle::Status		Pull();

    //
    // interfaces
    //

    // object
    declare(Configuration);

    // dumpable
    elle::Status		Dump(const elle::Natural32 = 0) const;

    // fileable
    elle::Status		Load();
    elle::Status		Store() const;
    elle::Status		Erase() const;
    elle::Status		Exist() const;

    //
    // attributes
    //
    struct
    {
      elle::Boolean		status;
      elle::Natural32		capacity;
      elle::Natural32		lifespan;
      elle::Natural32		frequency;
    }				shrub;

    struct
    {
      elle::Boolean		status;
      elle::Natural32		capacity;
    }				cache;

    struct
    {
      elle::Boolean		status;
      elle::Natural32		capacity;
    }				reserve;

    struct
    {
#if INFINIT_UNIX
      elle::Boolean		pig;
#endif

#if INFINIT_WIN32
      elle::Boolean             iig;
#endif

      elle::Boolean		etoile;
      elle::Boolean		nucleus;
      elle::Boolean		hole;
    }				debug;

    struct
    {
      elle::Boolean		status;

      struct
      {
	elle::Character		root;
	elle::Character		slab;
      }				indicator;
    }				history;

    struct
    {
      elle::Natural32		fuker;
    }				fuse;

  };

}

#endif
