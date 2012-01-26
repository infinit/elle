//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu apr  8 14:18:10 2010]
//

#ifndef ELLE_CONCURRENCY_ENVIRONMENT_HXX
#define ELLE_CONCURRENCY_ENVIRONMENT_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Report.hh>
#include <elle/standalone/Maid.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method stores an element.
    ///
    template <typename T>
    Status              Environment::Store(const String&        name,
                                           T*                   value)
    {
      enter();

      // push the (name, value) tuple.
      this->container.push_back(
        Environment::Value(name, static_cast<Meta*>(value)));

      leave();
    }

    ///
    /// this method loads a value.
    ///
    template <typename T>
    Status              Environment::Load(const String&         name,
                                          T*&                   value)
    {
      Environment::Iterator     iterator;

      enter();

      // go through the elements.
      for (iterator = this->container.begin();
           iterator != this->container.end();
           iterator++)
        {
          // check if this is the looked for element.
          if (iterator->first == name)
            {
              // return the value.
              value = static_cast<T*>(iterator->second);

              // remove the item
              this->container.erase(iterator);

              leave();
            }
        }

      escape("unable to locate the element `%s'", name.c_str());
    }

  }
}

#endif
