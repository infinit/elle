// Python
#include <Python.h>
#include <datetime.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/python.hpp>

#include <elle/python/datetime-converter.hh>

// Time duration Python conversion shamelessly stolen from the internet:
// http://code.activestate.com/recipes/576395-convert-datetimedatetime-objects-tofrom-boostpytho/static
static
long
get_usecs(boost::posix_time::time_duration const& d)
{
  static long resolution
    = boost::posix_time::time_duration::ticks_per_second();
  long fracsecs = d.fractional_seconds();
  if (resolution > 1000000)
    return fracsecs / (resolution / 1000000);
  else
    return fracsecs * (1000000 / resolution);
}

/* Convert ptime to/from python */
struct ptime_to_python_datetime
{
  static PyObject* convert(boost::posix_time::ptime const& pt)
  {
    boost::gregorian::date date = pt.date();
    boost::posix_time::time_duration td = pt.time_of_day();
    return PyDateTime_FromDateAndTime((int)date.year(),
                                      (int)date.month(),
                                      (int)date.day(),
                                      td.hours(),
                                      td.minutes(),
                                      td.seconds(),
                                      get_usecs(td));
  }
};

struct ptime_from_python_datetime
{
  ptime_from_python_datetime()
  {
    boost::python::converter::registry::push_back(
      &convertible,
      &construct,
      boost::python::type_id<boost::posix_time::ptime > ());
  }

  static void* convertible(PyObject * obj_ptr)
  {
    if ( ! PyDateTime_Check(obj_ptr))
      return 0;
    return obj_ptr;
  }

  static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data * data)
  {
    PyDateTime_DateTime const* pydate
      = reinterpret_cast<PyDateTime_DateTime*>(obj_ptr);

    // Create date object
    boost::gregorian::date _date(PyDateTime_GET_YEAR(pydate),
                                 PyDateTime_GET_MONTH(pydate),
                                 PyDateTime_GET_DAY(pydate));

    // Create time duration object
    boost::posix_time::time_duration
      _duration(PyDateTime_DATE_GET_HOUR(pydate),
                PyDateTime_DATE_GET_MINUTE(pydate),
                PyDateTime_DATE_GET_SECOND(pydate),
                0);
    // Set the usecs value
    _duration += boost::posix_time::microseconds(PyDateTime_DATE_GET_MICROSECOND(pydate));

    // Create posix time object
    void* storage = (
      (boost::python::converter::rvalue_from_python_storage<boost::posix_time::ptime>*)
      data)->storage.bytes;
    new (storage)
      boost::posix_time::ptime(_date, _duration);
    data->convertible = storage;
  }
};

/* Convert time_duration to/from python */
struct tduration_to_python_delta
{
  static PyObject* convert(boost::posix_time::time_duration d)
  {
    long days = d.hours() / 24;
    if (days < 0)
      days --;
    long seconds = d.total_seconds() - days*(24*3600);
    long usecs = get_usecs(d);
    if (days < 0)
      usecs = 1000000-1 - usecs;
    return PyDelta_FromDSU(days, seconds, usecs);
  }
};


/* Should support the negative values, but not the special boost time
   durations */
struct tduration_from_python_delta
{
  tduration_from_python_delta()
  {
    boost::python::converter::registry::push_back(
      &convertible,
      &construct,
      boost::python::type_id<boost::posix_time::time_duration>());
  }

  static void* convertible(PyObject * obj_ptr)
  {
    if ( ! PyDelta_Check(obj_ptr))
      return 0;
    return obj_ptr;
  }

  static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data * data)
  {
    PyDateTime_Delta const* pydelta
      = reinterpret_cast<PyDateTime_Delta*>(obj_ptr);

    long days = pydelta->days;
    bool is_negative = (days < 0);
    if (is_negative)
      days = -days;

    // Create time duration object
    boost::posix_time::time_duration
      duration = boost::posix_time::hours(24)*days
      + boost::posix_time::seconds(pydelta->seconds)
      + boost::posix_time::microseconds(pydelta->microseconds);
    if (is_negative)
      duration = duration.invert_sign();

    void* storage = (
      (boost::python::converter::rvalue_from_python_storage<boost::posix_time::time_duration>*)
      data)->storage.bytes;
    new (storage)
      boost::posix_time::time_duration(duration);
    data->convertible = storage;
  }
};

namespace elle
{
  namespace python
  {
    DatetimeConverter::DatetimeConverter()
    {
      PyDateTime_IMPORT;
      ptime_from_python_datetime();
      boost::python::to_python_converter<
        const boost::posix_time::ptime
        , ptime_to_python_datetime
        >();
      tduration_from_python_delta();
      boost::python::to_python_converter<
        const boost::posix_time::time_duration
        , tduration_to_python_delta
        >();
    }

    elle::Plugin<ConverterPlugin>::Register<DatetimeConverter>
    register_datetime_plugin;
    elle::Plugin<ConverterPlugin> datetime_converter(register_datetime_plugin);
  }
}
