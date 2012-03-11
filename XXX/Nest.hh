//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sat dec 24 13:42:41 2011]
//

#ifndef NUCLEUS_PROTON_NEST_HH
#define NUCLEUS_PROTON_NEST_HH

#include <elle/Elle.hh>

#include <nucleus/proton/Block.hh>
#include <XXX/Handle.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Nest
    {
    public:
      //
      // static methods
      //
      static elle::Status       Setup(
        const elle::Callback<
          elle::Status,
          elle::Parameters<
            Handle&
            >
          >&,
        const elle::Callback<
          elle::Status,
          elle::Parameters<
            Handle&
            >
          >&);

      //
      // static attributes
      //
      static
      elle::Callback<
        elle::Status,
        elle::Parameters<
          Handle&
          >
        >                       Load;
      static
      elle::Callback<
        elle::Status,
        elle::Parameters<
          Handle&
          >
        >                       Unload;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Nest.hxx>

#endif

