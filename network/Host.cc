#include <elle/network/Host.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/idiom/Close.hh>
# include <QNetworkInterface>
# include <QHostAddress>
# include <QList>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this contant defines a null host.
    ///
    const Host                  Null;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns in the given container the list of hosts
    /// addresses associated with the current host i.e computer.
    ///
    Status              Host::Hosts(Container&                  container)
    {
      ::QList< ::QHostAddress >         ql =
        ::QNetworkInterface::allAddresses();

      // go through all the host's addresses.
      for (auto scoutor = ql.begin();
           scoutor != ql.end();
           scoutor++)
        {
          ::QHostAddress                qha = *scoutor;
          Host                          host;

          // ignore all the special addresses though most of the checks
          // are needless since it is impossible for an interface's address
          // to be set to broadcast for instance.
          if ((qha            == ::QHostAddress::Null)            ||
              (qha            == ::QHostAddress::LocalHost)       ||
              (qha            == ::QHostAddress::LocalHostIPv6)   ||
              (qha            == ::QHostAddress::Broadcast)       ||
              (qha            == ::QHostAddress::Any)             ||
              (qha            == ::QHostAddress::AnyIPv6)         ||
              (qha.protocol() == ::QAbstractSocket::IPv6Protocol) ||
              (qha.protocol() == ::QAbstractSocket::UnknownNetworkLayerProtocol))
            continue;

          // create a host.
          if (host.Create(qha.toString().toStdString()) == Status::Error)
            escape("unable to create the host from '%s'",
                   qha.toString().toStdString().c_str());

          // add the host to the container.
          container.push_back(host);
        }

      return Status::Ok;
    }

    /*-------------.
    | Construction |
    `-------------*/

    Host::Host()
      : type(TypeNull)
    {}

    Host::Host(Type type)
      : Host()
    {
      Create(type);
    }


//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method enables the caller the creation of special hosts.
    ///
    Status              Host::Create(const Type         type)
    {
      // set the type.
      this->type = type;

      // set the host accordingly.
      switch (this->type)
        {
        case TypeNull:
        case TypeAny:
          {
            this->location = ::QHostAddress::Any;

            break;
          }
        case TypeBroadcast:
          {
            this->location = ::QHostAddress::Broadcast;

            break;
          }
        case TypeIP:
          {
            // nothing to do here.
          }
        }

      return Status::Ok;
    }

    ///
    /// this method creates an IPv4 or IPv6 address.
    ///
    Status              Host::Create(const String&              string)
    {
      // set the type.
      this->type = Host::TypeIP;

      // set the address.
      if (this->location.setAddress(string.c_str()) == false)
        escape("unable to set the location");

      return Status::Ok;
    }

    ///
    /// this method converts a host into a human-readable string.
    ///
    Status              Host::Convert(String&                   string) const
    {
      string = this->location.toString().toStdString();

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// checks if two objects match.
    ///
    Boolean             Host::operator==(const Host&            element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal values.
      if ((this->type != element.type) ||
          (this->location != element.location))
        return false;

      return true;
    }

    ///
    /// compare two objects.
    ///
    Boolean             Host::operator<(const Host&             element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return false;

      // compare the type
      if (this->type < element.type)
        return true;
      else if (this->type > element.type)
        return false;
      else
        {
          ::QString     first;
          ::QString     second;

          // generate the string.
          first = this->location.toString();
          second = element.location.toString();

          // compare the string.
          if (first < second)
            return true;
          else if (first > second)
            return false;
        }

      return false;
    }

    ///
    /// compare two objects.
    ///
    Boolean             Host::operator>(const Host&             element) const
    {
      return (!(this->operator<=(element)));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Host, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an host.
    ///
    Status              Host::Dump(const Natural32              margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Host]" << std::endl;

      std::cout << alignment << io::Dumpable::Shift << "[Type] "
                << this->type << std::endl;

      std::cout << alignment << io::Dumpable::Shift << "[Location] "
                << this->location.toString().toStdString() << std::endl;

      return Status::Ok;
    }

  }
}
