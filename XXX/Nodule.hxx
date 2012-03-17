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
    Nodule<V>::Nodule(const Type                                type):
      ContentHashBlock(type == TypeSeam ? V::S : V::Q),

      type(type),

      _footprint(*this)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes the given nodule _right_ and moves inlets to the
    /// current nodule in order to transfer an amount equal or greater that
    /// size bytes.
    ///
    /// note that the _right_ nodule is supposed to contain higher keys
    /// so that the inlets from the current nodule with the highest keys
    /// are moved to _right_ in order to maintain consistency.
    ///
    template <typename V>
    template <typename T>
    elle::Status        Nodule<V>::Export(T*                    right,
                                          const elle::Natural32 size)
    {
      T*                                left = static_cast<T*>(this);
      elle::Natural32                   footprint;
      auto                              end = left->container.end();
      typename T::Iterator::Forward     i;
      typename T::Iterator::Forward     j;

      // initialize the temporary footprint.
      footprint = T::Footprint;

      // go through the left nodule's entries until the future size is reached
      // after which all the remaining entries will be moved to the right
      // nodule.
      for (i = left->container.begin(); i != end; ++i)
        {
          typename T::I*                inlet = i->second;

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

      // go through the remaining entries in order to move them to
      // the right nodule.
      for (j = i; j != end; ++j)
        {
          typename T::I*                inlet = j->second;

          // substract the inlet's footprint from the current nodule since
          // it is getting moved to the right one.
          left->_footprint.size -= inlet->_footprint.size;

          // insert the inlet into the right nodule.
          if (right->Insert(inlet) == elle::StatusError)
            escape("unable to add the inlet");
        }

      // remove the right entries from the left nodule.
      left->container.erase(i, end);

      // set the nodules' states.
      left->_state = StateDirty;
      right->_state = StateDirty;

      // set both nodules' footprint as consistent.
      left->_footprint.state = elle::Footprint::StateConsistent;
      right->_footprint.state = elle::Footprint::StateConsistent;

      return elle::StatusOk;
    }

    ///
    /// this method does the opposite of Export(), moving at least _size_
    /// bytes of inlets from the _right_ nodule to the current one.
    ///
    /// note that the inlets with the lowest keys are moved in order to
    /// maintain consistency.
    ///
    template <typename V>
    template <typename T>
    elle::Status        Nodule<V>::Import(T*                    right,
                                          const elle::Natural32 size)
    {
      T*                                left = static_cast<T*>(this);
      elle::Natural32                   footprint;
      auto                              rend = right->container.rend();
      typename T::Iterator::Backward    i;
      typename T::Iterator::Backward    j;

      // initialize the temporary footprint.
      footprint = T::Footprint;

      // go through the right nodule's entries until the future size is reached
      // after which all the remaining entries will be moved to the left
      // nodule.
      for (i = right->container.rbegin(); i != rend; ++i)
        {
          typename T::I*                inlet = i->second;

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

      // go through the remaining entries in order to move them to
      // the left nodule.
      for (j = i; j != rend; ++j)
        {
          typename T::I*                inlet = j->second;

          // substract the inlet's footprint from the current nodule since
          // it is getting moved to the left one.
          right->_footprint.size -= inlet->_footprint.size;

          // insert the inlet into the left nodule.
          if (left->Insert(inlet) == elle::StatusError)
            escape("unable to add the inlet");
        }

      // remove the left entries from the right nodule.
      right->container.erase(right->container.begin(), i.base());

      // set the nodules' states.
      left->_state = StateDirty;
      right->_state = StateDirty;

      // set both nodules' footprint as consistent.
      left->_footprint.state = elle::Footprint::StateConsistent;
      right->_footprint.state = elle::Footprint::StateConsistent;

      return elle::StatusOk;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the nodule.
    ///
    template <typename V>
    elle::Status        Nodule<V>::Dump(const elle::Natural32   margin)
      const
    {
      elle::String              alignment(margin, ' ');

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

      // dump the footprint.
      std::cout << alignment << elle::Dumpable::Shift
                << "[_Footprint]" << std::endl;

      if (this->_footprint.Dump(margin + 4) == elle::StatusError)
        escape("unable to dump the footprint");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the nodule.
    ///
    template <typename V>
    elle::Status        Nodule<V>::Serialize(elle::Archive&     archive) const
    {
      // serialize the attributes.
      if (archive.Serialize(
            static_cast<elle::Natural8>(this->type),
            this->parent,
            this->left,
            this->right) == elle::StatusError)
        escape("unable to serialize the attributes");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the attributes.
    ///
    template <typename V>
    elle::Status        Nodule<V>::Extract(elle::Archive&       archive)
    {
      elle::Natural8    type;

      // extract the attributes.
      if (archive.Extract(type,
                          this->parent,
                          this->left,
                          this->right) == elle::StatusError)
        escape("unable to extract the attributes");

      // cast the type.
      this->type = static_cast<Nodule<V>::Type>(type);

      return elle::StatusOk;
    }

  }
}

#endif
