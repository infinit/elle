#ifndef NUCLEUS_PROTON_CONTENTS_HH
# define NUCLEUS_PROTON_CONTENTS_HH

# include <elle/cryptography/fwd.hh>

# include <nucleus/proton/ContentHashBlock.hh>
# include <nucleus/neutron/fwd.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

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
    template <typename T>
    class Contents
      : public proton::ContentHashBlock
      , public elle::concept::Serializable<Contents<T>>
      , public elle::concept::Serializable<
            Contents<T>, elle::serialize::BufferArchive
        >
    {
      //
      // constants
      //
    public:
      static const neutron::Component component;

    public:
      //
      // types
      //
      typedef T                 C;

      //
      // constructors & destructors
      //
      Contents();
      ~Contents();

      //
      // methods
      //
      elle::Status      Create();

      elle::Status Encrypt(elle::cryptography::SecretKey const& key);
      elle::Status Decrypt(elle::cryptography::SecretKey const& key);

      //
      // interfaces
      //

      // object
      declare(Contents<T>);

      // fileable
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Contents<T>);
      ELLE_CONCEPT_SERIALIZABLE_METHODS(Contents<T>, elle::serialize::BufferArchive);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      T*                content;

      elle::cryptography::Cipher*     cipher;
    };

  }
}

#include <nucleus/proton/Contents.hxx>

#endif
