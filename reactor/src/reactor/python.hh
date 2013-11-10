#ifndef REACTOR_PYTHON_HH
# define REACTOR_PYTHON_HH

# include <elle/Exception.hh>

namespace reactor
{
  class PythonException:
    public elle::Exception
  {
  public:
    PythonException();

    void
    restore() const;

    PyObject* type;
    PyObject* value;
    PyObject* traceback;
  };
}

#endif
