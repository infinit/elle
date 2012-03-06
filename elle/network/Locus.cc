//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat nov 28 13:01:48 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Locus.hh>

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the definition of a Null locus.
    ///
    const Locus                 Locus::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Locus::Locus():
      port(0)
    {
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
        escape("unable to locate the host/port separator ':'");

      // create the host.
      if (this->host.Create(string.substr(0, separator)) == StatusError)
        escape("unable to create the host");

      // create the port.
      this->port =
        static_cast<Port>(::strtoul(string.substr(separator + 1).c_str(),
                                    NULL, 0));

      return StatusOk;
    }

    ///
    /// this method creates an locus.
    ///
    Status              Locus::Create(const Host&               host,
                                      const Port                port)
    {
      this->host = host;
      this->port = port;

      return StatusOk;
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
        return StatusTrue;

      // compare the internal values.
      if ((this->host != element.host) ||
          (this->port != element.port))
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// compare two objects.
    ///
    Boolean             Locus::operator<(const Locus&           element) const
    {
      // check the locus as this may actually be the same object.
      if (this == &element)
        return StatusFalse;

      // compare the host.
      if (this->host < element.host)
        return StatusTrue;
      else if (this->host > element.host)
        return StatusFalse;

      // compare the port.
      if (this->port < element.port)
        return StatusTrue;
      else if (this->port > element.port)
        return StatusFalse;

      return StatusFalse;
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
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the locus.
    ///
    Status              Locus::Serialize(Archive&               archive) const
    {
      // serialize the host and port.
      if (archive.Serialize(this->host,
                            this->port) == StatusError)
        escape("unable to serialize the locus attributes");

      return StatusOk;
    }

    ///
    /// this method extracts an locus.
    ///
    Status              Locus::Extract(Archive&                 archive)
    {
      // extract the locus.
      if (archive.Extract(this->host,
                          this->port) == StatusError)
        escape("unable to extract the locus attributes");

      return StatusOk;
    }

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

      if (this->host.Dump(margin + 2) == StatusError)
        escape("unable to dump the host");

      std::cout << alignment << Dumpable::Shift << "[Port] "
                << std::dec << this->port << std::endl;

      return StatusOk;
    }

  }
}
