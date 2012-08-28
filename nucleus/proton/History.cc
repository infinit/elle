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

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this string defines the history files extension.
    ///
    const elle::String          History::Extension = ".hty";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds a version to the history.
    ///
    elle::Status        History::Register(const Version&        version)
    {
      // store the version in the history's vector.
      this->container.push_back(version);

      return elle::Status::Ok;
    }

    ///
    /// this method returns the version object corresponding to the given
    /// index number.
    ///
    elle::Status        History::Select(const Version::Type     index,
                                        Version&                version) const
    {
      // check if the index is out of bound.
      if (index >= this->container.size())
        escape("the version index is out of bound");

      // return the version.
      version = this->container[index];

      return elle::Status::Ok;
    }

    ///
    /// this method returns the size of the history.
    ///
    elle::Status        History::Size(Version::Type&            size) const
    {
      // return the size.
      size = this->container.size();

      return elle::Status::Ok;
    }

    elle::io::Path
    History::_path(Network const& network,
                   Address const& address)
    {
      return (elle::io::Path(lune::Lune::Network::Shelter::History,
                             elle::io::Piece("%NETWORK%", network.name),
                             elle::io::Piece("%ADDRESS%", address.unique())));
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       History::operator==(const History&      element) const
    {
      Version::Type     size;
      Version::Type     i;

      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // check the containers' size.
      if (this->container.size() != element.container.size())
        return false;

      // retrieve the size.
      size = this->container.size();

      // go through the container and compare.
      for (i = 0; i < size; i++)
        {
          // compare the containers.
          if (this->container[i] != element.container[i])
            return false;
        }

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(History, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an history object.
    ///
    elle::Status        History::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');
      Version::Type     i;

      // display the name.
      std::cout << alignment << "[History]" << std::endl;

      // go through the container.
      for (i = 0; i < this->container.size(); i++)
        {
          Version       version;

          // display the entry.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Entry]" << std::endl;

          // display the index.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Index] " << i << std::endl;

          // retrieve the version.
          version = this->container[i];

          // dump the version.
          if (version.Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the version");
        }

      return elle::Status::Ok;
    }

//
// ---------- fileable --------------------------------------------------------
//

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
