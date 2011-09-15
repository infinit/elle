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
// updated       julien quintard   [sun sep 11 21:12:17 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pthread.h>

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
      millisecond(0),
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
    /// this method fills the instance with the current time.
    ///
    Status		Time::Current()
    {
      ::QDateTime	current;

      enter();

      // retrieve the current date/time.
      current = ::QDateTime::currentDateTime();

      // set the date & time.
      if (this->Set(current) == StatusError)
	escape("unable to set the date and time");

      leave();
    }

    ///
    /// this method converts a time object into a time_t structure.
    ///
    Status		Time::Get(::time_t&			time) const
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
      tm.tm_isdst = 1;

      // create a time.
      if ((time = ::mktime(&tm)) == -1)
	escape(::strerror(errno));

      leave();
    }

    ///
    /// this method converts a time object into a QDateTime structure.
    ///
    Status		Time::Get(::QDateTime&			dt) const
    {
      enter();

      // retrieve the current time so as to build a valid date/time.
      //
      // indeed, if this step is ignored, the return date/time is always
      // invalid...
      dt = ::QDateTime::currentDateTime();

      // set the date.
      if (dt.date().setDate(this->year, this->month, this->day) == false)
	escape("unable to set the date");

      // set the time.
      if (dt.time().setHMS(this->hour, this->minute, this->second,
			   this->millisecond) == false)
	escape("unable to set the time");

      leave();
    }

    ///
    /// this method converts a time_t into a time object.
    ///
    Status		Time::Set(const ::time_t&		time)
    {
      struct ::tm	tm;

      enter();

      // retrieve from the time.
      if (localtime_r(&time, &tm) == NULL)
	escape(::strerror(errno));

      // set the attributes
      //
      // note that the milliseconds are ignored in this case.
      this->second = tm.tm_sec;
      this->minute = tm.tm_min;
      this->hour = tm.tm_hour;
      this->day = tm.tm_mday;
      this->month = 1 + tm.tm_mon;
      this->year = 1900 + tm.tm_year;

      leave();
    }

    ///
    /// this method converts a QDateTime into a time object.
    ///
    Status		Time::Set(const ::QDateTime&		dt)
    {
      enter();

      // set the attributes.
      this->millisecond = dt.time().msec();
      this->second = dt.time().second();
      this->minute = dt.time().minute();
      this->hour = dt.time().hour();
      this->day = dt.date().day();
      this->month = dt.date().month();
      this->year = dt.date().year();

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

      // compare the attributes.
      if ((this->millisecond != element.millisecond) ||
	  (this->second != element.second) ||
	  (this->minute != element.minute) ||
	  (this->hour != element.hour) ||
	  (this->day != element.day) ||
	  (this->month != element.month) ||
	  (this->year != element.year))
	false();

      true();
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		Time::operator<(const Time&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	false();

      // compare the attributes.
      if (this->year > element.year)
	false();
      else if (this->year < element.year)
	true();
      else if (this->month > element.month)
	false();
      else if (this->month < element.month)
	true();
      else if (this->day > element.day)
	false();
      else if (this->day < element.day)
	true();
      else if (this->hour > element.hour)
	false();
      else if (this->hour < element.hour)
	true();
      else if (this->minute > element.minute)
	false();
      else if (this->minute < element.minute)
	true();
      else if (this->second > element.second)
	false();
      else if (this->second < element.second)
	true();
      else if (this->millisecond > element.millisecond)
	false();
      else if (this->millisecond < element.millisecond)
	true();

      false();
    }

    ///
    /// this operator adds a time to the current one.
    ///
    Time		Time::operator+(const Time&		element)
    {
      ::QDateTime	dt;
      ::QDateTime	r;
      ::QDateTime	e;
      Time		result;

      enter();

      // build dt.
      if (this->Get(dt) == StatusError)
	goto _return;

      // build e.
      if (element.Get(e) == StatusError)
	goto _return;

      // perform the addition.
      r = dt.
	addMSecs(e.time().msec()).
	addSecs(e.time().second() +
		e.time().minute() * 60 +
		e.time().hour() * 60 * 60).
	addDays(e.date().day()).
	addMonths(e.date().month()).
	addYears(e.date().year());

      // convert back to the result.
      if (result.Set(r) == StatusError)
	goto _return;

    _return:
      release();

      return (result);
    }

    ///
    /// this operator substracts a time to the current one.
    ///
    Time		Time::operator-(const Time&		element)
    {
      ::QDateTime	dt;
      ::QDateTime	r;
      ::QDateTime	e;
      Time		result;

      enter();

      // build dt.
      if (this->Get(dt) == StatusError)
	goto _return;

      // build e.
      if (element.Get(e) == StatusError)
	goto _return;

      // perform the substraction.
      r = dt.
	addMSecs(-e.time().msec()).
	addSecs(-e.time().second() +
		-e.time().minute() * 60 +
		-e.time().hour() * 60 * 60).
	addDays(-e.date().day()).
	addMonths(-e.date().month()).
	addYears(-e.date().year());

      // convert back to the result.
      if (result.Set(r) == StatusError)
	goto _return;

    _return:
      release();

      return (result);
    }

    ///
    /// this operator adds a duration to the current time.
    ///
    Time		Time::operator+(const Duration&		duration)
    {
      ::QDateTime	dt;
      ::QDateTime	r;
      Time		result;

      enter();

      // build dt.
      if (this->Get(dt) == StatusError)
	goto _return;

      // depending on the unit.
      switch (duration.unit)
	{
	case Duration::UnitMilliseconds:
	  {
	    // add the value.
	    r = dt.addMSecs(duration.value);

	    break;
	  }
	case Duration::UnitSeconds:
	  {
	    // add the value.
	    r = dt.addSecs(duration.value);

	    break;
	  }
	case Duration::UnitMinutes:
	  {
	    // add the value.
	    r = dt.addSecs(duration.value * 60);

	    break;
	  }
	case Duration::UnitHours:
	  {
	    // add the value.
	    r = dt.addSecs(duration.value * 60 * 60);

	    break;
	  }
	case Duration::UnitDays:
	  {
	    // add the value.
	    r = dt.addDays(duration.value);

	    break;
	  }
	case Duration::UnitMonths:
	  {
	    // add the value.
	    r = dt.addMonths(duration.value);

	    break;
	  }
	case Duration::UnitYears:
	  {
	    // add the value.
	    r = dt.addYears(duration.value);

	    break;
	  }
	case Duration::UnitUnknown:
	  {
	    log("unknown duration unit");

	    goto _return;
	  }
	}

      // convert back to the result.
      if (result.Set(r) == StatusError)
	goto _return;

    _return:
      release();

      return (result);
    }

    ///
    /// this operator substracts a duration to the current time.
    ///
    Time		Time::operator-(const Duration&		duration)
    {
      ::QDateTime	dt;
      ::QDateTime	r;
      Time		result;

      enter();

      // build dt.
      if (this->Get(dt) == StatusError)
	goto _return;

      // depending on the unit.
      switch (duration.unit)
	{
	case Duration::UnitMilliseconds:
	  {
	    // substract the value.
	    r = dt.addMSecs(-duration.value);

	    break;
	  }
	case Duration::UnitSeconds:
	  {
	    // substract the value.
	    r = dt.addSecs(-duration.value);

	    break;
	  }
	case Duration::UnitMinutes:
	  {
	    // substract the value.
	    r = dt.addSecs(-duration.value * 60);

	    break;
	  }
	case Duration::UnitHours:
	  {
	    // substract the value.
	    r = dt.addSecs(-duration.value * 60 * 60);

	    break;
	  }
	case Duration::UnitDays:
	  {
	    // substract the value.
	    r = dt.addDays(-duration.value);

	    break;
	  }
	case Duration::UnitMonths:
	  {
	    // substract the value.
	    r = dt.addMonths(-duration.value);

	    break;
	  }
	case Duration::UnitYears:
	  {
	    // substract the value.
	    r = dt.addYears(-duration.value);

	    break;
	  }
	case Duration::UnitUnknown:
	  {
	    log("unknown duration unit");

	    goto _return;
	  }
	}

      // convert back to the result.
      if (result.Set(r) == StatusError)
	goto _return;

    _return:
      release();

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

      std::cout << alignment << Dumpable::Shift << "[Millisecond] "
		<< std::nouppercase << std::dec	
		<< this->millisecond << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Second] "
		<< std::nouppercase << std::dec	
		<< this->second << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Minute] "
		<< std::nouppercase << std::dec
		<< this->minute << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Hour] "
		<< std::nouppercase << std::dec
		<< this->hour << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Day] "
		<< std::nouppercase << std::dec
		<< this->day << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Month] "
		<< std::nouppercase << std::dec
		<< this->month << std::endl;
      std::cout << alignment << Dumpable::Shift << "[Year] "
		<< std::nouppercase << std::dec
		<< this->year << std::endl;

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
      if (archive.Serialize(this->millisecond,
			    this->second,
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
      if (archive.Extract(this->millisecond,
			  this->second,
			  this->minute,
			  this->hour,
			  this->day,
			  this->month,
			  this->year) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
