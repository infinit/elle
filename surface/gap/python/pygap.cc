
#include <surface/gap/python/python.hh>

#include <surface/gap/gap.h>

static boost::python::object
_get_networks(gap_State* state)
{
  boost::python::list networks;

  return networks;
}

BOOST_PYTHON_MODULE(_gap)
{
  namespace py = boost::python;

  py::enum_<gap_Status>("Status")
    .value("gap_ok", gap_ok)
    .value("gap_error", gap_error)
    .value("gap_network_error", gap_network_error)
    .value("gap_internal_error", gap_internal_error)
    .value("gap_api_error", gap_api_error)
    .export_values()
  ;
  py::def("new", &gap_new,
          py::return_value_policy<py::return_opaque_pointer>());

  py::def("free", &gap_free);

  py::def("hash_password", &gap_hash_password,
          py::return_value_policy<py::return_opaque_pointer>());

  py::def("hash_free", &gap_hash_free);

  py::def("meta_status", &gap_meta_status);

  py::def("refresh_networks", &gap_refresh_networks);

  py::def("path_to_network", &gap_path_to_network);

  py::def("login", &gap_login);

  py::def("register", &gap_register);

  py::def("set_device_name", &gap_set_device_name);

  py::def("get_networks", &_get_networks);
}

