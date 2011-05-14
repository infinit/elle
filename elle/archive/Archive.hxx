//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/archive/Archive.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [sat may 14 12:32:13 2011]
//

#ifndef ELLE_ARCHIVE_ARCHIVE_HXX
#define ELLE_ARCHIVE_ARCHIVE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;

  namespace archive
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// these templates provide the methods a way to link types to
    /// the enum Type value and a name, which can reveal useful for
    /// debugging.
    ///
    template <const Archive::Type V>
    struct ArchiveType
    {
      static const Byte		Value = V;
      static const Character*	Name;
    };

    ///
    /// this base template allows for specialized-template.
    ///
    template <typename T>
    class ArchiveMap:
      public ArchiveType<Archive::TypeUnknown>
    {
    };

    ///
    /// this macro-function link the type to the enum value in a simple
    /// call.
    ///
#define ArchiveDeclare(_type_)						\
  template <>								\
  class ArchiveMap<_type_>:						\
    public ArchiveType<Archive::Type ## _type_>				\
  {									\
  };

    ///
    /// this macro-function links the type to a name.
    ///
#define ArchiveName(_type_)						\
  template <>								\
  const Character*	ArchiveType<Archive::Type ## _type_>::Name = #_type_;

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for every basic type of the elle library.
    ///
    ArchiveDeclare(Null);
    ArchiveDeclare(Boolean);
    ArchiveDeclare(Character);
    ArchiveDeclare(Real);
    ArchiveDeclare(Integer8);
    ArchiveDeclare(Integer16);
    ArchiveDeclare(Integer32);
    ArchiveDeclare(Integer64);
    ArchiveDeclare(Natural8);
    ArchiveDeclare(Natural16);
    ArchiveDeclare(Natural32);
    ArchiveDeclare(Natural64);
    ArchiveDeclare(String);
    ArchiveDeclare(Large);
    ArchiveDeclare(Region);
    ArchiveDeclare(Archive);

//
// ---------- behaviours ------------------------------------------------------
//

    ///
    /// this method serialize a basic type.
    ///
    template <typename T, Boolean C>
    Status	Archive::Behaviour<T, C>::Serialize(Archive&	archive,
						    const T&	element)
    {
      const Byte	type = ArchiveMap<T>::Value;

      enter();

      // serialization mode only.
      if (archive.mode != Archive::ModeSerialization)
	escape("unable to serialize while not in serialization mode");

      // store the type.
      if (archive.Store(type) == StatusError)
	escape("unable to store the type");

      // store the element.
      if (archive.Store(element) == StatusError)
	escape("unable to store the element");

      leave();
    }

    ///
    /// this method extract a basic type.
    ///
    template <typename T, Boolean C>
    Status	Archive::Behaviour<T, C>::Extract(Archive&	archive,
						  T&		element)
    {
      Byte		type;

      enter();

      // serialization mode only.
      if (archive.mode != Archive::ModeExtraction)
	escape("unable to extract while not in extraction mode");

      // load the type.
      if (archive.Load(type) == StatusError)
	escape("unable to load the type");

      // check the type.
      if (type != ArchiveMap<T>::Value)
	escape("wrong type");

      // load the element.
      if (archive.Load(element) == StatusError)
	escape("unable to load the element");

      leave();
    }

    ///
    /// this method extract a compound type.
    ///
    template <typename T>
    inline Status	Archive::Behaviour<T, true>::Extract(
			  Archive&				archive,
			  Archivable&				element)
    {
      return (element.Extract(archive));
    }

    ///
    /// this method serialize a compound type.
    ///
    /// note that such objects must inherits the Archivable interface,
    /// hence provide the Serialize() and Extract() methods.
    ///
    template <typename T>
    inline Status	Archive::Behaviour<T, true>::Serialize(
			  Archive&				archive,
			  const Archivable&			element)
    {
      return (element.Serialize(archive));
    }

//
// ---------- serialize -------------------------------------------------------
//

    ///
    /// this template serializes a basic type.
    ///
    template <typename T>
    Status		Archive::Serialize(const T&		element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveMap<T>::Value
                              ==
                            Archive::TypeUnknown>::Serialize(*this, element));
    }

    ///
    /// this method serializes a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		Archive::Serialize(const T&		parameter,
					   const TT&...		parameters)
    {
      enter();

      // serialize the first items.
      if (this->Serialize(parameter) == StatusError)
	escape("unable to serialize the first item");

      // serialize the additional items.
      if (this->Serialize(parameters...) == StatusError)
	escape("unable to serialize the additional parameters");

      leave();
    }

//
// ---------- extract ---------------------------------------------------------
//

    ///
    /// this template extracts a basic type.
    ///
    template <typename T>
    Status		Archive::Extract(T&			element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveMap<T>::Value
                              ==
                            Archive::TypeUnknown>::Extract(*this, element));
    }

    ///
    /// this method extracts a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		Archive::Extract(T&			parameter,
					 TT&...			parameters)
    {
      enter();

      // extract the first item.
      if (this->Extract(parameter) == StatusError)
	escape("unable to extract the first item");

      // extract the additional items.
      if (this->Extract(parameters...) == StatusError)
	escape("unable to extract the additional items");

      leave();
    }

//
// ---------- update ----------------------------------------------------------
//

    ///
    /// this method updates a single of item.
    ///
    template <typename T>
    Status		Archive::Update(const Natural64&	offset,
					const T&		parameter)
    {
      Natural64		size;

      enter();

      // serialization mode only.
      if (this->mode != Archive::ModeSerialization)
	escape("unable to update while not in serialization mode");

      // first save the size in order to restore the archive properly later.
      size = this->size;

      // set the size at provided so that serializing can start again
      // from there.
      this->size = offset;

      // update the item.
      if (this->Serialize(parameter) == StatusError)
	escape("unable to update the item");

      // return an error of the new size exceeded the old one.
      if (this->size > size)
	escape("the new size exceeded the previous one");

      // restore the size.
      this->size = size;

      leave();
    }

    ///
    /// this method updates a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		Archive::Update(const Natural64&	offset,
					const T&		parameter,
					const TT&...		parameters)
    {
      Natural64		size;

      enter();

      // serialization mode only.
      if (this->mode != Archive::ModeSerialization)
	escape("unable to update while not in serialization mode");

      // first save the size in order to restore the archive properly later.
      size = this->size;

      // set the size at provided so that serializing can start again
      // from there.
      this->size = offset;

      // update the first item.
      if (this->Serialize(parameter, parameters...) == StatusError)
	escape("unable to update the items");

      // return an error of the new size exceeded the old one.
      if (this->size > size)
	escape("the new size exceeded the previous one");

      // restore the size.
      this->size = size;

      leave();
    }

//
// ---------- store -----------------------------------------------------------
//

    ///
    /// this method records an element of the given type in the archive.
    ///
    template <typename T>
    Status		Archive::Store(const T&			element)
    {
      enter();

      // serialization mode only.
      if (this->mode != Archive::ModeSerialization)
	escape("unable to extract while not in extraction mode");

      // check if this type is a basic type.
      if (ArchiveMap<T>::Value == Archive::TypeUnknown)
	escape("unable to extract value of unknown type");

      // align the size if it is necessary.
      if (this->Align(sizeof(T)) == StatusError)
	escape("unable to align the size");

      // possibly enlarge the archive.
      if (this->Reserve(sizeof(T)) == StatusError)
	escape("unable to reserve space for an upcoming serialization");

      // store the element.
      *((T*)(this->contents + this->size)) = element;

      // update the size.
      this->size += sizeof(T);

      leave();
    }

//
// ---------- load ------------------------------------------------------------
//

    ///
    /// this method loads an element of the given type from the archive.
    ///
    template <typename T>
    Status		Archive::Load(T&			element)
    {
      enter();

      // extraction mode only.
      if (this->mode != Archive::ModeExtraction)
	escape("unable to extract while not in extraction mode");

      // check if this type is a basic type.
      if (ArchiveMap<T>::Value == Archive::TypeUnknown)
	escape("unable to extract value of unknown type");

      // align the offset if it is necessary.
      if (this->Align(sizeof(T)) == StatusError)
	escape("unable to align the offset");

      // load the element.
      element = *((T*)(this->contents + this->offset));

      // update the offset.
      this->offset += sizeof(T);

      leave();
    }

//
// ---------- print -----------------------------------------------------------
//

    ///
    /// this method displays information on an element belonging to an
    /// archive.
    ///
    template <typename T>
    inline Status	Print(const T&				element,
			      const Natural32			margin)
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[" << ArchiveMap<T>::Name << "] "
		<< element << std::endl;

      leave();
    }

    ///
    /// this method specializes the Print() method for the Region type
    /// which is a non-core, non-archivable type, a rare exception.
    ///
    template <>
    inline Status	Print<Region>(const Region&		element,
				      const Natural32		margin)
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Region] "
		<< (Void*)element.contents << " :: "
		<< element.size << "/" << element.capacity << std::endl;

      leave();
    }

  }
}

#endif
