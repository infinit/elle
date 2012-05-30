

#include <nucleus/proton/Action.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// push-specifc constructor.
    ///
    Action::Action(const Address&                               address,
                   const Block*                                 block):
      type(Action::TypePush),
      address(address),
      block(block)
    {
    }

    ///
    /// wipe-specifc constructor.
    ///
    Action::Action(const Address&                               address):
      type(Action::TypeWipe),
      address(address)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an action.
    ///
    elle::Status        Action::Dump(const elle::Natural32      margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Action]" << std::endl;

      // display the type.
      std::cout << alignment << elle::Dumpable::Shift << "[Type] "
                << this->type << std::endl;

      // dump according to the type.
      switch (this->type)
        {
        case Action::TypePush:
          {
            // display the address.
            if (this->address.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the address");

            // display the block.
            if (this->block->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the block");

            break;
          }
        case Action::TypeWipe:
          {
            // display the address.
            if (this->address.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the address");

            break;
          }
        case Action::TypeUnknown:
          {
            escape("unknown action type");
          }
        }

      return elle::Status::Ok;
    }

  }
}
