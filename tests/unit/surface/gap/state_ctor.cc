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

auto make_worker = []
(State &s) -> std::thread
{
    auto poll = [&] {
        while (s.logged_in())
        {
            s.poll();
            ::sleep(1);
        }
    };
    return std::move(std::thread{poll});
};

int
main()
{
  lune::Lune::Initialize();

  surface::gap::State s1, s2;
  int timeout = 0;
  bool finish = false;
  std::string email1 = "usertest001@infinit.io";
  std::string email2 = "usertest002@infinit.io";


  std::string download_dir{"infinit_download_dir"};
  std::string to_send{"castor_dir"};

  elle::os::path::make_directory(download_dir);
  elle::os::path::make_directory(to_send);

  s2.transaction_callback([&] (TransactionNotification const& t, bool)
                          { auto_accept_transaction_cb(t, s2); });

  s1.transaction_status_callback([&] (TransactionStatusNotification const& t,
                                      bool)
                                 {
                                   close_on_finished_transaction_cb(
                                     t, s1, finish);
                                 });

  make_login(s1, "Bite", email1);
  make_login(s2, "Bite", email2);

  s2.output_dir(download_dir);

  std::thread t1 = make_worker(s1);
  std::thread t2 = make_worker(s2);

  unsigned int counter = 0;
  try
  {
    for (counter = 0; counter < 10; ++counter)
    {
      auto process_id = s1.send_files(email2, {to_send});

      auto process_status = surface::gap::State::ProcessStatus::running;

      timeout = 30;
      while (process_status == surface::gap::State::ProcessStatus::running)
      {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (--timeout < 0)
          throw std::runtime_error{"sending files timed out"};

        process_status = s1.process_status(process_id);
      }
      s1.process_finalize(process_id);

      timeout = 60;
      while (!finish)
      {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (--timeout < 0)
          throw std::runtime_error{"downloading files timed out"};
      }

      elle::os::path::remove_directory(download_dir);
      elle::os::path::make_directory(download_dir);
      finish = false;
    }
  }
  catch (...)
  {
    elle::os::path::remove_directory(download_dir);
  }

  s1.logout();
  s2.logout();

  if (t1.joinable())
    t1.join();

  if (t2.joinable())
    t2.join();

  elle::sprint("try: %s > failed: %s", counter, fail_counter);

  elle::print("tests done.");
  return 0;
}
