#include <surface/gap/State.hh>
#include <lune/Lune.hh>
#include <Infinit.hh>
#include <elle/print.hh>

int main()
{
  surface::gap::State s;
  surface::gap::UserStatusNotification a;

  lune::Lune::Initialize();
  a.user_id = "canard";
  a.status = 1;
  a.notification_type = plasma::trophonius::NotificationType::user_status;
  try
  {
      s.register_("manger",
                  "pif@pif.com",
                  s.hash_password("pif@pif.com",
                                  "bitebitebite"),
                  "bitebite");
  }
  catch (...)
  {
      s.login("pif@pif.com",
              s.hash_password("pif@pif.com",
                              "bitebitebite")
              );
  }

  auto &n = s.networks();
  while (1)
  {
      auto it = n.begin();
      auto ite = n.end();
      if (it == ite)
          break;
      s.delete_network(std::string(it->first));
  }

  std::vector<std::string> networks_id;

  networks_id.push_back(s.create_network("manger"));
  networks_id.push_back(s.create_network("des"));
  networks_id.push_back(s.create_network("enfants"));
  s._on_user_status_callback(a);

  for (auto &id: networks_id)
  {
      s.delete_network(id);
  }
  elle::print("tests done.");
  return 0;
}

