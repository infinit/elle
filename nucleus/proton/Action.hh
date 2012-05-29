//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri jun 24 14:45:52 2011]
//

#ifndef NUCLEUS_PROTON_ACTION_HH
#define NUCLEUS_PROTON_ACTION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class defines an action related to the blocks being either
    /// pushed/wiped to/from the storage layer.
    ///
    class Action:
      public elle::radix::Object
    {
    public:
      //
      // enumerations
      //
      enum Type
        {
          TypeUnknown,
          TypePush,
          TypeWipe
        };

      //
      // constructors & destructors
      //
      Action(const Address&,
             const Block*);
      Action(const Address&);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      const Type        type;

      const Address     address;
      const Block*      block;
    };

  }
}

#endif
