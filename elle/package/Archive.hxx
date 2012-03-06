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
      static const Natural8     Value = Archive::TypeUnknown;
    };

    ///
    /// this macro-function links the type to the enum value in a simple
    /// call.
    ///
#define ArchiveDeclare(_type_)                                          \
  template <>                                                           \
  struct ArchiveType<_type_>                                            \
  {                                                                     \
    static const Natural8                       Value =                 \
      Archive::Type ## _type_;                                          \
  };

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
    ArchiveDeclare(Large);
    ArchiveDeclare(String);
    ArchiveDeclare(Region);
    ArchiveDeclare(Archive);

//
// ---------- behaviours ------------------------------------------------------
//

    ///
    /// this method serialize a basic type.
    ///
    /// note that the Elle type is also serialized to make things easier.
    ///
    template <typename T, Boolean C>
    Status      Archive::Behaviour<T, C>::Serialize(Archive&    archive,
                                                    const T&    element)
    {
      const Natural8    type = ArchiveType<T>::Value;

      // serialization mode only.
      if (archive.mode != Archive::ModeSerialization)
        escape("unable to serialize while not in serialization mode");

      // store the type.
      if (archive.Store(type) == StatusError)
        escape("unable to store the type");

      // store the element.
      if (archive.Store(element) == StatusError)
        escape("unable to store the element");

      return StatusOk;
    }

    ///
    /// this method extract a basic type.
    ///
    template <typename T, Boolean C>
    Status      Archive::Behaviour<T, C>::Extract(Archive&      archive,
                                                  T&            element)
    {
      Natural8  type;

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

      return StatusOk;
    }

    ///
    /// this method serialize a compound type.
    ///
    /// note that such objects must inherits the Archivable interface,
    /// hence provide the Serialize() and Extract() methods.
    ///
    template <typename T>
    Status              Archive::Behaviour<T, true>::Serialize(
                          Archive&                              archive,
                          const Archivable&                     element)
    {
      return (element.Serialize(archive));
    }

    ///
    /// this method extract a compound type.
    ///
    template <typename T>
    Status              Archive::Behaviour<T, true>::Extract(
                          Archive&                              archive,
                          Archivable&                           element)
    {
      return (element.Extract(archive));
    }

//
// ---------- serialize -------------------------------------------------------
//

    ///
    /// this template serializes a single item.
    ///
    template <typename T>
    Status              Archive::Serialize(const T&             element)
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
    Status              Archive::Serialize(const T&             parameter,
                                           const TT&...         parameters)
    {
      // serialize the first items.
      if (this->Serialize(parameter) == StatusError)
        escape("unable to serialize the first item");

      // serialize the additional items.
      if (this->Serialize(parameters...) == StatusError)
        escape("unable to serialize the additional parameters");

      return StatusOk;
    }

//
// ---------- extract ---------------------------------------------------------
//

    ///
    /// this template extracts a single item.
    ///
    template <typename T>
    Status              Archive::Extract(T&                     element)
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
    Status              Archive::Extract(T&                     parameter,
                                         TT&...                 parameters)
    {
      // extract the first item.
      if (this->Extract(parameter) == StatusError)
        escape("unable to extract the first item");

      // extract the additional items.
      if (this->Extract(parameters...) == StatusError)
        escape("unable to extract the additional items");

      return StatusOk;
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method recycles an archive.
    ///
    template <typename T>
    Status              Archive::Recycle(const T*               object)
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
    /// this method displays information on an element belonging to an
    /// archive.
    ///
    template <typename T>
    Status              Archive::Print(const T&                 element,
                                       const Natural32          margin)
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[" << core::Type<T>::Name << "] "
                << element << std::endl;

      return StatusOk;
    }

  }
}

#endif
