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

#include <hole/Hole.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- macro-functions -------------------------------------------------
//

///
/// this macro-function loads a block.
///
#define SeamLoad(_object_, _element_)					\
  if (_object_->_element_ != Address::Null)				\
    {									\
      if (_object_->_ ## _element_ == NULL)				\
	{								\
	  printf("SEAM LOAD\n");					\
	}								\
    }									\
  if (_object_->_ ## _element_ != NULL)

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename V>
    Seam<V>::Seam():
      Nodule<V>(Nodule<V>::TypeSeam)
    {
    }

    ///
    /// specific constructor.
    ///
    template <typename V>
    Seam<V>::Seam(const elle::Callback<
		    elle::Status,
		    elle::Parameters<
		      const Address&,
		      Nodule<V>*&
		      >
		    >&						load,
		  const elle::Callback<
		    elle::Status,
		    elle::Parameters<
		      const Address&,
		      const Nodule<V>*
		      >
		    >&						unload):
      Nodule<V>(Nodule<V>::TypeSeam, load, unload)
    {
    }

    ///
    /// destructor.
    ///
    template <typename V>
    Seam<V>::~Seam()
    {
      typename Seam<V>::Scoutor::Forward	scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Seam<V>::I*	inlet = scoutor->second;

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
    elle::Status	Seam<V>::Create()
    {
      enter();

      // initialize the footprint.
      this->_footprint.size = Seam<V>::Footprint;

      // set the state.
      this->_state = StateDirty;

      leave();
    }

    ///
    /// this method inserts the given nodule in the seam.
    ///
    template <typename V>
    elle::Status	Seam<V>::Insert(const typename V::K&	key,
					Nodule<V>*		nodule)
    {
      typename Seam<V>::I*	inlet;

      enterx(instance(inlet));

      // create an inlet.
      inlet = new typename Seam<V>::I(key, nodule);

      // add the inlet to the seam.
      if (this->Insert(inlet) == elle::StatusError)
	escape("unable to add the value to the seam");

      // waive.
      waive(inlet);

      leave();
    }

    ///
    /// this method inserts the given inlet in the seam.
    ///
    template <typename V>
    elle::Status	Seam<V>::Insert(I*			inlet)
    {
      std::pair<typename Seam<V>::Iterator::Forward,
		elle::Boolean>					result;

      enter();

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

      // set the seam's parent link.
      inlet->_value->parent = Address::Some;
      inlet->_value->_parent = this;

      // compute the inlet's footprint.
      if (inlet->_footprint.Compute() == elle::StatusError)
	escape("unable to compute the inlet's footprint");

      // add the inlet footprint to the seam's.
      this->_footprint.size += inlet->_footprint.size;

      // set the state.
      this->_state = StateDirty;

      leave();
    }

    ///
    /// this method deletes the inlet referenced by the given iterator.
    ///
    template <typename V>
    elle::Status	Seam<V>::Delete(
			  typename Iterator::Forward&		iterator)
    {
      Seam<V>::I*	inlet;

      enter();

      // retrieve the inlet.
      inlet = iterator->second;

      // compute the inlet's footprint.
      if (inlet->_footprint.Compute() == elle::StatusError)
	escape("unable to compute the inlet's footprint");

      // substract the inlet footprint to the seam's.
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
    /// this method deletes the given nodule from the seam.
    ///
    template <typename V>
    elle::Status	Seam<V>::Delete(Nodule<V>*		nodule)
    {
      typename Seam<V>::Iterator::Forward	iterator;

      enter();

      // locate the inlet for the given nodule.
      if (this->Locate(nodule, iterator) == elle::StatusError)
	escape("unable to locate the given nodule");

      // delete the entry associated with the given iterator.
      if (this->Delete(iterator) == elle::StatusError)
	escape("unable to delete the entry associated with the iterator");

      leave();
    }

    ///
    /// this method deletes the given key from the seam.
    ///
    template <typename V>
    elle::Status	Seam<V>::Delete(const typename V::K&	key)
    {
      typename Seam<V>::Iterator::Forward	iterator;

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
    /// this method returns true if the given key exists.
    ///
    template <typename V>
    elle::Status	Seam<V>::Exist(const typename V::K&	key)
    {
      enter();

      // locate the given key.
      if (this->container.find(key) == this->container.end())
	false();

      true();
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
    elle::Status	Seam<V>::Lookup(
			  const typename V::K&			key,
			  typename Iterator::Forward&		iterator)
    {
      enter();

      // go through the container.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  Seam<V>::I*	inlet = iterator->second;

	  // check if this inlet is responsible for the given key or
	  // the end of the seam has been reached.
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
    elle::Status	Seam<V>::Lookup(const typename V::K&	key,
					I*&			inlet)
    {
      typename Seam<V>::Iterator::Forward	iterator;

      enter();

      // lookup the entry responsible for the given key.
      if (this->Lookup(key, iterator) == elle::StatusError)
	escape("unable to locate the entry");

      // return the inlet.
      inlet = iterator->second;

      leave();
    }

    ///
    /// this method returns the nodule responsible for the given key.
    ///
    template <typename V>
    elle::Status	Seam<V>::Lookup(const typename V::K&	key,
					Nodule<V>*&		nodule)
    {
      Seam<V>::I*	inlet;

      enter();

      // lookup the inlet associated with the given key.
      if (this->Lookup(key, inlet) == elle::StatusError)
	escape("unable to locate the inlet");

      // load the value block.
      SeamLoad(inlet, value);

      // return the nodule.
      nodule = inlet->_value;

      leave();
    }

    ///
    /// this method returns an iterator on the inlet associated with
    /// the given key.
    ///
    template <typename V>
    elle::Status	Seam<V>::Locate(
			  const typename V::K&			key,
			  typename Iterator::Forward&		iterator)
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
    elle::Status	Seam<V>::Locate(const typename V::K&	key,
					I*&			inlet)
    {
      typename Seam<V>::Iterator::Forward	iterator;

      enter();

      // locate the given key.
      if (this->Locate(key, iterator) == elle::StatusError)
	escape("unable to locate the entry associated with the given key");

      // return the inlet.
      inlet = iterator->second;

      leave();
    }

    ///
    /// this method returns the nodule associated with the given key.
    ///
    template <typename V>
    elle::Status	Seam<V>::Locate(const typename V::K&	key,
					Nodule<V>*&		nodule)
    {
      Seam<V>::I*	inlet;

      enter();

      // locate the given key.
      if (this->Locate(key, inlet) == elle::StatusError)
	escape("unable to locate the inlet associated with the given key");

      // load the value block.
      SeamLoad(inlet, value);

      // return the nodule.
      nodule = inlet->_value;

      leave();
    }

    ///
    /// this method takes the ancient key _from_, locates the inlet with
    /// this key and updates it with its new key i.e _to_.
    ///
    template <typename V>
    elle::Status	Seam<V>::Update(const typename V::K&	from,
					const typename V::K&	to)
    {
      typename Seam<V>::Iterator::Forward	iterator;
      Seam<V>::I*				inlet;

      enter();

      // locate the inlet.
      if (this->Locate(from, iterator) == elle::StatusError)
	escape("unable to locate the given inlet");

      // retrieve the inlet.
      inlet = iterator->second;

      // should the given key be different from the current one, update the
      // entry.
      if (to != inlet->key)
	{
	  std::pair<typename Seam<V>::Iterator::Forward,
		    elle::Boolean>		result;

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
	  this->_state = StateDirty;
	}

      leave();
    }

    ///
    /// this method is similar to Update() but also takes care of updating
    /// the parent nodule, if present, up to the tree's root nodule if
    /// necessary.
    ///
    template <typename V>
    elle::Status	Seam<V>::Propagate(const typename V::K&	from,
					   const typename V::K&	to)
    {
      typename V::K	ancient;
      typename V::K	recent;

      enter();

      // retrieve the current mayor key.
      if (this->Mayor(ancient) == elle::StatusError)
	escape("unable to retrieve the mayor key");

      // update the seam.
      if (this->Update(from, to) == elle::StatusError)
	escape("unable to update the nodule");

      // retrieve the new mayor key.
      if (this->Mayor(recent) == elle::StatusError)
	escape("unable to retrieve the mayor key");

      // finally, propagate the update should have the mayor key changed
      // and if a parent nodule exists.
      if ((recent != ancient) &&
	  (this->parent != Address::Null))
	{
	  // load the parent nodule.
	  SeamLoad(this, parent);

	  // progate the update.
	  if (this->_parent->Propagate(ancient, recent) == elle::StatusError)
	    escape("unable to propagate the update");
	}

      leave();
    }

    ///
    /// this method splits the current seam and returns a newly allocated
    /// seam i.e _right_.
    ///
    template <typename V>
    elle::Status	Seam<V>::Split(Seam<V>*&		right)
    {
      elle::Natural32	size;
      Seam<V>*		r;

      enterx(instance(r));

      // initialize _size_ as being the future left quills' size.
      size =
	hole::Hole::Descriptor.extent * hole::Hole::Descriptor.contention;

      // allocate a new seam.
      r = new Seam<V>(this->_load, this->_unload);

      // create the seam.
      if (r->Create() == elle::StatusError)
	escape("unable to create the seam");

      // export inlets from the current seam into the new seam.
      if (this->Export(r, size) == elle::StatusError)
	escape("unable to export inlets");

      // set the output right seam.
      right = r;

      // waive the r variable.
      waive(r);

      leave();
    }

    ///
    /// this method takes the given seam and merges its inlets with the
    /// current one.
    ///
    template <typename V>
    elle::Status	Seam<V>::Merge(Seam<V>*			seam)
    {
      typename V::K	t;
      typename V::K	s;

      enter();

      // retrieve the mayor key.
      if (this->Mayor(t) == elle::StatusError)
	escape("unable to retrieve the mayor key");

      // retrieve the mayor key.
      if (seam->Mayor(s) == elle::StatusError)
	escape("unable to retrieve the mayor key");

      // check which nodule has the lowest keys.
      if (s < t)
	{
	  // in this case, export the lower seam's inlets into the current's.
	  if (seam->Export(this) == elle::StatusError)
	    escape("unable to export the inlets");
	}
      else
	{
	  // otherwise, import the higher seam's inlets into the current's.
	  if (this->Import(seam) == elle::StatusError)
	    escape("unable to import the inlets");
	}

      leave();
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// this method returns the current seam's mayor key i.e the higehst
    /// key.
    ///
    template <typename V>
    elle::Status	Seam<V>::Mayor(typename V::K&		mayor) const
    {
      enter();

      // return the mayor key.
      mayor = this->container.rbegin()->first;

      leave();
    }

    ///
    /// this method returns the maiden key i.e the only remaining key.
    ///
    template <typename V>
    elle::Status	Seam<V>::Maiden(typename V::K&		maiden) const
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
    /// since the current nodule is not a quill, the request is forwarded
    /// to the child nodule which is responsible for the given key until
    /// the last level---i.e the quill---is reached.
    ///
    template <typename V>
    elle::Status	Seam<V>::Search(const typename V::K&	key,
					Quill<V>*&		quill)
    {
      typename Seam<V>::Iterator::Forward	iterator;
      Seam<V>::I*				inlet;

      enter();

      // look up the entry responsible for this key.
      if (this->Lookup(key, iterator) == elle::StatusError)
	escape("unable to look up the entry");

      // retrieve the inlet.
      inlet = iterator->second;

      // load the child nodule.
      SeamLoad(inlet, value);

      // search in this nodule.
      if (inlet->_value->Search(key, quill) == elle::StatusError)
	escape("unable to locate the quill for the given key");

      leave();
    }

    ///
    /// this method checks the seam's consistency by making sure the child
    /// nodules' mayor key is the one being referenced in the inlets.
    ///
    template <typename V>
    elle::Status	Seam<V>::Check() const
    {
      typename Seam<V>::Scoutor::Forward	scoutor;

      enter();

      // go through the inlets.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Seam<V>::I*				inlet = scoutor->second;
	  typename V::K				mayor;

	  // check the key.
	  if (inlet->key != scoutor->first)
	    escape("invalid key");

	  // load the value block.
	  SeamLoad(inlet, value);

	  // retrieve the child's mauor key.
	  if (inlet->_value->Mayor(mayor) == elle::StatusError)
	    escape("unable to retrieve the mayor key");

	  // compare the mayor key with the inlet's reference.
	  if (inlet->key != mayor)
	    escape("the child nodule's mayor key differs from its "
		   "reference");

	  // trigger the check on the child nodule.
	  if (inlet->_value->Check() == elle::StatusError)
	    escape("unable to check the child nodule's consistency");
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the seam.
    ///
    template <typename V>
    elle::Status	Seam<V>::Dump(const elle::Natural32	margin)
      const
    {
      elle::String				alignment(margin, ' ');
      typename Seam<V>::Scoutor::Forward	scoutor;

      enter();

      std::cout << alignment << "[Seam] " << this << std::endl;

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
    /// this method archives the seam.
    ///
    template <typename V>
    elle::Status	Seam<V>::Serialize(elle::Archive&	archive) const
    {
      typename Seam<V>::Scoutor::Forward	scoutor;
      elle::Natural32				size;

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
    elle::Status	Seam<V>::Extract(elle::Archive&	archive)
    {
      elle::Natural32	size;
      elle::Natural32	i;

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
	  Seam<V>::I*	inlet;

	  enterx(instance(inlet));

	  // allocate an inlet.
	  inlet = new Seam<V>::I;

	  // extract the key and inlet.
	  if (archive.Extract(*inlet) == elle::StatusError)
	    escape("unable to extract the key/inlet tuple");

	  // add the tuple to the seam.
	  if (this->Insert(inlet) == elle::StatusError)
	    escape("unable to add the key/tuple inlet to the seam");

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
    /// this variable defines the seams' initial footprint.
    ///
    template <typename V>
    elle::Natural32	Seam<V>::Footprint;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the seams.
    ///
    template <typename V>
    elle::Status	Seam<V>::Initialize()
    {
      Seam<V>		seam;

      enter();

      // compute the initial footprint from which the Insert(), Delete()
      // methods will work in order to adjust it.
      if (seam._footprint.Compute() == elle::StatusError)
	escape("unable to compute the footprint");

      // retrieve the initial seam footprint.
      Seam<V>::Footprint = seam._footprint.size;

      // register the seams to the nucleus' factory.
      {
	// register the catalog-specific seam.
	if (Nucleus::Factory.Register< proton::Seam<neutron::Catalog> >
	    (neutron::ComponentSeamCatalog) == elle::StatusError)
	  escape("unable to register the factory product");
      }

      leave();
    }

    ///
    /// this method cleans the seams.
    ///
    template <typename V>
    elle::Status	Seam<V>::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

  }
}

#endif
