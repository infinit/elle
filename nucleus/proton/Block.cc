//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 11 22:44:58 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Block.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the block files extension.
  ///
  const elle::String            Block::Extension = ".blk";

//
// ---------- constructs & destructors ----------------------------------------
//

    ///
    /// default constructor.
    ///
    Block::Block():
      family(FamilyUnknown),
      component(neutron::ComponentUnknown),
      _state(StateClean)
    {
    }

    ///
    /// specific constructor.
    ///
    Block::Block(const Family                                   family,
                 const neutron::Component                       component):
      family(family),
      component(component),
      _state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status        Block::Bind(Address&) const
    {
      ;

      escape("this method should never have been called");
    }

    ///
    /// this method returns StatusTrue if the block is valid, StatusFalse
    /// otherwise.
    ///
    elle::Status        Block::Validate(const Address&) const
    {
      ;

      escape("this method should never have been called");
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Block, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    elle::Status        Block::Dump(const elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');

      ;

      std::cout << alignment << "[Block]" << std::endl;

      // dump the network.
      if (this->network.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the network");

      // dump the family.
      std::cout << alignment << elle::Dumpable::Shift << "[Family] "
                << this->family << std::endl;

      // dump the component.
      std::cout << alignment << elle::Dumpable::Shift << "[Component] "
                << this->component << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[_State] "
                << this->_state << std::endl;

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    elle::Status        Block::Serialize(elle::Archive& archive) const
    {
      ;

      // serialize the attributes.
      if (archive.Serialize(
            this->network,
            static_cast<elle::Natural8>(this->family),
            static_cast<elle::Natural8>(this->component)) == elle::StatusError)
        escape("unable to serialize the block's attributes");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the attributes.
    ///
    elle::Status        Block::Extract(elle::Archive&           archive)
    {
      ;

      // extracts the attributes.
      if (archive.Extract(
            this->network,
            reinterpret_cast<elle::Natural8&>(this->family),
            reinterpret_cast<elle::Natural8&>(this->component)) ==
          elle::StatusError)
        escape("unable to extract the block's attributes");

      return elle::StatusOk;
    }

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method erases the block file.
    ///
    elle::Status        Block::Erase(const Network&,
                                     const Address&) const
    {
      ;

      escape("this method should never have been called");
    }

  }
}
