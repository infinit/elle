#include <boost/function.hpp>
#include <boost/python.hpp>

#include <reactor/exception.hh>
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
    throw;
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

  virtual
  ~Thread()
  {}

  virtual
  void
  _scheduler_release() override
  {
    boost::python::decref(this->_self);
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

BOOST_PYTHON_MODULE(reactor)
{
  terminate_exception = create_exception_class("Terminate");
  class_<reactor::Scheduler,
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
    .def("wait", &wait_wrap)
    ;
  boost::python::def("yield_", reactor::yield);

  boost::python::register_exception_translator<reactor::Terminate>(translator);
}
