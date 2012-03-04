//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sun dec 25 14:10:17 2011]
//

#ifndef NUCLEUS_PROTON_HANDLE_HXX
#define NUCLEUS_PROTON_HANDLE_HXX

namespace nucleus
{
  namespace proton
  {

///
/// XXX
///
template <typename T>
const Handle<T>                     Handle<T>::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename T>
    Handle<T>::Handle():
      _object(NULL)
    {
    }

    ///
    /// XXX
    ///
    template <typename T>
    Handle<T>::Handle(T*                                        object):
      address(Address::Some), // XXX[???]
      _object(object)
    {
    }

    ///
    /// XXX
    ///
    template <typename T>
    Handle<T>::Handle(const Address&                            address):
      address(address),
      _object(NULL)
    {
    }

    /* XXX
XXX
- on load un block.
- on veut loader son frere gauche, on load a partir de l addresse.
- le nest regarde si il a un mapping entre cette adresse et un block
  deja loade mais qui a ete modifie. si cest le cas, le mapping nous donne
  le placement du block et on le recupere.
- sinon on recupere a partir de hole.

- en soit on a 3 cas:
  - si le handle a un pointer, on l utilise.
  - si le handle a un placement, on recupere le block du nest
  - sinon, on prend l address, et on load a partir du nest (mais le
    mapping pourrait nous retourner un block deja loade)

- a noter que peut etre qu il serait utile de garder le handle dans chaque
  nodule pour eviter de recalculer.
    */

    ///
    /// the copy constructor.
    //
    template <typename T>
    Handle<T>::Handle(const Handle<T>&                          element):
      elle::Object(element),

      address(element.address),
      _object(element._object),
      _placement(element._placement)
    {
    }

    ///
    /// the destructor.
    ///
    template <typename T>
    Handle<T>::~Handle()
    {
      // delete the object, if present.
      if (this->_object != NULL)
        delete this->_object;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    elle::Status        Handle<T>::Create(const Address&        address,
                                          T*                    object)
    {
      // set the attributes.
      this->address = address;
      this->_object = object;

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    template <typename T>
    elle::Boolean       Handle<T>::operator==(const Handle<T>&  element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // XXX

      return elle::StatusTrue;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Handle<T>, _(template <typename T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    elle::Status        Handle<T>::Dump(const elle::Natural32) const
    {
      /* XXX
      // XXX

      // dump the hiearchy.
      if (this->_object != NULL)
        {
          // dump the hierarchy, if present.
          std::cout << alignment << elle::Dumpable::Shift
                    << "[_Object]" << std::endl;

          if (this->_object->Dump(margin + 4) == elle::StatusError)
            escape("unable to dump the nodule");
        }
      else
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << elle::Dumpable::Shift
                    << "[_Object] " << elle::none << std::endl;
        }
      */

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    template <typename T>
    elle::Status        Handle<T>::Serialize(elle::Archive&     archive) const
    {
      if (archive.Serialize(this->address) == elle::StatusError)
        escape("unable to serialize the attribtues");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the object.
    ///
    template <typename T>
    elle::Status        Handle<T>::Extract(elle::Archive&       archive)
    {
      if (archive.Extract(this->address) == elle::StatusError)
        escape("unable to extract the attribtues");

      return elle::StatusOk;
    }

  }
}

#endif
