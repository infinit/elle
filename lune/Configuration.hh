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
// updated       julien quintard   [mon jun 27 20:16:30 2011]
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
      struct				Path
      {
	static const elle::Natural32	Capacity;
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
	static const elle::Boolean	Meta;
	static const elle::Boolean	PIG;
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
    elle::Status	Load(const elle::String&);
    elle::Status	Store(const elle::String&) const;
    elle::Status	Erase(const elle::String&) const;
    elle::Status	Exist(const elle::String&) const;

    //
    // attributes
    //
    struct
    {
      elle::Natural32		capacity;
    }				path;

    struct
    {
      elle::Natural32		capacity;
    }				cache;

    struct
    {
      elle::Natural32		capacity;
    }				reserve;

    struct
    {
      elle::Boolean		meta;
      elle::Boolean		pig;
    }				debug;
  };

}

#endif
