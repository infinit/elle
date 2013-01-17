#include <nucleus/neutron/Attributes.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/proton/Ambit.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Limits.hh>

#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Attributes");

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Constants |
    `----------*/

    proton::Nature const Attributes::Constants::seam =
      proton::Nature::attributes_seam;
    proton::Nature const Attributes::Constants::quill =
      proton::Nature::attributes_quill;
    proton::Nature const Attributes::Constants::value =
      proton::Nature::attributes_value;
    proton::Nature const Attributes::Constants::nature =
      Attributes::Constants::value;

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    Attributes::transfer_right(Attributes& left,
                               Attributes& right,
                               proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Attributes>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto end = left._container.end();
      auto i = left._container.begin();

      // Go through the left attributes's traits until the future size is
      // reached after which all the remaining traits will be moved to
      // the right attributes.
      for (; i != end; ++i)
        {
          auto& trait = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this item in the left trait.
          //
          // Note however that another check is performed in order to make
          // sure that adding this trait will not make the container too large.
          if ((footprint + trait->footprint()) > extent)
            break;

          // Add the trait's footprint to the local _footprint_ variable.
          footprint += trait->footprint();
        }

      // Go through the remaining traits in order to move them to
      // the right attributes.
      for (auto j = i; j != end; ++j)
        {
          auto& trait = j->second;

          // Substract the trait's footprint from the left attributes since
          // it is getting moved to the right one.
          left.footprint(left.footprint() - trait->footprint());

          // Insert the trait into the right attributes.
          right.insert(trait);
        }

      // Remove the moved traits from the the current attributes.
      left._container.erase(i, end);
    }

    void
    Attributes::transfer_left(Attributes& left,
                              Attributes& right,
                              proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Attributes>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto rend = right._container.rend();
      auto i = right._container.rbegin();

      // Go through the right attributes's traits until the future size is
      // reached after which all the remaining traits will be moved to
      // the left attributes.
      for (; i != rend; ++i)
        {
          auto& trait = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this trait in the attributes.
          //
          // Note however that another check is performed in order to make
          // sure that adding this trait will not make the container too large.
          if ((footprint + trait->footprint()) > extent)
            break;

          // Add the trait's footprint to the footprint.
          footprint += trait->footprint();
        }

      // Go through the remaining traits in order to move them to
      // the left attributes.
      for (auto j = i; j != rend; ++j)
        {
          auto& trait = j->second;

          // Substract the trait's footprint from the right attributes since
          // it is getting moved to the left one.
          right.footprint(right.footprint() - trait->footprint());

          // Insert the trait into the left attributes.
          left.insert(trait);
        }

      // Remove the moved traits from the right attributes.
      right._container.erase(right._container.begin(), i.base());
    }

    /*-------------.
    | Construction |
    `-------------*/

    Attributes::Attributes():
      Value::Value()
    {
      static proton::Footprint const initial =
        elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Attributes, proton::Value)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Attributes::insert(Trait* trait)
    {
      ELLE_TRACE_METHOD(trait);

      ELLE_ASSERT(trait != nullptr);

      std::shared_ptr<Trait> pointer{trait};

      this->insert(pointer);
    }

    void
    Attributes::insert(std::shared_ptr<Trait> const& trait)
    {
      ELLE_TRACE_METHOD(trait);

      // Inject the trait in the container.
      this->_inject(trait);

      // Update the state.
      this->state(proton::State::dirty);
    }

    elle::Boolean
    Attributes::exist(elle::String const& name) const
    {
      ELLE_TRACE_METHOD(name);

      return (this->_container.find(name) != this->_container.end());
    }

    void
    Attributes::rename(elle::String const& from,
                       elle::String const& to)
    {
      ELLE_TRACE_METHOD(from, to);

      // Check if this name has already been recorded.
      if (this->_container.find(to) != this->_container.end())
        throw Exception("the name '%s' seems to have already been recorded",
                        to);

      // Locate the trait for the _from_ name.
      auto iterator = this->_iterator(from);

      // Retrieve the trait.
      std::shared_ptr<Trait> trait = iterator->second;

      // Substract the trait's current footprint.
      ELLE_ASSERT(trait->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= trait->footprint());
      this->footprint(this->footprint() - trait->footprint());

      // Rename the trait.
      trait->name(to);

      // Remove the trait from the container.
      this->_container.erase(iterator);

      // Re-insert the trait in the container.
      auto result =
        this->_container.insert(
          std::pair<const elle::String, std::shared_ptr<Trait>>(
            trait->name(), trait));

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to re-insert the trait in the container");

      // Add the trait's new footprint.
      ELLE_ASSERT(trait->footprint() != 0);
      this->footprint(this->footprint() + trait->footprint());

      // Update the state.
      this->state(proton::State::dirty);
    }

    void
    Attributes::update(elle::String const& name,
                       elle::String const& value)
    {
      ELLE_TRACE_METHOD(name, value);

      // Locate the trait.
      auto iterator = this->_iterator(name);

      // Retrieve the trait.
      auto& trait = iterator->second;

      // Substract the trait's current footprint.
      ELLE_ASSERT(trait->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= trait->footprint());
      this->footprint(this->footprint() - trait->footprint());

      // Update the trait.
      trait->value(value);

      // Add the trait's new footprint.
      ELLE_ASSERT(trait->footprint() != 0);
      this->footprint(this->footprint() + trait->footprint());

      // Update the state.
      this->state(proton::State::dirty);
    }

    Trait const&
    Attributes::locate(elle::String const& name) const
    {
      ELLE_TRACE_METHOD(name);

      auto scoutor = this->_iterator(name);
      auto& trait = scoutor->second;

      return (*trait);
    }

    Range<Trait>
    Attributes::consult(Index const& index,
                        Size const& size) const
    {
      Range<Trait> range;
      Index i(0);

      ELLE_TRACE_METHOD(index, size);

      for (auto& pair: this->_container)
        {
          auto& trait = pair.second;

          // If this trait lies in the selected range [index, index + size[
          if ((i >= index) && (i < (index + size)))
            {
              // Check if the trait is empty: this should never happen but you
              // never know! You do not want to throw an exception because a
              // malicious user created such an attribute.
              if (trait->name().empty() == true)
                continue;

              // Insert the trait to the range.
              range.insert(trait);
            }

          // Increment the index.
          //
          // Note that this is done at the end so that empty
          // traits are not taken into account.
          i++;
        }

      return (range);
    }

    void
    Attributes::erase(elle::String const& name)
    {
      ELLE_TRACE_METHOD(name);

      // Call take out and forget about the returned trait, hence (possibly)
      // deleting it.
      this->takeout(name);
    }

    std::shared_ptr<Trait>
    Attributes::takeout(elle::String const& name)
    {
      ELLE_TRACE_METHOD(name);

      // Locate the trait for the given name.
      auto iterator = this->_iterator(name);

      // Retrieve the trait.
      std::shared_ptr<Trait> trait = iterator->second;

      // Substract the trait footprint from the attributes's.
      ELLE_ASSERT(trait->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= trait->footprint());
      this->footprint(this->footprint() - trait->footprint());
      this->state(proton::State::dirty);

      // Remove the trait from the container.
      this->_container.erase(iterator);

      // And finally return the pointer.
      return (trait);
    }

    Size
    Attributes::size() const
    {
      return (static_cast<Size>(this->_container.size()));
    }

    void
    Attributes::_inject(std::shared_ptr<Trait> const& trait)
    {
      ELLE_DEBUG_METHOD(trait);

      // Check if this name has already been recorded.
      if (this->_container.find(trait->name()) != this->_container.end())
        throw Exception("the name '%s' seems to have already been recorded",
                        trait->name());

      // Insert the trait in the container.
      auto result =
        this->_container.insert(
          std::pair<const elle::String, std::shared_ptr<Trait>>(
            trait->name(), trait));

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the trait in the container");

      // Update the footprint.
      ELLE_ASSERT(trait->footprint() != 0);
      this->footprint(this->footprint() + trait->footprint());
    }

    typename Attributes::Scoutor
    Attributes::_iterator(elle::String const& name) const
    {
      Scoutor scoutor;

      if ((scoutor = this->_container.find(name)) == this->_container.end())
        throw Exception("unable to locate the given name: '%s'", name);

      return (scoutor);
    }

    typename Attributes::Iterator
    Attributes::_iterator(elle::String const& name)
    {
      Iterator iterator;

      if ((iterator = this->_container.find(name)) == this->_container.end())
        throw Exception("unable to locate the given name: '%s'", name);

      return (iterator);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Attributes::Dump(elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');
      Attributes::Scoutor scoutor;

      std::cout << alignment << "[Attributes] "
                << std::hex << this << std::endl;

      if (Value::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the value");

      std::cout << alignment << elle::io::Dumpable::Shift << "[Traits] #"
                << this->_container.size() << std::endl;

      // go through the traits.
      for (auto& pair: this->_container)
        {
          auto& trait = pair.second;

          std::cout << alignment << elle::io::Dumpable::Shift
                    << elle::io::Dumpable::Shift << *trait << std::endl;
        }

      return (elle::Status::Ok);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Attributes::print(std::ostream& stream) const
    {
      stream << "attributes(#"
             << this->_container.size()
             << ")";
    }

    /*---------.
    | Iterable |
    `---------*/

    typename Attributes::Scoutor
    Attributes::begin() const
    {
      return (this->_container.begin());
    }

    typename Attributes::Scoutor
    Attributes::end() const
    {
      return (this->_container.end());
    }

    typename Attributes::Iterator
    Attributes::begin()
    {
      return (this->_container.begin());
    }

    typename Attributes::Iterator
    Attributes::end()
    {
      return (this->_container.end());
    }

    /*------.
    | Value |
    `------*/

    elle::Boolean
    Attributes::empty() const
    {
      return (this->_container.empty());
    }

    elle::String
    Attributes::mayor() const
    {
      ELLE_ASSERT(this->_container.empty() == false);

      return (this->_container.rbegin()->first);
    }

    proton::Capacity
    Attributes::capacity() const
    {
      return (static_cast<proton::Capacity>(this->_container.size()));
    }

    proton::Handle
    Attributes::split()
    {
      ELLE_TRACE_METHOD("");

      // Allocate a new right attributes.
      proton::Contents* contents =
        new proton::Contents{this->nest().network(),
                             this->nest().agent_K(),
                             new Attributes};
      proton::Handle orphan{this->nest().attach(contents)};
      proton::Ambit<Attributes> right{this->nest(), orphan};

      // Load the new right attributes.
      right.load();

      // Export the traits from the current attributes into the new attributes.
      Attributes::transfer_right(*this,
                                 right(),
                                 this->nest().limits().extent() *
                                 this->nest().limits().contention());

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      right().state(proton::State::dirty);

      // Unload the new right attributes.
      right.unload();

      return (orphan);
    }

    void
    Attributes::merge(proton::Handle& other)
    {
      ELLE_TRACE_METHOD(other);

      proton::Ambit<Attributes> attributes(this->nest(), other);

      // Load the other attributes.
      attributes.load();

      // Check which value has the lowest keys.
      if (attributes().mayor() < this->mayor())
        {
          // In this case, export the traits from the given attributes
          // into the current's since these traits happen to have
          // lower keys.
          //
          // Note that the footprint-based number of traits to keep in
          // the left attributes is zero i.e transfer all traits.
          Attributes::transfer_right(attributes(), *this, 0);
        }
      else
        {
          // Otherwise, import the higher traits from the given attributes
          // into the current's.
          //
          // Note that the footprint-based number of traits to keep in
          // the right attributes is zero i.e transfer all traits.
          Attributes::transfer_left(*this, attributes(), 0);
        }

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      attributes().state(proton::State::dirty);

      ELLE_ASSERT(attributes()._container.size() == 0);

      // Unload the given attributes.
      attributes.unload();
    }
  }
}
