#include <elle/network/Locus.hh>
#include <elle/io/Dumpable.hh>
#include <elle/types.hh>

namespace elle
{
  namespace network
  {

    ///
    /// the definition of a Null locus.
    ///
    const Locus                 Locus::Null;

    /*-------------.
    | Construction |
    `-------------*/

    Locus::Locus()
      : port(0)
    {}


    Locus::Locus(std::string const& hostname, int port)
      : Locus()
    {
      Host h;
      h.Create(hostname);
      Create(h, port);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an locus from a string of the form
    /// 'host:port'.
    ///
    Status              Locus::Create(const String&             string)
    {
      size_t            separator;

      // locate the ':' separator.
      if ((separator = string.find_first_of(':')) == string.npos)
        escape("unable to locate the host/port separator in '%s'",
               string.c_str());

      // create the host.
      if (this->host.Create(string.substr(0, separator)) == Status::Error)
        escape("unable to create the host");

      // create the port.
      this->port =
        static_cast<Port>(::strtoul(string.substr(separator + 1).c_str(),
                                    nullptr, 0));

      return Status::Ok;
    }

    ///
    /// this method creates an locus.
    ///
    Status              Locus::Create(const Host&               host,
                                      const Port                port)
    {
      this->host = host;
      this->port = port;

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// checks if two objects match.
    ///
    Boolean             Locus::operator==(const Locus&          element) const
    {
      // check the locus as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal values.
      if ((this->host != element.host) ||
          (this->port != element.port))
        return false;

      return true;
    }

    ///
    /// compare two objects.
    ///
    Boolean             Locus::operator<(const Locus&           element) const
    {
      // check the locus as this may actually be the same object.
      if (this == &element)
        return false;

      // compare the host.
      if (this->host < element.host)
        return true;
      else if (this->host > element.host)
        return false;

      // compare the port.
      if (this->port < element.port)
        return true;
      else if (this->port > element.port)
        return false;

      return false;
    }

    ///
    /// compare two objects.
    ///
    Boolean             Locus::operator>(const Locus&           element) const
    {
      return (!(this->operator<=(element)));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Locus, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an locus.
    ///
    Status              Locus::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Locus]" << std::endl;

      if (this->host.Dump(margin + 2) == Status::Error)
        escape("unable to dump the host");

      std::cout << alignment << io::Dumpable::Shift << "[Port] "
                << std::dec << this->port << std::endl;

      return Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Locus::print(std::ostream& stream) const
    {
      std::string host;
      this->host.Convert(host);
      stream << host << ":" << this->port;
    }
  }
}
