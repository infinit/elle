#ifndef NUCLEUS_PROTON_CONTENTS_HH
# define NUCLEUS_PROTON_CONTENTS_HH

# include <nucleus/proton/ContentHashBlock.hh>
# include <nucleus/proton/Node.hh>
# include <nucleus/proton/Address.hh>

# include <cryptography/fwd.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

# include <elle/attribute.hh>
# include <elle/idiom/Open.hh>

// XXX
# include <cryptography/KeyPair.hh>

namespace nucleus
{
  namespace proton
  {
    /// The Contents block has for only purpose to abstract the fact that
    /// Nodes (Quill, Seam and Value instances such as Catalog etc.) must
    /// be serialized in a block in an encrypted form.
    ///
    /// This class therefore provides nothing more than a shell for protecting
    /// a node through encryption.
    ///
    /// Thus, the set of methods provided is quite obvious, enabling one to
    /// encrypt and decrypt the embedded node.
    ///
    /// Such a class is used as follows. Whenever a Contents block is retrieved
    /// from the storage layer, it is deserialized leaving the Contents block
    /// with a ciphered version of the final node. The decrypt() method can then
    /// be called with the proper secret key so as to decrypt and deserialized
    /// the final node.
    class Contents:
      public proton::ContentHashBlock,
      public elle::serialize::SerializableMixin<Contents>,
      public elle::concept::UniquableMixin<Contents>
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static neutron::Component const component;
      };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Contents(); // XXX[to deserialize]
      /// Construct a contents based on the given node which will thus be
      /// embedded. In order to ensure the privacy of the node's data, one
      /// must however call the encrypt() method.
      ///
      /// Note that the ownership of the given node is transferred to the
      /// contents.
      template <typename T>
      Contents(Network const& network,
               cryptography::PublicKey const& creator_K,
               T* node);
      /// Destructor.
      ~Contents();

      // XXX
      template <typename T>
      Contents(T* node): // XXX[TEMPORARY!!! TO ANNIHILATE]
        proton::ContentHashBlock(Network("test"),
                                 neutron::ComponentContents,
                                 cryptography::KeyPair::generate(1024).K()),

        _nature(T::Constants::nature),
        _node(node),
        _cipher(nullptr)
      {
      }
      // XXX

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Encrypt the embedded node with the given key.
      void
      encrypt(cryptography::SecretKey const& key);
      /// Decrypt the embedded node with the given key.
      void
      decrypt(cryptography::SecretKey const& key);
      /// Return the mode of the contents i.e either encrypted or decrypted.
      Mode
      mode() const;
      /// Return the node (in its decrypted form) referenced by the contents.
      Node const&
      node() const;
      /// Return the node (in its decrypted form) referenced by the contents.
      Node&
      node();
      /// Cede the ownership on the node to the caller.
      Node*
      cede();

      /*-----------.
      | Interfaces |
      `-----------*/
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

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      /// Define the nature of the node embedeed in the contents.
      ///
      /// This value is particularly important for dynamically allocating
      /// a final node (i.e with the proper type) given its nature only.
      ELLE_ATTRIBUTE(Nature, nature);
      /// The node having been either dynamically allocated for decrypting
      /// or passed at construction for encrypting.
      ELLE_ATTRIBUTE(Node*, node);
      /// The encrypted form of the node created through the encrypt() method.
      ELLE_ATTRIBUTE(cryptography::Cipher*, cipher);
    };
  }
}

# include <nucleus/proton/Contents.hxx>

#endif
