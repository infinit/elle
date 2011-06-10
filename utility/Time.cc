//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Time.cc
//
// created       julien quintard   [sat aug 22 00:03:52 2009]
// updated       julien quintard   [thu jun  9 21:41:39 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Time.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace utility
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
      year(0),
      dst(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method fills the instance with the current time.
    ///
    Status		Time::Current()
    {
      ::time_t		t;
      struct ::tm	tm;

      enter();

      // retrieve the local time.
      if (::time(&t) == -1)
	escape(::strerror(errno));

      if (::localtime_r(&t, &tm) == NULL)
	escape(::strerror(errno));

      // set the attributes.
      this->second = tm.tm_sec;
      this->minute = tm.tm_min;
      this->hour = tm.tm_hour;
      this->day = tm.tm_mday;
      this->month = 1 + tm.tm_mon;
      this->year = 1900 + tm.tm_year;
      this->dst = tm.tm_isdst;

      leave();
    }

    ///
    /// this method converts a time object into a time_t structure.
    ///
    Status		Time::Convert(::time_t&			time) const
    {
      struct ::tm	tm;

      enter();

      // build a tm structure.
      tm.tm_sec = this->second;
      tm.tm_min = this->minute;
      tm.tm_hour = this->hour;
      tm.tm_mday = this->day;
      tm.tm_mon = this->month - 1;
      tm.tm_year = this->year - 1900;
      tm.tm_isdst = this->dst;

      // create a time.
      if ((time = ::mktime(&tm)) == -1)
	escape(::strerror(errno));

      leave();
    }

//
// ---------- object ----------------------------------------------------------
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
	  (this->year != element.year) ||
	  (this->dst != element.dst))
	false();

      true();
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		Time::operator<(const Time&		element) const
    {
      return ((this->year < element.year) ||
	      (this->month < element.month) ||
	      (this->day < element.day) ||
	      (this->hour < element.hour) ||
	      (this->minute < element.minute) ||
	      (this->second < element.second));
    }

    ///
    /// this operator adds a time to the current one.
    ///
    Time		Time::operator+(const Time&		element)
    {
      Time		result;

      result.second = this->second + element.second;
      result.hour = this->hour + element.hour;
      result.minute = this->minute + element.minute;
      result.day = this->day + element.day;
      result.month = this->month + element.month;
      result.year = this->year + element.year;

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
    /// this macro-function call generates the object.
    ///
    embed(Time, _());

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

      std::cout << alignment << Dumpable::Shift << "[DST] "
		<< std::nouppercase << std::dec
		<< (Natural32)this->dst << std::endl;

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
			    this->year,
			    this->dst) == StatusError)
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
			  this->year,
			  this->dst) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
