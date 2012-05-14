//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 16 05:24:44 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Host.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
          if (host.Create(qha.toString().toStdString()) == StatusError)
            escape("unable to create the host from '%s'",
                   qha.toString().toStdString().c_str());

          // add the host to the container.
          container.push_back(host);
        }

      return StatusOk;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor initializes the object to Null.
    ///
    Host::Host():
      type(TypeNull)
    {
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

      return StatusOk;
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

      return StatusOk;
    }

    ///
    /// this method converts a host into a human-readable string.
    ///
    Status              Host::Convert(String&                   string) const
    {
      string = this->location.toString().toStdString();

      return StatusOk;
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
        return StatusTrue;

      // compare the internal values.
      if ((this->type != element.type) ||
          (this->location != element.location))
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// compare two objects.
    ///
    Boolean             Host::operator<(const Host&             element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return StatusFalse;

      // compare the type
      if (this->type < element.type)
        return StatusTrue;
      else if (this->type > element.type)
        return StatusFalse;
      else
        {
          ::QString     first;
          ::QString     second;

          // generate the string.
          first = this->location.toString();
          second = element.location.toString();

          // compare the string.
          if (first < second)
            return StatusTrue;
          else if (first > second)
            return StatusFalse;
        }

      return StatusFalse;
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
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the host.
    ///
    Status              Host::Serialize(Archive&                archive) const
    {
      String            host(this->location.toString().toStdString());

      // serialize the host and port.
      if (archive.Serialize(static_cast<Natural8>(this->type),
                            host) == StatusError)
        escape("unable to serialize the host");

      return StatusOk;
    }

    ///
    /// this method extracts a host.
    ///
    Status              Host::Extract(Archive&                  archive)
    {
      Natural8          type;
      String            host;

      // extract the host.
      if (archive.Extract(type, host) == StatusError)
        escape("unable to extract the host");

      // set the type.
      this->type = static_cast<Host::Type>(type);

      // set the location.
      this->location.setAddress(host.c_str());

      return StatusOk;
    }

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

      std::cout << alignment << Dumpable::Shift << "[Type] "
                << this->type << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Location] "
                << this->location.toString().toStdString() << std::endl;

      return StatusOk;
    }

  }
}
