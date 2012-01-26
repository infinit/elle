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
    Status      Factory::Register(const Product&                identifier)
    {
      Factory::Generatoid<T>*                   generatoid;
      std::pair<Factory::Iterator, Boolean>     result;

      enterx(instance(generatoid));

      // check if there is already such an identifier registerd.
      if (this->container.find(identifier) != this->container.end())
        escape("unable to register an already registered identifier");

      // create a generatoid.
      generatoid = new Factory::Generatoid<T>(identifier);

      // insert the generator in the container.
      result = this->container.insert(
                 std::pair<const Product,
                           Factory::Functionoid*>(identifier,
                                                  generatoid));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the generatoid into the container");

      // give up the tracking.
      waive(generatoid);

      leave();
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

      enter();

      // retrieve the associated generator.
      if ((scoutor = this->container.find(identifier)) ==
          this->container.end())
        escape("unable to locate the generatoid for the given identifier");

      // allocate an object of the type handled by the generatoid.
      if (scoutor->second->Allocate(
            reinterpret_cast<Meta*&>(object)) == StatusError)
        escape("unable to allocate the object");

      leave();
    }

  }
}

#endif
