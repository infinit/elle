#pragma once

#include <elle/Exception.hh>

namespace elle
{
  namespace reactor
  {
    class PythonException
      : public elle::Exception
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
}
