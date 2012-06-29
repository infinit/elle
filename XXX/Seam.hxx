//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 22:14:08 2011]
//

#ifndef NUCLEUS_PROTON_SEAM_HXX
#define NUCLEUS_PROTON_SEAM_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Ambit.hh>

#include <hole/Hole.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    /* XXX
    template <typename V>
    Seam<V>::Seam():
      Nodule<V>(Nodule<V>::TypeSeam)
    {
    }
    */

    ///
    /// destructor.
    ///
    template <typename V>
    Seam<V>::~Seam()
    {
      auto      iterator = this->container.begin();
      auto      end = this->container.end();

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Seam<V>::I*   inlet = iterator->second;

          // delete the inlet.
          delete inlet;
        }

      // clear the container.
      this->container.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Create()
    {
      // initialize the footprint.
      this->footprint.size = Seam<V>::Footprint;

      // set the state.
      this->contents()->state = StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method inserts the given inlet in the seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Insert(I*                      inlet)
    {
      std::pair<typename Seam<V>::Iterator::Forward,
                elle::Boolean>                                  result;

      // check if this key has already been recorded.
      if (this->container.find(inlet->key) != this->container.end())
        escape("this key seems to have already been recorded");

      // insert the inlet in the container.
      result = this->container.insert(
                 std::pair<const typename V::K,
                           Seam<V>::I*>(inlet->key, inlet));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the inlet in the container");

      // compute the inlet's footprint.
      if (inlet->footprint.Compute() == elle::Status::Error)
        escape("unable to compute the inlet's footprint");

      // add the inlet footprint to the seam's.
      this->footprint.size += inlet->footprint.size;

      // set the state.
      this->contents()->state = StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method inserts the given nodule in the seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Insert(const typename V::K&    key,
                                        Handle&                 value)
    {
      // create an inlet.
      auto              inlet =
        std::unique_ptr<typename Seam<V>::I>(
          new typename Seam<V>::I(key, value));

      // add the inlet to the seam.
      if (this->Insert(inlet.get()) == elle::Status::Error)
        escape("unable to add the value to the seam");

      // waive.
      inlet.release();

      return elle::Status::Ok;
    }

    ///
    /// this method deletes the inlet referenced by the given iterator.
    ///
    template <typename V>
    elle::Status        Seam<V>::Delete(
                          typename Iterator::Forward&           iterator)
    {
      Seam<V>::I*       inlet;

      // retrieve the inlet.
      inlet = iterator->second;

      // compute the inlet's footprint.
      if (inlet->footprint.Compute() == elle::Status::Error)
        escape("unable to compute the inlet's footprint");

      // substract the inlet footprint to the seam's.
      this->footprint.size -= inlet->footprint.size;

      // delete the inlet.
      delete inlet;

      // finally, erase the entry.
      this->container.erase(iterator);

      // set the state.
      this->contents()->state = StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method deletes the given key from the seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Delete(const typename V::K&    key)
    {
      typename Seam<V>::Iterator::Forward       iterator;

      // locate the inlet for the given key.
      if (this->Locate(key, iterator) == elle::Status::Error)
        escape("unable to locate the given key");

      // delete the entry associated with the given iterator.
      if (this->Delete(iterator) == elle::Status::Error)
        escape("unable to delete the entry associated with the iterator");

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the given key exists.
    ///
    template <typename V>
    elle::Status        Seam<V>::Exist(const typename V::K&     key)
    {
      // locate the given key.
      if (this->container.find(key) == this->container.end())
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// this method returns an iterator on the inlet responsible for the
    /// given key.
    ///
    /// note that contrary to Locate(), the Lookup() methods do not look
    /// for an exact match but instead return the inlet with the key
    /// immediately greater than the given key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Lookup(
                          const typename V::K&                  key,
                          typename Iterator::Forward&           iterator)
    {
      auto              end = this->container.end();
      auto              rbegin = this->container.rbegin();

      // go through the container.
      for (iterator = this->container.begin(); iterator != end; ++iterator)
        {
          Seam<V>::I*   inlet = iterator->second;

          // check if this inlet is responsible for the given key or
          // the end of the seam has been reached.
          if ((key <= iterator->first) || (inlet == rbegin->second))
            {
              // return with the correct iterator set.
              return elle::Status::Ok;
            }
        }

      escape("unable to look up the entry responsible for the given key");
    }

    ///
    /// this method returns the inlet responsible for the given key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Lookup(const typename V::K&    key,
                                        I*&                     inlet)
    {
      typename Seam<V>::Iterator::Forward       iterator;

      // lookup the entry responsible for the given key.
      if (this->Lookup(key, iterator) == elle::Status::Error)
        escape("unable to locate the entry");

      // return the inlet.
      inlet = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the nodule responsible for the given key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Lookup(const typename V::K&    key,
                                        Handle&                 handle)
    {
      Seam<V>::I*               inlet;

      // lookup the inlet associated with the given key.
      if (this->Lookup(key, inlet) == elle::Status::Error)
        escape("unable to locate the inlet");

      // return the nodule's handle.
      handle = inlet->value;

      return elle::Status::Ok;
    }

    ///
    /// this method returns an iterator on the inlet associated with
    /// the given key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Locate(
                          const typename V::K&                  key,
                          typename Iterator::Forward&           iterator)
    {
      // locate the given key.
      if ((iterator = this->container.find(key)) == this->container.end())
        escape("unable to locate the given key");

      return elle::Status::Ok;
    }

    ///
    /// this method returns the inlet associated with the given key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Locate(const typename V::K&    key,
                                        I*&                     inlet)
    {
      typename Seam<V>::Iterator::Forward       iterator;

      // locate the given key.
      if (this->Locate(key, iterator) == elle::Status::Error)
        escape("unable to locate the entry associated with the given key");

      // return the inlet.
      inlet = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the nodule associated with the given key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Locate(const typename V::K&    key,
                                        Handle&                 handle)
    {
      Seam<V>::I*       inlet;

      // locate the given key.
      if (this->Locate(key, inlet) == elle::Status::Error)
        escape("unable to locate the inlet associated with the given key");

      // return the nodule's handle.
      handle = inlet->value;

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    /// since seams reference sub-seams/quills, these must be
    /// updated so as to reference the parent seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Link(I*                        inlet,
                                      Handle&                   parent)
    {
      Ambit< Contents< Seam<V> > >      child(inlet->value);

      // load the child nodule.
      if (child.Load() == elle::Status::Error)
        escape("unable to load the block");

      // the child nodule must have been loaded.
      assert(child.nodule() != nullptr);

      // set the child nodule's parent handle.
      child.nodule()->parent = parent;

      // unload the child nodule.
      if (child.Unload() == elle::Status::Error)
        escape("unable to unload the block");

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status        Seam<V>::Link(Handle&                   parent)
    {
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      // go through the inlets.
      for (; iterator != end; ++iterator)
        {
          Seam<V>::I*                           inlet = iterator->second;
          Ambit< Contents< Nodule<V> > >        child(inlet->value);

          // load the value block.
          if (child.Load() == elle::Status::Error)
            escape("unable to load the block");

          // update the parent link.
          child.nodule()->parent = parent;

          // unload the value block.
          if (child.Unload() == elle::Status::Error)
            escape("unable to unload the block");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method takes the ancient key _from_, locates the inlet with
    /// this key and updates it with its new key i.e _to_.
    ///
    template <typename V>
    elle::Status        Seam<V>::Update(const typename V::K&    from,
                                        const typename V::K&    to)
    {
      typename Seam<V>::Iterator::Forward       iterator;
      Seam<V>::I*                               inlet;

      // locate the inlet.
      if (this->Locate(from, iterator) == elle::Status::Error)
        escape("unable to locate the given inlet");

      // retrieve the inlet.
      inlet = iterator->second;

      // should the given key be different from the current one, update the
      // entry.
      if (to != inlet->key)
        {
          std::pair<typename Seam<V>::Iterator::Forward,
                    elle::Boolean>              result;

          // manually erase the entry.
          this->container.erase(iterator);

          // update the inlet's key.
          inlet->key = to;

          // insert the inlet in the container.
          result = this->container.insert(
                     std::pair<const typename V::K,
                               Seam<V>::I*>(inlet->key, inlet));

          // check if the insertion was successful.
          if (result.second == false)
            escape("unable to insert the inlet in the container");

          // set the state.
          this->contents()->state = StateDirty;
        }

      return elle::Status::Ok;
    }

    ///
    /// this method is similar to Update() but also takes care of updating
    /// the parent nodule, if present, up to the tree's root nodule if
    /// necessary.
    ///
    template <typename V>
    elle::Status        Seam<V>::Propagate(const typename V::K& from,
                                           const typename V::K& to)
    {
      typename V::K     ancient;
      typename V::K     recent;

      // retrieve the current mayor key.
      if (this->Mayor(ancient) == elle::Status::Error)
        escape("unable to retrieve the mayor key");

      // update the seam.
      if (this->Update(from, to) == elle::Status::Error)
        escape("unable to update the nodule");

      // retrieve the new mayor key.
      if (this->Mayor(recent) == elle::Status::Error)
        escape("unable to retrieve the mayor key");

      // finally, propagate the update should have the mayor key changed
      // and if a parent nodule exists.
      if ((recent != ancient) &&
          (this->parent != Handle::Null))
        {
          Ambit< Contents< Seam<V> > >  parent(this->parent);

          // load the parent nodule.
          if (parent.Load() == elle::Status::Error)
            escape("unable to load the parent block");

          // at this point, the parent must have been loaded.
          assert(parent.nodule() != nullptr);

          // progate the update.
          if (parent.nodule()->Propagate(ancient,
                                  recent) == elle::Status::Error)
            escape("unable to propagate the update");

          // unload the parent nodule.
          if (parent.Unload() == elle::Status::Error)
            escape("unable to unload the block");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method takes the given seam and merges its inlets with the
    /// current one.
    ///
    /// XXX[a virer et mettre direct dans Porcupine, comme Split]
    template <typename V>
    elle::Status        Seam<V>::Merge(Seam<V>*                 seam)
    {
      typename V::K     t;
      typename V::K     s;

      // retrieve the mayor key.
      if (this->Mayor(t) == elle::Status::Error)
        escape("unable to retrieve the mayor key");

      // retrieve the mayor key.
      if (seam->Mayor(s) == elle::Status::Error)
        escape("unable to retrieve the mayor key");

      // check which nodule has the lowest keys.
      if (s < t)
        {
          // in this case, export the lower seam's inlets into the current's.
          if (seam->Export(this) == elle::Status::Error)
            escape("unable to export the inlets");
        }
      else
        {
          // otherwise, import the higher seam's inlets into the current's.
          if (this->Import(seam) == elle::Status::Error)
            escape("unable to import the inlets");
        }

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status        Seam<V>::Check(Handle&                  current,
                                       const Pins               pins)
    {
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      // go through the inlets.
      for (; iterator != end; ++iterator)
        {
          Seam<V>::I*                           inlet = iterator->second;
          Ambit< Contents< Nodule<V> > >        child(inlet->value);
          typename V::K                         mayor;

          // load the value block.
          if (child.Load() == elle::Status::Error)
            escape("unable to load the block");

          // the child nodule must have been loaded.
          assert(child.nodule() != nullptr);

          // check the address, if required.
          if (pins & PinAddress)
            {
              Address   address;

              // bind the child block.
              if (child.contents()->Bind(address) == elle::Status::Error)
                escape("unable to bind the block");

              // compare the addresses.
              if (inlet->value.address != address)
                escape("invalid address");
            }

          // check the keys, if required.
          if (pins & PinKey)
            {
              // check the key.
              if (inlet->key != iterator->first)
                escape("invalid key");

              // retrieve the child's mauor key.
              if (child.nodule()->Mayor(mayor) == elle::Status::Error)
                escape("unable to retrieve the mayor key");

              // compare the mayor key with the inlet's reference.
              if (inlet->key != mayor)
                escape("the child nodule's mayor key differs from its "
                       "reference");
            }

          // trigger the check on the child nodule.
          if (child.nodule()->Check(current,
                             inlet->value,
                             pins) == elle::Status::Error)
            escape("unable to check the child nodule's consistency");

          // unload the value block.
          if (child.Unload() == elle::Status::Error)
            escape("unable to unload the block");
        }

      return elle::Status::Ok;
    }
//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// this method returns the current seam's mayor key i.e the higehst
    /// key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Mayor(typename V::K&           mayor) const
    {
      // return the mayor key.
      mayor = this->container.rbegin()->first;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the maiden key i.e the only remaining key.
    ///
    template <typename V>
    elle::Status        Seam<V>::Maiden(typename V::K&          maiden) const
    {
      // check if a single inlet is present.
      if (this->container.size() != 1)
        escape("unable to retrieve the maiden; multiple inlets are present");

      // return the maiden key.
      maiden = this->container.begin()->first;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the quill responsible for the given key.
    ///
    /// since the current nodule is not a quill, the request is forwarded
    /// to the child nodule which is responsible for the given key until
    /// the last level---i.e the quill---is reached.
    ///
    template <typename V>
    elle::Status        Seam<V>::Search(const typename V::K&    key,
                                        Handle&                 handle)
    {
      typename Seam<V>::Iterator::Forward       iterator;
      Seam<V>::I*                               inlet;

      // look up the entry responsible for this key.
      if (this->Lookup(key, iterator) == elle::Status::Error)
        escape("unable to look up the entry");

      // retrieve the inlet.
      inlet = iterator->second;

      // set the handle as being the inlet's value.
      //
      // this is required because, should the child be a quill, the Search()
      // method implementation for Quills does nothing.
      //
      // therefore, the whole Search() mechanism relies on the fact that
      // the seam's Search() method sets the handle temporarily until the
      // child quill does nothing after which the call returns with the
      // temporary handle which happens to reference the quill.
      //
      handle = inlet->value;

      Ambit< Contents< Nodule<V> > >            child(inlet->value);

      // load the child nodule.
      if (child.Load() == elle::Status::Error)
        escape("unable to load the block");

      // the child nodule must have been loaded.
      assert(child.nodule() != nullptr);

      // search in this nodule.
      if (child.nodule()->Search(key, handle) == elle::Status::Error)
        escape("unable to locate the quill for the given key");

      // unload the child nodule.
      if (child.Unload() == elle::Status::Error)
        escape("unable to unload the block");

      return elle::Status::Ok;
    }

    ///
    /// this method checks the seam's consistency by making sure the child
    /// nodules' mayor key is the one being referenced in the inlets.
    ///
    template <typename V>
    elle::Status        Seam<V>::Check(Handle&                  parent,
                                       Handle&                  current,
                                       const Pins               pins)
    {
      // check the parent handle, if required.
      if (pins & PinParent)
        {
          if (this->parent != parent)
            escape("the parent handle is invalid");
        }

      // check the neighbours, if required.
      if (pins & PinNeighbours)
        {
          // XXX checker les addresses aussi

          if (this->left != Handle::Null)
            {
              Ambit< Contents< Seam<V> > >      left(this->left);

              if (left.Load() == elle::Status::Error)
                escape("unable to load the nodule");

              if (left.nodule()->right != current)
                escape("the neighbour link differs");

              if (left.Unload() == elle::Status::Error)
                escape("unable to unload the nodule");
            }

          if (this->right != Handle::Null)
            {
              Ambit< Contents< Seam<V> > >      right(this->right);

              if (right.Load() == elle::Status::Error)
                escape("unable to load the nodule");

              if (right.nodule()->left != current)
                escape("the neighbour link differs");

              if (right.Unload() == elle::Status::Error)
                escape("unable to unload the nodule");
            }
        }

      if (this->Check(current,
                      pins) == elle::Status::Error)
        escape("unable to check the seam consistency");

      return elle::Status::Ok;
    }

    ///
    /// this method traverses the seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Traverse(const elle::Natural32 margin)
    {
      elle::String      alignment(margin, ' ');
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      std::cout << alignment << "[Seam] " << this << std::endl;

      // dump the parent nodule.
      if (Nodule<V>::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent nodule");

      // dump the inlets.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Inlets] " << this->container.size() << std::endl;

      // go through the inlets.
      for (; iterator != end; ++iterator)
        {
          Seam<V>::I*                           inlet = iterator->second;
          Ambit< Contents< Nodule<V> > >        child(inlet->value);

          // dump the inlet.
          if (inlet->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the inlet");

          // load the value block.
          if (child.Load() == elle::Status::Error)
            escape("unable to load the block");

          // the child nodule must have been loaded.
          assert(child.nodule() != nullptr);

          // traverse the child.
          if (child.nodule()->Traverse(margin + 6) == elle::Status::Error)
            escape("unable to check the child nodule's consistency");

          // unload the value block.
          if (child.Unload() == elle::Status::Error)
            escape("unable to unload the block");
        }

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status        Seam<V>::Seal(elle::cryptography::SecretKey const&    secret,
                                      Address&                  address)
    {
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Seam<V>::I*                           inlet = iterator->second;
          Ambit< Contents< Nodule<V> > >        child(inlet->value);
          Address                               newaddress;

          // ignore nodules which have not been created or modified.
          //
          // such nodules can easily be identified since they have a
          // non-null placement.
          if (child.handle.placement == Placement::Null)
            continue;

          // load the value block.
          if (child.Load() == elle::Status::Error)
            escape("unable to load the block");

          // the child nodule must have been loaded.
          assert(child.nodule() != nullptr);

          // set the secret key.
          inlet->value.secret = secret;

          // seal the child.
          if (child.nodule()->Seal(inlet->value.secret,
                                   newaddress) == elle::Status::Error)
            escape("unable to seal the child nodule");

          // is the child dirty in which case the seam referencing
          // it would need updating.
          if (child.nodule()->state == StateDirty)
            {
              // update the child address.
              inlet->value.address = newaddress;

              // and set the seam as dirty i.e the nodule has been
              // modified and therefore needs sealing as well.
              this->contents()->state = StateDirty;
            }

          // unload the value block.
          if (child.Unload() == elle::Status::Error)
            escape("unable to unload the block");
        }

      // has the nodule been modified?
      if (this->contents()->state == StateDirty)
        {
          //
          // if so, the nodule needs to be encrypted with the new key
          // and bound so as to compute the new block address.
          //
          // this address will then be used to update the parent and
          // neighbour nodes.
          //

          // encrypt the seam itself with the secret.
          if (this->contents()->Encrypt(secret) == elle::Status::Error)
            escape("unable to encrypt the seam contents");

          // once the child addresses have been computed and recorded,
          // the seam contents can, in turn, be bound.
          if (this->contents()->Bind(address) == elle::Status::Error)
            escape("unable to bind the block");

          // this address can then be updated for the parent and neighbour
          // nodes.
          // XXX

          // set the quill as clean.
          this->contents()->state = StateClean;
        }
      else
        {
          //
          // otherwise, the nodule has not been updated since it was
          // loaded and can be thus ignored.
          //
        }

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the seam.
    ///
    template <typename V>
    elle::Status        Seam<V>::Dump(const elle::Natural32     margin)
      const
    {
      elle::String      alignment(margin, ' ');
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      std::cout << alignment << "[Seam] " << this << std::endl;

      // dump the parent nodule.
      if (Nodule<V>::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent nodule");

      // dump the inlets.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Inlets] " << this->container.size() << std::endl;

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          // dump the inlet.
          if (iterator->second->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the inlet");
        }

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the seam.
    ///
    //template <typename V>
    //elle::Status        Seam<V>::Serialize(elle::Archive&       archive) const
    //{
    //  auto              iterator = this->container.begin();
    //  auto              end = this->container.end();
    //  elle::Natural32   size;

    //  // serialize the parent nodule.
    //  if (Nodule<V>::Serialize(archive) == elle::Status::Error)
    //    escape("unable to serialize the parent nodule");

    //  // retrieve the container size.
    //  size = this->container.size();

    //  // serialize the container size.
    //  if (archive.Serialize(size) == elle::Status::Error)
    //    escape("unable to serialize the size");

    //  // go through the container.
    //  for (; iterator != end; ++iterator)
    //    {
    //      // serialize the key and inlet.
    //      if (archive.Serialize(*iterator->second) == elle::Status::Error)
    //        escape("unable to serialize the key/inlet tuple");
    //    }

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the attributes.
    /////
    //template <typename V>
    //elle::Status        Seam<V>::Extract(elle::Archive& archive)
    //{
    //  elle::Natural32   size;
    //  elle::Natural32   i;

    //  // extract the parent nodule.
    //  if (Nodule<V>::Extract(archive) == elle::Status::Error)
    //    escape("unable to extract the parent nodule");

    //  // extract the container size.
    //  if (archive.Extract(size) == elle::Status::Error)
    //    escape("unable to extract the size");

    //  // iterator.
    //  for (i = 0; i < size; i++)
    //    {
    //      // allocate an inlet.
    //      auto          inlet =
    //        std::unique_ptr< Seam<V>::I >(
    //          new Seam<V>::I);

    //      // extract the key and inlet.
    //      if (archive.Extract(*inlet.get()) == elle::Status::Error)
    //        escape("unable to extract the key/inlet tuple");

    //      // add the tuple to the seam.
    //      if (this->Insert(inlet.get()) == elle::Status::Error)
    //        escape("unable to add the key/tuple inlet to the seam");

    //      // release.
    //      inlet.release();
    //    }

    //  return elle::Status::Ok;
    //}

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the seams' initial footprint.
    ///
    template <typename V>
    elle::Natural32     Seam<V>::Footprint;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the seams.
    ///
    template <typename V>
    elle::Status        Seam<V>::Initialize()
    {
      Contents< Seam<V> >       contents;

      // create the contents.
      if (contents.Create() == elle::Status::Error)
        escape("unable to create the contents");

      // compute the initial footprint from which the Insert(), Delete()
      // methods will work in order to adjust it.
      if (contents.content->footprint.Compute() == elle::Status::Error)
        escape("unable to compute the footprint");

      // retrieve the initial seam footprint.
      Seam<V>::Footprint = contents.content->footprint.size;

      // register the seams to the nucleus' factory.
      {
        /* XXX[i.e Contents<>]
        // register the catalog-specific seam.
        if (Nucleus::Factory.Register< proton::Seam<neutron::Catalog> >
            (neutron::ComponentSeamCatalog) == elle::Status::Error)
          escape("unable to register the factory product");
        */

        // XXX
      }

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the seams.
    ///
    template <typename V>
    elle::Status        Seam<V>::Clean()
    {
      return elle::Status::Ok;
    }

  }
}

#endif
#ifndef  XXX_SEAM_HXX
# define XXX_SEAM_HXX

# include <cassert>

# include <XXX/Nodule.hh>
# include <XXX/Inlet.hh>

# include <XXX/Seam.hh>

namespace elle
{
  namespace serialize
  {

    template<typename V>
    struct ArchiveSerializer<nucleus::proton::Seam<V>>
      : public SplitSerializer<nucleus::proton::Seam<V>>
    {
    private:
      typedef nucleus::proton::Seam<V> Type;

    public:
      template<typename Archive> static void
        Save(Archive& ar, Type const& value, unsigned int version)
        {
          archive << static_cast<elle::proton::Nodule<V> const&>(value);
          Archive::ListSizeType size = value.container.size();
          archive << size;
          auto it = value.container.begin(),
               end = value.container.end();
          for (; it != end; ++it)
            {
              archive << it->first;
              archive << *(it->second);
            }
        }

      template<typename Archive> static void
        Load(Archive& ar, Type& value, unsigned int version)
        {
          archive >> static_cast<elle::proton::Nodule<V>&>(value);
          assert(value.container.size() == 0 && "Should not Load an already filled Seam<V>");
          Archive::ListSizeType size;
          archive >> size;
          for (Archive::ListSizeType i = 0; i < size; ++i)
            {
              typedef V::K k;
              archive >> k;
              auto v = archive.Construct<typename Type::I>();
              value.container[k] = v.get();
              v.release();
            }
        }
    };

  }
}

#endif
