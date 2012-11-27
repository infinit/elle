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
    | Functions |
    `----------*/

    static
    elle::utility::Factory<Contents::Type> const*
    _setup()
    {
      elle::utility::Factory<Contents::Type>* factory =
        new elle::utility::Factory<Contents::Type>;

      ELLE_TRACE("setting up the node factory");

      factory->record<Seam<neutron::Catalog>>(Contents::Type::catalog_seam);
      factory->record<Quill<neutron::Catalog>>(Contents::Type::catalog_quill);
      factory->record<neutron::Catalog>(Contents::Type::catalog_value);

      return (factory);
    }

    /*---------------.
    | Static Methods |
    `---------------*/

    elle::utility::Factory<Contents::Type> const&
    Contents::factory()
    {
      static elle::utility::Factory<Contents::Type> const* factory = _setup();

      ELLE_ASSERT(factory != nullptr);

      return (*factory);
    }

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

      delete this->_cipher;
      this->_cipher = new cryptography::Cipher;

      ELLE_TRACE("encrypting the node with the secret key");

      if (key.Encrypt(*this->_node, *this->_cipher) == elle::Status::Error)
        throw Exception("unable to encrypt the archived block");
    }

    void
    Contents::decrypt(cryptography::SecretKey const& key)
    {
      ELLE_TRACE_METHOD(key);

      this->_node = Contents::factory().allocate<Node>(this->_type);

      assert(this->_cipher != nullptr);
      assert(this->_node != nullptr);

      ELLE_TRACE("decrypting the cipher");

      if (key.Decrypt(*this->_cipher, *this->_node) == elle::Status::Error)
        throw Exception("unable to decrypt the cipher");
    }

    typename Contents::Mode
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

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Contents::Dump(elle::Natural32          margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Contents]" << std::endl;

      // dump the parent class.
      if (proton::ContentHashBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Type] " << this->_type << std::endl;

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

    /*----------.
    | Printable |
    `----------*/

    void
    Contents::print(std::ostream& stream) const
    {
      stream << "contents(" << this->_type << ")";
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Contents::Type const type)
    {
      switch (type)
        {
        case Contents::Type::data_seam:
          {
            stream << "data(seam)";
            break;
          }
        case Contents::Type::data_quill:
          {
            stream << "data(quill)";
            break;
          }
        case Contents::Type::data_value:
          {
            stream << "data(value)";
            break;
          }
        case Contents::Type::catalog_seam:
          {
            stream << "catalog(seam)";
            break;
          }
        case Contents::Type::catalog_quill:
          {
            stream << "catalog(quill))";
            break;
          }
        case Contents::Type::catalog_value:
          {
            stream << "catalog(value)";
            break;
          }
        case Contents::Type::reference_seam:
          {
            stream << "reference(seam)";
            break;
          }
        case Contents::Type::reference_quill:
          {
            stream << "reference(quill)";
            break;
          }
        case Contents::Type::reference_value:
          {
            stream << "reference(value)";
            break;
          }
        default:
          {
            throw Exception("unknown type: '%s'", static_cast<int>(type));
          }
        }

      return (stream);
    }

    std::ostream&
    operator <<(std::ostream& stream,
                Contents::Mode const mode)
    {
      switch (mode)
        {
        case Contents::Mode::encrypted:
          {
            stream << "encrypted";
            break;
          }
        case Contents::Mode::decrypted:
          {
            stream << "decrypted";
            break;
          }
        default:
          {
            throw Exception("unknown mode: '%s'", static_cast<int>(mode));
          }
        }

      return (stream);
    }
  }
}
