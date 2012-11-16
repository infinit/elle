#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Node.hh>
#include <nucleus/neutron/Component.hh>
#include <nucleus/Exception.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Contents");

namespace nucleus
{
  namespace proton
  {

//
// ---------- constants -------------------------------------------------------
//

    const neutron::Component Contents::component = neutron::ComponentContents;

//
// ---------- constructors & destructors --------------------------------------
//

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

//
// ---------- methods ---------------------------------------------------------
//

    /// XXX
    /// this method takes the contents in its block form, serialises it
    /// and encrypts the archive.
    ///
    elle::Status
    Contents::encrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE_SCOPE("encrypt(%s)", key);

      ELLE_TRACE("allocating a cipher");

      delete this->_cipher;
      this->_cipher = new cryptography::Cipher;

      ELLE_TRACE("encrypting the node with the secret key")
        {
          if (key.Encrypt(*this->_node, *this->_cipher) == elle::Status::Error)
            escape("unable to encrypt the archived block");
        }

      return elle::Status::Ok;
    }

    /// XXX
    /// this method takes the cipher extracted before, decrypts it with the
    /// given key, creating a new block.
    ///
    elle::Status
    Contents::decrypt(cryptography::SecretKey const& key)
    {
      static const elle::Boolean useless = Node::setup();
      (void)useless;

      ELLE_TRACE_SCOPE("decrypt(%s)", key);

      ELLE_TRACE("allocating a node of type '%s'", this->_type);

      this->_node = Node::factory.allocate<Node>(this->_type);

      assert(this->_cipher != nullptr);
      assert(this->_node != nullptr);

      ELLE_TRACE("decrypting the cipher")
        {
          if (key.Decrypt(*this->_cipher, *this->_node) == elle::Status::Error)
            escape("unable to decrypt the cipher");
        }

      return elle::Status::Ok;
    }

    typename Contents::Mode
    Contents::mode() const
    {
      return ((this->_node != nullptr) ? ModeDecrypted : ModeEncrypted);
    }

    Node*
    Contents::node()
    {
      if (this->_node == nullptr)
        throw Exception("the Contents does not seam to have been "
                        "created/decrypted");

      return (this->_node);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents.
    ///
    elle::Status
    Contents::Dump(elle::Natural32          margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Contents]" << std::endl;

      // dump the parent class.
      if (proton::ContentHashBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Type] " << std::dec << this->_type << std::endl;

      if (this->_node != nullptr)
        {
          if (this->_node->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the node");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Node] " << elle::none << std::endl;
        }

      // if present, dump the cipher.
      if (this->_cipher != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Cipher]" << std::endl;

          if (this->_cipher->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the cipher");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Cipher] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Contents::print(std::ostream& stream) const
    {
      stream << "contents(" << this->_type << ")";
    }

  }
}
