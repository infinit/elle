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
      type(type),

      _state(StateClean),
      _load(load),
      _unload(unload),
      _parent(NULL),
      _left(NULL),
      _right(NULL),
      _footprint(*this)
    {
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

      // dump the type.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Type] " << std::dec << this->type << std::endl;

      // dump the parent.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Parent]" << std::endl;

      if (this->parent.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the parent");

      // dump the left.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Left]" << std::endl;

      if (this->left.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the left");

      // dump the right.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Right]" << std::endl;

      if (this->right.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the right");

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
	    static_cast<elle::Natural8>(this->type)) == elle::StatusError)
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
      if (archive.Extract(type) == elle::StatusError)
	escape("unable to extract the attributes");

      // cast the type.
      this->type = static_cast<Nodule<V>::Type>(type);

      leave();
    }

  }
}

#endif
