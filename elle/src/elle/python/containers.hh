#ifndef ELLE_PYTHON_CONTAINERS_HH
# define ELLE_PYTHON_CONTAINERS_HH

namespace elle
{
  namespace python
  {
    /** Register conversion from python list to a container C that supports
    * push_back.
    */
    template<typename C>
    void
    container_from_python();
    /// Register conversion from container C to a python list-like type.
    template<typename C>
    void
    container_to_python();
    /// Register conversion both ways for given container type.
    template<typename C>
    void
    container();
  }
}

#include <elle/python/containers.hxx>

#endif
