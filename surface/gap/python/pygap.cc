
#include <surface/gap/python/python.hh>

#include <surface/gap/gap.h>

#include <surface/gap/State.hh>

static boost::python::object
_get_networks(gap_State* state)
{
  assert(state != nullptr);

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

static boost::python::object
_get_transactions(gap_State* state)
{
  assert(state != nullptr);

  boost::python::list transactions_;
  char** transactions = gap_transactions(state);
  if (transactions != nullptr)
    {
      for (char** ptr = transactions; *ptr != nullptr; ++ptr)
        {
          transactions_.append(boost::python::str(std::string(*ptr)));
        }
        gap_transactions_free(transactions);
    }
  return transactions_;
}

static boost::python::object
_get_swaggers(gap_State* state)
{
  assert(state != nullptr);

  boost::python::list swaggers_;
  char** swaggers = gap_swaggers(state);
  if (swaggers != nullptr)
    {
      for (char** ptr = swaggers; *ptr != nullptr; ++ptr)
        {
          swaggers_.append(boost::python::str(std::string(*ptr)));
        }
        gap_swaggers_free(swaggers);
    }
  return swaggers_;
}

static boost::python::object
_search_users(gap_State* state, std::string text)
{
  assert(state != nullptr);
  assert(text.length() > 0);

  boost::python::list users_;
  char** users = gap_search_users(state, text.c_str());
  if (users != nullptr)
    {
      for (char** ptr = users; *ptr != nullptr; ++ptr)
        {
          users_.append(boost::python::str(std::string(*ptr)));
        }
        gap_search_users_free(users);
    }
  return users_;
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
boost::python::object
_get_transaction(gap_State* state, std::string transaction_id)
{
  boost::python::object transaction = boost::python::class_<gap_Transaction, boost::noncopyable>("Transaction");

  gap_Transaction const* trans = gap_transaction(state, transaction_id.c_str());

  transaction.attr("transaction_id") = trans->transaction_id;
  transaction.attr("sender_id") = trans->sender_id;
  transaction.attr("sender_fullname") = trans->sender_fullname;
  transaction.attr("sender_device_id") = trans->sender_device_id;
  transaction.attr("recipient_id") = trans->recipient_id;
  transaction.attr("recipient_fullname") = trans->recipient_fullname;
  transaction.attr("recipient_device_id") = trans->recipient_device_id;
  transaction.attr("network_id") = trans->network_id;
  transaction.attr("first_filename") = trans->first_filename;
  transaction.attr("files_count") = trans->files_count;
  transaction.attr("total_size") = trans->total_size;
  transaction.attr("is_directory") = trans->is_directory;
  transaction.attr("status") = trans->status;

  return transaction;
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

extern "C"
{
  struct gap_State { /* dummy declaration for boost::python */ };
}

BOOST_PYTHON_MODULE(_gap)
{
  namespace py = boost::python;
  typedef py::return_value_policy<py::return_by_value> by_value;

  //////////////////////////
  // value MUST be gap_EnumName_name
  py::enum_<gap_Status>("Status")
    .value("ok", gap_ok)
    .value("error", gap_error)
    .value("network_error", gap_network_error)
    .value("internal_error", gap_internal_error)
    .value("api_error", gap_api_error)
    .value("no_device_error", gap_no_device_error)
    .value("not_logged_in", gap_not_logged_in)
    .value("bad_request", gap_bad_request)
    .value("already_logged_in", gap_already_logged_in)
    .value("not_logged_in", gap_not_logged_in)
    .value("email_not_valid", gap_email_not_valid)
    .value("handle_not_valid", gap_handle_not_valid)
    .value("device_not_valid", gap_device_not_valid)
    .value("password_not_valid", gap_password_not_valid)
    .value("user_id_not_valid", gap_user_id_not_valid)
    .value("network_id_not_valid", gap_network_id_not_valid)
    .value("device_id_not_valid", gap_device_id_not_valid)
    .value("field_is_empty", gap_field_is_empty)
    .value("activation_code_not_valid", gap_activation_code_not_valid)
    .value("deprecated", gap_deprecated)
    .value("email_already_registred", gap_email_already_registred)
    .value("handle_already_registred", gap_handle_already_registred)
    .value("device_already_registred", gap_device_already_registred)
    .value("activation_code_doesnt_exist", gap_activation_code_doesnt_exist)
    .value("email_password_dont_match", gap_email_password_dont_match)
    .value("unknown_user", gap_unknown_user)
    .value("user_already_in_network", gap_user_already_in_network)
    .value("network_not_found", gap_network_not_found)
    .value("device_not_found", gap_device_not_found)
    .value("device_not_in_network", gap_device_not_in_network)
    .value("root_block_already_exist", gap_root_block_already_exist)
    .value("root_block_badly_signed", gap_root_block_badly_signed)
    .value("user_already_invited", gap_user_already_invited)
    .value("user_already_in_infinit", gap_user_already_in_infinit)
    .value("file_name_empty", gap_file_name_empty)
    .value("unknown", gap_unknown)
  ;

  py::enum_<gap_TransactionStatus>("TransactionStatus")
    .value("none", gap_transaction_status_none)
    .value("pending", gap_transaction_status_pending)
    .value("rejected", gap_transaction_status_rejected)
    .value("accepted", gap_transaction_status_accepted)
    .value("started", gap_transaction_status_started)
    .value("canceled", gap_transaction_status_canceled)
    .value("finished", gap_transaction_status_finished)
    .value("count", _gap_transaction_status_count)
  ;

  //- gap ctor and dtor -------------------------------------------------------

  py::class_<gap_State>("State");

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
  py::def("connect", &gap_trophonius_connect);

  //- Notifications ------------------------------------------------------------

  py::def("pull_notifications", &gap_pull_notifications);
  py::def("notifications_read", &gap_notifications_read);
  py::def("poll", &gap_poll);

  ///////////////////////////
  // Data structures.

  //User status.
  py::class_<gap_UserStatusNotification, boost::noncopyable>("UserStatus", py::no_init)
    .def_readonly("user_id", &gap_UserStatusNotification::user_id)
    .def_readonly("status", &gap_UserStatusNotification::status)
  ;

  // File transfer request.
  py::class_<gap_TransactionNotification, boost::noncopyable>("TransferNotification", py::no_init)
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

  // File transfer status.
  py::class_<gap_TransactionStatusNotification, boost::noncopyable>("TransferStatusNotification", py::no_init)
    .def_readonly("transaction_id", &gap_TransactionStatusNotification::transaction_id)
    .def_readonly("status", &gap_TransactionStatusNotification::status)
    .def_readonly("new", &gap_TransactionStatusNotification::is_new)
  ;

  // Transaction.
  py::object Transaction_t = py::class_<gap_Transaction, boost::noncopyable>("Transaction", py::no_init)
    .def_readonly("transaction_id", &gap_Transaction::transaction_id)
    .def_readonly("first_filename", &gap_Transaction::first_filename)
    .def_readonly("files_count", &gap_Transaction::files_count)
    .def_readonly("total_size", &gap_Transaction::total_size)
    .def_readonly("is_directory", &gap_Transaction::is_directory)
    .def_readonly("network_id", &gap_Transaction::network_id)
    .def_readonly("sender_id", &gap_Transaction::sender_id)
    .def_readonly("sender_fullname", &gap_Transaction::sender_fullname)
    .def_readonly("sender_device_id", &gap_Transaction::sender_device_id)
    .def_readonly("recipient_id", &gap_Transaction::recipient_id)
    .def_readonly("recipient_fullname", &gap_Transaction::recipient_fullname)
    .def_readonly("recipient_device_id", &gap_Transaction::recipient_device_id)
    .def_readonly("status", &gap_Transaction::status)
  ;

  // Message.
  py::class_<gap_MessageNotification, boost::noncopyable>("MessageNotification", py::no_init)
    .def_readonly("sender_id", &gap_MessageNotification::sender_id)
    .def_readonly("message", &gap_MessageNotification::message)
    .def_readonly("new", &gap_MessageNotification::is_new)
  ;

  ///////////////////////////
  // Callbacks.

  py::def(
    "on_transaction",
    &_gap_set_callback<gap_TransactionNotification>
  );

  py::def(
    "on_transaction_status",
    &_gap_set_callback<gap_TransactionStatusNotification>
  );

  py::def(
    "on_message",
    &_gap_set_callback<gap_MessageNotification>
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
  py::def("search_users", &_search_users);
  py::def("get_swaggers", &_get_swaggers);

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

  //- Transactions- ------------------------------------------------------------

  py::def("transactions", &_get_transactions);
  py::def("send_files", &_send_files);
  py::def("update_transaction", &gap_update_transaction);
  py::def("set_output_dir", &gap_set_output_dir);
  py::def("transaction_status", &gap_transaction_status);
  py::def("transaction_owner", &gap_transaction_owner);
  py::def("lol", &_get_transaction);
}
