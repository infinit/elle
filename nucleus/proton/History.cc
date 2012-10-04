#include <nucleus/proton/History.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <lune/Lune.hh>

#include <elle/utility/Buffer.hh>
#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    /*------------.
    | Definitions |
    `------------*/

    const elle::String          History::Extension = ".hty";

    /*--------.
    | Methods |
    `--------*/

    elle::Status        History::Register(const Revision&        revision)
    {
      // store the revision in the history's vector.
      this->_container.push_back(revision);

      return elle::Status::Ok;
    }

    elle::Status        History::Select(const Revision::Type     index,
                                        Revision&                revision) const
    {
      // check if the index is out of bound.
      if (index >= this->_container.size())
        escape("the revision index is out of bound");

      // return the revision.
      revision = this->_container[index];

      return elle::Status::Ok;
    }

    elle::Status        History::Size(Revision::Type&            size) const
    {
      // return the size.
      size = this->_container.size();

      return elle::Status::Ok;
    }

    elle::io::Path
    History::_path(Network const& network,
                   Address const& address)
    {
      return (elle::io::Path(lune::Lune::Network::Shelter::History,
                             elle::io::Piece("%NETWORK%", network.name()),
                             elle::io::Piece("%ADDRESS%", address.unique())));
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    History::operator ==(History const& other) const
    {
      Revision::Type size;
      Revision::Type i;

      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // check the containers' size.
      if (this->_container.size() != other._container.size())
        return false;

      // retrieve the size.
      size = this->_container.size();

      // go through the container and compare.
      for (i = 0; i < size; i++)
        {
          // compare the containers.
          if (this->_container[i] != other._container[i])
            return false;
        }

      return true;
    }

    /*-----------.
    | Interfaces |
    `-----------*/

    elle::Status        History::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');
      Revision::Type     i;

      // display the name.
      std::cout << alignment << "[History]" << std::endl;

      // go through the container.
      for (i = 0; i < this->_container.size(); i++)
        {
          Revision       revision;

          // display the entry.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Entry]" << std::endl;

          // display the index.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Index] " << i << std::endl;

          // retrieve the revision.
          revision = this->_container[i];

          // dump the revision.
          if (revision.Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the revision");
        }

      return elle::Status::Ok;
    }

    void
    History::print(std::ostream& stream) const
    {
      stream << "history("
             << "#" << this->_container.size()
             << ")";
    }

    void
    History::load(Network const& network,
                  Address const& address)
    {
      this->load(History::_path(network, address));
    }

    void
    History::store(Network const& network,
                   Address const& address) const
    {
      this->store(History::_path(network, address));
    }

    void
    History::erase(Network const& network,
                   Address const& address)
    {
      elle::io::Path path(History::_path(network, address));

      if (elle::concept::Fileable<>::exists(path) == true)
        elle::concept::Fileable<>::erase(path);
    }

    elle::Boolean
    History::exists(Network const& network,
                    Address const& address)
    {
      return (elle::concept::Fileable<>::exists(
        History::_path(network, address)));
    }

  }
}
