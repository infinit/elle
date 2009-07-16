//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/archive/Archive.hxx
//
// created       julien quintard   [mon jan 26 14:09:50 2009]
// updated       julien quintard   [sat mar 28 15:57:12 2009]
//

#ifndef ELLE_ARCHIVE_ARCHIVE_HXX
#define ELLE_ARCHIVE_ARCHIVE_HXX

namespace elle
{
  using namespace core;
  using namespace misc;

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
    struct Map
    {
      static const Byte		value = V;
      static const Character*	name;
    };

    ///
    /// this base template allows for specialized-template.
    ///
    template <typename T>
    class Resolve:
      public Map<Archive::TypeUnknown>
    {
    };

    ///
    /// this macro-function link the type to the enum value in a simple
    /// call.
    ///
#define ArchiveDeclare(_type_)						\
  template <>								\
  class Resolve<_type_>:						\
    public Map<Archive::Type ## _type_>					\
  {									\
  };

    ///
    /// this macro-function link the type to a name.
    ///
#define ArchiveName(_type_)						\
  template <>								\
  const Character*	Map<Archive::Type ## _type_>::name = #_type_;

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for every basic type of the elle library.
    ///
    ArchiveDeclare(Void);
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
      const Byte	type = Resolve<T>::value;

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

      // serialization mode only.
      if (archive.mode != Archive::ModeExtraction)
	escape("unable to serialize while not in serialization mode");

      // load the type.
      if (archive.Load(type) == StatusError)
	escape("unable to load the type");

      // check the type.
      if (type != Resolve<T>::value)
	escape("wrong type");

      // load the element.
      if (archive.Load(element) == StatusError)
	escape("unable to load the element");

      leave();
    }

    ///
    /// this method serialize a compound type.
    ///
    /// note that such objects must inherits the Archivable interface,
    /// hence provide the Serialize() and Extract() methods.
    ///
    template <typename T>
    inline Status	Archive::Behaviour<T, true>::Serialize(Archive&	archive,
							       const Archivable& element)
    {
      return (element.Serialize(archive));
    }

    ///
    /// this method extract a compound type.
    ///
    template <typename T>
    inline Status	Archive::Behaviour<T, true>::Extract(Archive& archive,
							     Archivable& element)
    {
      return (element.Extract(archive));
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
	                    Resolve<T>::value
                              ==
                            Archive::TypeUnknown>::Serialize(*this, element));
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
	                    Resolve<T>::value
                              ==
                            Archive::TypeUnknown>::Extract(*this, element));
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
      // serialization mode only.
      if (this->mode != Archive::ModeSerialization)
	escape("unable to extract while not in extraction mode");

      // check if this type is a basic type.
      if (Resolve<T>::value == Archive::TypeUnknown)
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
      // extraction mode only.
      if (this->mode != Archive::ModeExtraction)
	escape("unable to extract while not in extraction mode");

      // check if this type is a basic type.
      if (Resolve<T>::value == Archive::TypeUnknown)
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
// ---------- show ------------------------------------------------------------
//

    template <typename T>
    Status		Show(const T&				element,
			     const Natural32			margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[" << Resolve<T>::name << "] " << element << std::endl;

      leave();
    }

//
// ---------- variadic templates ----------------------------------------------
//

    ///
    /// these methods make it easier to serialize/extract multiple items at
    /// the same time while keeping a way to catch errors.
    ///

    //
    // serialize
    //

    template <typename T1,
	      typename T2>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2)
    {
      return (this->Serialize(t1, t2,
			      vacuum, vacuum, vacuum, vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3)
    {
      return (this->Serialize(t1, t2, t3,
			      vacuum, vacuum, vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3,
					   const T4&		t4)
    {
      return (this->Serialize(t1, t2, t3, t4,
			      vacuum, vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3,
					   const T4&		t4,
					   const T5&		t5)
    {
      return (this->Serialize(t1, t2, t3, t4, t5,
			      vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3,
					   const T4&		t4,
					   const T5&		t5,
					   const T6&		t6)
    {
      return (this->Serialize(t1, t2, t3, t4, t5, t6,
			      vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3,
					   const T4&		t4,
					   const T5&		t5,
					   const T6&		t6,
					   const T7&		t7)
    {
      return (this->Serialize(t1, t2, t3, t4, t5, t6, t7,
			      vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3,
					   const T4&		t4,
					   const T5&		t5,
					   const T6&		t6,
					   const T7&		t7,
					   const T8&		t8)
    {
      return (this->Serialize(t1, t2, t3, t4, t5, t6, t7, t8,
			      vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		Archive::Serialize(const T1&		t1,
					   const T2&		t2,
					   const T3&		t3,
					   const T4&		t4,
					   const T5&		t5,
					   const T6&		t6,
					   const T7&		t7,
					   const T8&		t8,
					   const T9&		t9)
    {
      if (Resolve<T1>::value != Resolve<Void>::value)
	if (this->Serialize(t1) == StatusError)
	  escape("unable to serialize the first element");

      if (Resolve<T2>::value != Resolve<Void>::value)
	if (this->Serialize(t2) == StatusError)
	  escape("unable to serialize the second element");

      if (Resolve<T3>::value != Resolve<Void>::value)
	if (this->Serialize(t3) == StatusError)
	  escape("unable to serialize the third element");

      if (Resolve<T4>::value != Resolve<Void>::value)
	if (this->Serialize(t4) == StatusError)
	  escape("unable to serialize the fourth element");

      if (Resolve<T5>::value != Resolve<Void>::value)
	if (this->Serialize(t5) == StatusError)
	  escape("unable to serialize the fifth element");

      if (Resolve<T6>::value != Resolve<Void>::value)
	if (this->Serialize(t6) == StatusError)
	  escape("unable to serialize the sixth element");

      if (Resolve<T7>::value != Resolve<Void>::value)
	if (this->Serialize(t7) == StatusError)
	  escape("unable to serialize the seventh element");

      if (Resolve<T8>::value != Resolve<Void>::value)
	if (this->Serialize(t8) == StatusError)
	  escape("unable to serialize the eighth element");

      if (Resolve<T9>::value != Resolve<Void>::value)
	if (this->Serialize(t9) == StatusError)
	  escape("unable to serialize the ninth element");

      leave();
    }

    //
    // extract
    //

    template <typename T1,
	      typename T2>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2)
    {
      return (this->Extract(t1, t2,
			    vacuum, vacuum, vacuum, vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3)
    {
      return (this->Extract(t1, t2, t3,
			    vacuum, vacuum, vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3,
					 T4&			t4)
    {
      return (this->Extract(t1, t2, t3, t4,
			    vacuum, vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3,
					 T4&			t4,
					 T5&			t5)
    {
      return (this->Extract(t1, t2, t3, t4, t5,
			    vacuum, vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3,
					 T4&			t4,
					 T5&			t5,
					 T6&			t6)
    {
      return (this->Extract(t1, t2, t3, t4, t5, t6,
			    vacuum, vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3,
					 T4&			t4,
					 T5&			t5,
					 T6&			t6,
					 T7&			t7)
    {
      return (this->Extract(t1, t2, t3, t4, t5, t6, t7,
			    vacuum, vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3,
					 T4&			t4,
					 T5&			t5,
					 T6&			t6,
					 T7&			t7,
					 T8&			t8)
    {
      return (this->Extract(t1, t2, t3, t4, t5, t6, t7, t8,
			    vacuum));
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		Archive::Extract(T1&			t1,
					 T2&			t2,
					 T3&			t3,
					 T4&			t4,
					 T5&			t5,
					 T6&			t6,
					 T7&			t7,
					 T8&			t8,
					 T9&			t9)
    {
      if (Resolve<T1>::value != Resolve<Void>::value)
	if (this->Extract(t1) == StatusError)
	  escape("unable to extract the first element");

      if (Resolve<T2>::value != Resolve<Void>::value)
	if (this->Extract(t2) == StatusError)
	  escape("unable to extract the second element");

      if (Resolve<T3>::value != Resolve<Void>::value)
	if (this->Extract(t3) == StatusError)
	  escape("unable to extract the third element");

      if (Resolve<T4>::value != Resolve<Void>::value)
	if (this->Extract(t4) == StatusError)
	  escape("unable to extract the fourth element");

      if (Resolve<T5>::value != Resolve<Void>::value)
	if (this->Extract(t5) == StatusError)
	  escape("unable to extract the fifth element");

      if (Resolve<T6>::value != Resolve<Void>::value)
	if (this->Extract(t6) == StatusError)
	  escape("unable to extract the sixth element");

      if (Resolve<T7>::value != Resolve<Void>::value)
	if (this->Extract(t7) == StatusError)
	  escape("unable to extract the seventh element");

      if (Resolve<T8>::value != Resolve<Void>::value)
	if (this->Extract(t8) == StatusError)
	  escape("unable to extract the eighth element");

      if (Resolve<T9>::value != Resolve<Void>::value)
	if (this->Extract(t9) == StatusError)
	  escape("unable to extract the ninth element");

      leave();
    }

  }
}

#endif
