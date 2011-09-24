//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon jan 26 14:09:50 2009]
//

#ifndef ELLE_PACKAGE_ARCHIVE_HXX
#define ELLE_PACKAGE_ARCHIVE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Type.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/radix/Allege.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;

  namespace package
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this base template allows for specialized-template.
    ///
    template <typename T>
    struct ArchiveType
    {
      static const Natural8	Value = Archive::TypeUnknown;
    };

    ///
    /// this macro-function links the type to the enum value in a simple
    /// call.
    ///
#define ArchiveDeclare(_type_, _weight_)				\
  template <>								\
  struct ArchiveType<_type_>						\
  {									\
    static const Natural8			Value =			\
      Archive::Type ## _type_;						\
    static const Natural32			Weight =		\
      _weight_;								\
  };

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for every basic type of the elle library.
    ///
    ArchiveDeclare(Null, 0);
    ArchiveDeclare(Boolean, 1);
    ArchiveDeclare(Character, 1);
    ArchiveDeclare(Real, 8);
    ArchiveDeclare(Integer8, 1);
    ArchiveDeclare(Integer16, 2);
    ArchiveDeclare(Integer32, 4);
    ArchiveDeclare(Integer64, 8);
    ArchiveDeclare(Natural8, 1);
    ArchiveDeclare(Natural16, 2);
    ArchiveDeclare(Natural32, 4);
    ArchiveDeclare(Natural64, 8);
    ArchiveDeclare(Large, 0);
    ArchiveDeclare(String, 0);
    ArchiveDeclare(Region, 0);
    ArchiveDeclare(Archive, 0);

//
// ---------- behaviours ------------------------------------------------------
//

    ///
    /// this method serialize a basic type.
    ///
    /// note that the Elle type is also serialized to make things easier.
    ///
    template <typename T, Boolean C>
    Status	Archive::Behaviour<T, C>::Serialize(Archive&	archive,
						    const T&	element)
    {
      const Natural8	type = ArchiveType<T>::Value;

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
      Natural8	type;

      enter();

      // serialization mode only.
      if (archive.mode != Archive::ModeExtraction)
	escape("unable to extract while not in extraction mode");

      // load the type.
      if (archive.Load(type) == StatusError)
	escape("unable to load the type");

      // check the type.
      if (type != ArchiveType<T>::Value)
	escape("invalid type: next element of type '%u'",
	       type);

      // load the element.
      if (archive.Load(element) == StatusError)
	escape("unable to load the element");

      leave();
    }

    ///
    /// this method returns the weight of a basic type.
    ///
    /// the reader can notice that two bytes are added to every type's
    /// footprint. this comes from the fact that the both the Archive
    /// class and the serialization mechanism record the type of the element
    /// being serialized.
    ///
    template <typename T, Boolean C>
    Natural32	Archive::Behaviour<T, C>::Weight()
    {
      return (ArchiveType<Byte>::Weight +
	      ArchiveType<Natural8>::Weight +
	      ArchiveType<T>::Weight);
    }

    ///
    /// this method returns the footprint of a basic type.
    ///
    template <typename T, Boolean C>
    Natural32	Archive::Behaviour<T, C>::Footprint(const T&)
    {
      return (Archive::Weight<T>());
    }

    ///
    /// this method serialize a compound type.
    ///
    /// note that such objects must inherits the Archivable interface,
    /// hence provide the Serialize() and Extract() methods.
    ///
    template <typename T>
    Status		Archive::Behaviour<T, true>::Serialize(
			  Archive&				archive,
			  const Archivable&			element)
    {
      return (element.Serialize(archive));
    }

    ///
    /// this method extract a compound type.
    ///
    template <typename T>
    Status		Archive::Behaviour<T, true>::Extract(
			  Archive&				archive,
			  Archivable&				element)
    {
      return (element.Extract(archive));
    }

    ///
    /// this method returns the weight of a variable-size or compound type.
    ///
    /// note that this causes an compilation failure because Archive
    /// cannot determine the weight of such types i.e an instance is
    /// required. the Footprint() method should be used instead.
    ///
    template <typename T>
    Natural32		Archive::Behaviour<T, true>::Weight()
    {
      // stop the compiling process.
      allege(false);

      return (0);
    }

    ///
    /// this method returns the footprint of a compound type.
    ///
    template <typename T>
    Natural32		Archive::Behaviour<T, true>::Footprint(
			  const Archivable&			element)
    {
      return (element.Footprint());
    }

//
// ---------- serialize -------------------------------------------------------
//

    ///
    /// this template serializes a single item.
    ///
    template <typename T>
    Status		Archive::Serialize(const T&		element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveType<T>::Value
                              ==
                            Archive::TypeUnknown>::Serialize(*this,
							     element));
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

    ///
    /// this template serializes a single item.
    ///
    template <typename T>
    Status		Archive::Serialize(const T*		element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveType<T>::Value
                              ==
                            Archive::TypeUnknown>::Serialize(*this,
							     *element));
    }

    ///
    /// this method serializes a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		Archive::Serialize(const T*		parameter,
					   const TT*...		parameters)
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
    /// this template extracts a single item.
    ///
    template <typename T>
    Status		Archive::Extract(T&			element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveType<T>::Value
                              ==
                            Archive::TypeUnknown>::Extract(*this,
							   element));
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

    ///
    /// this template extracts a single item.
    ///
    template <typename T>
    Status		Archive::Extract(T*			element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveType<T>::Value
                              ==
                            Archive::TypeUnknown>::Extract(*this,
							   *element));
    }

    ///
    /// this method extracts a set of items.
    ///
    template <typename T,
	      typename... TT>
    Status		Archive::Extract(T*			parameter,
					 TT*...			parameters)
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
// ---------- store -----------------------------------------------------------
//

    ///
    /// this method records an element of the given type in the archive.
    ///
    /// note that this method is used whenever a type-specific one has
    /// not been written. the actual purpose of this method is therefore
    /// to handle all the numeric types: integers and naturals.
    ///
    template <typename T>
    Status		Archive::Store(const T&			element)
    {
      enter();

      // pack the element.
      ::msgpack::pack(this->buffer, element);

      leave();
    }

//
// ---------- load ------------------------------------------------------------
//

    ///
    /// this method loads an element of the given type from the archive.
    ///
    /// note that this method is used whenever a type-specific one has
    /// not been written. the actual purpose of this method is therefore
    /// to handle all the numeric types: integers and naturals.
    ///
    template <typename T>
    Status		Archive::Load(T&			element)
    {
      ::msgpack::unpacked	message;
      ::msgpack::object		object;

      enter();

      // extract the unpacked message.
      ::msgpack::unpack(&message,
			reinterpret_cast<const char*>(this->contents),
			this->size,
			&this->offset);

      // retrieve the object.
      object = message.get();

      // check the object's type.
      if ((object.type != ::msgpack::type::POSITIVE_INTEGER) &&
	  (object.type != ::msgpack::type::NEGATIVE_INTEGER))
	escape("the next element does not seem to be a numeric type");

      // extract the element.
      object >> element;

      leave();
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method recycles an archive.
    ///
    template <typename T>
    Status		Archive::Recycle(const T*		object)
    {
      // release the resources.
      this->~Archive();

      if (object == NULL)
	{
	  // initialize the object with default values.
	  new (this) T;
	}
      else
	{
	  // initialize the object with defined values.
	  new (this) T(*object);
	}

      // return StatusOk in order to avoid including Report, Status and Maid.
      return (StatusOk);
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this template returns the weight of a single type i.e once serialized.
    ///
    template <typename T>
    Natural32		Archive::Weight()
    {
      return
	(Archive::Behaviour<T,
			    (ArchiveType<T>::Value
			       ==
			     ArchiveType<Large>::Value) ||
			    (ArchiveType<T>::Value
			       ==
			     ArchiveType<String>::Value) ||
			    (ArchiveType<T>::Value
			       ==
			     ArchiveType<Region>::Value) ||
			    (ArchiveType<T>::Value
			       ==
			     ArchiveType<Archive>::Value) ||
	                    (ArchiveType<T>::Value
			       ==
			     Archive::TypeUnknown)>::Weight());
    }

    ///
    /// this method returns the weight of a set of types.
    ///
    template <typename T,
	      typename TT,
	      typename... TTT>
    Natural32		Archive::Weight()
    {
      return (Archive::Weight<T>() +
	      Archive::Weight<TT, TTT...>());
    }

    ///
    /// this template returns the footprint of a single item.
    ///
    template <typename T>
    Natural32		Archive::Footprint(const T&		element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveType<T>::Value
                              ==
                            Archive::TypeUnknown>::Footprint(element));
    }

    ///
    /// this method returns the footprint of a set of items.
    ///
    template <typename T,
	      typename... TT>
    Natural32		Archive::Footprint(const T&		parameter,
					   const TT&...		parameters)
    {
      return (Archive::Footprint(parameter) +
	      Archive::Footprint(parameters...));
    }

    ///
    /// this template returns the footprint of a single item.
    ///
    template <typename T>
    Natural32		Archive::Footprint(const T*		element)
    {
      return
	(Archive::Behaviour<T,
	                    ArchiveType<T>::Value
                              ==
                            Archive::TypeUnknown>::Footprint(*element));
    }

    ///
    /// this method returns the footprint of a set of items.
    ///
    template <typename T,
	      typename... TT>
    Natural32		Archive::Footprint(const T*		parameter,
					   const TT*...		parameters)
    {
      return (Archive::Footprint(parameter) +
	      Archive::Footprint(parameters...));
    }

    ///
    /// this method displays information on an element belonging to an
    /// archive.
    ///
    template <typename T>
    Status		Archive::Print(const T&			element,
				       const Natural32		margin)
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[" << core::Type<T>::Name << "] "
		<< element << std::endl;

      leave();
    }

  }
}

#endif
