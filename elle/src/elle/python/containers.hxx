#ifndef ELLE_PYTHON_CONTAINERS_HXX
# define ELLE_PYTHON_CONTAINERS_HXX

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace elle
{
  namespace python
  {
    template<typename C>
    struct container_from_python_impl
    {
      static void* convertible(PyObject* obj_ptr)
      {
        // weak check
        if (!PyList_Check(obj_ptr))
          return nullptr;
        return obj_ptr;
      }
      static void construct(
        PyObject* obj_ptr,
        boost::python::converter::rvalue_from_python_stage1_data* data)
      {
        void* storage = (
          (boost::python::converter::rvalue_from_python_storage<C>*)
          data)->storage.bytes;
        C* v = new (storage) C();
        Py_ssize_t len = PyList_Size(obj_ptr);

        for (Py_ssize_t i = 0; i < len; i++)
        {
          PyObject* current = PyList_GetItem(obj_ptr, i);
          v->push_back(boost::python::extract<typename C::value_type>(current));
        }

        data->convertible = storage;
        // fill it
      }
      container_from_python_impl()
      {
        boost::python::converter::registry::push_back(
          &convertible,
          &construct,
          boost::python::type_id<C>());
      }
    };

    template<typename C>
    void
    container_from_python()
    {
      container_from_python_impl<C>();
    }

    template<typename C>
    void
    container_to_python()
    {
      // We need a python name, which is never used explicitly.
      std::string name = std::string("convert_helper_")
        + typeid(C).name();
      boost::python::class_<std::vector<unsigned int>>(name.c_str())
      .def(boost::python::vector_indexing_suite<std::vector<unsigned int>>() );
    }

    template<typename C>
    void
    container()
    {
      container_from_python<C>();
      container_to_python<C>();
    }
  }


}

#endif
