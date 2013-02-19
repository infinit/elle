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
#include <algorithm>

// USER_COUNT.
#define USER_COUNT 8

struct UniqueUser
{
  std::unique_ptr<plasma::meta::Client> client;
  plasma::meta::User user;
  std::string network_id;
  std::string device_id;

  UniqueUser(std::string const& name):
    client{new plasma::meta::Client{
      common::meta::host(),
        common::meta::port(),
        true}},
    user{"",
        name,
        name + "@infinit.io",
        "",
        0},
    network_id{"99510fbd8ae7798906b80000"},
    device_id{"aa510fbd8ae7798906b80000"}
  {}

  UniqueUser(UniqueUser&& u)
  {
    client = std::move(u.client);
    this->user = u.user;
    this->network_id = u.network_id;
    this->device_id = u.device_id;
  }
};

typedef std::vector<UniqueUser> Users;

Users
init_users()
{
  Users users;

  for (unsigned int i = 0; i < USER_COUNT; ++i)
  {
    std::string fullname = "random" + std::to_string(std::rand() % 1000000);

    users.emplace_back(fullname);
  }

  return users;
}

void
test_register(Users const& users)
{
  std::string password = std::string(64, 'c');
  std::string activation_code = "bitebite";

  for(UniqueUser const& u: users)
  {
    auto res = u.client->register_(u.user.email,
                                   u.user.fullname,
                                   password,
                                   activation_code);

    if (res.success() != true)
      throw std::runtime_error("register failed");
  }
}

void
test_login(Users& users)
{
  std::string password = std::string(64, 'c');;

  for (UniqueUser& u: users)
  {
    // Login.
    {
      auto res = u.client->login(u.user.email,
                                 password);

      if (res.success() != true)
        throw std::runtime_error("login failed");

      if (res.token.length() == 0         ||
          res.fullname != u.user.fullname ||
          res.email != u.user.email       ||
          res.identity.length() == 0      ||
          res._id.length() == 0)
        throw std::runtime_error("login data corrupted");
    }

    // Get data using self.
    {
      auto res = u.client->self();

      if (res.success() != true)
        throw std::runtime_error("self failed");

      if (res._id.length() == 0             ||
          res.fullname != u.user.fullname   ||
          res.email != u.user.email         ||
          res.public_key.length() == 0)
        throw std::runtime_error("self data corrupted");

      u.user = res;
    }

    // Get data using public key search.
    {
      auto res = u.client->user_from_public_key(u.user.public_key);

      if (res.success() != true)
        throw std::runtime_error("search from public key failed");

      if (res._id != u.user._id               ||
          res.fullname != u.user.fullname     ||
          res.email != u.user.email           ||
          res.public_key != u.user.public_key)
        throw std::runtime_error("search from public key data corrupted");
    }
  }
}

void
test_device(Users& users)
{
  for (UniqueUser& u: users)
  {
    // Create device.
    {
      std::string device_name = u.user.fullname + "device";
      auto res = u.client->create_device(device_name);

      if (res.success() != true)
        throw std::runtime_error("create device failed");

      if (res.created_device_id.length() == 0 ||
          res.passport.length() == 0)
        throw std::runtime_error("device creation data corrupted");

      u.device_id = res.created_device_id;
    }

    // Update device.
    {
      auto device_name = std::string("device") + u.user.fullname;
      auto res = u.client->update_device(u.device_id,
                                         device_name);

      if (res.success() != true)
        throw std::runtime_error("update device failed");

      if (res.updated_device_id.length() == 0 ||
          res.passport.length() == 0)
        throw std::runtime_error("device update data corrupted");

      u.device_id = res.updated_device_id;
    }
  }
}

void
test_logout(Users const& users)
{
  // Logout every users.
  for (UniqueUser const& u: users)
  {
    auto res = u.client->logout();

    if (res.success() != true)
      throw std::runtime_error("lougout failed");

    if (u.client->email().length() != 0)
      throw std::runtime_error("lougout didn't clear email");
    if (u.client->token().length() != 0)
      throw std::runtime_error("lougout didn't clear token");
    if (u.client->identity().length() != 0)
      throw std::runtime_error("lougout didn't clear identity");
  }
}

void
test_search(Users const& users)
{
  std::string password = std::string(64, 'c');;
  std::string prefix("random");

  // Search users with fullname starting by prefix.
  for (UniqueUser const& u: users)
  {
    auto res = u.client->search_users(prefix, USER_COUNT, 0);

    if (res.success() != true)
      throw std::runtime_error("search failed");

    if (res.users.size() < USER_COUNT)
      throw std::runtime_error("too few results");

    // Ensure search returned only users with the good fullname.
    for (auto user_id: res.users)
    {
      auto user = u.client->user(user_id);

      if (user._id != user_id)
        throw std::runtime_error("found wrong user");

      if (user.fullname.compare(0, prefix.size(), prefix))
        throw std::runtime_error("found wrong user again");
    }
  }
}

void
test_delete_network(Users const& users, bool force)
{
  for (UniqueUser const& u: users)
  {
    // Delete network.
    {
      auto res = u.client->delete_network(u.network_id, force);

      if (res.success() != true)
        throw std::runtime_error(std::string{"delete network " \
              "failed with"} + (force ? "" : "out") + " force");

      if (!force && res.deleted_network_id != u.network_id)
        throw std::runtime_error("delete network wrong returned value");
    }

    // Ensure network is delete.
    // XXX
  }
}

void
test_create_network(Users& users)
{
  for (UniqueUser& u: users)
  {
    unsigned count;

    // Get network list to count networks before creation.
    {
      auto res = u.client->networks();

      if (res.success() != true)
        throw std::runtime_error("getting networks failed");

      count = res.networks.size();
    }

    // Create a network.
    {
      auto res = u.client->create_network(u.user.fullname);

      if (res.success() != true)
        throw std::runtime_error("create network failed");

      if (res.created_network_id.length() == 0)
        throw std::runtime_error("network creation data corrupted");

      u.network_id = res.created_network_id;

      // Check network data.
      {
        auto network_res = u.client->network(u.network_id);

        if (network_res.success() != true)
          throw std::runtime_error("getting network failed");

        if (network_res.owner != u.user._id      ||
            network_res._id   != u.network_id    ||
            network_res.name  != u.user.fullname)
        {
          std::cout << network_res.owner << " != " << u.user._id << std::endl;
          std::cout << network_res._id << " != " << u.network_id << std::endl;
          std::cout << network_res.name << " != " << u.user.fullname << std::endl;

          throw std::runtime_error("network created but data is corrupted");
        }
      }

      // Check nodes in network.
      {
        auto nodes_res = u.client->network_nodes(u.network_id);

        if (nodes_res.success() != true)
          throw std::runtime_error("getting nodes failed");


        //XXX
      }
    }

    // Count user's networks.
    {
      auto res = u.client->networks();

      if (res.success() != true)
        throw std::runtime_error("getting networks failed");

      if (res.networks.size() != (count + 1))
        throw std::runtime_error("network not in list");

      auto found = std::find_if(res.networks.begin(),
                                res.networks.end(),
                                [&](std::string n) -> bool
                                { return (n == u.network_id); });

      if (found == res.networks.end())
        throw std::runtime_error("network not in list");
    }

    // Check if number of user in network is USER_COUNT.
    {
      auto res = u.client->network(u.network_id);

      if (res.success() != true)
        throw std::runtime_error("getting network failed");

      if (res.users.size() != 1)
        throw std::runtime_error("network not in list");
    }


    // Add users in network.
    for (UniqueUser const& u_guest: users)
    {
      if (u_guest.user._id == u.user._id)
        continue;

      auto res = u.client->network_add_user(u.network_id,
                                            u_guest.user._id);

      if (res.success() != true)
        throw std::runtime_error("add user to network failed");

      if (res.updated_network_id != u.network_id)
        throw std::runtime_error("addi user to network data corrupted");
    }

    // Count user in the network.
    {
      auto res = u.client->network(u.network_id);

      if (res.success() != true)
        throw std::runtime_error("getting networks failed");

      if (res.users.size() != USER_COUNT)
        throw std::runtime_error("Wrong number of user in network");
    }
  }
}

void
test_connect_device(Users const& users)
{
  for (UniqueUser const& u: users)
  {
    // std::string fake_ip{"83.123.17.199"};
    // uint16_t fakeport = 9123;

    // u.client->network_connect_device(u.network_id,
    //                                  u.device_id,
    //                                  fake_ip,
    //                                  fakeport);

  }
}

int
main()
{
  // Initialization.
  std::srand(std::time(0));

  assert((USER_COUNT % 2) == 0);

  Users users = init_users();

  test_register(users);
  test_login(users);
  test_logout(users);
  test_login(users);
  test_search(users);
  test_delete_network(users, true);
  test_create_network(users);
  test_delete_network(users, false);

  test_create_network(users);
  test_connect_device(users);
  test_delete_network(users, false);


  elle::print("tests done.");

  return 0;
};
