#ifndef ELLE_PYTHON_GIL_HH
# define ELLE_PYTHON_GIL_HH

#include <boost/python.hpp>

/* boost::python call policy that release/acquires the gil
* (global interpreter lock).
* Pass as third argument to def, or to init's [] operator:
* - def("foo", &foo, release_gil_call_policies())
* - init<...>()[release_gil_call_policies]
*/

namespace elle
{
  namespace python
  {
    namespace
    {
      static std::vector<PyThreadState*> state;
    }
    struct release_gil_call_policies: public boost::python::default_call_policies
    {
      template <class ArgumentPackage>
      static bool precall(ArgumentPackage const& arg)
      {
        state.push_back(PyEval_SaveThread());
        return boost::python::default_call_policies::precall(arg);
      }
      template <class ArgumentPackage>
      static PyObject* postcall(ArgumentPackage const& args, PyObject* result)
      {
        PyEval_RestoreThread(state.back());
        state.pop_back();
        return boost::python::default_call_policies::postcall(args, result);
      }
    };
}}

#endif
