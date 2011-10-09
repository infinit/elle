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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::Seam():
      Nodule<V>(Nodule<V>::TypeSeam)
    {
    }

    ///
    /// XXX
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
    /// XXX
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
    /// XXX
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
    /// XXX
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
    /// XXX
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
      //
      // note that the actual parent nodule's address is not computed as
      // it may need recomputing afterwards. therefore, the address is set
      // as being different from null but undefined so as to get computed only
      // once.
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
    /// XXX
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
    /// XXX
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
    /// XXX
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
    /// XXX 
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
    /// XXX lookup
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
    /// XXX lookup
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

      // load the value nodule, if necessary.
      if (inlet->_value == NULL)
	{
	  // XXX load the value nodule.
	}

      // return the nodule.
      nodule = inlet->_value;

      leave();
    }

    ///
    /// XXX locate
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
    /// XXX locate
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
    /// XXX locate
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

      // load the value nodule, if necessary.
      if (inlet->_value == NULL)
	{
	  // XXX load the value nodule.
	}

      // return the nodule.
      nodule = inlet->_value;

      leave();
    }

    ///
    /// XXX
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
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Propagate(const typename V::K&	from,
					   const typename V::K&	to)
    {
      typename V::K	ancient;
      typename V::K	recent;

      enter();

      // retrieve the current major key.
      if (this->Major(ancient) == elle::StatusError)
	escape("unable to retrieve the major key");

      // update the seam.
      if (this->Update(from, to) == elle::StatusError)
	escape("unable to update the nodule");

      // retrieve the new major key.
      if (this->Major(recent) == elle::StatusError)
	escape("unable to retrieve the major key");

      // finally, propagate the update should have the major key changed
      // and if a parent nodule exists.
      if ((recent != ancient) && (this->parent != Address::Null))
	{
	  // load the parent nodule, if necessary.
	  if (this->_parent == NULL)
	    {
	      // XXX load the parent.
	    }

	  // progate the update.
	  if (this->_parent->Propagate(ancient, recent) == elle::StatusError)
	    escape("unable to propagate the update");
	}

      leave();
    }

    ///
    /// XXX
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
      if (this->Export(size, r) == elle::StatusError)
	escape("unable to export inlets");

      // set the output right seam.
      right = r;

      // waive the r variable.
      waive(r);

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Balance()
    {
      enter();

	  // XXX 1) balance: s = [size(l) + size(c) + size(r)] / 3
	  // XXX   si parent.{l,c,r} equivalent && .state && s > balancing
	  // XXX 2) merge

      leave();
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Major(typename V::K&		major) const
    {
      enter();

      // return the major key.
      major = this->container.rbegin()->first;

      leave();
    }

    ///
    /// XXX
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

      // load the child nodule, if necessary
      if (inlet->_value == NULL)
	{
	  /* XXX load value
	  // load the child nodule.
	  if (this->_load.Call(inlet->address,
			       inlet->_value) == elle::StatusError)
	    escape("unable to load the child nodule");
	  */
	}

      // search in this nodule.
      if (inlet->_value->Search(key, quill) == elle::StatusError)
	escape("unable to locate the quill for the given key");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
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
    /// XXX
    ///
    template <typename V>
    elle::Natural32		Seam<V>::Footprint;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Initialize()
    {
      Seam<V>		seam;

      enter();

      // create the seam.
      if (seam.Create() == elle::StatusError)
	escape("unable to create a seam");

      // retrieve the initial seam footprint.
      Seam<V>::Footprint = seam._footprint.size;

      leave();
    }

    ///
    /// XXX
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
