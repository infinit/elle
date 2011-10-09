//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun sep 18 14:35:06 2011]
//

#ifndef NUCLEUS_PROTON_NODULE_HXX
#define NUCLEUS_PROTON_NODULE_HXX

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
    /// default constructor.
    ///
    template <typename V>
    Nodule<V>::Nodule(const Type				type):
      ContentHashBlock(type == TypeSeam ? V::S : V::Q),

      type(type),

      _parent(NULL),
      _left(NULL),
      _right(NULL),
      _footprint(*this)
    {
    }

    ///
    /// specific constructor.
    ///
    template <typename V>
    Nodule<V>::Nodule(const Type				type,
		      const elle::Callback<
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
      ContentHashBlock(type == TypeSeam ? V::S : V::Q),

      type(type),

      _load(load),
      _unload(unload),
      _parent(NULL),
      _left(NULL),
      _right(NULL),
      _footprint(*this)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    template <typename T>
    elle::Status	Nodule<V>::Export(const elle::Natural32	size,
					  T*			right)
    {
      T*				left = static_cast<T*>(this);
      elle::Natural32			footprint;
      typename T::Iterator::Forward	i;
      typename T::Iterator::Forward	j;

      enter();

      // initialize the temporary footprint.
      footprint = T::Footprint;

      // go through the left nodule's entries until the future size is reached
      // after which all the remaining entries will be moved to the right
      // nodule.
      for (i = left->container.begin();
	   i != left->container.end();
	   i++)
	{
	  typename T::I*		inlet = i->second;

	  // check whether the new nodule's size has been reached.
	  if (footprint > size)
	    break;

	  //
	  // otherwise, leave this inlet in the nodule.
	  //

	  // note however that another check is performed in order to make
	  // sure that adding this inlet will not make the nodule too large.
	  if ((footprint +
	       inlet->_footprint.size) > hole::Hole::Descriptor.extent)
	    break;

	  // add the inlet's footprint to the footprint.
	  footprint += inlet->_footprint.size;
	}

      // set the nodule's state.
      left->_state = StateDirty;

      // go through the remaining entries in order to move them to
      // the right nodule.
      for (j = i; j != left->container.end(); j++)
	{
	  typename T::I*		inlet = j->second;

	  // substract the inlet's footprint from the current nodule since
	  // it is getting moved to the right one.
	  left->_footprint.size -= inlet->_footprint.size;

	  // insert the inlet into the right nodule.
	  if (right->Insert(inlet) == elle::StatusError)
	    escape("unable to add the inlet");
	}

      // remove the right entries from the left nodule.
      left->container.erase(i, left->container.end());

      // set both nodules' footprint as consistent.
      left->_footprint.state = elle::Footprint::StateConsistent;
      right->_footprint.state = elle::Footprint::StateConsistent;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Nodule<V>::Dump(const elle::Natural32	margin)
      const
    {
      elle::String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Nodule]" << std::endl;

      // dump the parent.
      if (ContentHashBlock::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the content hash block");

      // dump the type.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Type] " << std::dec << this->type << std::endl;

      // dump the parent.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Parent]" << std::endl;

      if (this->parent.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the parent");

      // dump the left.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Left]" << std::endl;

      if (this->left.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the left");

      // dump the right.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Right]" << std::endl;

      if (this->right.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the right");

      /* XXX
      // dump the load callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Load]" << std::endl;

      if (this->load.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the unload callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Unload]" << std::endl;

      if (this->unload.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the callback");
      */

      // dump the parent link.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Parent] " << std::hex << this->_parent << std::endl;

      // dump the left link.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Left] " << std::hex << this->_left << std::endl;

      // dump the right link.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Right] " << std::hex << this->_right << std::endl;

      // dump the footprint.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Footprint]" << std::endl;

      if (this->_footprint.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the footprint");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the nodule.
    ///
    template <typename V>
    elle::Status	Nodule<V>::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(
	    static_cast<elle::Natural8>(this->type),
	    this->parent,
	    this->left,
	    this->right) == elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    template <typename V>
    elle::Status	Nodule<V>::Extract(elle::Archive&	archive)
    {
      elle::Natural8	type;

      enter();

      // extract the attributes.
      if (archive.Extract(type,
			  this->parent,
			  this->left,
			  this->right) == elle::StatusError)
	escape("unable to extract the attributes");

      // cast the type.
      this->type = static_cast<Nodule<V>::Type>(type);

      leave();
    }

  }
}

#endif
