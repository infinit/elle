//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/package/Derivable.hxx
//
// created       julien quintard   [fri may 13 13:12:01 2011]
// updated       julien quintard   [tue jul 12 08:10:29 2011]
//

#ifndef ELLE_PACKAGE_DERIVABLE_HXX
#define ELLE_PACKAGE_DERIVABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Derivable.hh>

namespace elle
{
  namespace package
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename T>
    Derivable<T>::Derivable():
      policy(Derivable::PolicyUnknown),
      factory(NULL),
      object(NULL)
    {
    }

    ///
    /// specific constructor specifying the object; used whenever such
    /// an instance is about to be serialized.
    ///
    template <typename T>
    Derivable<T>::Derivable(const Product&			product,
			    const T&				object):
      policy(Derivable::PolicyStatic),
      factory(NULL),
      product(product),
      object(const_cast<T*>(&object))
    {
    }

    ///
    /// specific constructor used whenever the derivable instance is
    /// about to be extracted from an archive for instance.
    ///
    template <typename T>
    Derivable<T>::Derivable(const Factory&			factory):
      policy(Derivable::PolicyDynamic),
      factory(&factory),
      product(0),
      object(NULL)
    {
    }

    ///
    /// specific constructor specifying the object. this constructu is
    /// useful in order to extract the data directly into the given object
    /// without allocating a new object.
    ///
    template <typename T>
    Derivable<T>::Derivable(const Factory&			factory,
			    const T&				object):
      policy(Derivable::PolicyStatic),
      factory(&factory),
      product(0),
      object(const_cast<T*>(&object))
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename T>
    Derivable<T>::Derivable(const Derivable<T>&			derivable):
      Object(derivable),

      policy(derivable.policy),
      factory(derivable.factory),
      product(derivable.product)
    {
      // clone the object if necessary.
      if ((this->policy == Derivable::PolicyDynamic) &&
	  (this->object != NULL))
	this->object = new T(*derivable.object);
    }

    ///
    /// destructor.
    ///
    template <typename T>
    Derivable<T>::~Derivable()
    {
      // delete the object, if present.
      if ((this->policy == Derivable::PolicyDynamic) &&
	  (this->object != NULL))
	delete this->object;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method statically cast the given type into the one behind
    /// the derivable instance and therefore retrieves the object.
    ///
    template <typename T>
    template <typename U>
    Status		Derivable<T>::Infer(U*&			object) const
    {
      enter();

      // check the internal object.
      if (this->object == NULL)
	escape("no object has been provided");

      // hand over the object pointer.
      object = static_cast<U*>(this->object);

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Derivable<T>, _(template <typename T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a derivable.
    ///
    template <typename T>
    Status		Derivable<T>::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Derivable]" << std::endl;

      // dump the factory and product.
      if (this->factory != NULL)
	{
	  // dump the factory.
	  if (this->factory->Dump(margin + 2) == StatusError)
	    escape("unable to dump the factory");
	}

      // dump the product.
      std::cout << alignment << Dumpable::Shift
		<< "[Product] " << this->product << std::endl;

      // dump the object, if present.
      if (this->object != NULL)
	{
	  // dump the object's structure.
	  if (this->object->Dump(margin + 2) == StatusError)
	    escape("unable to dump the object");
	}
      else
	{
	  // dump none.
	  std::cout << alignment << Dumpable::Shift
		    << "[Object] " << none << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a derivable.
    ///
    template <typename T>
    Status		Derivable<T>::Serialize(Archive&	archive) const
    {
      enter();

      // check that an object has been provided.
      if (this->object == NULL)
	escape("no derivable object has been provided");

      // serialize the factory product and object.
      if (archive.Serialize(this->product,
			    *this->object) == StatusError)
	escape("unable to serialize the derivable");

      leave();
    }

    ///
    /// this method extracts a derivable.
    ///
    template <typename T>
    Status		Derivable<T>::Extract(Archive&		archive)
    {
      enter();

      // extract the product.
      if (archive.Extract(this->product) == StatusError)
	escape("unable to extract the factory product");

      // allocate an object if no one is present.
      if (this->object == NULL)
	{
	  // check that a factory is present.
	  if (this->factory == NULL)
	    escape("no factory has been provided");

	  // build the object according to the product.
	  if (this->factory->Build(this->product, this->object) == StatusError)
	    escape("unable to build the object");
	}

      // finally, extract the object.
      if (archive.Extract(*this->object) == StatusError)
	escape("unable to extract the object");

      leave();
    }

  }
}

#endif
