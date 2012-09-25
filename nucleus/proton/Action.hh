#ifndef NUCLEUS_PROTON_ACTION_HH
# define NUCLEUS_PROTON_ACTION_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class defines an action related to the blocks being either
    /// pushed/wiped to/from the storage layer.
    ///
    class Action
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
