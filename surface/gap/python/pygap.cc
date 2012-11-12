
#include <surface/gap/python/python.hh>

#include <surface/gap/gap.h>

#include <surface/gap/State.hh>

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

static std::string
_hash_password(gap_State* state, std::string email, std::string password)
{
  assert(state != nullptr);

  char* hash = gap_hash_password(state, email.c_str(), password.c_str());

  if (hash == nullptr)
    throw std::runtime_error("Couldn't hash the password");

  std::string res(hash);
  gap_hash_free(hash);
  return res;
}

static
gap_Status
_send_files(gap_State* state,
            std::string const& recipient,
            boost::python::list const& files)
{
  boost::python::ssize_t len = boost::python::len(files);
  char const** list = (char const**) calloc(sizeof(char*), (len + 1));

  if (list == nullptr)
    throw std::bad_alloc();

  for (int i = 0; i < len; ++i)
    {
      list[i] = boost::python::extract<char const*>(files[i]);
    }

  gap_Status res = gap_send_files(state,
                                  recipient.c_str(),
                                  list);

  free(list);

  return res;
}


namespace
{
  template<typename T>
  struct wrap
  {
    boost::python::object o;
    void operator ()(T const* b)
    {
      this->o(boost::python::ptr(b));
    }
  };

  template<typename T>
  void
  _gap_set_callback(gap_State* state,
                    boost::python::object obj)
  {
    assert(state != nullptr);

    reinterpret_cast<surface::gap::State*>(state)->attach_callback<T>(wrap<T>{obj});
  }
}

BOOST_PYTHON_MODULE(_gap)
{
  namespace py = boost::python;
  typedef py::return_value_policy<py::return_by_value> by_value;

  py::enum_<gap_Status>("Status")
    .value("gap_ok", gap_ok)
    .value("gap_error", gap_error)
    .value("gap_network_error", gap_network_error)
    .value("gap_internal_error", gap_internal_error)
    .value("gap_api_error", gap_api_error)
    .value("gap_no_device_error", gap_no_device_error)
    .value("gap_not_logged_in", gap_not_logged_in)
    .value("gap_bad_request", gap_bad_request)
    .value("gap_already_logged_in", gap_already_logged_in)
    .value("gap_not_logged_in", gap_not_logged_in)
    .value("gap_email_not_valid", gap_email_not_valid)
    .value("gap_handle_not_valid", gap_handle_not_valid)
    .value("gap_device_not_valid", gap_device_not_valid)
    .value("gap_password_not_valid", gap_password_not_valid)
    .value("gap_user_id_not_valid", gap_user_id_not_valid)
    .value("gap_network_id_not_valid", gap_network_id_not_valid)
    .value("gap_device_id_not_valid", gap_device_id_not_valid)
    .value("gap_field_is_empty", gap_field_is_empty)
    .value("gap_activation_code_not_valid", gap_activation_code_not_valid)
    .value("gap_deprecated", gap_deprecated)
    .value("gap_email_already_registred", gap_email_already_registred)
    .value("gap_handle_already_registred", gap_handle_already_registred)
    .value("gap_device_already_registred", gap_device_already_registred)
    .value("gap_activation_code_doesnt_exist", gap_activation_code_doesnt_exist)
    .value("gap_email_password_dont_match", gap_email_password_dont_match)
    .value("gap_unknown_user", gap_unknown_user)
    .value("gap_user_already_in_network", gap_user_already_in_network)
    .value("gap_network_not_found", gap_network_not_found)
    .value("gap_device_not_found", gap_device_not_found)
    .value("gap_device_not_in_network", gap_device_not_in_network)
    .value("gap_root_block_already_exist", gap_root_block_already_exist)
    .value("gap_root_block_badly_signed", gap_root_block_badly_signed)
    .value("gap_user_already_invited", gap_user_already_invited)
    .value("gap_user_already_in_infinit", gap_user_already_in_infinit)
    .value("gap_file_name_empty", gap_file_name_empty)
    .value("gap_unknown", gap_unknown)
    .export_values()
  ;

  //- gap ctor and dtor -------------------------------------------------------

  py::def("new",
          &gap_new,
          py::return_value_policy<py::return_opaque_pointer>());
  py::def("free", &gap_free);

  py::def("enable_debug", &gap_enable_debug);

  //- Authentication and registration -----------------------------------------

  py::def("hash_password", &_hash_password, by_value());
  py::def("login", &gap_login);
  py::def("logout", &gap_logout);
  py::def("register", &gap_register);

  py::def("invite_user", &gap_invite_user);
  py::def("send_message", &gap_message);
  py::def("send_files", &_send_files);
  py::def("update_transaction", &gap_update_transaction);
  py::def("start_transaction", &gap_start_transaction);
  py::def("connect", &gap_trophonius_connect);
  py::def("get_notifications", &gap_meta_pull_notification);
  py::def("notifications_red", &gap_meta_notifications_red);
  py::def("scratch_db", &gap_debug);

  py::def("poll", &gap_poll);

  //- Notifications ------------------------------------------------------------

  ////////////////////////////////
  // User status.
  py::class_<gap_UserStatusNotification, boost::noncopyable>("UserStatus", py::no_init)
    .def_readonly("sender_id", &gap_UserStatusNotification::sender_id)
    .def_readonly("status", &gap_UserStatusNotification::status)
  ;
  py::def(
    "OnBite",
    &_gap_set_callback<gap_BiteNotification>
  );

  ////////////////////////////////
  // File transfer request.
  py::class_<gap_FileTransferRequestNotification, boost::noncopyable>("FileTransferRequest", py::no_init)
    .def_readonly("sender_id", &gap_FileTransferRequestNotification::sender_id)
    .def_readonly("transaction_id", &gap_FileTransferRequestNotification::transaction_id)
    .def_readonly("file_name", &gap_FileTransferRequestNotification::file_name)
    .def_readonly("file_size", &gap_FileTransferRequestNotification::file_size)
    .def_readonly("new", &gap_FileTransferRequestNotification::is_new)
  ;
  py::def(
    "OnFileTransfer",
    &_gap_set_callback<gap_FileTransferRequestNotification>
  );

  ////////////////////////////////
  // File transfer status.
  py::class_<gap_FileTransferStatusNotification, boost::noncopyable>("FileTransferStatus", py::no_init)
    .def_readonly("network_id", &gap_FileTransferStatusNotification::network_id)
    .def_readonly("status", &gap_FileTransferStatusNotification::status)
    .def_readonly("new", &gap_FileTransferStatusNotification::is_new)
  ;

  py::def(
    "OnFileTransferStatus",
    &_gap_set_callback<gap_FileTransferStatusNotification>
  );

  ////////////////////////////////
  // Message.
  py::class_<gap_MessageNotification, boost::noncopyable>("Message", py::no_init)
    .def_readonly("sender_id", &gap_MessageNotification::sender_id)
    .def_readonly("message", &gap_MessageNotification::message)
    .def_readonly("new", &gap_MessageNotification::is_new)
  ;

  py::def(
    "OnMessage",
    &_gap_set_callback<gap_MessageNotification>
    );


  py::class_<gap_BiteNotification, boost::noncopyable>("Bite", py::no_init)
    .def_readonly("debug", &gap_BiteNotification::debug)
    .def_readonly("new", &gap_BiteNotification::is_new)
  ;
  py::def(
    "OnBite",
    &_gap_set_callback<gap_BiteNotification>
  );


   //- Infinit services status -------------------------------------------------

  py::def("meta_status", &gap_meta_status);

  //- Device ------------------------------------------------------------------

  py::def("device_status", &gap_device_status);
  py::def("set_device_name", &gap_set_device_name);

  //- Network -----------------------------------------------------------------

  py::def("networks", &_get_networks);
  py::def("create_network", &gap_create_network);
  py::def("network_name", &gap_network_name, by_value());
  py::def("network_mount_point", &gap_network_mount_point, by_value());
  py::def("network_add_user", &gap_network_add_user);

  //- Users -------------------------------------------------------------------

  py::class_<gap_User, boost::noncopyable>("User")
    .def_readonly("_id", &gap_User::_id)
    .def_readonly("fullname", &gap_User::fullname)
    .def_readonly("email", &gap_User::email)
    .def_readonly("public_key", &gap_User::public_key)
  ;

  py::def("user_fullname", &gap_user_fullname, by_value());
  py::def("user_email", &gap_user_email, by_value());

  //- Watchdog ----------------------------------------------------------------

  py::def("launch_watchdog", &gap_launch_watchdog);
  py::def("refresh_networks", &gap_refresh_networks);
  py::def("stop_watchdog", &gap_stop_watchdog);

  //- Permissions -------------------------------------------------------------

  py::enum_<gap_Permission>("Permission")
    .value("gap_read", gap_read)
    .value("gap_write", gap_write)
    .value("gap_exec", gap_exec)
    .export_values()
  ;

  py::def("set_permissions", &gap_set_permissions);

}
