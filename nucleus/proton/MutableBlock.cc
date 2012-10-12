#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/History.hh>
#include <nucleus/proton/Revision.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/Nucleus.hh>

#include <elle/log.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <elle/idiom/Close.hh>
# include <boost/lexical_cast.hpp>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.MutableBlock");

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructs & destructors ----------------------------------------
//

    MutableBlock::MutableBlock():
      Block()
    {
    }

    MutableBlock::MutableBlock(Network const network,
                               Family const family,
                               neutron::Component const component,
                               elle::cryptography::PublicKey const& creator_K):
      Block(network, family, component, creator_K),

      _revision(Revision::First)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    elle::Boolean
    MutableBlock::derives(MutableBlock const& other) const
    {
      return (this->_revision > other._revision);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    MutableBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[MutableBlock]" << std::endl;

      if (Block::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent");

      if (this->_revision.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the revision");

      if (this->_base.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the revision");

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    MutableBlock::print(std::ostream& stream) const
    {
      stream << "mutable block{"
             << this->_revision
             << "}";
    }
  }
}
