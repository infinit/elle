#include <etoile/gear/Action.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>

#include <elle/assert.hh>

namespace etoile
{
  namespace gear
  {
//
// ---------- Action ----------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    Action::Action(Type const type):
      _type(type)
    {
    }

    namespace action
    {
//
// ---------- Push ------------------------------------------------------------
//

      /*-------------.
      | Construction |
      `-------------*/

      Push::Push(nucleus::proton::Address const& address,
                 std::unique_ptr<nucleus::proton::Block const>&& block):
        Action(Action::Type::push),

        _address(address),
        _block(std::move(block))
      {
        ELLE_ASSERT(this->_block != nullptr);
      }

      /*--------.
      | Methods |
      `--------*/

      nucleus::proton::Block const&
      Push::block() const
      {
        ELLE_ASSERT(this->_block != nullptr);

        return (*this->_block);
      }

      /*-----------.
      | Interfaces |
      `-----------*/

      elle::Status
      Push::Dump(const elle::Natural32      margin) const
      {
        elle::String alignment(margin, ' ');

        std::cout << alignment << "[Push]" << std::endl;

        if (this->_address.Dump(margin + 2) == elle::Status::Error)
          throw elle::Exception("unable to dump the address");

        if (this->_block->Dump(margin + 2) == elle::Status::Error)
          throw elle::Exception("unable to dump the block");

        return elle::Status::Ok;
      }

      void
      Push::print(std::ostream& stream) const
      {
        ELLE_ASSERT(this->_block != nullptr);

        stream << "push{"
               << this->_address
               << ", "
               << *this->_block
               << "}";
      }

//
// ---------- Wipe ------------------------------------------------------------
//

      /*-------------.
      | Construction |
      `-------------*/

      Wipe::Wipe(nucleus::proton::Address const& address):
        Action(Action::Type::wipe),

        _address(address)
      {
      }

      /*-----------.
      | Interfaces |
      `-----------*/

      elle::Status
      Wipe::Dump(const elle::Natural32      margin) const
      {
        elle::String alignment(margin, ' ');

        std::cout << alignment << "[Wipe]" << std::endl;

        if (this->_address.Dump(margin + 2) == elle::Status::Error)
          throw elle::Exception("unable to dump the address");

        return elle::Status::Ok;
      }

      void
      Wipe::print(std::ostream& stream) const
      {
        stream << "wipe{"
               << this->_address
               << "}";
      }
    }

    /*----------.
    | Operators |
    `----------*/

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
