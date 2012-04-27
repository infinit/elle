#ifndef NUCLEUS_PROTON_CONTENTS_HXX
# define NUCLEUS_PROTON_CONTENTS_HXX

# include <elle/cryptography/Cipher.hh>

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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename T>
    Contents<T>::Contents():
      proton::ContentHashBlock(ContentsMap<T>::Component),

      content(NULL),
      cipher(NULL)
    {
    }

    ///
    /// the destructor releases the allocated resources.
    ///
    template <typename T>
    Contents<T>::~Contents()
    {
      // release the block, if present.
      if (this->content != NULL)
        delete this->content;

      // release the cipher, if present.
      if (this->cipher != NULL)
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
      if (this->content == NULL)
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
      if (this->cipher == NULL)
        escape("unable to decrypt a non-existing cipher");

      // if the block already exist, delete it.
      if (this->content != NULL)
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
      if (this->content != NULL)
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Content]" << std::endl;

          if (this->content->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the content");
        }
      else
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Content] " << elle::none << std::endl;
        }

      // if present, dump the cipher.
      if (this->cipher != NULL)
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Cipher]" << std::endl;

          if (this->cipher->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the cipher");
        }
      else
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Cipher] " << elle::none << std::endl;
        }

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the contents by serializing the object,
    /// creating the cipher attribute corresponding to the block in its
    /// encrypted form.
    ///
    //template <typename T>
    //elle::Status        Contents<T>::Serialize(elle::Archive&   archive) const
    //{
    //  // check if the block's ciphered version is ready.
    //  if (this->cipher == NULL)
    //    escape("unable to serialize an unciphered content");

    //  // call the parent class.
    //  if (proton::ContentHashBlock::Serialize(archive) == elle::Status::Error)
    //    escape("unable to serialize the underlying CHB");

    //  // just serialize the cipher in the archive.
    //  if (archive.Serialize(*this->cipher) == elle::Status::Error)
    //    escape("unable to serialize the cipher");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the contents by extracting a cipher i.e the
    ///// block in its encrypted form.
    /////
    //template <typename T>
    //elle::Status        Contents<T>::Extract(elle::Archive&     archive)
    //{
    //  // call the parent class.
    //  if (proton::ContentHashBlock::Extract(archive) == elle::Status::Error)
    //    escape("unable to extract the underlying CHB");

    //  /* XXX[to remove]
    //  // compare the component.
    //  if (this->component != ContentsMap<T>::Component)
    //    escape("the archive does not seem to contain a content");
    //  */

    //  // allocate a new cipher.
    //  this->cipher = new elle::cryptography::Cipher;

    //  // extract the cipher from the archive.
    //  if (archive.Extract(*this->cipher) == elle::Status::Error)
    //    escape("unable to serialize the cipher");

    //  return elle::Status::Ok;
    //}

  }
}

#endif
