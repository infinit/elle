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
    static const elle::String                   Extension;

    struct                                      Default
    {
      struct                                    Etoile
      {
        static const elle::Boolean              Debug;

        struct                                  Gear
        {
          static const elle::Natural32          Containment;
        };

        struct                                  Shrub
        {
          static const elle::Boolean            Status;
          static const elle::Natural32          Capacity;
          static const elle::Natural32          Lifespan;
          static const elle::Natural32          Frequency;
        };

        struct                                  History
        {
          static const elle::Boolean            Status;

          struct                                Indicator
          {
            static const elle::Character        Root;
            static const elle::Character        Slab;
          };
        };
      };

      struct                                    Nucleus
      {
        static const elle::Boolean              Debug;
      };

      struct                                    Hole
      {
        static const elle::Boolean              Debug;
      };

      struct                                    Facade
      {
        static const elle::Boolean              Debug;

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
        static const elle::Natural32            FUker;
#elif defined(INFINIT_WINDOWS)
        // XXX
#else
# error "unsupported platform"
#endif
      };
    };

    //
    // methods
    //
    elle::Status                Push();
    elle::Status                Pull();

    //
    // interfaces
    //

    // object
    declare(Configuration);

    // dumpable
    elle::Status                Dump(const elle::Natural32 = 0) const;

    // fileable
    elle::Status                Load();
    elle::Status                Store() const;
    elle::Status                Erase() const;
    elle::Status                Exist() const;

    //
    // attributes
    //
    struct
    {
      elle::Boolean             debug;

      struct
      {
        elle::Natural32         containment;
      }                         gear;

      struct
      {
        elle::Boolean           status;
        elle::Natural32         capacity;
        elle::Natural32         lifespan;
        elle::Natural32         frequency;
      }                         shrub;

      struct
      {
        elle::Boolean           status;

        struct
        {
          elle::Character       root;
          elle::Character       slab;
        }                       indicator;
      }                         history;
    }                           etoile;

    struct
    {
      elle::Boolean             debug;
    }                           nucleus;

    struct
    {
      elle::Boolean             debug;
    }                           hole;

    struct
    {
      elle::Boolean             debug;

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      elle::Natural32           fuker;
#elif defined(INFINIT_WINDOWS)
      // XXX
#else
# error "unsupported platform"
#endif
    }                           facade;
  };

}

#endif
