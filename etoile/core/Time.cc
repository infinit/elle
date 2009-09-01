//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Time.cc
//
// created       julien quintard   [sat aug 22 00:03:52 2009]
// updated       julien quintard   [mon aug 31 21:37:39 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Time.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor.
    ///
    Time::Time():
      seconds(0),
      minutes(0),
      hours(0),
      day(0),
      month(0),
      year(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    /// \todo XXX localtime should be locked cause it accesses a shared
    ///   zone.
    ///
    Status		Time::Current()
    {
      ::time_t		timer;
      struct ::tm*	tm;

      // retrieve the local time.
      if (::time(&timer) == -1)
	escape(::strerror(errno));

      if ((tm = ::localtime(&timer)) == NULL)
	escape(::strerror(errno));

      // set the attributes.
      this->seconds = tm->tm_sec;
      this->minutes = tm->tm_min;
      this->hours = tm->tm_hour;
      this->day = tm->tm_mday;
      this->month = 1 + tm->tm_mon;
      this->year = 1900 + tm->tm_year;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the time.
    ///
    Time&		Time::operator=(const Time&		element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the time.
      if (this->Recycle<Time>(&element) == StatusError)
	yield("unable to recycle the time", *this);

      return (*this);
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Time::operator==(const Time&		element) const
    {
      return ((this->seconds == element.seconds) &&
	      (this->minutes == element.minutes) &&
	      (this->hours == element.hours) &&
	      (this->day == element.day) &&
	      (this->month == element.month) &&
	      (this->year == element.year));
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean		Time::operator!=(const Time&		element) const
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an time object.
    ///
    Status		Time::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Time]" << std::endl;

      std::cout << alignment << shift << "[Seconds] "
		<< std::nouppercase << std::dec	
		<< (Natural32)this->seconds << std::endl;
      std::cout << alignment << shift << "[Minutes] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->minutes << std::endl;
      std::cout << alignment << shift << "[Hours] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->hours << std::endl;

      std::cout << alignment << shift << "[Day] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->day << std::endl;
      std::cout << alignment << shift << "[Month] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->month << std::endl;
      std::cout << alignment << shift << "[Year] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->year << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the time object.
    ///
    Status		Time::Serialize(Archive&		archive) const
    {
      // serialize the internal attributes.
      if (archive.Serialize(this->seconds,
			    this->minutes,
			    this->hours,
			    this->day,
			    this->month,
			    this->year) == StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the time object.
    ///
    Status		Time::Extract(Archive&			archive)
    {
      // extract the internal attributes.
      if (archive.Extract(this->seconds,
			  this->minutes,
			  this->hours,
			  this->day,
			  this->month,
			  this->year) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
