//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat aug 22 00:03:52 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Time.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>

# include <pthread.h>
# include <ctime>

#include <elle/idiom/Open.hh>

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
      nanoseconds(0)
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
      enter();

      // gets the number of nanoseconds since Epoch UTC
      this->nanoseconds = ::QDateTime::currentMSecsSinceEpoch() * 1000000;

      leave();
    }

    ///
    /// this method converts a time object into a time_t structure.
    ///
    Status		Time::Get(::time_t&			time) const
    {
      enter();

      // set the time i.e in seconds.
      time = this->nanoseconds / 1000000000;

      leave();
    }

    ///
    /// this method converts a time_t into a time object.
    ///
    Status		Time::Set(const ::time_t&		time)
    {
      enter();

      this->nanoseconds = time * 1000000000;

      leave();
    }

    ///
    /// this method converts a time object into a QDateTime structure.
    ///
    Status		Time::Get(::QDateTime&			dt) const
    {
      enter();

      // set the date/time.
      dt.setMSecsSinceEpoch(this->nanoseconds / 1000000);

      leave();
    }

    ///
    /// this method converts a QDateTime into a time object.
    ///
    Status		Time::Set(const ::QDateTime&		dt)
    {
      enter();

      // set the attributes.
      this->nanoseconds = dt.toMSecsSinceEpoch() * 1000000;

      leave();
    }

#if defined(INFINIT_WIN32)
    ///
    /// This method converts a FILETIME into a Time object.
    ///
    Status              Time::Get(::FILETIME&			ft) const
    {
      enter();

      ULARGE_INTEGER    value;

      // quad part is in 100ns, since 1601-01-01 ... fuck ms
      value.QuadPart = this->nanoseconds / 100
        + ((369ULL * 365 + 89) * 24 * 60 * 60 * 1000 * 1000 * 10);
      //   y         d     bis   h    m    s    ms     us     100ns

      ft.dwLowDateTime  = value.LowPart;
      ft.dwHighDateTime = value.HighPart;

      leave();
    }

    ///
    /// This method converts a FILETIME into a Time object.
    ///
    Status              Time::Set(const ::FILETIME&		ft)
    {
      enter();

      ULARGE_INTEGER    value;

      value.LowPart = ft.dwLowDateTime;
      value.HighPart = ft.dwHighDateTime;

      this->nanoseconds = value.QuadPart * 100;

      leave();
    }
#endif

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Time::operator==(const Time&		element) const
    {
      enter();

      if (this->nanoseconds != element.nanoseconds)
        false();

      true();
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		Time::operator<(const Time&		element) const
    {
      enter();

      if (this->nanoseconds < element.nanoseconds)
	true();

      false();
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		Time::operator>(const Time&		element) const
    {
      enter();

      if (this->nanoseconds > element.nanoseconds)
	true();

      false();
    }

    ///
    /// this operator adds a time to the current one.
    ///
    Time		Time::operator+(const Time&		element)
    {
      Time              time;

      time.nanoseconds = this->nanoseconds + element.nanoseconds;

      return (time);
    }

    ///
    /// this operator substracts a time to the current one.
    ///
    Time		Time::operator-(const Time&		element)
    {
      Time              time;

      time.nanoseconds = this->nanoseconds - element.nanoseconds;

      return (time);
    }

    ///
    /// this operator adds a duration to the current time.
    ///
    Time		Time::operator+(const Duration&		duration)
    {
      Time              result(*this);

      enter();

      // depending on the unit.
      switch (duration.unit)
	{
	case Duration::UnitNanoseconds:
	  {
	    // add the value.
	    result.nanoseconds += duration.value;

	    break;
	  }
	case Duration::UnitMicroseconds:
	  {
	    // add the value.
	    result.nanoseconds += duration.value * 1000;

	    break;
	  }
	case Duration::UnitMilliseconds:
	  {
	    // add the value.
            result.nanoseconds += duration.value * 1000000;

	    break;
	  }
	case Duration::UnitSeconds:
	  {
	    // add the value.
            result.nanoseconds += duration.value * 1000000000LU;

	    break;
	  }
	case Duration::UnitMinutes:
	  {
	    // add the value.
            result.nanoseconds += duration.value * 1000000000LU * 60;

	    break;
	  }
	case Duration::UnitUnknown:
	  {
	    log("unknown duration unit");

	    goto _return;
	  }
	}

    _return:
      // release.
      release();

      return (result);
    }

    ///
    /// this operator substracts a duration to the current time.
    ///
    Time		Time::operator-(const Duration&		duration)
    {
      Time		result(*this);

      enter();

      // depending on the unit.
      switch (duration.unit)
	{
	case Duration::UnitNanoseconds:
	  {
	    // add the value.
            result.nanoseconds -= duration.value;

	    break;
	  }
	case Duration::UnitMicroseconds:
	  {
	    // add the value.
            result.nanoseconds -= duration.value * 1000;

	    break;
	  }
	case Duration::UnitMilliseconds:
	  {
	    // add the value.
            result.nanoseconds -= duration.value * 1000000;

	    break;
	  }
	case Duration::UnitSeconds:
	  {
	    // add the value.
            result.nanoseconds -= duration.value * 1000000000L;

	    break;
	  }
	case Duration::UnitMinutes:
	  {
	    // add the value.
            result.nanoseconds -= duration.value * 1000000000 * 60;

	    break;
	  }
	case Duration::UnitUnknown:
	  {
	    log("unknown duration unit");

	    goto _return;
	  }
	}

    _return:

      // release.
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
      ::tm*		tm;
      ::time_t		time;

      enter();

      // convert the nanoseconds in a time_t.
      time = this->nanoseconds / 1000000000;

      // retrieve a _tm_ structure.
      tm = ::gmtime(&time);

      // display the time.
      std::cout << alignment << "[Time] "
                << std::dec
		<< (1900 + tm->tm_year) << "-" << (1 + tm->tm_mon)
                << "-" << (1 + tm->tm_mday) << " "
                << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec
                << "." << (this->nanoseconds % 1000)
                << std::endl;

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
      if (archive.Serialize(this->nanoseconds) == StatusError)
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
      if (archive.Extract(this->nanoseconds) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
