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
      Status Bucket<T>::Add(T const& object)
      {
        // add the object to the container.
        this->container.push_back(object);

        return elle::StatusOk;
      }

      ///
      /// this method removes an object from the bucket.
      ///
      template <typename T>
      Status Bucket<T>::Remove(T const& object)
      {
        auto it = this->container.begin(),
             end = this->container.end();
        for (; it != end; ++it)
          {
            // check if this object is the one we are looking for.
            if (*it == object)
              {
                // remove the object from the container.
                this->container.erase(it);
                return elle::StatusOk;
              }
          }

        return elle::StatusOk;
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

        std::cout << alignment << "[Bucket] "
                  << std::dec << this->container.size() << std::endl;

        return elle::StatusOk;
      }

    }
  }
}

#endif
