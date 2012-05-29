//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 21 07:43:32 2011]
//

#ifndef ELLE_CONTAINER_TIMELINE_TIMELINE_HXX
#define ELLE_CONTAINER_TIMELINE_TIMELINE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <cassert>
#include <memory>

#include <elle/types.hh>

#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace standalone;

  namespace container
  {
    namespace timeline
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor
      ///
      template <typename T>
      Timeline<T>::Timeline()
      {
      }

      ///
      /// destructor.
      ///
      template <typename T>
      Timeline<T>::~Timeline()
      {
        auto it= this->container.begin();
        auto end = this->container.end();

        for (; it != end; ++it)
          delete it->second;

        this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// this method insert an object in the timeline container.
      ///
      template <typename T>
      Status Timeline<T>::Insert(const Time& timestamp, T const& object)
      {
        Bucket<T>* bucket = 0;

        auto it = this->container.find(timestamp);
        // try to look up an bucket for the given timestamp.
        if (it == this->container.end())
          {
            auto bucket_ptr = std::unique_ptr<Bucket<T>>(new Bucket<T>);

            // record the bucket with its timestamp.
            auto result = this->container.insert(
                typename Container::value_type(timestamp, bucket_ptr.get())
            );

            // check the result.
            if (result.second == false)
              escape("unable to insert the object");
            else // the bucket was successfuly stored
              bucket = bucket_ptr.release();
          }
        else
          {
            bucket = it->second;
          }

        assert(bucket != 0);

        // add the object to the bucket.
        if (bucket->Add(object) == Status::Error)
          escape("unable to add the object to the bucket");

        return Status::Ok;
      }

      ///
      /// this method deletes an object from the timeline.
      ///
      template <typename T>
      Status            Timeline<T>::Delete(const Time& timestamp,
                                            T const& object)
      {
        Bucket<T>*                      bucket;

        auto it = this->container.find(timestamp);

        // try to look up the bucket in the container.
        if (it == this->container.end())
          escape("unable to locate the given timestamp");

        // retrieve the bucket.
        bucket = it->second;
        assert(bucket != nullptr);

        // remove the object from the bucket.
        if (bucket->Remove(object) == Status::Error)
          escape("unable to remove the object");

        // check if the bucket is empty.
        if (bucket->container.empty() == true)
          {
            this->container.erase(it);
            delete bucket;
          }
        return Status::Ok;
      }

      ///
      /// this method clears the timeline container.
      ///
      template <typename T>
      Status            Timeline<T>::Clear()
      {
        this->container.clear();
        return Status::Ok;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the timeline container.
      ///
      template <typename T>
      Status            Timeline<T>::Dump(const Natural32 margin) const
      {
        String alignment(margin, ' ');

        std::cout << alignment << "[Timeline] "
                  << std::dec << this->container.size() << std::endl;

        auto it = this->container.begin();
        auto end = this->container.end();
        for (; it != end; ++it)
          {
            // dump the timestamp.
            if (it->first.Dump(margin + 2) == Status::Error)
              escape("unable to dump the timestamp");

            // dump the bucket.
            if (it->second->Dump(margin + 2) == Status::Error)
              escape("unable to dump the bucket");
          }
        return Status::Ok;
      }

    }
  }
}

#endif
