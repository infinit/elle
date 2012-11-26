#ifndef ELLE_UTILITY_FACTORY_HXX
# define ELLE_UTILITY_FACTORY_HXX

# include <elle/Exception.hh>
# include <elle/log.hh>
# include <elle/finally.hh>

namespace elle
{
  namespace utility
  {
//
// ---------- generatoid ------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    template <typename P>
    template <typename T>
    Factory<P>::Generatoid<T>::Generatoid(P const& product):
      _product(product)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename P>
    template <typename T>
    void*
    Factory<P>::Generatoid<T>::allocate() const
    {
      return (new T);
    }

//
// ---------- factory ---------------------------------------------------------
//

    /*-------------.
    | Construction |
    `-------------*/

    template <typename P>
    Factory<P>::~Factory()
    {
      for (auto scoutor: this->_container)
        delete scoutor.second;

      this->_container.clear();
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename P>
    template <typename T>
    void
    Factory<P>::record(P const& product)
    {
      ELLE_LOG_COMPONENT("elle.utility.Factory");
      ELLE_TRACE_METHOD(product);

      // Check if there is already such an product registerd.
      if (this->_container.find(product) != this->_container.end())
        throw Exception("unable to register an already registered product");

      // Create a generatoid.
      auto generatoid = new Factory<P>::Generatoid<T>(product);

      ELLE_FINALLY_ACTION_DELETE(generatoid);

      // Insert the generator in the container.
      auto result = this->_container.insert(
        std::pair<P const, Factory<P>::Functionoid*>(product, generatoid));

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the generatoid into the container");

      ELLE_FINALLY_ABORT(generatoid);
    }

    template <typename P>
    template <typename T>
    T*
    Factory<P>::allocate(P const& product) const
    {
      ELLE_LOG_COMPONENT("elle.utility.Factory");
      ELLE_TRACE_METHOD(product);

      Factory<P>::Scoutor scoutor;

      // Retrieve the associated generator.
      if ((scoutor = this->_container.find(product)) ==
          this->_container.end())
        throw Exception("unable to locate the generatoid for the "
                        "given product");

      // Allocate and return the instance.
      return (reinterpret_cast<T*>(scoutor->second->allocate()));
    }

    /*---------.
    | Dumpable |
    `---------*/

    template <typename P>
    Status
    Factory<P>::Dump(const Natural32 margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Factory]" << std::endl;

      for (auto scoutor: this->_container)
        std::cout << alignment << io::Dumpable::Shift
                  << scoutor.first << std::endl;

      return Status::Ok;
    }
  }
}

#endif
