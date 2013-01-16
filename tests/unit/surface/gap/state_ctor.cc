#include <surface/gap/State.hh>
#include <lune/Lune.hh>
#include <Infinit.hh>
#include <elle/print.hh>
#include <thread>
#include <ctime>

using State = surface::gap::State;
using TransactionNotification = ::plasma::trophonius::TransactionNotification;

auto on_transaction_accepted_cb = []
(TransactionNotification const &tn, State &s) -> void {
    s.update_transaction(tn.transaction.transaction_id,
                         gap_transaction_status_accepted);
};

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
  s.launch_watchdog();
};

auto make_worker = []
(State &s) -> std::thread
{
    auto poll = [&] {
        std::cerr << __PRETTY_FUNCTION__ << std::endl;
        while (1)
        {
            s.poll();
            ::nanosleep((struct timespec[]){{0, 500000000L}}, NULL);
        }
    };
    std::cerr << __PRETTY_FUNCTION__ << std::endl;
    return std::move(std::thread{poll});
};
// {{{
auto dead = [] {
  //std::vector<std::string> networks_id;

  //networks_id.push_back(s.create_network("manger"));
  //networks_id.push_back(s.create_network("des"));
  //networks_id.push_back(s.create_network("enfants"));

  //for (auto &id: networks_id)
  //{
  //    s.delete_network(id);
  //}
};
// }}}

void agregate()
{
}

template <typename T, typename ...L>
void agregate(T const &head, L ...tail)
{
    std::cerr << "do " << __PRETTY_FUNCTION__ << std::endl;
    head();
    agregate(std::forward<L>(tail)...);
}

int main()
{
  surface::gap::State s1;
  //surface::gap::State s2;

  lune::Lune::Initialize();

  std::string email1 = "pif@pif.com";
  std::string email2 = "pichot.fabien@gmail.com";

  make_login(s1, "Bite", email1);
  //make_login(s2, "Botte", email2);

  //auto fn = [&] (TransactionNotification const &tn, bool) {
  //    on_transaction_accepted_cb(tn, s2);
  //}; 
  //s2.transaction_callback(fn);

  s1.send_files(email2, {"/tmp/lol"});

  std::thread t1 = make_worker(s1);
  //std::thread t2 = make_worker(s2);

  agregate([&] {t1.join();});
           //[&] {t2.join();});

  elle::print("tests done.");
  return 0;
}

