#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Node.hh>
#include <nucleus/proton/Quill.hh>
#include <nucleus/proton/Seam.hh>
#include <nucleus/neutron/Component.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/Exception.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Contents");

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Constants |
    `----------*/

    neutron::Component const Contents::Constants::component{
      neutron::ComponentContents};

    /*-------------.
    | Construction |
    `-------------*/

    Contents::Contents():
      proton::ContentHashBlock()
    {
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Contents::encrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE_METHOD(key);

      this->_shell.encrypt(key);
    }

    void
    Contents::decrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE_METHOD(key);

      this->_shell.decrypt(key);
    }

    Mode
    Contents::mode() const
    {
      return (this->_shell.mode());
    }

    Node const&
    Contents::node() const
    {
      return (this->_shell.node());
    }

    Node&
    Contents::node()
    {
      return (this->_shell.node());
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Contents::Dump(elle::Natural32          margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Contents]" << std::endl;

      if (proton::ContentHashBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      this->_shell.Dump(margin + 2);

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Contents::print(std::ostream& stream) const
    {
      stream << "contents(" << this->_shell << ")";
    }
  }
}
