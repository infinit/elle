#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/proton/Ambit.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Limits.hh>

#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Catalog");

namespace nucleus
{
  namespace neutron
  {

    /*----------.
    | Constants |
    `----------*/

    proton::Nature const Catalog::Constants::seam =
      proton::Nature::catalog_seam;
    proton::Nature const Catalog::Constants::quill =
      proton::Nature::catalog_quill;
    proton::Nature const Catalog::Constants::value =
      proton::Nature::catalog_value;
    proton::Nature const Catalog::Constants::nature =
      Catalog::Constants::value;

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    Catalog::transfer_right(Catalog& left,
                            Catalog& right,
                            proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Catalog>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto end = left._container.end();
      auto i = left._container.begin();

      // Go through the left catalog's entries until the future size is
      // reached after which all the remaining entries will be moved to
      // the right catalog.
      for (; i != end; ++i)
        {
          auto& entry = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this item in the left entry.

          // Note however that another check is performed in order to make
          // sure that adding this entry will not make the container too large.
          if ((footprint + entry->footprint()) > extent)
            break;

          // Add the entry's footprint to the local _footprint_ variable.
          footprint += entry->footprint();
        }

      // Go through the remaining entries in order to move them to
      // the right catalog.
      for (auto j = i; j != end; ++j)
        {
          auto& entry = j->second;

          // Substract the entry's footprint from the left catalog since
          // it is getting moved to the right one.
          left.footprint(left.footprint() - entry->footprint());

          // Insert the entry into the right catalog.
          right.insert(entry);

          // Needless to update the capacities: the right's is updated
          // by calling insert() while the left's is updated at the
          // end.
        }

      // Remove the moved entries from the the current catalog.
      left._container.erase(i, end);

      // Update the left's capacity with the number of remaining entries.
      left.capacity(left._container.size());

      ELLE_ASSERT(left.capacity() == left._container.size());
      ELLE_ASSERT(right.capacity() == right._container.size());
    }

    void
    Catalog::transfer_left(Catalog& left,
                           Catalog& right,
                           proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Catalog>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto rend = right._container.rend();
      auto i = right._container.rbegin();

      // Go through the right catalog's entries until the future size is
      // reached after which all the remaining entries will be moved to
      // the left catalog.
      for (; i != rend; ++i)
        {
          auto& entry = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          //
          // Otherwise, leave this entry in the catalog.
          //

          // Note however that another check is performed in order to make
          // sure that adding this entry will not make the container too large.
          if ((footprint + entry->footprint()) > extent)
            break;

          // Add the entry's footprint to the footprint.
          footprint += entry->footprint();
        }

      // Go through the remaining entries in order to move them to
      // the left catalog.
      for (auto j = i; j != rend; ++j)
        {
          auto& entry = j->second;

          // Substract the entry's footprint from the right catalog since
          // it is getting moved to the left one.
          right.footprint(right.footprint() - entry->footprint());

          // Insert the entry into the left catalog.
          left.insert(entry);

          // Needless to update the capacities: the left's is updated
          // by calling insert() while the right's is updated at the
          // end.
        }

      // Remove the moved entries from the right catalog.
      right._container.erase(right._container.begin(), i.base());

      // Update the right's capacity.
      right.capacity(right._container.size());

      ELLE_ASSERT(left.capacity() == left._container.size());
      ELLE_ASSERT(right.capacity() == right._container.size());
    }

    /*-------------.
    | Construction |
    `-------------*/

    Catalog::Catalog():
      Value::Value()
    {
      static proton::Footprint const initial =
        elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Catalog::insert(Entry* entry)
    {
      ELLE_TRACE_METHOD(entry);

      ELLE_ASSERT(entry != nullptr);

      std::shared_ptr<Entry> pointer{entry};

      this->insert(pointer);
    }

    void
    Catalog::insert(std::shared_ptr<Entry> const& entry)
    {
      ELLE_TRACE_METHOD(entry);

      // Inject the entry in the container.
      this->_inject(entry);

      // Update the capacity and state.
      this->capacity(this->capacity() + 1);
      this->state(proton::State::dirty);
    }

    elle::Boolean
    Catalog::exist(elle::String const& name) const
    {
      ELLE_TRACE_METHOD(name);

      return (this->_container.find(name) != this->_container.end());
    }

    void
    Catalog::rename(elle::String const& from,
                    elle::String const& to)
    {
      ELLE_TRACE_METHOD(from, to);

      // Check if this name has already been recorded.
      if (this->_container.find(to) != this->_container.end())
        throw Exception("the name '%s' seems to have already been recorded",
                        to);

      // Locate the entry for the _from_ name.
      auto iterator = this->_iterator(from);

      // Retrieve the entry.
      std::shared_ptr<Entry> entry = iterator->second;

      // Rename the entry.
      entry->name(to);

      // Remove the entry from the container.
      this->_container.erase(iterator);

      // Re-insert the entry in the container.
      auto result =
        this->_container.insert(
          std::pair<const elle::String, std::shared_ptr<Entry>>(
            entry->name(), entry));

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to re-insert the entry in the container");
    }

    Entry const&
    Catalog::locate(elle::String const& name) const
    {
      ELLE_TRACE_METHOD(name);

      auto scoutor = this->_iterator(name);
      auto& entry = scoutor->second;

      return (*entry);
    }

    Entry&
    Catalog::locate(elle::String const& name)
    {
      ELLE_TRACE_METHOD(name);

      auto iterator = this->_iterator(name);
      auto& entry = iterator->second;

      return (*entry);
    }

    Range<Entry>
    Catalog::consult(Index const& index,
                     Size const& size) const
    {
      Range<Entry> range;
      Index i(0);

      ELLE_TRACE_METHOD(index, size);

      for (auto& pair: this->_container)
        {
          auto& entry = pair.second;

          // If this entry lies in the selected range [index, index + size[
          if ((i >= index) && (i < (index + size)))
            {
              // Check if the entry is empty: this should never
              // happen but you never know! You do not want to throw an
              // exception because a malicious user created such a catalog.
              if (entry->name().empty() == true)
                continue;

              // Insert the entry to the range.
              range.insert(entry);
            }

          // Increment the index.
          //
          // Note that this is done at the end so that empty
          // entries are not taken into account.
          i++;
        }

      return (range);
    }

    void
    Catalog::erase(elle::String const& name)
    {
      ELLE_TRACE_METHOD(name);

      // Call take out and forget about the returned
      // entry, hence (possibly) deleting it.
      this->takeout(name);
    }

    std::shared_ptr<Entry>
    Catalog::takeout(elle::String const& name)
    {
      ELLE_TRACE_METHOD(name);

      // Locate the entry for the given name.
      auto iterator = this->_iterator(name);

      // Retrieve the entry.
      std::shared_ptr<Entry> entry = iterator->second;

      // Substract the entry footprint from the catalog's.
      ELLE_ASSERT(entry->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= entry->footprint());
      this->footprint(this->footprint() - entry->footprint());
      ELLE_ASSERT(this->capacity() > 0);
      this->capacity(this->capacity() - 1);
      this->state(proton::State::dirty);

      // Remove the entry from the container.
      this->_container.erase(iterator);

      // And finally return the pointer.
      return (entry);
    }

    Size
    Catalog::size() const
    {
      return (static_cast<Size>(this->_container.size()));
    }

    void
    Catalog::_inject(std::shared_ptr<Entry> const& entry)
    {
      ELLE_TRACE_METHOD(entry);

      // Check if this name has already been recorded.
      if (this->_container.find(entry->name()) != this->_container.end())
        throw Exception("the name '%s' seems to have already been recorded",
                        entry->name());

      // Insert the entry in the container.
      auto result =
        this->_container.insert(
          std::pair<const elle::String, std::shared_ptr<Entry>>(
            entry->name(), entry));

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the entry in the container");

      // Update the footprint.
      ELLE_ASSERT(entry->footprint() != 0);
      this->footprint(this->footprint() + entry->footprint());
    }

    typename Catalog::Scoutor
    Catalog::_iterator(elle::String const& name) const
    {
      Scoutor scoutor;

      if ((scoutor = this->_container.find(name)) == this->_container.end())
        throw Exception("unable to locate the given name: '%s'", name);

      return (scoutor);
    }

    typename Catalog::Iterator
    Catalog::_iterator(elle::String const& name)
    {
      Iterator iterator;

      if ((iterator = this->_container.find(name)) == this->_container.end())
        throw Exception("unable to locate the given name: '%s'", name);

      return (iterator);
    }

    /*------.
    | Value |
    `------*/

    elle::Boolean
    Catalog::empty() const
    {
      return (this->_container.empty());
    }

    elle::String
    Catalog::mayor() const
    {
      ELLE_ASSERT(this->_container.empty() == false);

      return (this->_container.rbegin()->first);
    }

    proton::Handle
    Catalog::split()
    {
      ELLE_TRACE_METHOD("");

      // Allocate a new right catalog.
      proton::Contents* contents{new proton::Contents{new Catalog}};
      proton::Handle orphan{this->nest().attach(contents)};
      proton::Ambit<Catalog> right{this->nest(), orphan};

      // Load the new right catalog.
      right.load();

      // Export the entries from the current catalog into the new catalog.
      Catalog::transfer_right(*this,
                              right(),
                              this->nest().limits().extent() *
                              this->nest().limits().contention());

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      right().state(proton::State::dirty);

      // Unload the new right nodule.
      right.unload();

      return (orphan);
    }

    void
    Catalog::merge(proton::Handle& other)
    {
      ELLE_TRACE_METHOD(other);

      proton::Ambit<Catalog> catalog(this->nest(), other);

      // Load the other catalog.
      catalog.load();

      // Check which value has the lowest keys.
      if (catalog().mayor() < this->mayor())
        {
          // In this case, export the entries from the given catalog
          // into the current's since these entries happen to have
          // lower keys.
          //
          // Note that the footprint-based number of entries to keep in
          // the left catalog is zero i.e transfer all entries.
          Catalog::transfer_right(catalog(), *this, 0);
        }
      else
        {
          // Otherwise, import the higher entries from the given catalog
          // into the current's.
          //
          // Note that the footprint-based number of entries to keep in
          // the right catalog is zero i.e transfer all entries.
          Catalog::transfer_left(*this, catalog(), 0);
        }

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      catalog().state(proton::State::dirty);

      ELLE_ASSERT(catalog()._container.size() == 0);

      // Unload the given catalog.
      catalog.unload();
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Catalog::Dump(elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');
      Catalog::Scoutor scoutor;

      std::cout << alignment << "[Catalog]" << std::endl;

      if (Value::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the value");

      std::cout << alignment << elle::io::Dumpable::Shift << "[Entries] #"
                << this->_container.size() << std::endl;

      // go through the entries.
      for (auto& pair: this->_container)
        {
          auto& entry = pair.second;

          // dump the entry.
          if (entry->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the entry");
        }

      return (elle::Status::Ok);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Catalog::print(std::ostream& stream) const
    {
      stream << "catalog(#"
             << this->_container.size()
             << ")";
    }

    /*---------.
    | Iterable |
    `---------*/

    typename Catalog::Scoutor
    Catalog::begin() const
    {
      return (this->_container.begin());
    }

    typename Catalog::Scoutor
    Catalog::end() const
    {
      return (this->_container.end());
    }

    typename Catalog::Iterator
    Catalog::begin()
    {
      return (this->_container.begin());
    }

    typename Catalog::Iterator
    Catalog::end()
    {
      return (this->_container.end());
    }

  }
}
