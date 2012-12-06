#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Node.hh>
#include <nucleus/proton/Quill.hh>
#include <nucleus/proton/Seam.hh>
#include <nucleus/neutron/Component.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/Exception.hh>
#include <nucleus/factory.hh>

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
      proton::ContentHashBlock(),

      _node(nullptr),
      _cipher(nullptr)
    {
    }

    Contents::~Contents()
    {
      delete this->_node;
      delete this->_cipher;
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Contents::encrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE_METHOD(key);

      ELLE_ASSERT(this->_node != nullptr);
      ELLE_ASSERT(this->_cipher == nullptr);

      this->_cipher = new cryptography::Cipher;

      if (key.Encrypt(*this->_node, *this->_cipher) == elle::Status::Error)
        throw Exception("unable to encrypt the archived block");
    }

    void
    Contents::decrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE_METHOD(key);

      ELLE_ASSERT(this->_node == nullptr);
      ELLE_ASSERT(this->_cipher != nullptr);

      this->_node = factory::node().allocate<Node>(this->_nature);

      ELLE_TRACE("decrypting the cipher with the secret key");

      if (key.Decrypt(*this->_cipher, *this->_node) == elle::Status::Error)
        throw Exception("unable to decrypt the cipher");
    }

    Mode
    Contents::mode() const
    {
      return ((this->_node != nullptr) ? Mode::decrypted : Mode::encrypted);
    }

    Node const&
    Contents::node() const
    {
      ELLE_ASSERT(this->_node != nullptr);

      return (*this->_node);
    }

    Node&
    Contents::node()
    {
      ELLE_ASSERT(this->_node != nullptr);

      return (*this->_node);
    }

    Node*
    Contents::cede()
    {
      ELLE_ASSERT(this->_node != nullptr);

      Node* node = this->_node;
      this->_node = nullptr;

      return (node);
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

      if (this->_node != nullptr)
        this->_node->Dump(margin + 2);

      if (this->_cipher != nullptr)
        this->_cipher->Dump(margin + 2);

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Contents::print(std::ostream& stream) const
    {
      stream << this->_nature;

      if (this->_node != nullptr)
        stream << "(" << *this->_node << ")";
    }
  }
}
