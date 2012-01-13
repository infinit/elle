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

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    const Handle<T>			Handle::Null;

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
    Handle<T>::Handle(T*					object):
      XXX Address::Some

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
    elle::Status	Handle<T>::Create(const Address&	address,
					  T*			object)
    {
      enter();

      // set the attributes.
      this->address = address;
      this->_object = object;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    elle::Status	Handle<T>::Dump(const elle::Natural32	margin)
    {
      enter();

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

      leave();
    }

  }
}

#endif
