#include <elle/utility/Time.hh>

#include <elle/log.hh>

#include <elle/utility/Duration.hh>

#include <ctime>

ELLE_LOG_COMPONENT("elle.utility.Time");

namespace elle
{
  namespace utility
  {
    /*---------------.
    | Static Methods |
    `---------------*/

    Time
    Time::current()
    {
      Time time;

      time.Current();

      return (time);
    }

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

    Time::~Time()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method fills the instance with the current time.
    ///
    Status              Time::Current()
    {
      auto now = std::chrono::system_clock::now();

      // gets the number of nanoseconds since Epoch UTC
       this->nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();

      ELLE_DEBUG("get current time: %s", this->nanoseconds);

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean             Time::operator==(const Time&            element) const
    {
      return (this->nanoseconds == element.nanoseconds);
    }

    ///
    /// this operator compares two times.
    ///
    Boolean             Time::operator<(const Time&             element) const
    {
      if (this->nanoseconds < element.nanoseconds)
        return true;

      return false;
    }

    ///
    /// this operator compares two times.
    ///
    Boolean             Time::operator>(const Time&             element) const
    {
      if (this->nanoseconds > element.nanoseconds)
        return true;

      return false;
    }

    ///
    /// this operator adds a time to the current one.
    ///
    Time                Time::operator+(const Time&             element)
    {
      Time              time;

      time.nanoseconds = this->nanoseconds + element.nanoseconds;

      return (time);
    }

    ///
    /// this operator substracts a time to the current one.
    ///
    Time                Time::operator-(const Time&             element)
    {
      Time              time;

      time.nanoseconds = this->nanoseconds - element.nanoseconds;

      return (time);
    }

    ///
    /// this operator adds a duration to the current time.
    ///
    Time                Time::operator+(const Duration&         duration)
    {
      Time              result(*this);

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
          goto _return;
        }

    _return:
      return (result);
    }

    ///
    /// this operator substracts a duration to the current time.
    ///
    Time                Time::operator-(const Duration&         duration)
    {
      Time              result(*this);

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
          break;
        }

      return (result);
    }

    Natural64
    Time::microseconds()
    {
      elle::unreachable();
    }

    Natural64
    Time::milliseconds()
    {
      elle::unreachable();
    }

    Natural64
    Time::seconds()
    {
      elle::unreachable();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an time object.
    ///
    Status              Time::Dump(Natural32                    margin) const
    {
      String            alignment(margin, ' ');
      ::tm*             tm;
      ::time_t          time;

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

      return Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Time::print(std::ostream& stream) const
    {
      ::tm* tm;
      ::time_t time;

      // Convert the nanoseconds in a time_t.
      time = this->nanoseconds / 1000000000;

      // Retrieve a _tm_ structure.
      tm = ::gmtime(&time);

      stream << std::dec
             << (1900 + tm->tm_year) << "-" << (1 + tm->tm_mon)
             << "-" << (1 + tm->tm_mday) << " "
             << tm->tm_hour << ":" << tm->tm_min << ":" << tm->tm_sec
             << "." << (this->nanoseconds % 1000);
    }
  }
}
