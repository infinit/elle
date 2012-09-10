#ifndef NUCLEUS_PROTON_CONTENTS_HXX
# define NUCLEUS_PROTON_CONTENTS_HXX

# include <elle/cryptography/Cipher.hh>
# include <elle/cryptography/SecretKey.hh>

# include <nucleus/neutron/Data.hh>
# include <nucleus/neutron/Catalog.hh>
# include <nucleus/neutron/Reference.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this base template allows for specialized-template.
    ///
    template <typename T>
    struct ContentsMap
    {
    };

    ///
    /// this macro-function associates a component to a content type
    /// such as ComponentData with the Data class.
    ///
#define ContentsDeclare(_type_, _component_)                            \
  template <>                                                           \
  struct ContentsMap<_type_>                                            \
  {                                                                     \
    static const enum neutron::Component        Component =             \
      neutron::Component ## _component_;                                \
  };

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for content type.
    ///
    ContentsDeclare(neutron::Data, Data);
    ContentsDeclare(neutron::Catalog, Catalog);
    ContentsDeclare(neutron::Reference, Reference);

//
// ---------- constants -------------------------------------------------------
//

    template <typename T>
    const neutron::Component Contents<T>::component = T::component;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename T>
    Contents<T>::Contents():
      proton::ContentHashBlock(ContentsMap<T>::Component),

      content(nullptr),
      cipher(nullptr)
    {
    }

    ///
    /// the destructor releases the allocated resources.
    ///
    template <typename T>
    Contents<T>::~Contents()
    {
      // release the block, if present.
      if (this->content != nullptr)
        delete this->content;

      // release the cipher, if present.
      if (this->cipher != nullptr)
        delete this->cipher;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes the contents in its block form, serialises it
    /// and encrypts the archive.
    ///
    template <typename T>
    elle::Status Contents<T>::Encrypt(elle::cryptography::SecretKey const& key)
    {
      // if there is no block, this operation cannot be performed.
      if (this->content == nullptr)
        escape("unable to encrypt a non-existing block");

      // if the block already exist, delete it.
      delete this->cipher;
      this->cipher = nullptr;

      // allocate a new cipher.
      this->cipher = new elle::cryptography::Cipher;

      // encrypt the archive with the given secret key.
      if (key.Encrypt(*this->content, *cipher) == elle::Status::Error)
        escape("unable to encrypt the archived block");

      return elle::Status::Ok;
    }

    ///
    /// this method takes the cipher extracted before, decrypts it with the
    /// given key, creating a new block.
    ///
    template <typename T>
    elle::Status Contents<T>::Decrypt(elle::cryptography::SecretKey const& key)
    {
      // if there is no cipher, this operation cannot be performed.
      if (this->cipher == nullptr)
        escape("unable to decrypt a non-existing cipher");

      // if the block already exist, delete it.
      if (this->content != nullptr)
        delete this->content;

      // allocate a new block.
      this->content = new T(*this);

      // decrypt the cipher.
      if (key.Decrypt(*this->cipher, *this->content) == elle::Status::Error)
        escape("unable to decrypt the cipher");

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Contents<T>, _(template <typename T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method simply allocates the fundamental attributes.
    ///
    template <typename T>
    elle::Status Contents<T>::Create()
    {
      // allocate the block.
      this->content = new T(*this);

      // create the content.
      if (this->content->Create() == elle::Status::Error)
        escape("unable to create the content");

      return elle::Status::Ok;
    }

    ///
    /// this method dumps the contents.
    ///
    template <typename T>
    elle::Status        Contents<T>::Dump(elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Contents]" << std::endl;

      // dump the parent class.
      if (proton::ContentHashBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      // if present, dump the content.
      if (this->content != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Content]" << std::endl;

          if (this->content->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the content");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Content] " << elle::none << std::endl;
        }

      // if present, dump the cipher.
      if (this->cipher != nullptr)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Cipher]" << std::endl;

          if (this->cipher->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the cipher");
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Cipher] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SIMPLE_T1(nucleus::proton::Contents, ar, value, version)
{
  enforce(version == 0);
#undef base_class
  ar & base_class< ::nucleus::proton::ContentHashBlock>(value);
  ar & elle::serialize::alive_pointer(value.cipher);
}

#endif
