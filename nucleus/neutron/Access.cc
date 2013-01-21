#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Fellow.hh>
#include <nucleus/proton/Ambit.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Limits.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/Exception.hh>

#include <elle/log.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/Buffer.hh>

#include <cryptography/Digest.hh>
#include <cryptography/oneway.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

ELLE_LOG_COMPONENT("infinit.nucleus.neutron.Access");

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Constants |
    `----------*/

    proton::Nature const Access::Constants::seam =
      proton::Nature::access_seam;
    proton::Nature const Access::Constants::quill =
      proton::Nature::access_quill;
    proton::Nature const Access::Constants::value =
      proton::Nature::access_value;
    proton::Nature const Access::Constants::nature =
      Access::Constants::value;

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    Access::transfer_right(Access& left,
                           Access& right,
                           proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Access>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto end = left._container.end();
      auto i = left._container.begin();

      // Go through the left access's records until the future size is
      // reached after which all the remaining records will be moved to
      // the right access.
      for (; i != end; ++i)
        {
          auto& record = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this item in the left record.
          //
          // Note however that another check is performed in order to make
          // sure that adding this record will not make the container too large.
          if ((footprint + record->footprint()) > extent)
            break;

          // Add the record's footprint to the local _footprint_ variable.
          footprint += record->footprint();
        }

      // Go through the remaining records in order to move them to
      // the right access.
      for (auto j = i; j != end; ++j)
        {
          auto& record = j->second;

          // Substract the record's footprint from the left access since
          // it is getting moved to the right one.
          left.footprint(left.footprint() - record->footprint());

          // Insert the record into the right access.
          right.insert(record);
        }

      // Remove the moved records from the the current access.
      left._container.erase(i, end);
    }

    void
    Access::transfer_left(Access& left,
                          Access& right,
                          proton::Extent const size)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint<Access>();

      ELLE_TRACE_FUNCTION(left, right, size);

      proton::Extent const extent = left.nest().limits().extent();
      proton::Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto rend = right._container.rend();
      auto i = right._container.rbegin();

      // Go through the right access's records until the future size is
      // reached after which all the remaining records will be moved to
      // the left access.
      for (; i != rend; ++i)
        {
          auto& record = i->second;

          // Check whether the container's contention size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this record in the access.
          //
          // Note however that another check is performed in order to make
          // sure that adding this record will not make the container too large.
          if ((footprint + record->footprint()) > extent)
            break;

          // Add the record's footprint to the footprint.
          footprint += record->footprint();
        }

      // Go through the remaining records in order to move them to
      // the left access.
      for (auto j = i; j != rend; ++j)
        {
          auto& record = j->second;

          // Substract the record's footprint from the right access since
          // it is getting moved to the left one.
          right.footprint(right.footprint() - record->footprint());

          // Insert the record into the left access.
          left.insert(record);
        }

      // Remove the moved records from the right access.
      right._container.erase(right._container.begin(), i.base());
    }

    /*-------------.
    | Construction |
    `-------------*/

    Access::Access():
      Value::Value()
    {
      static proton::Footprint const initial =
        elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Access, proton::Value)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Access::insert(Record* record)
    {
      ELLE_TRACE_METHOD(record);

      ELLE_ASSERT(record != nullptr);

      std::shared_ptr<Record> pointer{record};

      this->insert(pointer);
    }

    void
    Access::insert(std::shared_ptr<Record> const& record)
    {
      ELLE_TRACE_METHOD(record);

      // Inject the record in the container.
      this->_inject(record);

      // Update the state.
      this->state(proton::State::dirty);
    }

    elle::Boolean
    Access::exist(Subject const& subject) const
    {
      ELLE_TRACE_METHOD(subject);

      return (this->_container.find(subject) != this->_container.end());
    }

    void
    Access::update(Subject const& subject,
                   Permissions const permissions)
    {
      ELLE_TRACE_METHOD(subject, permissions);

      // Locate the record.
      auto iterator = this->_iterator(subject);

      // Retrieve the record.
      auto& record = iterator->second;

      // Substract the record's current footprint.
      ELLE_ASSERT(record->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= record->footprint());
      this->footprint(this->footprint() - record->footprint());

      // Update the record.
      record->permissions(permissions);

      // Add the record's new footprint.
      ELLE_ASSERT(record->footprint() != 0);
      this->footprint(this->footprint() + record->footprint());

      // Update the state.
      this->state(proton::State::dirty);
    }

    void
    Access::update(Subject const& subject,
                   Token const& token)
    {
      ELLE_TRACE_METHOD(subject, token);

      // Locate the record.
      auto iterator = this->_iterator(subject);

      // Retrieve the record.
      auto& record = iterator->second;

      // Substract the record's current footprint.
      ELLE_ASSERT(record->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= record->footprint());
      this->footprint(this->footprint() - record->footprint());

      // Update the record.
      record->token(token);

      // Add the record's new footprint.
      ELLE_ASSERT(record->footprint() != 0);
      this->footprint(this->footprint() + record->footprint());

      // Update the state.
      this->state(proton::State::dirty);
    }

    Record const&
    Access::locate(Subject const& subject) const
    {
      ELLE_TRACE_METHOD(subject);

      auto scoutor = this->_iterator(subject);
      auto& record = scoutor->second;

      return (*record);
    }

    Index
    Access::seek(Subject const& subject) const
    {
      ELLE_TRACE_METHOD(subject);

      Index index{0};

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          if (record->subject() == subject)
            return (index);

          index++;
        }

      throw Exception("unable to locate the given subject '%s'", subject);
    }

    Record const&
    Access::select(Index const& index) const
    {
      ELLE_TRACE_METHOD(index);

      Index i{0};

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          if (i == index)
            return (*record);
        }

      throw Exception("unable to locate the record at the given index '%s'",
                      index);
    }

    Range<Record>
    Access::consult(Index const& index,
                    Size const& size) const
    {
      ELLE_TRACE_METHOD(index);

      Range<Record> range;
      Index i{0};

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          // If this record lies in the selected range [index, index + size[.
          if ((i >= index) && (i < (index + size)))
            range.insert(record);

          i++;
        }

      return (range);
    }

    void
    Access::erase(Subject const& subject)
    {
      ELLE_TRACE_METHOD(subject);

      // Call take out and forget about the returned record, hence (possibly)
      // deleting it.
      this->takeout(subject);
    }

    std::shared_ptr<Record>
    Access::takeout(Subject const& subject)
    {
      // Locate the record for the given subject.
      auto iterator = this->_iterator(subject);

      // Retrieve the record.
      std::shared_ptr<Record> record = iterator->second;

      // Substract the record footprint from the access's.
      ELLE_ASSERT(record->footprint() != 0);
      ELLE_ASSERT(this->footprint() >= record->footprint());
      this->footprint(this->footprint() - record->footprint());

      // Remove the record from the container.
      this->_container.erase(iterator);

      // Update the state.
      this->state(proton::State::dirty);

      // And finally return the pointer.
      return (record);
    }

    Size
    Access::size() const
    {
      ELLE_TRACE_METHOD("");

      return (static_cast<Size>(this->_container.size()));
    }

    void
    Access::_inject(std::shared_ptr<Record> const& record)
    {
      ELLE_DEBUG_METHOD(record);

      // Check that the record's subject does not already exist.
      if (this->_container.find(record->subject()) != this->_container.end())
        throw Exception("the subject '%s' seems to already exist",
                        record->subject());

      // Insert the record to the container.
      auto result =
        this->_container.insert(
          std::pair<Subject const, std::shared_ptr<Record>>{
            record->subject(), record});

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the record the container");

      // Update the footprint.
      ELLE_ASSERT(record->footprint() != 0);
      this->footprint(this->footprint() + record->footprint());
    }

    typename Access::Scoutor
    Access::_iterator(Subject const& subject) const
    {
      Scoutor scoutor;

      if ((scoutor = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject: '%s'", subject);

      return (scoutor);
    }

    typename Access::Iterator
    Access::_iterator(Subject const& subject)
    {
      Iterator iterator;

      if ((iterator = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject: '%s'", subject);

      return (iterator);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Access::Dump(elle::Natural32            margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Access] #"
                << this->_container.size() << std::endl;

      Value::Dump(margin + 2);

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          if (record->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the record");
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Access::print(std::ostream& stream) const
    {
      stream << "access(#"
             << this->_container.size()
             << ")";
    }

    /*------.
    | Value |
    `------*/

    elle::Boolean
    Access::empty() const
    {
      return (this->_container.empty());
    }

    Subject
    Access::mayor() const
    {
      ELLE_ASSERT(this->_container.empty() == false);

      return (this->_container.rbegin()->first);
    }

    proton::Capacity
    Access::capacity() const
    {
      return (static_cast<proton::Capacity>(this->_container.size()));
    }

    proton::Handle
    Access::split()
    {
      ELLE_TRACE_METHOD("");

      // Allocate a new right access.
      proton::Contents* contents =
        new proton::Contents{this->nest().network(),
                             this->nest().agent_K(),
                             new Access};
      proton::Handle orphan{this->nest().attach(contents)};
      proton::Ambit<Access> right{this->nest(), orphan};

      // Load the new right access.
      right.load();

      // Export the records from the current access into the new access.
      Access::transfer_right(*this,
                             right(),
                             this->nest().limits().extent() *
                             this->nest().limits().contention());

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      right().state(proton::State::dirty);

      // Unload the new right access.
      right.unload();

      return (orphan);
    }

    void
    Access::merge(proton::Handle& other)
    {
      ELLE_TRACE_METHOD(other);

      proton::Ambit<Access> access(this->nest(), other);

      // Load the other access.
      access.load();

      // Check which value has the lowest keys.
      if (access().mayor() < this->mayor())
        {
          // In this case, export the records from the given access
          // into the current's since these records happen to have
          // lower keys.
          //
          // Note that the footprint-based number of records to keep in
          // the left access is zero i.e transfer all records.
          Access::transfer_right(access(), *this, 0);
        }
      else
        {
          // Otherwise, import the higher records from the given access
          // into the current's.
          //
          // Note that the footprint-based number of records to keep in
          // the right access is zero i.e transfer all records.
          Access::transfer_left(*this, access(), 0);
        }

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      access().state(proton::State::dirty);

      ELLE_ASSERT(access()._container.size() == 0);

      // Unload the given access.
      access.unload();
    }

    /*---------.
    | Iterable |
    `---------*/

    typename Access::Scoutor
    Access::begin() const
    {
      return (this->_container.begin());
    }

    typename Access::Scoutor
    Access::end() const
    {
      return (this->_container.end());
    }

    typename Access::Iterator
    Access::begin()
    {
      return (this->_container.begin());
    }

    typename Access::Iterator
    Access::end()
    {
      return (this->_container.end());
    }

    namespace access
    {
      /*----------.
      | Functions |
      `----------*/

      cryptography::Digest
      fingerprint(proton::Porcupine<Access> const& porcupine)
      {
        ELLE_TRACE_FUNCTION(porcupine);

        static cryptography::oneway::Algorithm algorithm =
          cryptography::oneway::Algorithm::sha256;

        elle::Buffer buffer;

        for (proton::Capacity index = 0; index < porcupine.size(); )
          {
            auto pair = porcupine.seek(index);
            // XXX[what we want] proton::Door<Fellow const>& door = pair.first;
            auto& door = pair.first;

            ELLE_ASSERT(index == pair.second);

            door.open();

            for (auto _pair: door())
              {
                auto& record = _pair.second;

                buffer.writer() << record->subject()
                                << record->permissions();
              }

            index += door().size();

            door.close();
          }

        return (cryptography::oneway::hash(
                  cryptography::Plain{elle::WeakBuffer{buffer}},
                  algorithm));
      }
    }
  }
}
