//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 20:56:15 2011]
//

#ifndef NUCLEUS_PROTON_QUILL_HXX
#define NUCLEUS_PROTON_QUILL_HXX

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
    Quill<V>::Quill():
      Nodule<V>(Nodule<V>::TypeQuill)
    {
    }
    */

    ///
    /// destructor.
    ///
    template <typename V>
    Quill<V>::~Quill()
    {
      auto      iterator = this->container.begin();
      auto      end = this->container.end();

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Quill<V>::I*  inlet = iterator->second;

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
    /// this method creates a quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Create()
    {
      // initialize the footprint.
      this->footprint.size = Quill<V>::Footprint;

      // set the state.
      this->contents()->state = StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method inserts an inlet in the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Insert(I*                     inlet)
    {
      std::pair<typename Quill<V>::Iterator::Forward,
                elle::Boolean>                                  result;

      // check if this key has already been recorded.
      if (this->container.find(inlet->key) != this->container.end())
        escape("this key seems to have already been recorded");

      // insert the inlet in the container.
      result = this->container.insert(
                 std::pair<const typename V::K,
                           Quill<V>::I*>(inlet->key, inlet));

      // check if the insertion was successful.
      if (result.second == false)
        escape("unable to insert the inlet in the container");

      // compute the inlet's footprint.
      if (inlet->footprint.Compute() == elle::Status::Error)
        escape("unable to compute the inlet's footprint");

      // add the inlet footprint to the quill's.
      this->footprint.size += inlet->footprint.size;

      // set the state.
      this->contents()->state = StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method inserts a value in the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Insert(const typename V::K&   key,
                                         Handle&                value)
    {
      // create an inlet.
      auto              inlet =
        std::unique_ptr< typename Quill<V>::I >(
          new typename Quill<V>::I(key, value));

      // add the inlet to the quill.
      if (this->Insert(inlet.get()) == elle::Status::Error)
        escape("unable to add the value to the quill");

      // release the tracking.
      inlet.release();

      return elle::Status::Ok;
    }

    ///
    /// this method deletes an entry from the quill, given an iterator.
    ///
    template <typename V>
    elle::Status        Quill<V>::Delete(
                          typename Iterator::Forward&           iterator)
    {
      Quill<V>::I*      inlet;

      // retrieve the inlet.
      inlet = iterator->second;

      // compute the inlet's footprint.
      if (inlet->footprint.Compute() == elle::Status::Error)
        escape("unable to compute the inlet's footprint");

      // substract the inlet footprint to the quill's.
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
    /// this method deletes a key from the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Delete(const typename V::K&   key)
    {
      typename Quill<V>::Iterator::Forward      iterator;

      // locate the inlet for the given key.
      if (this->Locate(key, iterator) == elle::Status::Error)
        escape("unable to locate the given key");

      // delete the entry associated with the given iterator.
      if (this->Delete(iterator) == elle::Status::Error)
        escape("unable to delete the entry associated with the iterator");

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the given key exists in the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Exist(const typename V::K&    key)
    {
      // locate the given key.
      if (this->container.find(key) == this->container.end())
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// this method returns an iterator on the inlet responsible for
    /// the given key.
    ///
    /// note that contrary to Locate(), Lookup() does not look for the
    /// exact key but for the key just greater than the given one.
    ///
    template <typename V>
    elle::Status        Quill<V>::Lookup(
                          const typename V::K&                  key,
                          typename Iterator::Forward&           iterator)
    {
      auto              end = this->container.end();
      auto              rbegin = this->container.rbegin();

      // go through the container.
      for (iterator = this->container.begin(); iterator != end; ++iterator)
        {
          Quill<V>::I*  inlet = iterator->second;

          // check if this inlet is responsible for the given key or
          // the end of the quill has been reached.
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
    elle::Status        Quill<V>::Lookup(const typename V::K&   key,
                                         I*&                    inlet)
    {
      typename Quill<V>::Iterator::Forward      iterator;

      // lookup the entry responsible for the given key.
      if (this->Lookup(key, iterator) == elle::Status::Error)
        escape("unable to locate the entry");

      // return the inlet.
      inlet = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the value responsible for the given key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Lookup(const typename V::K&   key,
                                         Handle&                handle)
    {
      Quill<V>::I*      inlet;

      // lookup the inlet associated with the given key.
      if (this->Lookup(key, inlet) == elle::Status::Error)
        escape("unable to locate the inlet");

      // return the handle.
      handle = inlet->value;

      return elle::Status::Ok;
    }

    ///
    /// this method returns an iterator on the inlet associated with
    /// the given key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Locate(
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
    elle::Status        Quill<V>::Locate(const typename V::K&   key,
                                         I*&                    inlet)
    {
      typename Quill<V>::Iterator::Forward      iterator;

      // locate the given key.
      if (this->Locate(key, iterator) == elle::Status::Error)
        escape("unable to locate the entry associated with the given key");

      // return the inlet.
      inlet = iterator->second;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the value associated with the given key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Locate(const typename V::K&   key,
                                         Handle&                handle)
    {
      Quill<V>::I*      inlet;

      // locate the given key.
      if (this->Locate(key, inlet) == elle::Status::Error)
        escape("unable to locate the inlet associated with the given key");

      // return the handle.
      handle = inlet->value;

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    /// there is nothing to do in this case since quills
    /// reference values i.e data objects.
    ///
    /// these data objects do not reference the parent
    /// nodule. therefore, there is nothing to update
    /// at this point.
    ///
    template <typename V>
    elle::Status        Quill<V>::Link(I*,
                                       Handle&)
    {
      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status        Quill<V>::Link(Handle&)
    {
      return elle::Status::Ok;
    }

    ///
    /// this method takes the current quill and the given one and merges
    /// them into a single one i.e the current one.
    ///
    template <typename V>
    elle::Status        Quill<V>::Merge(Quill<V>*               quill)
    {
      typename V::K     t;
      typename V::K     q;

      // retrieve the mayor key.
      if (this->Mayor(t) == elle::Status::Error)
        escape("unable to retrieve the mayor key");

      // retrieve the mayor key.
      if (quill->Mayor(q) == elle::Status::Error)
        escape("unable to retrieve the mayor key");

      // check which nodule has the lowest keys.
      if (q < t)
        {
          // in this case, export the lower quill's inlets into the current's.
          if (quill->Export(this) == elle::Status::Error)
            escape("unable to export the inlets");
        }
      else
        {
          // otherwise, import the higher quill's inlets into the current's.
          if (this->Import(quill) == elle::Status::Error)
            escape("unable to import the inlets");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method checks the quill's consistency.
    ///
    template <typename V>
    elle::Status        Quill<V>::Check(Handle&,
                                        const Pins              pins)
    {
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Quill<V>::I*          inlet = iterator->second;
          Ambit< Contents<V> >  value(inlet->value);

          // load the value block.
          if (value.Load() == elle::Status::Error)
            escape("unable to load the block");

          // the value block must have been loaded.
          assert(value.content() != nullptr);

          // check the address, if required.
          if (pins & PinAddress)
            {
              Address   address;

              // bind the value block.
              if (value.contents()->Bind(address) == elle::Status::Error)
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
            }

          // unload the value block.
          if (value.Unload() == elle::Status::Error)
            escape("unable to unload the block");
        }

      return elle::Status::Ok;
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// this method returns the mayor key i.e the key with the higest value.
    ///
    template <typename V>
    elle::Status        Quill<V>::Mayor(typename V::K&          mayor) const
    {
      // return the mayor key.
      mayor = this->container.rbegin()->first;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the maiden key i.e the single remaining key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Maiden(typename V::K&         maiden) const
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
    /// since the current nodule is a quill, it is its responsability.
    ///
    template <typename V>
    elle::Status        Quill<V>::Search(const typename V::K&,
                                         Handle&)
    {
      // do nothing as the seam's Search() method will have updated the
      // handle with the reference to _this_ quill..

      return elle::Status::Ok;
    }

    ///
    /// this method checks the quill's consistency.
    ///
    template <typename V>
    elle::Status        Quill<V>::Check(Handle&                 parent,
                                        Handle&                 current,
                                        const Pins              pins)
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
              Ambit< Contents< Quill<V> > >     left(this->left);

              if (left.Load() == elle::Status::Error)
                escape("unable to load the nodule");

              if (left.nodule()->right != current)
                escape("the neighbour link differs");

              if (left.Unload() == elle::Status::Error)
                escape("unable to unload the nodule");
            }

          if (this->right != Handle::Null)
            {
              Ambit< Contents< Quill<V> > >     right(this->right);

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
        escape("unable to check the quill consistency");

      return elle::Status::Ok;
    }

    ///
    /// this method traverses the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Traverse(const elle::Natural32 margin)
    {
      elle::String      alignment(margin, ' ');
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      std::cout << alignment << "[Quill] " << this << std::endl;

      // dump the parent nodule.
      if (Nodule<V>::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent nodule");

      // dump the inlets.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Inlets] " << this->container.size() << std::endl;

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Quill<V>::I*  inlet = iterator->second;

          // dump the inlet.
          if (inlet->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the inlet");
        }

      return elle::Status::Ok;
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status        Quill<V>::Seal(elle::cryptography::SecretKey const&   secret,
                                       Address&                 address)
    {
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Quill<V>::I*          inlet = iterator->second;
          Ambit< Contents<V> >  value(inlet->value);

          // ignore blocks which have not been created or modified.
          //
          // such blocks can easily be identified since they have a
          // non-null placement.
          if (value.handle.placement == Placement::Null)
            continue;

          // load the value block.
          if (value.Load() == elle::Status::Error)
            escape("unable to load the block");

          // the value block must have been loaded.
          assert(value.content() != nullptr);

          // besides, look at the value state to see whether the
          // value has been modified i.e has a dirty state. if
          // not, the block has probably been loaded but never
          // modified.
          if (value.contents()->state == StateClean)
            continue;

          //
          // at this point, the value has been modified.
          //

          // set the secret.
          inlet->value.secret = secret;

          // encrypt the value contents.
          if (value.contents()->Encrypt(inlet->value.secret) == elle::Status::Error)
            escape("unable to encrypt the block");

          // bind the value block.
          if (value.contents()->Bind(inlet->value.address) == elle::Status::Error)
            escape("unable to bind the block");

          // unload the value block.
          if (value.Unload() == elle::Status::Error)
            escape("unable to unload the block");

          // set the quill as dirty i.e the nodule has been
          // modified and therefore needs sealing as well.
          this->contents()->state = StateDirty;
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

          //
          // this address can then be updated for the parent and neighbour
          // nodes.
          //

          // update the parent link.
          // XXX IMPOSSIBLE: pour connaitre son adresse, le parent doit etre
          //     sealed mais pour etre sealed son fils doit etre sealed: cycle!

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
    /// this method dumps the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Dump(const elle::Natural32    margin)
      const
    {
      elle::String      alignment(margin, ' ');
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      std::cout << alignment << "[Quill] " << this << std::endl;

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
    /// this method archives the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Serialize(elle::Archive&      archive) const
    {
      auto              iterator = this->container.begin();
      auto              end = this->container.end();
      elle::Natural32   size;

      // serialize the parent nodule.
      if (Nodule<V>::Serialize(archive) == elle::Status::Error)
        escape("unable to serialize the parent nodule");

      // retrieve the container size.
      size = this->container.size();

      // serialize the container size.
      if (archive.Serialize(size) == elle::Status::Error)
        escape("unable to serialize the size");

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          // serialize the key and inlet.
          if (archive.Serialize(*iterator->second) == elle::Status::Error)
            escape("unable to serialize the key/inlet tuple");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method extracts the attributes.
    ///
    template <typename V>
    elle::Status        Quill<V>::Extract(elle::Archive&        archive)
    {
      elle::Natural32   size;
      elle::Natural32   i;

      // extract the parent nodule.
      if (Nodule<V>::Extract(archive) == elle::Status::Error)
        escape("unable to extract the parent nodule");

      // extract the container size.
      if (archive.Extract(size) == elle::Status::Error)
        escape("unable to extract the size");

      // iterator.
      for (i = 0; i < size; i++)
        {
          // allocate an inlet.
          auto          inlet =
            std::unique_ptr<Quill<V>::I>(
              new Quill<V>::I);

          // extract the key and inlet.
          if (archive.Extract(*inlet.get()) == elle::Status::Error)
            escape("unable to extract the key/inlet tuple");

          // add the tuple to the quill.
          if (this->Insert(inlet.get()) == elle::Status::Error)
            escape("unable to add the key/tuple inlet to the quill");

          // release the tracking.
          inlet.release();
        }

      return elle::Status::Ok;
    }

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the quills' initial footprint.
    ///
    template <typename V>
    elle::Natural32     Quill<V>::Footprint;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the quills.
    ///
    template <typename V>
    elle::Status        Quill<V>::Initialize()
    {
      Contents< Quill<V> >      contents;

      // create the contents.
      if (contents.Create() == elle::Status::Error)
        escape("unable to create the contents");

      // compute the initial footprint from which the Insert(), Delete()
      // methods will work in order to adjust it.
      if (contents.content->footprint.Compute() == elle::Status::Error)
        escape("unable to compute the footprint");

      // retrieve the initial quill footprint.
      Quill<V>::Footprint = contents.content->footprint.size;

      // register the quills to the nucleus' factory.
      {
        /* XXX[i.e Contents<>]
        // register the catalog-specific quill.
        if (Nucleus::Factory.Register< proton::Quill<neutron::Catalog> >
            (neutron::ComponentQuillCatalog) == elle::Status::Error)
          escape("unable to register the factory product");
        */

        // XXX
      }

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the quills.
    ///
    template <typename V>
    elle::Status        Quill<V>::Clean()
    {
      return elle::Status::Ok;
    }

  }
}

#endif
#ifndef  XXX_QUILL_HXX
# define XXX_QUILL_HXX

# include <cassert>

# include <XXX/Nodule.hh>
# include <XXX/Inlet.hh>

namespace elle
{
  namespace serialize
  {

    template<typename V>
    struct ArchiveSerializer<nucleus::proton::Quill<V>>
      : public SplitSerializer<nucleus::proton::Quill<V>>
    {
    private:
      typedef nucleus::proton::Quill<V> Type;

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
          assert(value.container.size() == 0 && "Should not Load an already filled Quill<V>");
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
