#ifdef INFINIT_WINDOWS // Fix missing ::hypot.
# include <cmath>
#endif

#include <boost/python.hpp>

#include <elle/log/Logger.hh>
#include <elle/log/Send.hh>
#include <elle/log/SysLogger.hh>
#include <elle/log/TextLogger.hh>
#include <elle/memory.hh>

using boost::python::class_;
using boost::python::def;
using boost::python::enum_;

// Pacify -Wmissing-declarations
extern "C"
{
  PyObject* PyInit__log();
}

class TextLogger: public elle::log::TextLogger
{
public:
  TextLogger():
    elle::log::TextLogger(std::cerr)
  {}
};

static
void
set_logger(boost::python::object& object)
{
  elle::generic_unique_ptr<elle::log::Logger> logger(
    boost::python::extract<elle::log::Logger*>(object),
    [] (elle::log::Logger*)
    {
      /* The logger will be deleted by python itself upon exit */
    });
  boost::python::incref(object.ptr());
  elle::log::logger(std::move(logger));
}

class Send: public elle::log::detail::Send
{
public:
  Send(elle::log::Logger::Level level,
       elle::log::Logger::Type type,
       std::string const& component,
       std::string const& file,
       unsigned int line,
       std::string const& function,
       std::string const& message):
    elle::log::detail::Send(
      level,
      type,
      false,
      component,
      file.c_str(),
      line,
      function.c_str(),
      boost::algorithm::replace_all_copy(message, "%", "%%").c_str())
  {}

  void
  enter()
  {
    this->_indent();
  }

  void
  exit(boost::python::object const&,
       boost::python::object const&,
       boost::python::object const&)
  {
    this->_unindent();
  }
};

BOOST_PYTHON_MODULE(_log)
{
  enum_<elle::log::Logger::Level>("level")
    .value("none", elle::log::Logger::Level::none)
    .value("log", elle::log::Logger::Level::log)
    .value("trace", elle::log::Logger::Level::trace)
    .value("debug", elle::log::Logger::Level::debug)
    .value("dump", elle::log::Logger::Level::dump)
    ;
  enum_<elle::log::Logger::Type>("type")
    .value("info", elle::log::Logger::Type::info)
    .value("warning", elle::log::Logger::Type::warning)
    .value("error", elle::log::Logger::Type::error)
    ;
  class_<elle::log::Logger,
         boost::noncopyable>
    ("Logger", boost::python::no_init)
    //.def("message", &elle::log::Logger::message)
    ;
  class_<TextLogger,
         boost::python::bases<elle::log::Logger>,
         boost::noncopyable>
    ("TextLogger", boost::python::init<>())
    ;
  class_<elle::log::SysLogger,
         boost::python::bases<elle::log::Logger>,
         boost::noncopyable>
    ("SysLogger", boost::python::init<std::string const&>())
    ;
  def("set_logger", &set_logger);
  class_<Send,
         boost::noncopyable>
    ("Send", boost::python::init<
     elle::log::Logger::Level,
     elle::log::Logger::Type,
     std::string const&,
     std::string const&,
     unsigned int,
     std::string const&,
     std::string const&>())
    .def("__enter__", &Send::enter)
    .def("__exit__", &Send::exit)
    ;
}
