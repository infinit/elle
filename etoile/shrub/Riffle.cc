#include <etoile/shrub/Riffle.hh>
#include <etoile/shrub/Shrub.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Riffle::Riffle():
      parent(NULL)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a riffle given the slab, its location and
    /// the parent riffle.
    ///
    elle::Status        Riffle::Create(const path::Slab&        slab,
                                       const nucleus::proton::Location& location,
                                       Riffle*                  parent)
    {

      // set the attributes.
      this->slab = slab;
      this->location = location;
      this->parent = parent;

      // retrieve the current time.
      if (this->timestamp.Current() == elle::Status::Error)
        escape("unable to retrieve the current time");

      return elle::Status::Ok;
    }

    ///
    /// this method tries to resolve a slab within the riffle's scope.
    ///
    elle::Status        Riffle::Resolve(const path::Slab&       slab,
                                        Riffle*&                riffle)
    {
      Riffle::Scoutor   scoutor;


      // initialize the pointer.
      riffle = NULL;

      // look up the element in the current riffle ... stop if not present.
      if ((scoutor = this->children.find(slab)) == this->children.end())
        return elle::Status::Ok;

      // return the resolved riffle.
      riffle = scoutor->second;

      return elle::Status::Ok;
    }

    ///
    /// this method updates the riffle with the given slab/location tuple.
    ///
    elle::Status        Riffle::Update(const path::Slab&        slab,
                                       const nucleus::proton::Location& location)
    {
      auto it = this->children.find(slab);

      // try to look up the element in the current riffle.
      if (it == this->children.end())
        {
          // check that available slots remain i.e it is possible that
          // the whole shrub's capacity is not large enough to hold
          // all the the route's slabs.
          if (Shrub::Queue.container.size() >=
              Infinit::Configuration.etoile.shrub.capacity)
            return elle::Status::Ok;

          // allocate a new riffle.
          std::unique_ptr<Riffle> riffle(new Riffle);

          // create the riffle.
          if (riffle->Create(slab, location, this) == elle::Status::Error)
            escape("unable to create the riffle");

          // add the riffle to the queue.
          if (Shrub::Queue.Insert(riffle->timestamp,
                                  riffle.get()) == elle::Status::Error)
            escape("unable to add the riffle");

          // insert it.
          auto result = this->children.insert(
              Riffle::Value(riffle->slab, riffle.get())
          );

          // XXX if riffle is in the Shrub::Queue but previous insert failed ...

          // check the result.
          if (result.second == false)
            escape("unable to insert the new riffle");
          else
            riffle.release();
        }
      else
        {
          // retrieve the pointer.
          Riffle* riffle = it->second;

          // if found, update it with the new address.
          riffle->location = location;

          // remove the riffle from the queue.
          if (Shrub::Queue.Delete(riffle->timestamp,
                                  riffle) == elle::Status::Error)
            escape("unable to remove the riffle");

          // refresh the timestamp.
          if (riffle->timestamp.Current() == elle::Status::Error)
            escape("unable to retrieve the current time");

          // finally, add the riffle back to the queue i.e at its new position.
          if (Shrub::Queue.Insert(riffle->timestamp,
                                  riffle) == elle::Status::Error)
            escape("unable to add the riffle");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method destroys the entry associated with the given slab.
    ///
    elle::Status        Riffle::Destroy(const path::Slab&       slab)
    {
      Riffle::Iterator  iterator;
      Riffle*           riffle;


      // try to look up the element in the current riffle.
      if ((iterator = this->children.find(slab)) == this->children.end())
        escape("unable to locate the given slab to destroy");

      // retrieve the riffle pointer.
      riffle = iterator->second;

      // flush the riffle.
      if (riffle->Flush() == elle::Status::Error)
        escape("unable to flush the riffle");

      // release the shrub slot.
      if (Shrub::Queue.Delete(riffle->timestamp,
                              riffle) == elle::Status::Error)
        escape("unable to remove the riffle");

      // delete the referenced riffle, along with its children.
      delete riffle;

      // remove the entry from the container.
      this->children.erase(iterator);

      return elle::Status::Ok;
    }

    ///
    /// this method flushes the riffle's content.
    ///
    elle::Status        Riffle::Flush()
    {
      Riffle::Scoutor   scoutor;

      // go through the children.
      for (scoutor = this->children.begin();
           scoutor != this->children.end();
           scoutor++)
        {
          Riffle*       riffle = scoutor->second;

          // flush the riffle recursively.
          if (riffle->Flush() == elle::Status::Error)
            escape("unable to flush the riffle");

          // release the shrub slot.
          if (Shrub::Queue.Delete(riffle->timestamp,
                                  riffle) == elle::Status::Error)
            escape("unable to remove the riffle");

          // delete the riffle.
          delete riffle;
        }

      // clear the container.
      this->children.clear();

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the riffle in a recursive way.
    ///
    elle::Status        Riffle::Dump(const elle::Natural32      margin)
    {
      elle::String      alignment(margin, ' ');
      Riffle::Scoutor   scoutor;

      std::cout << alignment << "[Riffle] "
                << std::hex << this << std::endl;

      // dump the attributes.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Slab] "
                << this->slab << std::endl;

      if (this->location.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the location");

      if (this->timestamp.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the timestamp");

      std::cout << alignment << elle::io::Dumpable::Shift << "[Parent] "
                << std::hex << this->parent << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Children]" << std::endl;

      // recursively dump all the sub-riffles.
      for (scoutor = this->children.begin();
           scoutor != this->children.end();
           scoutor++)
        {
          // dump the sub-riffle.
          if (scoutor->second->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the sub-riffle");
        }

      return elle::Status::Ok;
    }

  }
}
