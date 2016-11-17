#include <boost/python.hpp>
// Retarded CPython defines these macros...
// http://bugs.python.org/issue10910
#ifdef tolower
# undef isalnum
# undef isalpha
# undef islower
# undef isspace
# undef isupper
# undef tolower
# undef toupper
#endif

#include <boost/function.hpp>

#include <reactor/exception.hh>
#include <reactor/python.hh>
#include <reactor/scheduler.hh>

using boost::python::class_;

// Pacify -Wmissing-declarations
extern "C"
{
  PyObject* PyInit_reactor();
}

/*-----------.
| Exceptions |
`-----------*/

static
PyObject* terminate_exception = nullptr;

static
void
translator(reactor::Terminate const& e)
{
  PyErr_SetString(terminate_exception, e.what());
}

/*-------.
| Thread |
`-------*/

// Shamelessly taken from the internet:
// http://misspent.wordpress.com/2009/10/11/boost-python-and-handling-python-exceptions/
static
void
wrap(reactor::Thread::Action const& action)
{
  try
  {
    action();
  }
  catch (const boost::python::error_already_set&)
  {
    auto python_exception = std::current_exception();
    using namespace boost::python;

    PyObject *e, *v, *t;
    PyErr_Fetch(&e, &v, &t);

    // A NULL e means that there is not available Python exception
    if (!e)
      return;

    // See if the exception was Terminate. If so, throw a C++ version of that
    // exception
    if (PyErr_GivenExceptionMatches(e, terminate_exception))
    {
      // We construct objects now since we plan to keep
      // ownership of the references.
      object e_obj(handle<>(allow_null(e)));
      object v_obj(handle<>(allow_null(v)));
      object t_obj(handle<>(allow_null(t)));

      throw reactor::Terminate("restored from python (XXX)");
    }
    // We didn't do anything with the Python exception, and we never took
    // ownership of the refs, so it's safe to simply pass them back to
    // PyErr_Restore
    PyErr_Restore(e, v, t);

    // Rethrow the exception.
    std::rethrow_exception(python_exception);
  }
}

namespace reactor
{
  PythonException::PythonException():
    elle::Exception("python error"), // XXX: pretty print the python value
    type(nullptr),
    value(nullptr),
    traceback(nullptr)
  {
    PyErr_Fetch(&this->type, &this->value, &this->traceback);
  }

  void
  PythonException::restore() const
  {
    PyErr_Restore(this->type, this->value, this->traceback);
    throw boost::python::error_already_set();
  }
}

class Thread:
  public reactor::Thread,
  public boost::python::wrapper<reactor::Thread>
{
public:
  Thread(PyObject* instance,
         reactor::Scheduler& s,
         std::string const& name,
         boost::function<void ()> const& action):
    reactor::Thread(s, name, [action] { wrap(action); }, false),
    _self(instance)
  {
    boost::python::incref(instance);
  }
  Thread(PyObject* instance,
         std::string const& name,
         boost::function<void ()> const& action):
    reactor::Thread(name, [action] { wrap(action); }, false),
    _self(instance)
  {
    boost::python::incref(instance);
  }
  virtual
  ~Thread()
  {}

  virtual
  void
  _scheduler_release() override
  {
    boost::python::decref(this->_self);
  }

protected:
  // FIXME: factor with parent method
  virtual
  void
  _action_wrapper(const Thread::Action& action) override
  {
    ELLE_LOG_COMPONENT("reactor.Thread");
    try
    {
      if (this->_exception)
      {
        ELLE_TRACE("%s: re-raise exception: %s",
                   *this, elle::exception_string(this->_exception));
        std::exception_ptr tmp = this->_exception;
        this->_exception = std::exception_ptr{};
        std::rethrow_exception(tmp);
      }
      action();
    }
    catch (reactor::Terminate const&)
    {}
    catch (boost::python::error_already_set const&)
    {
      ELLE_WARN("%s: python exception escaped", *this);
      _exception_thrown = std::make_exception_ptr(reactor::PythonException());
    }
    catch (elle::Exception const& e)
    {
      ELLE_WARN("%s: exception escaped: %s", *this, elle::exception_string())
      {
        ELLE_DUMP("exception type: %s", elle::demangle(typeid(e).name()));
        ELLE_DUMP("backtrace:\n%s", e.backtrace());
      }
      _exception_thrown = std::current_exception();
    }
    catch (...)
    {
      ELLE_WARN("%s: exception escaped: %s", *this, elle::exception_string());
      _exception_thrown = std::current_exception();
    }
  }

private:
  PyObject* _self;
};

// Shamelessly taken from the internet:
// http://stackoverflow.com/questions/9620268/boost-python-custom-exception-class
static
PyObject*
create_exception_class(const char* name,
                       PyObject* baseTypeObj = PyExc_Exception)
{
  namespace bp = boost::python;

  std::string scopeName =
    bp::extract<std::string>(bp::scope().attr("__name__"));
  std::string qualifiedName0 = scopeName + "." + name;
  char* qualifiedName1 = const_cast<char*>(qualifiedName0.c_str());

  PyObject* typeObj = PyErr_NewException(qualifiedName1, baseTypeObj, 0);
  if(!typeObj) bp::throw_error_already_set();
  bp::scope().attr(name) = bp::handle<>(bp::borrowed(typeObj));
  return typeObj;
}

static void wait_wrap(reactor::Thread* t)
{
  t->Waitable::wait();
}

class Scheduler:
  public reactor::Scheduler,
  public boost::python::wrapper<reactor::Scheduler>
{
public:
  typedef reactor::Scheduler Super;

private:
  virtual
  void
  _rethrow_exception(std::exception_ptr e) const override
  {
    try
    {
      std::rethrow_exception(_eptr);
    }
    catch (reactor::PythonException const& e)
    {
      e.restore();
    }
  }
};

BOOST_PYTHON_MODULE(reactor)
{
  terminate_exception = create_exception_class("Terminate");
  class_<Scheduler,
         boost::noncopyable>
    ("Scheduler", boost::python::init<>())
    .def("run", &reactor::Scheduler::run)
    ;
  class_<reactor::Thread,
         Thread,
         boost::noncopyable>
    ("Thread", boost::python::init<reactor::Scheduler&,
                                   std::string const&,
                                   boost::python::object>())
    .def(boost::python::init<std::string const&,
                                   boost::python::object>())
    .def("wait", &wait_wrap)
    ;
  boost::python::def("yield_", reactor::yield);
  boost::python::def("sleep",
                     static_cast<void (*)(reactor::Duration)>(reactor::sleep));
  boost::python::def("scheduler", &reactor::scheduler,
    boost::python::return_value_policy<boost::python::copy_non_const_reference>());

  boost::python::register_exception_translator<reactor::Terminate>(translator);
}
