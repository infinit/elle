
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

    reinterpret_cast<surface::gap::State*>(state)->attach_callback(
      std::function<void (T const*)>(wrap<T>{obj})
    );
  }
}

BOOST_PYTHON_MODULE(_gap)
{
  namespace py = boost::python;
  typedef py::return_value_policy<py::return_by_value> by_value;

  //////////////////////////
  // value MUST be gap_EnumName_name
  py::enum_<gap_Status>("Status")
    .value("gap_Status_ok", gap_ok)
    .value("gap_Status_error", gap_error)
    .value("gap_Status_network_error", gap_network_error)
    .value("gap_Status_internal_error", gap_internal_error)
    .value("gap_Status_api_error", gap_api_error)
    .value("gap_Status_no_device_error", gap_no_device_error)
    .value("gap_Status_not_logged_in", gap_not_logged_in)
    .value("gap_Status_bad_request", gap_bad_request)
    .value("gap_Status_already_logged_in", gap_already_logged_in)
    .value("gap_Status_not_logged_in", gap_not_logged_in)
    .value("gap_Status_email_not_valid", gap_email_not_valid)
    .value("gap_Status_handle_not_valid", gap_handle_not_valid)
    .value("gap_Status_device_not_valid", gap_device_not_valid)
    .value("gap_Status_password_not_valid", gap_password_not_valid)
    .value("gap_Status_user_id_not_valid", gap_user_id_not_valid)
    .value("gap_Status_network_id_not_valid", gap_network_id_not_valid)
    .value("gap_Status_device_id_not_valid", gap_device_id_not_valid)
    .value("gap_Status_field_is_empty", gap_field_is_empty)
    .value("gap_Status_activation_code_not_valid", gap_activation_code_not_valid)
    .value("gap_Status_deprecated", gap_deprecated)
    .value("gap_Status_email_already_registred", gap_email_already_registred)
    .value("gap_Status_handle_already_registred", gap_handle_already_registred)
    .value("gap_Status_device_already_registred", gap_device_already_registred)
    .value("gap_Status_activation_code_doesnt_exist", gap_activation_code_doesnt_exist)
    .value("gap_Status_email_password_dont_match", gap_email_password_dont_match)
    .value("gap_Status_unknown_user", gap_unknown_user)
    .value("gap_Status_user_already_in_network", gap_user_already_in_network)
    .value("gap_Status_network_not_found", gap_network_not_found)
    .value("gap_Status_device_not_found", gap_device_not_found)
    .value("gap_Status_device_not_in_network", gap_device_not_in_network)
    .value("gap_Status_root_block_already_exist", gap_root_block_already_exist)
    .value("gap_Status_root_block_badly_signed", gap_root_block_badly_signed)
    .value("gap_Status_user_already_invited", gap_user_already_invited)
    .value("gap_Status_user_already_in_infinit", gap_user_already_in_infinit)
    .value("gap_Status_file_name_empty", gap_file_name_empty)
    .value("gap_Status_unknown", gap_unknown)
    .export_values()
  ;

  py::enum_<gap_TransactionStatus>("TransactionStatus")
    .value("gap_TransactionStatus_none", gap_transaction_status_none)
    .value("gap_TransactionStatus_pending", gap_transaction_status_pending)
    .value("gap_TransactionStatus_rejected", gap_transaction_status_rejected)
    .value("gap_TransactionStatus_accepted", gap_transaction_status_accepted)
    .value("gap_TransactionStatus_ready", gap_transaction_status_ready)
    .value("gap_TransactionStatus_started", gap_transaction_status_started)
    .value("gap_TransactionStatus_deleted", gap_transaction_status_deleted)
    .value("gap_TransactionStatus_finished", gap_transaction_status_finished)
    .value("gap_TransactionStatus_count", _gap_transaction_status_count)
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
  py::def("connect", &gap_trophonius_connect);
  py::def("get_notifications", &gap_meta_pull_notification);
  py::def("notifications_red", &gap_meta_notifications_red);
  py::def("scratch_db", &gap_debug);
  py::def("poll", &gap_poll);

  //- Notifications ------------------------------------------------------------

  ////////////////////////////////
  // User status.
  py::class_<gap_UserStatusNotification, boost::noncopyable>("UserStatus", py::no_init)
    .def_readonly("user_id", &gap_UserStatusNotification::user_id)
    .def_readonly("status", &gap_UserStatusNotification::status)
  ;
  py::def(
    "OnBite",
    &_gap_set_callback<gap_BiteNotification>
  );

  ////////////////////////////////
  // File transfer request.
  py::class_<gap_TransactionNotification, boost::noncopyable>("TransactionNotification", py::no_init)
    .def_readonly("transaction_id", &gap_TransactionNotification::transaction_id)
    .def_readonly("first_filename", &gap_TransactionNotification::first_filename)
    .def_readonly("files_count", &gap_TransactionNotification::files_count)
    .def_readonly("total_size", &gap_TransactionNotification::total_size)
    .def_readonly("is_directory", &gap_TransactionNotification::is_directory)
    .def_readonly("network_id", &gap_TransactionNotification::network_id)
    .def_readonly("sender_id", &gap_TransactionNotification::sender_id)
    .def_readonly("sender_fullname", &gap_TransactionNotification::sender_fullname)
    .def_readonly("recipient_id", &gap_TransactionNotification::recipient_id)
    .def_readonly("new", &gap_TransactionNotification::is_new)
  ;
  py::def(
    "OnTransaction",
    &_gap_set_callback<gap_TransactionNotification>
  );

  ////////////////////////////////
  // File transfer status.
  py::class_<gap_TransactionStatusNotification, boost::noncopyable>("TransactionStatusNotification", py::no_init)
    .def_readonly("transaction_id", &gap_TransactionStatusNotification::transaction_id)
    .def_readonly("network_id", &gap_TransactionStatusNotification::network_id)
    .def_readonly("sender_device_id", &gap_TransactionStatusNotification::sender_device_id)
    .def_readonly("sender_id", &gap_TransactionStatusNotification::sender_id)
    .def_readonly("recipient_id", &gap_TransactionStatusNotification::recipient_id)
    .def_readonly("recipient_device_name", &gap_TransactionStatusNotification::recipient_device_name)
    .def_readonly("recipient_device_id", &gap_TransactionStatusNotification::recipient_device_id)
    .def_readonly("status", &gap_TransactionStatusNotification::status)
    .def_readonly("new", &gap_TransactionStatusNotification::is_new)
  ;

  py::def(
    "OnTransactionStatus",
    &_gap_set_callback<gap_TransactionStatusNotification>
  );

  ////////////////////////////////
  // Message.
  py::class_<gap_MessageNotification, boost::noncopyable>("MessageNotification", py::no_init)
    .def_readonly("sender_id", &gap_MessageNotification::sender_id)
    .def_readonly("message", &gap_MessageNotification::message)
    .def_readonly("new", &gap_MessageNotification::is_new)
  ;

  py::def(
    "OnMessage",
    &_gap_set_callback<gap_MessageNotification>
    );


  py::class_<gap_BiteNotification, boost::noncopyable>("BiteNotification", py::no_init)
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

  py::def("user_fullname", &gap_user_fullname, by_value());
  py::def("user_email", &gap_user_email, by_value());
  py::def("_id", &gap_self_id, by_value());

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
