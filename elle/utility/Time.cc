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
      millisecond(0)
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

      // gets the number of milliseconds since Epoch UTC
      this->millisecond = ::QDateTime::currentMSecsSinceEpoch();

      leave();
    }

    ///
    /// this method converts a time object into a time_t structure.
    ///
    Status		Time::Get(::time_t&			time) const
    {
      enter();

      time = this->millisecond / 1000;

      leave();
    }

    ///
    /// this method converts a time object into a QDateTime structure.
    ///
    Status		Time::Get(::QDateTime&			dt) const
    {
      enter();

      dt.setMSecsSinceEpoch(this->millisecond);

      leave();
    }

#if INFINIT_WIN32
    ///
    /// This method converts a FILETIME into a Time object.
    ///
    Status              Time::Get(::FILETIME&			ft) const
    {
      enter();

      uint64_t time = this->millisecond * 10;

      ft.dwLowDateTime  = time & 0xffffffff;
      ft.dwHighDateTime = time >> 32;

      leave();
    }
#endif

    ///
    /// this method converts a time_t into a time object.
    ///
    Status		Time::Set(const ::time_t&		time)
    {
      enter();

      this->millisecond = time * 1000;

      leave();
    }

    ///
    /// this method converts a QDateTime into a time object.
    ///
    Status		Time::Set(const ::QDateTime&		dt)
    {
      enter();

      // set the attributes.
      this->millisecond = dt.toMSecsSinceEpoch();

      leave();
    }

#if INFINIT_WIN32
    ///
    /// This method converts a FILETIME into a Time object.
    ///
    Status              Time::Set(const ::FILETIME&		ft)
    {
      enter();

      uint64_t time = ft.dwLowDateTime |
        (static_cast<uint64_t> (ft.dwHighDateTime) << 32);
      this->millisecond = time / 1000;

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

      if (this->millisecond != element.millisecond)
        false();

      true();
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		Time::operator<(const Time&		element) const
    {
      enter();

      if (this->millisecond < element.millisecond)
	true();

      false();
    }

    ///
    /// this operator compares two times.
    ///
    Boolean		Time::operator>(const Time&		element) const
    {
      enter();

      if (this->millisecond < element.millisecond)
	true();

      false();
    }

    ///
    /// this operator adds a time to the current one.
    ///
    Time		Time::operator+(const Time&		element)
    {
      Time              time;

      time.millisecond = this->millisecond + element.millisecond;
      return (time);
    }

    ///
    /// this operator substracts a time to the current one.
    ///
    Time		Time::operator-(const Time&		element)
    {
      Time              time;

      time.millisecond = this->millisecond - element.millisecond;
      return (time);
    }

    ///
    /// this operator adds a duration to the current time.
    ///
    Time		Time::operator+(const Duration&		duration)
    {
      Time              result(*this);

      // depending on the unit.
      switch (duration.unit)
	{
	case Duration::UnitMilliseconds:
	  {
	    // add the value.
            result.millisecond += duration.value;

	    break;
	  }
	case Duration::UnitSeconds:
	  {
	    // add the value.
            result.millisecond += duration.value * 1000;

	    break;
	  }
	case Duration::UnitMinutes:
	  {
	    // add the value.
            result.millisecond += duration.value * 1000 * 60;

	    break;
	  }
	case Duration::UnitUnknown:
	  {
	    log("unknown duration unit");

	    return result;
	  }
	}

      return (result);
    }

    ///
    /// this operator substracts a duration to the current time.
    ///
    Time		Time::operator-(const Duration&		duration)
    {
      Time		result(*this);

      // depending on the unit.
      switch (duration.unit)
	{
	case Duration::UnitMilliseconds:
	  {
	    // add the value.
            result.millisecond -= duration.value;

	    break;
	  }
	case Duration::UnitSeconds:
	  {
	    // add the value.
            result.millisecond -= duration.value * 1000;

	    break;
	  }
	case Duration::UnitMinutes:
	  {
	    // add the value.
            result.millisecond -= duration.value * 1000 * 60;

	    break;
	  }
	case Duration::UnitUnknown:
	  {
	    log("unknown duration unit");

	    return result;
	  }
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
      ::tm *            tm;
      ::time_t          time;

      enter();

      time = this->millisecond / 1000;

      tm = ::gmtime(&time);

      std::cout << alignment << "[Time] "
                << (1900 + tm->tm_year) << "-" << (1 + tm->tm_mon)
                << "-" << (1 + tm->tm_mday) << " "
                << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec
                << "." << (this->millisecond % 1000)
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
      if (archive.Serialize(this->millisecond) == StatusError)
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
      if (archive.Extract(this->millisecond) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
