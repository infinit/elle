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
// updated       julien quintard   [fri feb  5 01:58:03 2010]
//

#ifndef ELLE_FACTORY_FACTORY_HXX
#define ELLE_FACTORY_FACTORY_HXX

namespace elle
{
  namespace factory
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// this functionoid contains a method for generating an Entity object
    /// of the given type.
    ///
    template <typename T>
    class FactoryGenerator:
      public FactoryFunctionoid
    {
    public:
      //
      // constructors & destructors
      //

      ///
      /// a default constructor in order to keep the identifier.
      ///
      FactoryGenerator(const String				identifier)
      {
	this->identifier = identifier;
      }

      //
      // methods
      //

      ///
      /// this method allocates a new object of the type of the functionoid.
      ///
      Status		Allocate(Entity*&			entity) const
      {
	entity = new T;

	leave();
      }

      //
      // attributes
      //
      String		identifier;
    };

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method registers a new type associated with a identifier string.
    ///
    template <typename T>
    Status		Factory::Register(const String&		identifier)
    {
      // check if there is already such an identifier registerd.
      if (Factory::Map.find(identifier) == Factory::Map.end())
	escape("unable to register an already registered identifier");

      // create a generator.
      FactoryGenerator<T>* functionoid =
	new FactoryGenerator<T>(identifier);

      // insert the callback in the container.
      std::pair<Factory::Iterator, Boolean> value =
	Factory::Map.insert(std::pair<const String,
			              FactoryFunctionoid*>(identifier,
							   functionoid));

      // check if the insertion was successful.
      if (value.second == false)
	escape("unable to insert the generator into the container");

      leave();
    }

    ///
    /// this method, given an identifier, allocates an object of the
    /// corresponding type.
    ///
    template <typename U>
    Status		Factory::Build(const String&		identifier,
				       U*&			object)
    {
      Factory::Scoutor	scoutor;

      // retrieve the associated generator.
      scoutor = Factory::Map.find(identifier);

      // check if this identifier has been registered.
      if (scoutor == Factory::Map.end())
	escape("unable to locate the generator for the given identifier");

      // allocate an object of the type handled by the generator.
      if (scoutor->second->Allocate((Entity*&)object) == StatusError)
	escape("unable to allocate the object");

      leave();
    }

  }
}

#endif
