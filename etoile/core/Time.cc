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
// updated       julien quintard   [thu mar 25 17:48:39 2010]
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
      second(0),
      minute(0),
      hour(0),
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

      enter();

      // retrieve the local time.
      if (::time(&timer) == -1)
	escape(::strerror(errno));

      if ((tm = ::localtime(&timer)) == NULL)
	escape(::strerror(errno));

      // set the attributes.
      this->second = tm->tm_sec;
      this->minute = tm->tm_min;
      this->hour = tm->tm_hour;
      this->day = tm->tm_mday;
      this->month = 1 + tm->tm_mon;
      this->year = 1900 + tm->tm_year;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Time::operator==(const Time&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes.
      if ((this->second != element.second) ||
	  (this->minute != element.minute) ||
	  (this->hour != element.hour) ||
	  (this->day != element.day) ||
	  (this->month != element.month) ||
	  (this->year != element.year))
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Time);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an time object.
    ///
    Status		Time::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
 
      enter();

      std::cout << alignment << "[Time]" << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Second] "
		<< std::nouppercase << std::dec	
		<< (Natural32)this->second << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Minute] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->minute << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Hour] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->hour << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Day] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->day << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Month] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->month << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Year] "
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
      enter();

      // serialize the internal attributes.
      if (archive.Serialize(this->second,
			    this->minute,
			    this->hour,
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
      enter();

      // extract the internal attributes.
      if (archive.Extract(this->second,
			  this->minute,
			  this->hour,
			  this->day,
			  this->month,
			  this->year) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator adds two times together.
    ///
    Time		operator+(const Time&			lhs,
				  const Time&			rhs)
    {
      Time		result(lhs);

      enter();

      result.second += rhs.second;
      result.hour += rhs.hour;
      result.minute += rhs.minute;
      result.day += rhs.day;
      result.month += rhs.month;
      result.year += rhs.year;

      if (result.second > 60)
	{
	  result.minute += result.second / 60;
	  result.second = result.second % 60;
	}

      if (result.minute > 60)
	{
	  result.hour += 1;
	  result.minute -= 60;
	}

      if (result.hour > 60)
	{
	  result.day += 1;
	  result.hour -= 60;
	}

      /// \todo XXX[does not handle special months]
      if (result.day > 31)
	{
	  result.month += 1;
	  result.day -= 31;
	}

      if (result.month > 12)
	{
	  result.year += 1;
	  result.month -= 12;
	}

      return (result);
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		operator<(const Time&			lhs,
				  const Time&			rhs)
    {
      return ((lhs.year < rhs.year) ||
	      (lhs.month < rhs.month) ||
	      (lhs.day < rhs.day) ||
	      (lhs.hour < rhs.hour) ||
	      (lhs.minute < rhs.minute) ||
	      (lhs.second < rhs.second));
    }

  }
}
