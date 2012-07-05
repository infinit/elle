#include <etoile/nest/Nest.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace nest
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    Nest::P::Container      Nest::Placements;

    ///
    /// XXX
    ///
    Nest::A::Container      Nest::Addresses;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status            Nest::Exist(const nucleus::Placement&   placement)
    {
      if (Nest::Placements.find(placement) == Nest::Placements.end())
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Exist(const nucleus::Address&     address)
    {
      if (Nest::Addresses.find(address) == Nest::Addresses.end())
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Insert(const nucleus::Placement&  placement,
                                         Pod*                       pod)
    {
      std::pair<Nest::P::Iterator, elle::Boolean>   result;

      // insert the pod in the container.
      result =
        Nest::Placements.insert(
          std::pair<const nucleus::Placement, Pod*>(placement, pod));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the pod in the container");

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Insert(const nucleus::Placement&  placement,
                                         const nucleus::Address&    address,
                                         Pod*                       pod)
    {
      struct
      {
        std::pair<Nest::A::Iterator, elle::Boolean>         address;
        std::pair<Nest::P::Iterator, elle::Boolean>         placement;
      }                                                     result;

      // insert the pod in the container.
      result.placement =
        Nest::Placements.insert(
          std::pair<const nucleus::Placement, Pod*>(placement, pod));

      // check if the insertion was successful.
      if (result.placement.second == false)
        escape("unable to insert the pod in the container");

      // insert the pod in the container.
      result.address =
        Nest::Addresses.insert(
          std::pair<const nucleus::Address, Pod*>(address, pod));

      // check if the insertion was successful.
      if (result.address.second == false)
        escape("unable to insert the pod in the container");

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Retrieve(const nucleus::Placement& placement,
                                           Pod*&                    pod)
    {
      Nest::P::Iterator     iterator;

      if ((iterator = Nest::Placements.find(placement)) == Nest::Placements.end())
        escape("unable to locate a pod for the given placement");

      pod = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Retrieve(const nucleus::Address&  address,
                                           Pod*&                    pod)
    {
      Nest::A::Iterator     iterator;

      if ((iterator = Nest::Addresses.find(address)) == Nest::Addresses.end())
        escape("unable to locate a pod for the given address");

      pod = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Delete(const nucleus::Placement&  placement)
    {
      Pod*                  pod;

      //
      // retrieve the pod.
      //

      if (Nest::Retrieve(placement, pod) == elle::Status::Error)
        escape("unable to retrieve the pod associated with this placement");

      //
      // remove the pod from the containers.
      //

      if (pod->address != nucleus::Address::Null)
        Nest::Addresses.erase(pod->address);

      Nest::Placements.erase(pod->placement);

      //
      // delete the pod.
      //

      delete pod;

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Delete(const nucleus::Address&    address)
    {
      Pod*                  pod;

      //
      // retrieve the pod.
      //

      if (Nest::Retrieve(address, pod) == elle::Status::Error)
        escape("unable to retrieve the pod associated with this address");

      assert(pod->placement != nucleus::Placement::Null);

      //
      // remove the pod from the containers.
      //

      Nest::Addresses.erase(pod->address);

      Nest::Placements.erase(pod->placement);

      //
      // delete the pod.
      //

      delete pod;

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Attach(nucleus::Block*            block,
                                         nucleus::Handle&           handle)
    {
      nucleus::Placement    placement;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Attach()\n");

      // make sure placement is null.
      assert(handle.placement == nucleus::Placement::Null);

      // generate a placement.
      if (nucleus::Placement::Generate(placement) == elle::Status::Error)
        escape("unable to generate a unique placement");

      // create a new selectionoid.
      auto                                          pod =
        std::unique_ptr<Pod>(new Pod(placement, block));

      // insert the pod.
      if (Nest::Insert(pod.get()->placement, pod.get()) == elle::Status::Error)
        escape("unable to insert the pod");

      // return a handle.
      handle = nucleus::Handle(pod->placement);

      // release track.
      pod.release();

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    /// XXX detach then unload
    ///
    elle::Status            Nest::Detach(nucleus::Handle&           handle)
    {
      Pod*                  pod;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Detach()\n");

      // make sure placement is non-null.
      assert(handle.placement != nucleus::Placement::Null);

      // retrieve the pod.
      if (Nest::Retrieve(handle.placement, pod) == elle::Status::Error)
        escape("unable to retrieve the pod");

      // update the pod's nature.
      pod->nature = Pod::NatureOrphan;

      if (pod->counter == 0)
        {
          if (Nest::Delete(pod->placement) == elle::Status::Error)
            escape("unable to delete the pod");
        }

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Load(nucleus::Handle&             handle)
    {
      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Load()\n");

      // make sure the given handle is valid.
      assert(handle != nucleus::Handle::Null);

      if (handle.placement != nucleus::Placement::Null)
        {
          Pod*              pod;

          if (Nest::Retrieve(handle.placement, pod) == elle::Status::Error)
            escape("unable to retrieve the pod");

          if (pod->Load(handle) == elle::Status::Error)
            escape("unable to load the pod");
        }
      else if (handle.address != nucleus::Address::Null)
        {
          Pod*              pod;

          if (Nest::Retrieve(handle.address, pod) == elle::Status::Error)
            escape("unable to retrieve the pod");

          if (pod->Load(handle) == elle::Status::Error)
            escape("unable to load the pod");
        }
      else
        escape("unable to load a null handle");

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Unload(nucleus::Handle&             handle)
    {
      Pod*                  pod;

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] nest::Nest::Unload()\n");

      // make sure the given handle is valid.
      assert(handle.placement != nucleus::Placement::Null);

      if (Nest::Retrieve(handle.placement, pod) == elle::Status::Error)
        escape("unable to retrieve the pod");

      if (pod->Unload(handle) == elle::Status::Error)
        escape("unable to unload the pod");

      if ((pod->nature == Pod::NatureOrphan) &&
          (pod->counter == 0))
        {
          if (Nest::Delete(pod->placement) == elle::Status::Error)
            escape("unable to delete the pod");
        }

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    elle::Status            Nest::Show(const elle::Natural32        margin)
    {
      elle::String          alignment(margin, ' ');
      auto                  i = Nest::Placements.begin();
      auto                  e = Nest::Placements.end();

      std::cout << alignment << "[Nest]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[Placements] #"
                << Nest::Placements.size() << std::endl;

      for (; i != e; ++i)
        {
          if (i->second->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the pod");
        }

      auto                  j = Nest::Addresses.begin();
      auto                  f = Nest::Addresses.end();

      std::cout << alignment << elle::io::Dumpable::Shift << "[Addresses] #"
                << Nest::Addresses.size() << std::endl;

      for (; j != f; ++j)
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift
                    << "[Pod] " << std::hex << j->second << std::endl;
        }

      return elle::Status::Ok;
    }

  }
}
