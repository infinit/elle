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
    elle::Status	Quill<V>::Insert(const typename V::K&	key,
					 V*			value)
    {
      typename Quill<V>::I*	inlet;

      enter(instance(inlet));

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
      std::pair<Quill<V>::Iterator, elle::Boolean>	result;

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

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Locate(const typename V::K&	key,
					 V*&			value)
      const
    {
      Quill<V>::Scoutor	scoutor;
      Quill<V>::I*	inlet;

      enter();

      // find the inlet associated with the given key.
      if ((scoutor = this->container.find(key)) != this->container.end())
	escape("unable to locate the given key");

      // retrieve the inlet.
      inlet = scoutor->second;

      // load the value, if necessary.
      if (inlet->value == NULL)
	{
	  // load the value.
	  if (this->load.Call(inlet->address,
			      inlet->value) == elle::StatusError)
	    escape("unable to load the value");
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
      Quill<V>::Iterator	i;
      Quill<V>::Iterator	j;
      elle::Natural32		footprint;
      elle::Natural32		size;
      Quill<V>*			r;

      enter(instance(r));

      // initialize the footprint as being the original quill's footprint.
      footprint = this->_footprint.size;

      // compute the future quills' sizes.
      size = footprint / 2;

      // reinitialize the current quill's footprint.
      this->_footprint.size = 0;

      // go through the quill's entries until the future size is reached
      // after which all the remaining entries will be moved to the
      // new quill.
      for (i = this->container.begin();
	   i != this->container.end();
	   i++)
	{
	  Quill<V>::I*		inlet = i->second;

	  // check whether the new quill's size has been reached.
	  if (this->_footprint.size >= size)
	    break;

	  //
	  // otherwise, add this inlet to the quill.
	  //

	  // note however that another check is performed in order to make
	  // sure that additing this inlet will not make the quill too large.
	  if ((this->_footprint.size +
	       inlet->_footprint.size) > hole::Hole::Descriptor.extent)
	    break;

	  // add the inlet's footprint to the footprint.
	  this->_footprint.size += inlet->_footprint.size;
	}

      // allocate a new quill.
      r = new Quill<V>(this->_load, this->_unload);

      // create the quill.
      if (r->Create() == elle::StatusError)
	escape("unable to create the quill");

      // go through the remaining entries in order to move them to
      // the new (r) quill.
      for (j = i; j != this->container.end(); j++)
	{
	  Quill<V>::I*		inlet = j->second;

	  // check whether the new quill is about to exceed the extent
	  // which would mean that splitting the quill has not resulted
	  // in two valid quills.
	  if ((this->_footprint.size +
	       inlet->_footprint.size) > hole::Hole::Descriptor.extent)
	    escape("unable to split the quill into two extent-valid quills");

	  // insert the inlet to the new quill.
	  if (r->Insert(inlet) == elle::StatusError)
	    escape("unable to add the inlet");
	}

      // remove the right entries from the left (this) quill.
      this->container.erase(i, this->container.end());

      // set both quills' footprint as consistent.
      this->_footprint.state = elle::Footprint::StateConsistent;
      r->_footprint.state = elle::Footprint::StateConsistent;

      // set the output right quill.
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
    elle::Status	Quill<V>::Lookup(const typename V::K&,
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
      elle::String			alignment(margin, ' ');
      typename Quill<V>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Quill] " << this << std::endl;

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

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the quill.
    ///
    template <typename V>
    elle::Status	Quill<V>::Serialize(elle::Archive&	archive) const
    {
      Quill<V>::Scoutor	scoutor;
      elle::Natural32	size;

      enter();

      // XXX call Nodule

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

      // extract the container size.
      if (archive.Extract(size) == elle::StatusError)
	escape("unable to extract the size");

      // iterator.
      for (i = 0; i < size; i++)
	{
	  Quill<V>::I*	inlet;

	  enter(instance(inlet));

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

  }
}

#endif
