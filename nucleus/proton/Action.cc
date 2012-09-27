#include <nucleus/proton/Action.hh>
#include <nucleus/proton/Block.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    Action::Action(Address const& address,
                   Block const* block):
      _type(Action::Type::push),
      _address(address),
      _block(block)
    {
    }

    Action::Action(Address const& address):
      _type(Action::Type::wipe),
      _address(address)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    Block const&
    Action::block() const
    {
      assert(this->_block != nullptr);

      return (*this->_block);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status        Action::Dump(const elle::Natural32      margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Action]" << std::endl;

      // display the type.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Type] "
                << this->_type << std::endl;

      // dump according to the type.
      switch (this->_type)
        {
        case Action::Type::push:
          {
            // display the address.
            if (this->_address.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the address");

            // display the block.
            if (this->_block->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the block");

            break;
          }
        case Action::Type::wipe:
          {
            // display the address.
            if (this->_address.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the address");

            break;
          }
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Action::print(std::ostream& stream) const
    {
      stream << "action{"
             << this->_type
             << ", "
             << this->_address
             << ", "
             << this->_block
             << "}";
    }

    std::ostream&
    operator <<(std::ostream& stream,
                Action::Type type)
    {
      switch (type)
        {
        case Action::Type::push:
          {
            stream << "push";
            break;
          }
        case Action::Type::wipe:
          {
            stream << "wipe";
            break;
          }
        }

      return (stream);
    }

  }
}
