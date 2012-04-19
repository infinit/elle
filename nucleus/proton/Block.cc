
#include <elle/standalone/Log.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/BlockSerializer.hxx>

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
      state(StateClean)
    {
    }

    ///
    /// specific constructor.
    ///
    Block::Block(const Family                                   family,
                 const neutron::Component                       component):
      family(family),
      component(component),
      state(StateClean)
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
      escape("this method should never have been called");
    }

    ///
    /// this method returns Status::True if the block is valid, Status::False
    /// otherwise.
    ///
    elle::Status        Block::Validate(const Address&) const
    {
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

      std::cout << alignment << "[Block]" << std::endl;

      // dump the network.
      if (this->network.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the network");

      // dump the family.
      std::cout << alignment << elle::Dumpable::Shift << "[Family] "
                << std::dec << this->family << std::endl;

      // dump the component.
      std::cout << alignment << elle::Dumpable::Shift << "[Component] "
                << std::dec << this->component << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[State] "
                << std::dec << this->state << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    //elle::Status        Block::Serialize(elle::Archive& archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(
    //        this->network,
    //        static_cast<elle::Natural8>(this->family),
    //        static_cast<elle::Natural8>(this->component)) == elle::Status::Error)
    //    escape("unable to serialize the block's attributes");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the attributes.
    /////
    //elle::Status        Block::Extract(elle::Archive&           archive)
    //{
    //  // extracts the attributes.
    //  if (archive.Extract(
    //        this->network,
    //        reinterpret_cast<elle::Natural8&>(this->family),
    //        reinterpret_cast<elle::Natural8&>(this->component)) ==
    //      elle::Status::Error)
    //    escape("unable to extract the block's attributes");

    //  return elle::Status::Ok;
    //}

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method erases the block file.
    ///
    elle::Status        Block::Erase(const Network&,
                                     const Address&) const
    {
      escape("this method should never have been called");
    }

  }
}
