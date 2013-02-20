// For std::this_thread::sleep_for until gcc4.8
#define _GLIBCXX_USE_NANOSLEEP 1

#include <surface/gap/State.hh>

#include <lune/Lune.hh>
#include <elle/os/path.hh>
#include <elle/print.hh>

#include <thread>
#include <chrono>
#include <ctime>

ELLE_LOG_COMPONENT("test.State");

using State = surface::gap::State;
using TransactionNotification =
  ::plasma::trophonius::TransactionNotification;
using TransactionStatusNotification =
  ::plasma::trophonius::TransactionStatusNotification;

int fail_counter = 0;

void
auto_accept_transaction_cb(TransactionNotification const &tn, State &s)
{
  s.update_transaction(tn.transaction.transaction_id,
                       gap_transaction_status_accepted);
}

void
close_on_finished_transaction_cb(TransactionStatusNotification const &tn,
                                 State &,
                                 bool& finish_test)
{
  if (tn.status == gap_transaction_status_canceled)
    {
      ELLE_ERR("transaction canceled")
        ++fail_counter;
      finish_test = true;
    }
  else if (tn.status == gap_transaction_status_finished)
    finish_test = true;
}

auto kill_all = []
(State &s) {
  auto &n = s.networks();
  while (1)
    {
      auto it = n.begin();
      auto ite = n.end();
      if (it == ite)
        break;
      s.delete_network(std::string(it->first));
    }
};

auto make_login = []
(State &s, std::string user, std::string email)
{
  try
    {
      s.register_(user,
                  email,
                  s.hash_password(email, "bitebitebite"),
                  "bitebite");
    }
  catch (...)
    {
      s.login(email,
              s.hash_password(email, "bitebitebite"));
    }
  s.connect();
  s.update_device("device" + user);
};


int
main()
{
  lune::Lune::Initialize();

  surface::gap::State s;

  make_login(s, "test_state_connect", "test_state_connect@infinit.io");
  s.logout();
  elle::print("tests done.");
  return 0;
}
