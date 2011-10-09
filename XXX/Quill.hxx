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
    Quill<V>::Quill():
      Nodule<V>(Nodule<V>::TypeQuill)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Quill(const elle::Callback<
		      elle::Status,
		      elle::Parameters<
		        const Address&,
			Nodule<V>*&
			>
		      >&					load,
		    const elle::Callback<
		      elle::Status,
		      elle::Parameters<
			const Address&,
			const Nodule<V>*
			>
		      >&					unload):
      Nodule<V>(Nodule<V>::TypeQuill, load, unload)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::~Quill()
    {
      typename Quill<V>::Scoutor::Forward	scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Quill<V>::I*	inlet = scoutor->second;

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
    elle::Status	Quill<V>::Create()
    {
      enter();

      // initialize the footprint.
      this->_footprint.size = Quill<V>::Footprint;

      // set the state.
      this->_state = StateDirty;

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Insert(const typename V::K&	key,
					 V*			value)
    {
      typename Quill<V>::I*	inlet;

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
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Insert(I*			inlet)
    {
      std::pair<typename Quill<V>::Iterator::Forward,
		elle::Boolean>					result;

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
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Delete(
			  typename Iterator::Forward&		iterator)
    {
      Quill<V>::I*	inlet;

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
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Delete(V*			value)
    {
      typename Quill<V>::Iterator::Forward	iterator;

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
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Delete(const typename V::K&	key)
    {
      typename Quill<V>::Iterator::Forward	iterator;

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
    elle::Status	Quill<V>::Lookup(
			  const typename V::K&			key,
			  typename Iterator::Forward&		iterator)
    {
      enter();

      // go through the container.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  Quill<V>::I*	inlet = iterator->second;

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
    /// XXX lookup
    ///
    template <typename V>
    elle::Status	Quill<V>::Lookup(const typename V::K&	key,
					 I*&			inlet)
    {
      typename Quill<V>::Iterator::Forward	iterator;

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
    elle::Status	Quill<V>::Lookup(const typename V::K&	key,
					 V*&			value)
    {
      Quill<V>::I*	inlet;

      enter();

      // lookup the inlet associated with the given key.
      if (this->Lookup(key, inlet) == elle::StatusError)
	escape("unable to locate the inlet");

      // load the value block, if necessary.
      if (inlet->_value == NULL)
	{
	  // XXX load the value block: _value
	}

      // return the value.
      value = inlet->_value;

      leave();
    }

    ///
    /// XXX locate
    ///
    template <typename V>
    elle::Status	Quill<V>::Locate(
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
    elle::Status	Quill<V>::Locate(const typename V::K&	key,
					 I*&			inlet)
    {
      typename Quill<V>::Iterator::Forward	iterator;

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
    elle::Status	Quill<V>::Locate(const typename V::K&	key,
					 V*&			value)
    {
      Quill<V>::I*	inlet;

      enter();

      // locate the given key.
      if (this->Locate(key, inlet) == elle::StatusError)
	escape("unable to locate the inlet associated with the given key");

      // load the value block, if necessary.
      if (inlet->_value == NULL)
	{
	  // XXX load the value block: _value
	}

      // return the value.
      value = inlet->_value;

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Split(Quill<V>*&		right)
    {
      elle::Natural32	size;
      Quill<V>*		r;

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
      if (this->Export(size, r) == elle::StatusError)
	escape("unable to export inlets");

      // set the output right quill.
      right = r;

      // waive the r variable.
      waive(r);

      leave();
    }

    ///
    /// XXX
    ///
    /// XXX try not to load the left/right parent nodes i.e compare the
    ///     addresses
    template <typename V>
    elle::Status	Quill<V>::Balance()
    {
      elle::Natural32	size;
      Quill<V>*		left;
      Quill<V>*		right;

      enter();

      // retrieve the parent, if present.
      if (this->parent != Address::Null)
	{
	  // load the parent nodule, if necessary.
	  if (this->_parent == NULL)
	    {
	      // XXX load the parent nodule
	    }
	}

      // retrieve the left quill.
      if (this->left != Address::Null)
	{
	  // load the left nodule, if necessary.
	  if (this->_left == NULL)
	    {
	      // XXX load the left quill
	    }

	  // set the left.
	  left = static_cast<Quill<V>*>(this->_left);
	}
      else
	{
	  left = NULL;
	}

      // retrieve the right quill.
      if (this->right != Address::Null)
	{
	  // load the right nodule, if necessary.
	  if (this->_right == NULL)
	    {
	      // XXX load the right quill
	    }

	  // set the right.
	  right = static_cast<Quill<V>*>(this->_right);
	}
      else
	{
	  right = NULL;
	}

      // XXX
      if ((left != NULL) && (right != NULL) &&
	  (left->_state == StateDirty) &&
	  (right->_state == StateDirty) &&
	  ((size = ((left->_footprint.size +
		     this->_footprint.size +
		     right->_footprint.size) / 3)) >
	   static_cast<elle::Natural32>(
	     hole::Hole::Descriptor.extent *
	     hole::Hole::Descriptor.balancing)) &&
	  (left->parent == this->parent) &&
	  (right->parent == this->parent))
	{
	  typename V::K		major;

	  printf("CAS(1)\n");

	  printf("size = %u / %u\n", size,
		 (elle::Natural32)(hole::Hole::Descriptor.extent *
		  hole::Hole::Descriptor.balancing));

	  // XXX
	  left->Dump();
	  this->Dump();
	  right->Dump();

	  // XXX
	  if (left->_footprint.size > size)
	    {
	      elle::Natural32				footprint;
	      typename Quill<V>::Iterator::Forward	i;
	      typename Quill<V>::Iterator::Forward	j;

	      // XXX
	      footprint = Quill<V>::Footprint;

	      // XXX
	      for (i = left->container.begin();
		   i != left->container.end();
		   i++)
		{
		  Quill<V>::I*	inlet = i->second;

		  // XXX
		  if (footprint > size)
		    break;

		  // XXX compute footprint inlet

		  // XXX
		  footprint += inlet->_footprint.size;
		}

	      // XXX
	      for (j = i;
		   j != left->container.end();
		   j++)
		{
		  Quill<V>::I*	inlet = j->second;

		  // XXX
		  left->_footprint.size -= inlet->_footprint.size;

		  // XXX
		  if (this->Insert(inlet) == elle::StatusError)
		    escape("unable to insert the inlet");
		}

	      // XXX
	      left->container.erase(i, left->container.end());

	      // XXX
	      left->_footprint.state = elle::Footprint::StateConsistent;
	    }

	  // XXX
	  if (this->_footprint.size > size)
	    {
	      elle::Natural32				footprint;
	      typename Quill<V>::Iterator::Forward	i;
	      typename Quill<V>::Iterator::Forward	j;

	      // XXX
	      footprint = Quill<V>::Footprint;

	      // XXX
	      for (i = this->container.begin();
		   i != this->container.end();
		   i++)
		{
		  Quill<V>::I*	inlet = i->second;

		  // XXX
		  if (footprint > size)
		    break;

		  // XXX compute footprint inlet

		  // XXX
		  footprint += inlet->_footprint.size;
		}

	      // XXX
	      for (j = i;
		   j != this->container.end();
		   j++)
		{
		  Quill<V>::I*	inlet = j->second;

		  // XXX
		  this->_footprint.size -= inlet->_footprint.size;

		  // XXX
		  if (right->Insert(inlet) == elle::StatusError)
		    escape("unable to insert the inlet");
		}

	      // XXX
	      this->container.erase(i, this->container.end());

	      // XXX
	      this->_footprint.state = elle::Footprint::StateConsistent;
	    }

	  // XXX
	  if (right->_footprint.size > size)
	    {
	      elle::Natural32				footprint;
	      typename Quill<V>::Iterator::Backward	i;
	      typename Quill<V>::Iterator::Backward	j;

	      // XXX
	      footprint = Quill<V>::Footprint;

	      // XXX
	      for (i = right->container.rbegin();
		   i != right->container.rend();
		   i++)
		{
		  Quill<V>::I*	inlet = i->second;

		  // XXX
		  if (footprint > size)
		    break;

		  // XXX compute footprint inlet

		  // XXX
		  footprint += inlet->_footprint.size;
		}

	      // XXX
	      for (j = i;
		   j != right->container.rend();
		   j++)
		{
		  Quill<V>::I*	inlet = j->second;

		  // XXX
		  right->_footprint.size -= inlet->_footprint.size;

		  // XXX
		  if (this->Insert(inlet) == elle::StatusError)
		    escape("unable to insert the inlet");
		}

	      // XXX
	      right->container.erase(right->container.begin(), i.base());

	      // XXX
	      right->_footprint.state = elle::Footprint::StateConsistent;
	    }

	  // XXX
	  left->Dump();
	  this->Dump();
	  right->Dump();

	  /* XXX
	  // XXX
	  if (left->Major(major) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // update the parent seam.
	  if (left->_parent->Update(major) == elle::StatusError)
	    escape("unable to update the parent seam");

	  // XXX
	  if (this->Major(major) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // update the parent seam.
	  if (this->_parent->Update(major) == elle::StatusError)
	    escape("unable to update the parent seam");

	  // XXX
	  if (right->Major(major) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // update the parent seam.
	  if (right->_parent->Update(major) == elle::StatusError)
	    escape("unable to update the parent seam");
	  */

	  // XXX pas si simple car en faisant ca le Insert pourrait spliter?

	  // XXX on pourrait optimiser Insert() pour ne pas faire l'Update
	  //  mais plutot appeler a la main quand necessaire.

	  // XXX on peut equilibrer sur 3 noeuds
	}
      else if ((left != NULL) &&
	       (left->_state == StateDirty) &&
	       (left->parent == this->parent))
	{
	  printf("CAS(2)\n");

	  // XXX on peut equilibrer sur 2 noeuds: left
	}
      else if ((right != NULL) &&
	       (right->_state = StateDirty) &&
	       (right->parent == this->parent))
	{
	  printf("CAS(3)\n");

	  // XXX on peut equilibrer sur 2 noeuds: right
	}
      else
	{
	  printf("CAS(4) NOTHING TO DO ... OR ... TRY TO MERGE\n");
	}

      leave();
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Major(typename V::K&		major) const
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
    elle::Status	Quill<V>::Search(const typename V::K&,
					 Quill<V>*&		quill)
    {
      enter();

      // return this quill.
      quill = this;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Dump(const elle::Natural32	margin)
      const
    {
      elle::String				alignment(margin, ' ');
      typename Quill<V>::Scoutor::Forward	scoutor;

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
    elle::Status	Quill<V>::Serialize(elle::Archive&	archive) const
    {
      typename Quill<V>::Scoutor::Forward	scoutor;
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
    elle::Status	Quill<V>::Extract(elle::Archive&	archive)
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
	  Quill<V>::I*	inlet;

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
    /// XXX
    ///
    template <typename V>
    elle::Natural32		Quill<V>::Footprint;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Initialize()
    {
      Quill<V>		quill;

      enter();

      // create the quill.
      if (quill.Create() == elle::StatusError)
	escape("unable to create a quill");

      // retrieve the initial quill footprint.
      Quill<V>::Footprint = quill._footprint.size;

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

  }
}

#endif
