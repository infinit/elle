//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 21 07:35:21 2011]
//

#ifndef ELLE_CONTAINER_TIMELINE_BUCKET_HXX
#define ELLE_CONTAINER_TIMELINE_BUCKET_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace standalone;

  namespace container
  {
    namespace timeline
    {

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// this method adds an object to the bucket.
      ///
      template <typename T>
      Status            Bucket<T>::Add(T&                       object)
      {
        typename Bucket<T>::Iterator    iterator;

        enter();

        // add the object to the container.
        this->container.push_back(object);

        leave();
      }

      ///
      /// this method removes an object from the bucket.
      ///
      template <typename T>
      Status            Bucket<T>::Remove(T&                    object)
      {
        typename Bucket<T>::Iterator    iterator;

        enter();

        // go through the container.
        for (iterator = this->container.begin();
             iterator != this->container.end();
             iterator++)
          {
            // check if this object is the one we are looking for.
            if (*iterator == object)
              {
                // remove the object from the container.
                this->container.erase(iterator);

                break;
              }
          }

        leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the bucket.
      ///
      template <typename T>
      Status            Bucket<T>::Dump(const Natural32         margin) const
      {
        String          alignment(margin, ' ');

        enter();

        std::cout << alignment << "[Bucket] "
                  << std::dec << this->container.size() << std::endl;

        leave();
      }

    }
  }
}

#endif
