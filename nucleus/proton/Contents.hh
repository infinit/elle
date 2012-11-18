#ifndef NUCLEUS_PROTON_CONTENTS_HH
# define NUCLEUS_PROTON_CONTENTS_HH

# include <cryptography/fwd.hh>

# include <nucleus/proton/ContentHashBlock.hh>
# include <nucleus/proton/Node.hh>
# include <nucleus/proton/Address.hh>

# include <elle/idiom/Open.hh>

// XXX
# include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

namespace nucleus
{
  namespace proton
  {

    ///
    /// XXX[boxes]
    ///
    /// this class abstracts the data, catalog or reference by embedding
    /// it since the contents is always encrypted. XXX[will soon be overwritten
    /// by the porcupine code]
    ///
    /// therefore, once a contents is retrieved from the storage, the Extract()
    /// method is called which basically extracts an archive i.e the encrypted
    /// version of the embedded block. Then the Decrypt() method can be called
    /// in order to (i) decrypt the embedded archive (ii) extract it (iii)
    /// build the embedded unencrypted object.
    ///
    /// the _cipher_ attribute contains the data once encrypted, often
    /// just before being stored in the storage layer since there is no
    /// benefit in encrypting the data for fun.
    ///
    /// XXX expliquer que contents ca represente un block de donnee chiffree
    ///     qui peut potentiellement depasser la taille (extent) et qui sera
    ///     donc split ou merge si trop petit.
    ///     pour cette raison cette classe contient des methodes communes
    ///     a beaucoup de sous-classes: Seam, Quill, Catalog etc.
    ///
    class Contents:
      public proton::ContentHashBlock,
      public elle::serialize::SerializableMixin<Contents>,
      public elle::concept::UniquableMixin<Contents>
    {
      //
      // constants
      //
    public:
      static const neutron::Component _component;

      //
      // enumerations
      //
    public:
      enum Mode
        {
          ModeEncrypted,
          ModeDecrypted
        };

      //
      // constructors & destructors
      //
    public:
      Contents(); // XXX[to deserialize]
      template <typename T>
      Contents(Network const& network,
               cryptography::PublicKey const& creator_K,
               T*); // XXX[use unique_ptr]
      ~Contents();

      // XXX
      template <typename T>
      Contents(T* node): // XXX[TEMPORARY!!! TO ANNIHILATE]
        proton::ContentHashBlock(Network("local"),
                                 neutron::ComponentContents,
                                 cryptography::KeyPair::generate(1024).K()),

        _type(T::Constants::type),
        _node(node),
        _cipher(nullptr)
      {
      }
      // XXX

      //
      // methods
      //
    public:
      /// XXX
      elle::Status
      encrypt(cryptography::SecretKey const& key);
      /// XXX
      elle::Status
      decrypt(cryptography::SecretKey const& key);
      /// XXX
      Mode
      mode() const;
      /// XXX
      Node*
      node();

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Contents);
      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Contents);
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
    private:
      Node::Type _type;
      Node* _node;
      cryptography::Cipher* _cipher;
    };

  }
}

# include <nucleus/proton/Contents.hxx>

#endif
