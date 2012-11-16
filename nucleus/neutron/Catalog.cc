#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/proton/Ambit.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Limits.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Catalog");

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    const proton::Node::Type Catalog::Constants::seam =
      proton::Node::TypeSeamCatalog;
    const proton::Node::Type Catalog::Constants::quill =
      proton::Node::TypeQuillCatalog;
    const proton::Node::Type Catalog::Constants::value =
      proton::Node::TypeValueCatalog;
    const proton::Node::Type Catalog::Constants::type =
      Catalog::Constants::value;

//
// ---------- static methods --------------------------------------------------
//

    /// XXX
    /// this method takes a given catalog _right_ and moves entries from
    /// the _left_ catalog in order to transfer enough entries so as to
    /// retain _size_ bytes in the current catalog.
    ///
    /// note that the _right_ catalog is supposed to contain higher names
    /// so that the entries from the left catalog with the highest names
    /// are moved to _right_ in order to maintain consistency.
    ///
    /// XXX highly redundant with Nodules.hxx::Export!
    ///
    void
    Catalog::transfer_right(Catalog* left,
                            Catalog* right,
                            proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Catalog>();
      auto end = left->container().end();
      auto i = left->container().begin();
      proton::Extent const extent = left->nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_TRACE_SCOPE("transfer_right(%s, %s, %s)",
                       left, right, size);

      assert(left->nest().limits().extent() == right->nest().limits().extent());

      // go through the left catalog's entries until the future size is
      // reached after which all the remaining entries will be moved to
      // the right range.
      for (; i != end; ++i)
        {
          Entry* entry = i->second;

          // check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          //
          // otherwise, leave this item in the left entry.
          //

          // note however that another check is performed in order to make
          // sure that adding this entry will not make the container too large.
          if ((footprint + entry->footprint()) > extent)
            break;

          // add the entry's footprint to the footprint.
          footprint += entry->footprint();
        }

      // go through the remaining entries in order to move them to
      // the right catalog.
      for (auto j = i; j != end; ++j)
        {
          Entry* entry = j->second;

          // substract the entry's footprint from the left catalog since
          // it is getting moved to the right one.
          left->footprint(left->footprint() - entry->footprint());

          // insert the entry into the right catalog.
          right->insert(entry);
        }

      // remove the moved entries from the the current catalog.
      left->container().erase(i, end);

      left->capacity(left->container().size());
    }

    ///
    /// this method does the opposite of Export(), moving the lowest
    /// entries from the _right_ catalog to the _left_ in order to
    /// maintain the _right_ catalog with _size_ bytes.
    ///
    /// note that the entries with the lowest names are moved in order to
    /// maintain consistency.
    ///
    void
    Catalog::transfer_left(Catalog* left,
                           Catalog* right,
                           proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Catalog>();
      auto rend = right->container().rend();
      auto i = right->container().rbegin();
      proton::Extent const extent = left->nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_TRACE_SCOPE("transfer_left(%s, %s, %s)",
                       left, right, size);

      assert(left->nest().limits().extent() == right->nest().limits().extent());

      // go through the right catalog's entries until the future size is
      // reached after which all the remaining entries will be moved to
      // the left catalog.
      for (; i != rend; ++i)
        {
          Entry* entry = i->second;

          // check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          //
          // otherwise, leave this entry in the catalog.
          //

          // note however that another check is performed in order to make
          // sure that adding this entry will not make the container too large.
          if ((footprint + entry->footprint()) > extent)
            break;

          // add the entry's footprint to the footprint.
          footprint += entry->footprint();
        }

      // go through the remaining entries in order to move them to
      // the left catalog.
      for (auto j = i; j != rend; ++j)
        {
          Entry* entry = j->second;

          // substract the entry's footprint from the right catalog since
          // it is getting moved to the left one.
          right->footprint(right->footprint() - entry->footprint());

          // insert the entry into the left catalog.
          left->insert(entry);
        }

      // remove the moved entries from the right catalog.
      right->container().erase(right->container().begin(), i.base());

      right->capacity(right->container().size());
    }

//
// ---------- constructor & destructors ---------------------------------------
//

    Catalog::Catalog():
      Value::Value()
    {
      static proton::Footprint const initial =
        elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    Catalog::~Catalog()
    {
      auto iterator = this->_container.begin();
      auto end = this->_container.end();

      for (; iterator != end; ++iterator)
        delete iterator->second;

      this->_container.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    /// XXX
    /// this method inserts the given entry to the catalog.
    ///
    void
    Catalog::insert(Entry* entry)
    {
      std::pair<typename Catalog::Iterator, elle::Boolean> result;

      ELLE_TRACE_SCOPE("insert(%s)", entry);

      // check if this name has already been recorded.
      if (this->_container.find(entry->name()) != this->_container.end())
        throw Exception(
          elle::sprintf("The name '%s' seems to have already been recorded",
                        entry->name()));

      // insert the entry in the container.
      result = this->_container.insert(
        std::pair<const elle::String, Entry*>(entry->name(), entry));

      // check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the entry in the container");

      assert(entry->footprint() != 0);
      this->footprint(this->footprint() + entry->footprint());
      this->capacity(this->capacity() + 1);
      this->state(proton::StateDirty);
    }

    elle::Boolean
    Catalog::exists(elle::String const& name) const
    {
      return (this->_container.find(name) != this->_container.end());
    }

    Catalog::Scoutor
    Catalog::locate_iterator(elle::String const& name) const
    {
      Catalog::Scoutor scoutor;

      // locate the entry.
      if ((scoutor = this->_container.find(name)) == this->_container.end())
        throw Exception(
          elle::sprintf("unable to locate the given name: '%s'", name));

      return (scoutor);
    }

    Catalog::Iterator
    Catalog::locate_iterator(elle::String const& name)
    {
      Catalog::Iterator iterator;

      // locate the entry.
      if ((iterator = this->_container.find(name)) == this->_container.end())
        throw Exception(
          elle::sprintf("unable to locate the given name: '%s'", name));

      return (iterator);
    }

    Entry const*
    Catalog::locate_entry(elle::String const& name) const
    {
      Catalog::Scoutor scoutor;

      // locate the entry.
      scoutor = this->locate_iterator(name);

      return (scoutor->second);
    }

    Range<Entry>
    Catalog::consult(Index const index,
                     Size const size) const
    {
      auto end = this->_container.end();
      auto iterator = this->_container.begin();
      Range<Entry> range;
      Index i(0);

      ELLE_TRACE_SCOPE("consult(%s, %s)", index, size);

      for (; iterator != end; ++iterator)
        {
          Entry* entry = iterator->second;

          // if this entry lies in the selected range [index, index + size[
          if ((i >= index) && (i < (index + size)))
            {
              // check if the entry is empty: this should never
              // happen but you never know!
              if (entry->name().empty() == true)
                continue;

              // insert the entry to the range.
              if (range.Add(entry) == elle::Status::Error)
                throw Exception("unable to add the entry to the rane");
            }

          // increment the index.
          //
          // note that this is done at the end so that empty
          // entries are not taken into account.
          i++;
        }

      return (range);
    }

    void
    Catalog::erase(elle::String const& name)
    {
      typename Catalog::Iterator iterator;
      Entry* entry;

      ELLE_TRACE_SCOPE("erase(%s)", name);

      // locate the entry for the given name.
      iterator = this->locate_iterator(name);

      // retrieve the entry.
      entry = iterator->second;

      // substract the entry footprint from the catalog's.
      assert(entry->footprint() != 0);
      assert(this->footprint() >= entry->footprint());
      this->footprint(this->footprint() - entry->footprint());
      assert(this->capacity() > 0);
      this->capacity(this->capacity() - 1);
      this->state(proton::StateDirty);

      // delete the entry.
      delete entry;

      // finally, erase the entry.
      this->_container.erase(iterator);
    }

    proton::Handle
    Catalog::split()
    {
      ELLE_TRACE_SCOPE("split()");

      std::unique_ptr<proton::Contents> contents{
        new proton::Contents(new Catalog)};
      proton::Handle orphan(this->nest().attach(std::move(contents)));
      proton::Ambit<Catalog> right(this->nest(), orphan);

      // load the new right catalog.
      right.load();

      // export the entries from the current catalog into the new catalog.
      Catalog::transfer_right(this,
                              right(),
                              this->nest().limits().extent() *
                              this->nest().limits().contention());

      // set both nodules' state as dirty.
      this->state(proton::StateDirty);
      right()->state(proton::StateDirty);

      // unload the new right nodule.
      right.unload();

      return (orphan);
    }

    void
    Catalog::merge(proton::Handle& handle)
    {
      proton::Ambit<Catalog> contents(this->nest(), handle);

      ELLE_TRACE_SCOPE("merge(%s)", handle);

      contents.load();

      // check which value has the lowest keys.
      if (contents()->mayor() < this->mayor())
        {
          // in this case, export the entries from the given catalog
          // into the current's since these entries happen to have
          // lower keys.
          Catalog::transfer_right(contents(), this, 0);
        }
      else
        {
          // otherwise, import the higher entries from the given catalog
          // into the current's.
          Catalog::transfer_left(this, contents(), 0);
        }

      // set both nodules' state as dirty.
      this->state(proton::StateDirty);
      contents()->state(proton::StateDirty);

      // unload the given catalog.
      contents.unload();
    }

    Catalog::Container&
    Catalog::container()
    {
      return (this->_container);
    }

//
// ---------- value -----------------------------------------------------------
//

    elle::Boolean
    Catalog::empty() const
    {
      return (this->_container.empty());
    }

    elle::String
    Catalog::mayor() const
    {
      assert(this->_container.empty() == false);

      return (this->_container.rbegin()->first);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the catalog.
    ///
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
      for (scoutor = this->_container.begin();
           scoutor != this->_container.end();
           scoutor++)
        {
          Entry* entry = scoutor->second;

          // dump the entry.
          if (entry->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the entry");
        }

      return (elle::Status::Ok);
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Catalog::print(std::ostream& stream) const
    {
      stream << "catalog("
             << "XXX"
             << ")";
    }

  }
}
