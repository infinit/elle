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
      Seam<V>::Scoutor	scoutor;

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

      // compute the initial footprint from which the Insert(), Delete()
      // methods will work in order to adjust it.
      if (this->_footprint.Compute() == elle::StatusError)
	escape("unable to compute the footprint");

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

      enter(instance(inlet));

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
      std::pair<Seam<V>::Iterator, elle::Boolean>	result;

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

      // set the quill's parent link.
      inlet->_value->_parent = this;

      // compute the inlet's footprint.
      if (inlet->_footprint.Compute() == elle::StatusError)
	escape("unable to compute the inlet's footprint");

      // add the inlet footprint to the seam's.
      this->_footprint.size += inlet->_footprint.size;

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Split(Seam<V>*&		right)
    {
      Seam<V>::Iterator	i;
      Seam<V>::Iterator	j;
      elle::Natural32	footprint;
      elle::Natural32	size;
      Seam<V>*		r;

      enter(instance(r));

      // initialize the footprint as being the original seam's footprint.
      footprint = this->_footprint.size;

      // compute the future seams' sizes.
      size = footprint / 2;

      // reinitialize the current seam's footprint.
      this->_footprint.size = 0;

      // go through the seam's entries until the future size is reached
      // after which all the remaining entries will be moved to the
      // new seam.
      for (i = this->container.begin();
	   i != this->container.end();
	   i++)
	{
	  Seam<V>::I*		inlet = i->second;

	  // check whether the new seam's size has been reached.
	  if (this->_footprint.size >= size)
	    break;

	  //
	  // otherwise, add this inlet to the seam.
	  //

	  // note however that another check is performed in order to make
	  // sure that additing this inlet will not make the seam too large.
	  if ((this->_footprint.size +
	       inlet->_footprint.size) > hole::Hole::Descriptor.extent)
	    break;

	  // add the inlet's footprint to the footprint.
	  this->_footprint.size += inlet->_footprint.size;
	}

      // allocate a new seam.
      r = new Seam<V>(this->_load, this->_unload);

      // create the seam.
      if (r->Create() == elle::StatusError)
	escape("unable to create the seam");

      // go through the remaining entries in order to move them to
      // the new (r) seam.
      for (j = i; j != this->container.end(); j++)
	{
	  Seam<V>::I*		inlet = j->second;

	  // check whether the new seam is about to exceed the extent
	  // which would mean that splitting the seam has not resulted
	  // in two valid seams.
	  if ((this->_footprint.size +
	       inlet->_footprint.size) > hole::Hole::Descriptor.extent)
	    escape("unable to split the seam into two extent-valid seams");

	  // insert the inlet to the new seam.
	  if (r->Insert(inlet) == elle::StatusError)
	    escape("unable to add the inlet");
	}

      // remove the right entries from the left (this) seam.
      this->container.erase(i, this->container.end());

      // set both seams' footprint as consistent.
      this->_footprint.state = elle::Footprint::StateConsistent;
      r->_footprint.state = elle::Footprint::StateConsistent;

      // set the output right seam.
      right = r;

      // waive the r variable.
      waive(r);

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
    elle::Status	Seam<V>::Lookup(const typename V::K&	key,
					Quill<V>*&		quill)
    {
      Seam<V>::Scoutor	scoutor;

      enter();

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Seam<V>::I*	inlet = scoutor->second;

	  // check if this inlet is responsible for the given key or
	  // the end of the seam has been reached.
	  if ((key <= scoutor->first) ||
	      (inlet == this->container.rbegin()->second))
	    {
	      // load the child, if necessary
	      if (inlet->_value == NULL)
		{
		  // load the child nodule.
		  if (this->_load.Call(inlet->address,
				       inlet->_value) == elle::StatusError)
		    escape("unable to load the child nodule");
		}

	      // lookup in this nodule.
	      if (inlet->_value->Lookup(key, quill) == elle::StatusError)
		escape("unable to locate the quill for the given key");

	      break;
	    }
	}

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
      elle::String		alignment(margin, ' ');
      typename Seam<V>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Seam] " << this << std::endl;

      /* XXX
      // dump the load callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Load]" << std::endl;

      if (this->load.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the unload callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Unload]" << std::endl;

      if (this->unload.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the callback");
      */

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

      // dump the footprint.
      if (this->_footprint.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the footprint");

      leave();
    }

  }
}

#endif
