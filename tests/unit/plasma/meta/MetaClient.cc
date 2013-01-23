#include <common/common.hh>
#include <plasma/meta/Client.hh>

#include <elle/print.hh>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <utility>

// USER_COUNT.
#define USER_COUNT 8

struct UniqueUser
{
  std::unique_ptr<plasma::meta::Client> client;
  plasma::meta::User user;

  UniqueUser(std::string const& name):
    client{new plasma::meta::Client{
             common::meta::host(),
             common::meta::port(),
             true}},
    user{"",
        name,
        name + "@infinit.io",
        "",
        0}
   {}

  UniqueUser(UniqueUser&& u)
  {
    client = std::move(u.client);
    this->user = u.user;
  }
};

typedef std::vector<UniqueUser> Users;

Users
init_users()
{
  Users users;

  for (unsigned int i = 0; i < USER_COUNT; ++i)
  {
    std::string fullname = "random" + std::to_string(std::rand() % 10000);

    users.emplace_back(fullname);
  }

  return users;
}

int
test_register(Users const& users)
{
  std::string password = std::string(64, 'c');
  std::string activation_code = "bitebite";

  std::for_each(users.begin(), users.end(), [&](UniqueUser const& u) -> void
                {
                  auto res = u.client->register_(u.user.email,
                                                 u.user.fullname,
                                                 password,
                                                 activation_code);

                  if (res.success() != true)
                    throw;
                  std::cout << "Registering user " << u.user.fullname << std::endl;
                });

  return 0;
}

int
test_login(Users const& users)
{
  std::string password = std::string(64, 'c');;

  std::for_each(users.begin(), users.end(), [&](UniqueUser const& u) -> void
                {
                  auto res = u.client->login(u.user.email,
                                             password);

                  if (res.success() != true)
                    throw;

                  if (res.token.length() == 0         ||
                      res.fullname != u.user.fullname ||
                      res.email != u.user.email       ||
                      res.identity.length() == 0      ||
                      res._id.length() == 0)
                    throw;

                  std::cout << "loging user " << u.user.fullname << std::endl;
                });

  return 0;
}

int
test_logout(Users const& users)
{
  std::string password = std::string(64, 'c');;

  std::for_each(users.begin(), users.end(), [&](UniqueUser const& u) -> void
                {
                  auto res = u.client->logout();

                  if (res.success() != true)
                    throw;

                  if (u.client->email().length() != 0)
                    throw;
                  if (u.client->token().length() != 0)
                    throw;
                  if (u.client->identity().length() != 0)
                    throw;

                  std::cout << "logout user " << u.user.fullname << std::endl;
                });
  return 0;
}

int
test_sendfiles(Users const& users)
{
  int i = 0;
  std::for_each(users.begin(), users.end(), [&](UniqueUser const& u) -> void
                {
                  i++;
                });
  return 0;
}


int
main()
{
  // Initialization.
  std::srand(std::time(0));

  assert((USER_COUNT % 2) == 0);

  Users users = init_users();

  if (test_register(users) != 0)
    throw;

  if (test_login(users) != 0)
    throw;

  if (test_logout(users) != 0)
    throw;

  if (test_login(users) != 0)
    throw;

  elle::print("tests done.");

  return 0;
};
