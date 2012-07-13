
#include <surface/gap/python/python.hh>

#include <surface/gap/gap.h>

static boost::python::object
_get_networks(gap_State* state)
{
  boost::python::list networks_;
  char** networks = gap_networks(state);
  if (networks != nullptr)
    {
      for (char** ptr = networks; *ptr != nullptr; ++ptr)
        {
          networks_.append(boost::python::str(std::string(*ptr)));
        }
        gap_networks_free(networks);
    }
  return networks_;
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
    .value("gap_no_device_error", gap_no_device_error)
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

  py::def("login", &gap_login);

  py::def("register", &gap_register);

  py::def("device_status", &gap_device_status);

  py::def("set_device_name", &gap_set_device_name);

  py::def("networks", &_get_networks);

  py::def("create_network", &gap_create_network);

  py::def("launch_watchdog", &gap_launch_watchdog);

  py::def("stop_watchdog", &gap_stop_watchdog);

  py::enum_<gap_Permission>("Permission")
    .value("gap_read", gap_read)
    .value("gap_write", gap_write)
    .value("gap_exec", gap_exec)
    .export_values()
  ;

  py::def("set_permissions", &gap_set_permissions);
}

