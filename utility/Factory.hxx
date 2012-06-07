//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu jan 28 18:56:42 2010]
//

#ifndef ELLE_UTILITY_FACTORY_HXX
#define ELLE_UTILITY_FACTORY_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- generatoid ------------------------------------------------------
//

    ///
    /// a default constructor in order to keep the identifier.
    ///
    template <typename T>
    Factory::Generatoid<T>::Generatoid(const Product&           identifier):
      identifier(identifier)
    {
    }

    ///
    /// this method allocates a new object of the type of the functionoid.
    ///
    template <typename T>
    Status      Factory::Generatoid<T>::Allocate(Meta*&         meta) const
    {
      // allocate the object.
      meta = new T;

      return Status::Ok;
    }

//
// ---------- factory ---------------------------------------------------------
//

    ///
    /// this method registers a new type associated with a identifier.
    ///
    template <typename T>
    Status      Factory::Register(const Product&                identifier)
    {
      // check if there is already such an identifier registerd.
      if (this->container.find(identifier) != this->container.end())
        escape("unable to register an already registered identifier");

      // create a generatoid.
      auto generatoid = new Factory::Generatoid<T>(identifier);

      // insert the generator in the container.
      auto result = this->container.insert(
        std::pair<const Product,Factory::Functionoid*>(identifier, generatoid)
      );

      // check if the insertion was successful.
      if (result.second == false)
        {
          delete generatoid;
          escape("unable to insert the generatoid into the container");
        }

      return Status::Ok;
    }

    ///
    /// this method, given an identifier, allocates an object of the
    /// corresponding type.
    ///
    template <typename U>
    Status              Factory::Build(const Product&           identifier,
                                       U*&                      object) const
    {
      Factory::Scoutor  scoutor;

      // retrieve the associated generator.
      if ((scoutor = this->container.find(identifier)) ==
          this->container.end())
        escape("unable to locate the generatoid for the given identifier");

      // allocate an object of the type handled by the generatoid.
      if (scoutor->second->Allocate(
            reinterpret_cast<Meta*&>(object)) == Status::Error)
        escape("unable to allocate the object");

      return Status::Ok;
    }

  }
}

#endif
