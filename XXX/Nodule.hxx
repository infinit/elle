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
		<< "[Parent] " << std::hex << this->_parent << std::endl;

      // dump the left link.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Left] " << std::hex << this->_left << std::endl;

      // dump the right link.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Right] " << std::hex << this->_right << std::endl;

      // dump the footprint.
      if (this->_footprint.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the footprint");

      leave();
    }

  }
}

#endif
