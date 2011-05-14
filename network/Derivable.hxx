//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Derivable.hxx
//
// created       julien quintard   [fri may 13 13:12:01 2011]
// updated       julien quintard   [fri may 13 15:45:58 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Derivable.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor, used whenever the derivable instance is
    /// going to be extract from an archive for instance.
    ///
    template <typename T>
    Derivable<T>::Derivable():
      policy(Derivable::PolicyDynamic),
      identifier(0),
      object(NULL)
    {
    }

    ///
    /// specific constructor specifying the factory identifier along with
    /// the object; used whenever such an instance is sent.
    ///
    template <typename T>
    Derivable<T>::Derivable(const Factory::Identifier&		identifier,
			    const T&				object):
      policy(Derivable::PolicyStatic),
      identifier(identifier),
      object(const_cast<T*>(&object))
    {
    }

    ///
    /// specific constructor specifying the an object only. this is
    /// used whenever the serialized instance should be directly extracted
    /// in the given object.
    ///
    template <typename T>
    Derivable<T>::Derivable(const T&				object):
      policy(Derivable::PolicyStatic),
      identifier(0),
      object(const_cast<T*>(&object))
    {
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

      // dump the identifier.
      std::cout << alignment << Dumpable::Shift << "[Identifier] "
		<< this->identifier << std::endl;

      // dump the object, if present.
      if (this->object == NULL)
	{
	  // dump none.
	  std::cout << alignment << Dumpable::Shift
		    << "[Object] none" << std::endl;
	}
      else
	{
	  // dump the object's structure.
	  if (this->object->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the object");
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

      // serialize the identifier and object.
      if (archive.Serialize(this->identifier,
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

      // extract the identifier.
      if (archive.Extract(this->identifier) == StatusError)
	escape("unable to extract the factory identifier");

      // allocate an object if no one is present.
      if (this->object == NULL)
	{
	  // build the object according to the identifier.
	  if (Factory::Build(identifier, this->object) == StatusError)
	    escape("unable to build the object");
	}

      // finally, extract the object.
      if (archive.Extract(*this->object) == StatusError)
	escape("unable to extract the object");

      leave();
    }

  }
}
