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

#include <XXX/Porcupine.hh>
#include <XXX/Nest.hh>

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
    template <typename V>
    Quill<V>::Quill():
      Nodule<V>(Nodule<V>::TypeQuill)
    {
    }

    ///
    /// specific constructor.
    ///
    template <typename V>
    Quill<V>::Quill(const elle::Callback<
                      elle::Status,
                      elle::Parameters<
                        const Address&,
                        Nodule<V>*&
                        >
                      >&                                        load,
                    const elle::Callback<
                      elle::Status,
                      elle::Parameters<
                        const Address&,
                        const Nodule<V>*
                        >
                      >&                                        unload):
      Nodule<V>(Nodule<V>::TypeQuill, load, unload)
    {
    }

    ///
    /// destructor.
    ///
    template <typename V>
    Quill<V>::~Quill()
    {
      typename Quill<V>::Scoutor::Forward       scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          Quill<V>::I*  inlet = scoutor->second;

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
      enter();

      // initialize the footprint.
      this->_footprint.size = Quill<V>::Footprint;

      // set the state.
      this->_state = StateDirty;

      leave();
    }

    ///
    /// this method inserts a value in the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Insert(const typename V::K&   key,
                                         V*                     value)
    {
      typename Quill<V>::I*     inlet;

      enterx(instance(inlet));

      // create an inlet.
      inlet = new typename Quill<V>::I(key, value);

      // add the inlet to the quill.
      if (this->Insert(inlet) == elle::StatusError)
        escape("unable to add the value to the quill");

      // waive.
      waive(inlet);

      leave();
    }

    ///
    /// this method inserts an inlet in the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Insert(I*                     inlet)
    {
      std::pair<typename Quill<V>::Iterator::Forward,
                elle::Boolean>                                  result;

      enter();

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
      if (inlet->_footprint.Compute() == elle::StatusError)
        escape("unable to compute the inlet's footprint");

      // add the inlet footprint to the quill's.
      this->_footprint.size += inlet->_footprint.size;

      // set the state.
      this->_state = StateDirty;

      leave();
    }

    ///
    /// this method deletes an entry from the quill, given an iterator.
    ///
    template <typename V>
    elle::Status        Quill<V>::Delete(
                          typename Iterator::Forward&           iterator)
    {
      Quill<V>::I*      inlet;

      enter();

      // retrieve the inlet.
      inlet = iterator->second;

      // compute the inlet's footprint.
      if (inlet->_footprint.Compute() == elle::StatusError)
        escape("unable to compute the inlet's footprint");

      // substract the inlet footprint to the quill's.
      this->_footprint.size -= inlet->_footprint.size;

      // delete the inlet.
      delete inlet;

      // finally, erase the entry.
      this->container.erase(iterator);

      // set the state.
      this->_state = StateDirty;

      leave();
    }

    ///
    /// this method deletes a value from the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Delete(V*                     value)
    {
      typename Quill<V>::Iterator::Forward      iterator;

      enter();

      // locate the inlet for the given value.
      if (this->Locate(value, iterator) == elle::StatusError)
        escape("unable to locate the given nodule");

      // delete the entry associated with the given iterator.
      if (this->Delete(iterator) == elle::StatusError)
        escape("unable to delete the entry associated with the iterator");

      leave();
    }

    ///
    /// this method deletes a key from the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Delete(const typename V::K&   key)
    {
      typename Quill<V>::Iterator::Forward      iterator;

      enter();

      // locate the inlet for the given key.
      if (this->Locate(key, iterator) == elle::StatusError)
        escape("unable to locate the given key");

      // delete the entry associated with the given iterator.
      if (this->Delete(iterator) == elle::StatusError)
        escape("unable to delete the entry associated with the iterator");

      leave();
    }

    ///
    /// this method returns true if the given key exists in the quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Exist(const typename V::K&    key)
    {
      enter();

      // locate the given key.
      if (this->container.find(key) == this->container.end())
        false();

      true();
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
      enter();

      // go through the container.
      for (iterator = this->container.begin();
           iterator != this->container.end();
           iterator++)
        {
          Quill<V>::I*  inlet = iterator->second;

          // check if this inlet is responsible for the given key or
          // the end of the quill has been reached.
          if ((key <= iterator->first) ||
              (inlet == this->container.rbegin()->second))
            {
              // return with the correct iterator set.
              
              leave();
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

      enter();

      // lookup the entry responsible for the given key.
      if (this->Lookup(key, iterator) == elle::StatusError)
        escape("unable to locate the entry");

      // return the inlet.
      inlet = iterator->second;

      leave();
    }

    ///
    /// this method returns the value responsible for the given key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Lookup(const typename V::K&   key,
                                         V*&                    value)
    {
      Quill<V>::I*      inlet;

      enter();

      // lookup the inlet associated with the given key.
      if (this->Lookup(key, inlet) == elle::StatusError)
        escape("unable to locate the inlet");

      // load the value block.
      NestLoad(inlet, value);

      // return the value.
      value = inlet->_value;

      leave();
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
      enter();

      // locate the given key.
      if ((iterator = this->container.find(key)) == this->container.end())
        escape("unable to locate the given key");

      leave();
    }

    ///
    /// this method returns the inlet associated with the given key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Locate(const typename V::K&   key,
                                         I*&                    inlet)
    {
      typename Quill<V>::Iterator::Forward      iterator;

      enter();

      // locate the given key.
      if (this->Locate(key, iterator) == elle::StatusError)
        escape("unable to locate the entry associated with the given key");

      // return the inlet.
      inlet = iterator->second;

      leave();
    }

    ///
    /// this method returns the value associated with the given key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Locate(const typename V::K&   key,
                                         V*&                    value)
    {
      Quill<V>::I*      inlet;

      enter();

      // locate the given key.
      if (this->Locate(key, inlet) == elle::StatusError)
        escape("unable to locate the inlet associated with the given key");

      // load the value block.
      NestLoad(inlet, value);

      // return the value.
      value = inlet->_value;

      leave();
    }

    ///
    /// this method splits the current quill and returns in the given
    /// _right_ variable the newly created quill.
    ///
    template <typename V>
    elle::Status        Quill<V>::Split(Quill<V>*&              right)
    {
      elle::Natural32   size;
      Quill<V>*         r;

      enterx(instance(r));

      // initialize _size_ as being the future left quills' size.
      size =
        hole::Hole::Descriptor.extent * hole::Hole::Descriptor.contention;

      // allocate a new quill.
      r = new Quill<V>(this->_load, this->_unload);

      // create the quill.
      if (r->Create() == elle::StatusError)
        escape("unable to create the quill");

      // export inlets from the current seam into the new seam.
      if (this->Export(r, size) == elle::StatusError)
        escape("unable to export inlets");

      // set the output right quill.
      right = r;

      // waive the r variable.
      waive(r);

      leave();
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

      enter();

      // retrieve the mayor key.
      if (this->Mayor(t) == elle::StatusError)
        escape("unable to retrieve the mayor key");

      // retrieve the mayor key.
      if (quill->Mayor(q) == elle::StatusError)
        escape("unable to retrieve the mayor key");

      // check which nodule has the lowest keys.
      if (q < t)
        {
          // in this case, export the lower quill's inlets into the current's.
          if (quill->Export(this) == elle::StatusError)
            escape("unable to export the inlets");
        }
      else
        {
          // otherwise, import the higher quill's inlets into the current's.
          if (this->Import(quill) == elle::StatusError)
            escape("unable to import the inlets");
        }

      leave();
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
      enter();

      // return the mayor key.
      mayor = this->container.rbegin()->first;

      leave();
    }

    ///
    /// this method returns the maiden key i.e the single remaining key.
    ///
    template <typename V>
    elle::Status        Quill<V>::Maiden(typename V::K&         maiden) const
    {
      enter();

      // check if a single inlet is present.
      if (this->container.size() != 1)
        escape("unable to retrieve the maiden; multiple inlets are present");

      // return the maiden key.
      maiden = this->container.begin()->first;

      leave();
    }

    ///
    /// this method returns the quill responsible for the given key.
    ///
    /// since the current nodule is a quill, it is its responsability.
    ///
    template <typename V>
    elle::Status        Quill<V>::Search(const typename V::K&,
                                         Quill<V>*&             quill)
    {
      enter();

      // return this quill.
      quill = this;

      leave();
    }

    ///
    /// this method checks the quill's consistency.
    ///
    template <typename V>
    elle::Status        Quill<V>::Check() const
    {
      typename Quill<V>::Scoutor::Forward       scoutor;

      enter();

      // go through the inlets.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          Quill<V>::I*                          inlet = scoutor->second;

          // check the key.
          if (inlet->key != scoutor->first)
            escape("invalid key");
        }

      leave();
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
      elle::String                              alignment(margin, ' ');
      typename Quill<V>::Scoutor::Forward       scoutor;

      enter();

      std::cout << alignment << "[Quill] " << this << std::endl;

      // dump the parent nodule.
      if (Nodule<V>::Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the parent nodule");

      // dump the inlets.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Inlets] " << this->container.size() << std::endl;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          // dump the inlet.
          if (scoutor->second->Dump(margin + 4) == elle::StatusError)
            escape("unable to dump the inlet");
        }

      leave();
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
      typename Quill<V>::Scoutor::Forward       scoutor;
      elle::Natural32                           size;

      enter();

      // serialize the parent nodule.
      if (Nodule<V>::Serialize(archive) == elle::StatusError)
        escape("unable to serialize the parent nodule");

      // retrieve the container size.
      size = this->container.size();

      // serialize the container size.
      if (archive.Serialize(size) == elle::StatusError)
        escape("unable to serialize the size");

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          // serialize the key and inlet.
          if (archive.Serialize(*scoutor->second) == elle::StatusError)
            escape("unable to serialize the key/inlet tuple");
        }

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    template <typename V>
    elle::Status        Quill<V>::Extract(elle::Archive&        archive)
    {
      elle::Natural32   size;
      elle::Natural32   i;

      enter();

      // extract the parent nodule.
      if (Nodule<V>::Extract(archive) == elle::StatusError)
        escape("unable to extract the parent nodule");

      // extract the container size.
      if (archive.Extract(size) == elle::StatusError)
        escape("unable to extract the size");

      // iterator.
      for (i = 0; i < size; i++)
        {
          Quill<V>::I*  inlet;

          enterx(instance(inlet));

          // allocate an inlet.
          inlet = new Quill<V>::I;

          // extract the key and inlet.
          if (archive.Extract(*inlet) == elle::StatusError)
            escape("unable to extract the key/inlet tuple");

          // add the tuple to the quill.
          if (this->Insert(inlet) == elle::StatusError)
            escape("unable to add the key/tuple inlet to the quill");

          // waive.
          waive(inlet);

          release();
        }

      leave();
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
      Quill<V>          quill;

      enter();

      // compute the initial footprint from which the Insert(), Delete()
      // methods will work in order to adjust it.
      if (quill._footprint.Compute() == elle::StatusError)
        escape("unable to compute the footprint");

      // retrieve the initial quill footprint.
      Quill<V>::Footprint = quill._footprint.size;

      // register the quills to the nucleus' factory.
      {
        // register the catalog-specific quill.
        if (Nucleus::Factory.Register< proton::Quill<neutron::Catalog> >
            (neutron::ComponentQuillCatalog) == elle::StatusError)
          escape("unable to register the factory product");
      }

      leave();
    }

    ///
    /// this method cleans the quills.
    ///
    template <typename V>
    elle::Status        Quill<V>::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

  }
}

#endif
