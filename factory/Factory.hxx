//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/factory/Factory.hxx
//
// created       julien quintard   [thu jan 28 18:56:42 2010]
// updated       julien quintard   [sat may 14 12:52:10 2011]
//

#ifndef ELLE_FACTORY_FACTORY_HXX
#define ELLE_FACTORY_FACTORY_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace factory
  {

//
// ---------- generator -------------------------------------------------------
//

    ///
    /// a default constructor in order to keep the identifier.
    ///
    template <typename T>
    Factory::Generator<T>::Generator(const Identifier&		identifier):
      identifier(identifier)
    {
    }

    ///
    /// this method allocates a new object of the type of the functionoid.
    ///
    template <typename T>
    Status		Factory::Generator<T>::Allocate(Meta*&	meta) const
    {
      enter();

      // allocate the object.
      meta = new T;

      leave();
    }

//
// ---------- factory ---------------------------------------------------------
//

    ///
    /// this method registers a new type associated with a identifier.
    ///
    template <typename T>
    Status		Factory::Register(const Identifier&	identifier)
    {
      Factory::Generator<T>*			generator;
      std::pair<Factory::Iterator, Boolean>	result;

      enter(instance(generator));

      // reject the zero identifier.
      if (identifier == 0)
	escape("unable to use the zero identifier for registration");

      // check if there is already such an identifier registerd.
      if (Factory::Map.find(identifier) != Factory::Map.end())
	escape("unable to register an already registered identifier");

      // create a generator.
      generator = new Factory::Generator<T>(identifier);

      // lock in writing.
      Factory::Control.Lock(ModeWrite);
      {
	// insert the generator in the container.
	result = Factory::Map.insert(
		   std::pair<const Identifier,
		             Factory::Functionoid*>(identifier,
						    generator));
      }
      Factory::Control.Unlock();

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the generator into the container");

      // give up the tracking.
      waive(generator);

      leave();
    }

    ///
    /// this method, given an identifier, allocates an object of the
    /// corresponding type.
    ///
    template <typename U>
    Status		Factory::Build(const Identifier&	identifier,
				       U*&			object)
    {
      Factory::Scoutor	scoutor;

      enter();

      // lock in reading.
      Factory::Control.Lock(ModeRead);
      {
	// retrieve the associated generator.
	scoutor = Factory::Map.find(identifier);

	// check if this identifier has been registered.
	if (scoutor == Factory::Map.end())
	  {
	    Factory::Control.Unlock();
	    escape("unable to locate the generator for the given identifier");
	  }

	// allocate an object of the type handled by the generator.
	if (scoutor->second->Allocate((Meta*&)object) == StatusError)
	  {
	    Factory::Control.Unlock();
	    escape("unable to allocate the object");
	  }
      }
      Factory::Control.Unlock();

      leave();
    }

  }
}

#endif
