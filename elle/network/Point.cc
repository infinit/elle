//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Point.cc
//
// created       julien quintard   [sat nov 28 13:01:48 2009]
// updated       julien quintard   [wed sep  7 18:11:49 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Point.hh>

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
    /// the definition of a Null point.
    ///
    const Point			Point::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Point::Point():
      port(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an point from a string of the form
    /// 'host:port'.
    ///
    Status		Point::Create(const String&		string)
    {
      Natural32		separator;

      enter();

      // locate the ':' separator.
      separator = string.find_first_of(':');

      // create the host.
      if (this->host.Create(string.substr(0, separator)) == StatusError)
	escape("unable to create the host");

      // create the port.
      this->port =
	static_cast<Port>(::strtoul(string.substr(separator + 1).c_str(),
				    NULL, 0));

      leave();
    }

    ///
    /// this method creates an point.
    ///
    Status		Point::Create(const Host&		host,
				      const Port		port)
    {
      enter();

      this->host = host;
      this->port = port;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// checks if two objects match.
    ///
    Boolean		Point::operator==(const Point&		element) const
    {
      enter();

      // check the point as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal values.
      if ((this->host != element.host) ||
	  (this->port != element.port))
	false();

      true();
    }

    ///
    /// compare two objects.
    ///
    Boolean		Point::operator<(const Point&		element) const
    {
      enter();

      // check the point as this may actually be the same object.
      if (this == &element)
	false();

      // compare the host.
      if (this->host < element.host)
	true();
      else if (this->host > element.host)
	false();

      // compare the port.
      if (this->port < element.port)
	true();
      else if (this->port > element.port)
	false();

      false();
    }

    ///
    /// compare two objects.
    ///
    Boolean		Point::operator>(const Point&		element) const
    {
      return (!(this->operator<=(element)));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Point, _());

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the point.
    ///
    Status		Point::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the host and port.
      if (archive.Serialize(this->host,
			    this->port) == StatusError)
	escape("unable to serialize the point attributes");

      leave();
    }

    ///
    /// this method extracts an point.
    ///
    Status		Point::Extract(Archive&			archive)
    {
      enter();

      // extract the point.
      if (archive.Extract(this->host,
			  this->port) == StatusError)
	escape("unable to extract the point attributes");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an point.
    ///
    Status		Point::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Point]" << std::endl;

      if (this->host.Dump(margin + 2) == StatusError)
	escape("unable to dump the host");

      std::cout << alignment << Dumpable::Shift << "[Port] "
		<< std::dec << this->port << std::endl;

      leave();
    }

  }
}
