//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Configuration.hh
//
// created       julien quintard   [sun jun 19 23:13:28 2011]
// updated       julien quintard   [thu aug 11 10:36:50 2011]
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
  /// this class represents a configuration file.
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
	static const elle::Natural32	Frequence;
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
	static const elle::Boolean	PIG;
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
	static const elle::String	FUker;
      };
    };

    //
    // methods
    //
    elle::Status	Push();
    elle::Status	Pull();

    //
    // interfaces
    //

    // object
    declare(Configuration);

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // fileable
    elle::Status	Load();
    elle::Status	Store() const;
    elle::Status	Erase() const;
    elle::Status	Exist() const;

    //
    // attributes
    //
    struct
    {
      elle::Boolean		status;
      elle::Natural32		capacity;
      elle::Natural32		lifespan;
      elle::Natural32		frequence;
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
      elle::Boolean		pig;
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
      elle::String		fuker;
    }				fuse;

  };

}

#endif
