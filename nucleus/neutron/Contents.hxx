//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Contents.hxx
//
// created       julien quintard   [sun jan 31 21:15:18 2010]
// updated       julien quintard   [wed sep  7 11:10:56 2011]
//

#ifndef NUCLEUS_NEUTRON_CONTENTS_HXX
#define NUCLEUS_NEUTRON_CONTENTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Data.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Reference.hh>
#include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace neutron
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
#define ContentsDeclare(_type_)						\
  template <>								\
  struct ContentsMap<_type_>						\
  {									\
    static const enum Component		Component =			\
      Component ## _type_;						\
  };

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for content type.
    ///
    ContentsDeclare(Data);
    ContentsDeclare(Catalog);
    ContentsDeclare(Reference);

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
    elle::Status	Contents<T>::Encrypt(const elle::SecretKey& key)
    {
      elle::Archive	archive;

      enter();

      // if there is no block, this operation cannot be performed.
      if (this->content == NULL)
	escape("unable to encrypt a non-existing block");

      // if the block already exist, delete it.
      if (this->cipher != NULL)
	delete this->cipher;

      // allocate a new cipher.
      this->cipher = new elle::Cipher;

      // create the archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // serialize the block.
      if (this->content->Serialize(archive) == elle::StatusError)
	escape("unable to serialize the block");

      // encrypt the archive with the given secret key.
      if (key.Encrypt(archive, *cipher) == elle::StatusError)
	escape("unable to encrypt the archived block");

      leave();
    }

    ///
    /// this method takes the cipher extracted before, decrypts it with the
    /// given key, creating a new block.
    ///
    template <typename T>
    elle::Status	Contents<T>::Decrypt(const elle::SecretKey& key)
    {
      elle::Archive	archive;
      elle::Clear	clear;

      enter();

      // if there is no cipher, this operation cannot be performed.
      if (this->cipher == NULL)
	escape("unable to decrypt a non-existing cipher");

      // if the block already exist, delete it.
      if (this->content != NULL)
	delete this->content;

      // allocate a new block.
      this->content = new T;

      // decrypt the cipher.
      if (key.Decrypt(*this->cipher, clear) == elle::StatusError)
	escape("unable to decrypt the cipher");

      // prepare the archive with the clear, which is basically a region.
      if (archive.Acquire(clear) == elle::StatusError)
	escape("unable to prepare the archive");

      // detach the 'clear' region as it will be taken over by the archive.
      if (clear.Detach() == elle::StatusError)
	escape("unable to detach the region from the clear");

      // extract the block.
      if (archive.Extract(*this->content) == elle::StatusError)
	escape("unable to extract the block");

      leave();
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
    elle::Status	Contents<T>::Create()
    {
      enter();

      // allocate the block.
      this->content = new T;

      leave();
    }

    ///
    /// this method dumps the contents.
    ///
    template <typename T>
    elle::Status	Contents<T>::Dump(elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Contents]" << std::endl;

      // dump the parent class.
      if (proton::ContentHashBlock::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the underlying block");

      // if present, dump the content.
      if (this->content != NULL)
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Content]" << std::endl;

	  if (this->content->Dump(margin + 4) == elle::StatusError)
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

	  if (this->cipher->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the cipher");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Cipher] " << elle::none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the contents by serializing the object,
    /// creating the cipher attribute corresponding to the block in its
    /// encrypted form.
    ///
    template <typename T>
    elle::Status	Contents<T>::Serialize(elle::Archive&	archive) const
    {
      enter();

      // check if the block's ciphered version is ready.
      if (this->cipher == NULL)
	escape("unable to serialize an unciphered content");

      // call the parent class.
      if (proton::ContentHashBlock::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying CHB");

      // just serialize the cipher in the archive.
      if (archive.Serialize(*this->cipher) == elle::StatusError)
	escape("unable to serialize the cipher");

      leave();
    }

    ///
    /// this method extracts the contents by extracting a cipher i.e the
    /// block in its encrypted form.
    ///
    template <typename T>
    elle::Status	Contents<T>::Extract(elle::Archive&	archive)
    {
      enter();

      // call the parent class.
      if (proton::ContentHashBlock::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying CHB");

      // compare the component.
      if (this->component != ContentsMap<T>::Component)
	escape("the archive does not seem to contain a content");

      // allocate a new cipher.
      this->cipher = new elle::Cipher;

      // extract the cipher from the archive.
      if (archive.Extract(*this->cipher) == elle::StatusError)
	escape("unable to serialize the cipher");

      leave();
    }

  }
}

#endif
