#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/Exception.hh>

#include <elle/Buffer.hh>

#include <cryptography/Digest.hh>

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Constants |
    `----------*/

    cryptography::oneway::Algorithm const Access::Constants::oneway_algorithm{
      cryptography::oneway::Algorithm::sha256};

    Component const Access::Constants::component{ComponentAccess};

    /*-------------.
    | Construction |
    `-------------*/

    Access::Access():
      proton::ContentHashBlock()
    {
    }

    Access::Access(proton::Network const& network,
                   cryptography::PublicKey const& creator_K):
      proton::ContentHashBlock(network, ComponentAccess, creator_K)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Access::insert(Record* record)
    {
      std::shared_ptr<Record> pointer{record};

      this->_insert(pointer);
    }

    elle::Boolean
    Access::exist(Subject const& subject) const
    {
      return (this->_container.find(subject) != this->_container.end());
    }

    Record const&
    Access::locate(Subject const& subject) const
    {
      Scoutor scoutor;

      if ((scoutor = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject's record");

      auto& record = scoutor->second;

      return (*record);
    }

    Record&
    Access::locate(Subject const& subject)
    {
      Iterator iterator;

      if ((iterator = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject's record");

      auto& record = iterator->second;

      return (*record);
    }

    Index
    Access::seek(Subject const& subject) const
    {
      Index index{0};

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          if (record->subject() == subject)
            return (index);

          index++;
        }

      throw Exception("unable to locate the given subject");
    }

    Record const&
    Access::select(Index const& index) const
    {
      Index i{0};

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          if (i == index)
            return (*record);
        }

      throw Exception("unable to locate the record at the given index");
    }

    Record&
    Access::select(Index const& index)
    {
      Index i{0};

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          if (i == index)
            return (*record);

          i++;
        }

      throw Exception("unable to locate the record at the given index");
    }

    Range<Record>
    Access::consult(Index const& index,
                    Size const& size) const
    {
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
      Iterator iterator;

      // Locate the record.
      if ((iterator = this->_container.find(subject)) == this->_container.end())
        throw Exception("unable to locate the given subject's record");

      // Remove the record from the container. Should that be that last
      // reference on the record, it would be deleted.
      this->_container.erase(iterator);

      // Set the block as dirty.
      this->state(proton::State::dirty);
    }

    Size
    Access::size() const
    {
      return (static_cast<Size>(this->_container.size()));
    }

    cryptography::Digest
    Access::fingerprint() const
    {
      elle::Buffer buffer;

      for (auto& pair: this->_container)
        {
          auto& record = pair.second;

          buffer.writer() << record->subject()
                          << record->permissions();
        }

      cryptography::Digest digest{
        cryptography::oneway::hash(
          cryptography::Plain{elle::WeakBuffer{buffer}},
          Access::Constants::oneway_algorithm)};

      return (digest);
    }

    void
    Access::_insert(std::shared_ptr<Record> const& record)
    {
      // Check that the record's subject does not already exist.
      if (this->_container.find(record->subject()) != this->_container.end())
        throw Exception("the record's subject '%s' seems to already exist",
                        record->subject());

      // Insert the record to the container.
      auto result =
        this->_container.insert(
          std::pair<Subject const, std::shared_ptr<Record>>{
            record->subject(), record});

      // Check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the record the container");

      // Set the block as dirty.
      this->state(proton::State::dirty);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status        Access::Dump(elle::Natural32            margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Access] #"
                << this->_container.size() << std::endl;

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
  }
}
