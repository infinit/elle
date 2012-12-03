#include <nucleus/proton/Shell.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Shell");

namespace nucleus
{
  namespace proton
  {
    /*--------.
    | Methods |
    `--------*/

    void
    Shell::encrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE("encrypting the node with the secret key");

      ELLE_ASSERT(this->_node != nullptr);
      ELLE_ASSERT(this->_cipher == nullptr);

      this->_cipher = new cryptography::Cipher;

      if (key.Encrypt(*this->_node, *this->_cipher) == elle::Status::Error)
        throw Exception("unable to encrypt the archived block");
    }

    void
    Shell::decrypt(cryptography::SecretKey const& key)
    {
      ELLE_ASSERT(this->_node == nullptr);
      ELLE_ASSERT(this->_cipher != nullptr);

      this->_node = factory::node().allocate<Node>(this->_breed);

      ELLE_TRACE("decrypting the cipher with the secret key");

      if (key.Decrypt(*this->_cipher, *this->_node) == elle::Status::Error)
        throw Exception("unable to decrypt the cipher");
    }

    Mode
    Shell::mode() const
    {
      return ((this->_node != nullptr) ? Mode::decrypted : Mode::encrypted);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Shell::print(std::ostream& stream) const
    {
      stream << this->_breed;

      if (this->_node != nullptr)
        stream << "(" << *this->_node << ")";
    }
  }
}
