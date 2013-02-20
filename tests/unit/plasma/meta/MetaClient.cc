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

#define USER_COUNT 4

/*==============================================================================
  Tested features
  ==============================================================================
  - Register
  - Login
  - Logout
  - Self
  - User from public key
  - Device creation
  - Device update
  - Search
  - User from ID
  - Network creation
  - Network destruction
  - Network data
  - Network add user

  ==============================================================================
  XXX: Missing features
  ==============================================================================
  - Connect device
  - Get Endpoints
  - Transaction creation
  - Transaction update
  - Swaggers
  - Network add device
  - User icon
*/

#define PRETTY_THROW(str)                                               \
  throw std::runtime_error(std::string{__PRETTY_FUNCTION__} + str)

struct UniqueUser
{
  std::unique_ptr<plasma::meta::Client> client;
  plasma::meta::User user;
  std::string network_id;
  std::string device_id;
  std::string device_name;
  std::string transaction_id;
  std::string local_ip;
  int local_port;
  std::string external_ip;
  int external_port;

  UniqueUser(std::string const& name)
    : client{new plasma::meta::Client{
      common::meta::host(), common::meta::port(), true}}
    , user{"", name, name + "@infinit.io", "", 0}
    , network_id{"99510fbd8ae7798906b80000"}
    , device_id{"aa510fbd8ae7798906b80000"}
    , device_name{device_id + "device"}
    , transaction_id{}
    , local_ip{elle::sprintf("%s.%s.%s.%s",
                            std::rand() % 255,
                            std::rand() % 255,
                            std::rand() % 255,
                            std::rand() % 255)}
    , local_port{std::rand() % 65535}
    , external_ip{elle::sprintf("%s.%s.%s.%s",
                               std::rand() % 255,
                               std::rand() % 255,
                               std::rand() % 255,
                               std::rand() % 255)}
    , external_port{std::rand() % 65535}
  {}

  UniqueUser(UniqueUser&& u)
  {
    client = std::move(u.client);
    this->user = u.user;
    this->network_id = u.network_id;
    this->device_id = u.device_id;
    this->device_name = u.device_name;
    this->transaction_id = u.transaction_id;
    this->local_ip = u.local_ip;
    this->local_port = u.local_port;
    this->external_ip = u.external_ip;
    this->external_port = u.external_port;
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

//XXX: This may failed, due to random.
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

    if (!res.success())
      PRETTY_THROW(elle::sprintf("register failed. %s",
                                 res.response_details));
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
        PRETTY_THROW(elle::sprintf("login failed. %s",
                                   res.response_details));

      if (res.token.empty())
        PRETTY_THROW("token is empty");
      if (res.fullname.empty())
        PRETTY_THROW("fullname is empty");
      if (res.email.empty())
        PRETTY_THROW("email is empty");
      if (res.identity.empty())
        PRETTY_THROW("identity is empty");
      if (res._id.empty())
        PRETTY_THROW("_id is empty");
    }

    // Get data using self.
    {
      auto res = u.client->self();

      if (res.success() != true)
        PRETTY_THROW(elle::sprintf("self failed. %s",
                                   res.response_details));

      if (res._id.empty())
        PRETTY_THROW("_id is empty");
      if (res.fullname != u.user.fullname)
        PRETTY_THROW("fullnames don't match");
      if (res.email != u.user.email)
        PRETTY_THROW("emails don't match");
      if (res.public_key.empty())
        PRETTY_THROW("public_keys is empty");

      u.user = res;
    }

    // Get data using public key search.
    {
      auto res = u.client->user_from_public_key(u.user.public_key);

      if (res.success() != true)
        PRETTY_THROW(elle::sprintf("search from pkey failed. %s",
                                   res.response_details));

      if (res._id != u.user._id)
        PRETTY_THROW("_ids don't match");
      if (res.fullname != u.user.fullname)
        PRETTY_THROW("fullnames don't match");
      if (res.email != u.user.email)
        PRETTY_THROW("emails don't match");
      if (res.public_key != u.user.public_key)
        PRETTY_THROW("public_keys don't match");
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
      u.device_name = u.user.fullname + "device";
      auto res = u.client->create_device(u.device_name);

      if (res.success() != true)
        PRETTY_THROW(elle::sprintf("create device failed. %s",
                                   res.response_details));

      if (res.created_device_id.empty())
        PRETTY_THROW("device id is empty");
      if (res.passport.empty())
        PRETTY_THROW("passport is empty");

      u.device_id = res.created_device_id;
    }

    // Update device.
    {
      auto device_name = std::string("device") + u.user.fullname;
      auto res = u.client->update_device(u.device_id,
                                         device_name);

      if (res.success() != true)
        PRETTY_THROW(elle::sprintf("update device failed. %s",
                                   res.response_details));

      if (res.updated_device_id.empty())
        PRETTY_THROW("device id is empty");
      if (res.passport.empty())
        PRETTY_THROW("passport is empty");

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
      PRETTY_THROW(elle::sprintf("lougout failed. %s",
                                 res.response_details));

    if (!u.client->email().empty())
      PRETTY_THROW("lougout didn't clear email");
    if (!u.client->token().empty())
      PRETTY_THROW("lougout didn't clear token");
    if (!u.client->identity().empty())
      PRETTY_THROW("lougout didn't clear identity");
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
      PRETTY_THROW(elle::sprintf("search failed. %s",
                                 res.response_details));

    if (res.users.size() < USER_COUNT)
      PRETTY_THROW("too few results");

    // Ensure search returned only users with the good fullname.
    for (auto user_id: res.users)
    {
      auto user = u.client->user(user_id);

      if (user._id != user_id)
        PRETTY_THROW("found wrong user");
      if (user.fullname.compare(0, prefix.size(), prefix))
        PRETTY_THROW("found wrong user again");
    }
  }
}

//- Networks -------------------------------------------------------------------
void
_create_network(UniqueUser& u)
{
  auto res = u.client->create_network(u.user.fullname);

  if (res.success() != true)
    PRETTY_THROW(elle::sprintf("create network failed, %s",
                               res.response_details));

  if (res.created_network_id.length() == 0)
    PRETTY_THROW("network creation data corrupted");

  u.network_id = res.created_network_id;
}

void
_delete_network(UniqueUser const& u, bool force)
{
  auto res = u.client->delete_network(u.network_id, force);

  if (res.success() != true)
    PRETTY_THROW(
      elle::sprintf("delete network failed with%s force. %s",
                    (force ? "" : "out"), res.response_details));

  if (!force && res.deleted_network_id != u.network_id)
    PRETTY_THROW(elle::sprintf("netorks id don't match",
                               res.response_details));
}

int
_count_networks(UniqueUser const& u)
{
  auto res = u.client->networks();

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting networks failed. %s",
                               res.response_details));

  return res.networks.size();
}

void
_check_network_data(UniqueUser const& u)
{
  auto res = u.client->network(u.network_id);

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting network failed. %s",
                               res.response_details));

  if (res.owner != u.user._id)
    PRETTY_THROW("owner don't match");
  if (res._id != u.network_id)
    PRETTY_THROW("network_id don't match");
  if (res.name != u.user.fullname)
    PRETTY_THROW("network names don't match");
}

void
_check_nodes(UniqueUser const& u)
{
  auto res = u.client->network_nodes(u.network_id);

  if (res.success() != true)
    PRETTY_THROW(elle::sprintf("getting nodes failed. %s",
                               res.response_details));

  //XXX
}

bool
_is_network_in_list(UniqueUser const& u)
{
  auto res = u.client->networks();

  if (res.success() != true)
    PRETTY_THROW(elle::sprintf("getting networks failed. %s",
                               res.response_details));

  auto found = std::find_if(res.networks.begin(),
                            res.networks.end(),
                            [&](std::string n) -> bool
                            { return (n == u.network_id); });

  if (found == res.networks.end())
    return false;

  return true;
}

int
_count_users_in_network(UniqueUser const& u)
{
  auto res = u.client->network(u.network_id);

  if (res.success() != true)
    PRETTY_THROW(elle::sprintf("getting network failed. %s",
                               res.response_details));

  return res.users.size();
}

void
_network_add_user(UniqueUser& u, UniqueUser const& guest)
{
  if (guest.user._id == u.user._id)
    return;

  auto res = u.client->network_add_user(u.network_id, guest.user._id);

  if (res.success() != true)
    PRETTY_THROW(elle::sprintf("add user to network failed",
                               res.response_details));

  if (res.updated_network_id != u.network_id)
    PRETTY_THROW("add user to network data corrupted");
}

void
_network_add_device(UniqueUser& u, UniqueUser const& guest)
{
  // if (guest.user._id == u.user._id)
  //   return;

  auto res = u.client->network_add_device(u.network_id, guest.device_id);

  if (res.success() != true)
    PRETTY_THROW(elle::sprintf("add device to network failed",
                               res.response_details));

  if (res.updated_network_id != u.network_id)
    PRETTY_THROW("add device to network data corrupted");
}

void
_network_connect_device(UniqueUser const& u, UniqueUser const& guest)
{
  auto res = u.client->network_connect_device(u.network_id,
                                              guest.device_id,
                                              &guest.local_ip,
                                              guest.local_port,
                                              &guest.external_ip,
                                              guest.external_port);

  if (!res.success())
    PRETTY_THROW(elle::sprintf("connecting device failed. %s",
                               res.response_details));

  if (res.updated_network_id != u.network_id)
    PRETTY_THROW("network_ids don't match");

}

bool
_is_device_connect(UniqueUser const& u, UniqueUser const& peer)
{
  auto res = u.client->device_endpoints(u.network_id,
                                        u.device_id,
                                        peer.device_id);

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting device endpoints failed. %s",
                               res.response_details));

  if (res.externals.empty() && peer.user._id != u.user._id)
    return false;
  if (res.locals.empty())
    return false;

  std::string peer_externals = peer.external_ip + ":" + std::to_string(peer.external_port);
  auto itext = std::find(res.externals.begin(), res.externals.end(),
                         peer_externals);

  if (itext == res.externals.end() && peer.user._id != u.user._id)
    return false;
  std::string peer_locals = peer.local_ip + ":" + std::to_string(peer.local_port);
  auto itloc = std::find(res.locals.begin(), res.locals.end(),
                         peer_locals);

  if (itloc == res.locals.end())
    return false;

  return true;
}


void
test_delete_network(Users const& users, bool force)
{
  for (UniqueUser const& u: users)
  {
    int count = _count_networks(u);
    _delete_network(u, force);
    if (!force && _count_networks(u) != (count - 1))
      PRETTY_THROW("network count is wrong");
    if (_is_network_in_list(u))
      PRETTY_THROW("network not deleted");
  }
}

void
test_create_network(Users& users)
{
  for (UniqueUser& u: users)
  {
    // count networks before creation.
    int count = _count_networks(u);

    _create_network(u);
    _check_network_data(u);
    _check_nodes(u);

    if (_count_networks(u) != (count + 1))
      PRETTY_THROW("network count is bad");
    if (!_is_network_in_list(u))
      PRETTY_THROW("network not added in the list");


    // Check if number of user in network is 1 (the user only).
    if (_count_users_in_network(u) != 1)
      PRETTY_THROW("network should contain only 1 user");

    // Add users in network.
    for (UniqueUser const& guest: users)
    {
      _network_add_user(u, guest);
      _network_add_device(u, guest);
      _network_connect_device(u, guest);
    }

    for (UniqueUser const& guest: users)
    {
      if (!_is_device_connect(u, guest))
        PRETTY_THROW("user is not connected in this network");
    }

    // Count user in the network. Should be USER_COUNT.
    if (_count_users_in_network(u) != USER_COUNT)
      PRETTY_THROW("Wrong number of user in network");

  }
}

//- Transactions ---------------------------------------------------------------
std::string filename = "beaver.txt";
int count = 1;
int size = 9139;
bool is_dir = false;

void
_create_transaction(UniqueUser& u, UniqueUser const& recipient)
{
  if (recipient.user._id == u.user._id)
    return;

  auto res = u.client->create_transaction(
    recipient.user._id,
    filename,
    count,
    size,
    is_dir,
    u.network_id,
    u.device_id);

  if (!res.success())
    PRETTY_THROW(
      elle::sprintf("creating transaction failed. %s",
                    res.response_details));

  if (res.created_transaction_id.empty())
    PRETTY_THROW("transaction_id is empty");

  u.transaction_id = res.created_transaction_id;
}

void
_check_transaction_data(UniqueUser const& u, UniqueUser const& recipient)
{
  auto res = u.client->transaction(u.transaction_id);

  if (res.transaction_id.empty())
    PRETTY_THROW("transaction_id is empty");
  if (res.sender_id != u.user._id)
    PRETTY_THROW("sender_ids don't match");
  if (res.sender_fullname != u.user.fullname)
    PRETTY_THROW("sender_fullname don't match");
  if (res.sender_device_id != u.device_id)
    PRETTY_THROW("sender_device_id don't match");
  if (res.recipient_id != recipient.user._id)
    PRETTY_THROW("recipient_id don't match");
  if (res.recipient_fullname != recipient.user.fullname)
    PRETTY_THROW("recipient_fullname don't match");
  if (res.network_id != u.network_id)
    PRETTY_THROW("network_id don't match");
  if (!res.message.empty())
    PRETTY_THROW("message should be empty");
  if (res.first_filename != filename)
    PRETTY_THROW("first_filenames don't match");
  if (res.files_count != count)
    PRETTY_THROW("files_count don't match");
  if (res.total_size != size)
    PRETTY_THROW("total_sizes don't match");
  if (res.is_directory != is_dir)
    PRETTY_THROW("is_directory don't match");

  if (res.status == 1)
  {
    if (!res.recipient_device_id.empty())
      PRETTY_THROW("we shouldn't know the recipient device id");
    if (!res.recipient_device_name.empty())
      PRETTY_THROW("we shouldn't know the recipient name");
  }
  else if (res.status != 5)
  {
    if (res.recipient_device_id != recipient.device_id)
      PRETTY_THROW("recipient device id don't match");
    if (res.recipient_device_name != recipient.device_name)
      PRETTY_THROW("we shouldn't know the recipient name");
  }
}

int
_count_transactions(UniqueUser const& u)
{
  auto res = u.client->transactions();

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting transactions failed. %s",
                               res.response_details));

  return res.transactions.size();

}

void
_accept_transactions(UniqueUser const& u)
{
  int count = _count_transactions(u);
  // I send to the (USER_COUNT - 1) other users and every users did the same.
  if (count != (USER_COUNT - 1) * 2)
    PRETTY_THROW(
      elle::sprintf("wrong number of notif. expected %i, found %i",
                    (USER_COUNT - 1) * 2, count));

  auto res = u.client->transactions();

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting transactions failed. %s",
                               res.response_details));

  for (std::string const& transaction_id: res.transactions)
  {
    auto transaction_res = u.client->transaction(transaction_id);

    if (!transaction_res.success())
      PRETTY_THROW(elle::sprintf("getting transaction failed. %s",
                                 transaction_res.response_details));

    // Only recipient can accept.
    if (transaction_res.recipient_id != u.user._id)
      continue;

    u.client->update_transaction(transaction_id,
                                 plasma::TransactionStatus::accepted,
                                 u.device_id,
                                 u.device_name);

  }
}

void
_prepare_transactions(UniqueUser const& u)
{
  int count = _count_transactions(u);
  // I send to the (USER_COUNT - 1) other users and every users did the same.
  if (count != (USER_COUNT - 1) * 2)
    PRETTY_THROW(
      elle::sprintf("wrong number of notif. expected %i, found %i",
                    (USER_COUNT - 1) * 2, count));

  auto res = u.client->transactions();

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting transactions failed. %s",
                               res.response_details));

  for (std::string const& transaction_id: res.transactions)
  {
    auto transaction_res = u.client->transaction(transaction_id);

    if (!transaction_res.success())
      PRETTY_THROW(elle::sprintf("getting transaction failed. %s",
                                 transaction_res.response_details));

    // Only recipient can accept.
    if (transaction_res.sender_id != u.user._id)
      continue;

    u.client->update_transaction(transaction_id,
                                 plasma::TransactionStatus::prepared);

  }
}

void
_start_transactions(UniqueUser const& u)
{
  int count = _count_transactions(u);
  // I send to the (USER_COUNT - 1) other users and every users did the same.
  if (count != (USER_COUNT - 1) * 2)
    PRETTY_THROW(
      elle::sprintf("wrong number of notif. expected %i, found %i",
                    (USER_COUNT - 1) * 2, count));

  auto res = u.client->transactions();

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting transactions failed. %s",
                               res.response_details));

  for (std::string const& transaction_id: res.transactions)
  {
    auto transaction_res = u.client->transaction(transaction_id);

    if (!transaction_res.success())
      PRETTY_THROW(elle::sprintf("getting transaction failed. %s",
                                 transaction_res.response_details));

    // Only recipient can prepare.
    if (transaction_res.recipient_id != u.user._id)
      continue;

    u.client->update_transaction(transaction_id,
                                 plasma::TransactionStatus::started);

  }
}

void
_finish_transactions(UniqueUser const& u)
{
  auto res = u.client->transactions();

  if (!res.success())
    PRETTY_THROW(elle::sprintf("getting transactions failed. %s",
                               res.response_details));

  for (std::string const& transaction_id: res.transactions)
  {
    auto transaction_res = u.client->transaction(transaction_id);

    if (!transaction_res.success())
      PRETTY_THROW(elle::sprintf("getting transaction failed. %s",
                                 transaction_res.response_details));

    // Only recipient can finish.
    if (transaction_res.recipient_id != u.user._id)
      continue;

    u.client->update_transaction(transaction_id,
                                 plasma::TransactionStatus::finished);

  }
}

void
test_transactions(Users& users)
{
  for (UniqueUser& u: users)
  {
    for (UniqueUser const& recipient: users)
    {
      // Can't send to your self (on the same device).
      if (recipient.user._id == u.user._id)
        continue;

      _create_transaction(u, recipient);
      _check_transaction_data(u, recipient);
    }
  }

  // Accept each transaction.
  for (UniqueUser& u: users)
    _accept_transactions(u);
  // Prepare each transaction.
  for (UniqueUser& u: users)
    _prepare_transactions(u);
  // Start each transaction.
  for (UniqueUser& u: users)
    _start_transactions(u);
  // Finish each transaction.
  for (UniqueUser& u: users)
    _finish_transactions(u);
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
  test_device(users);
  test_search(users);
  test_delete_network(users, true);

  test_create_network(users);

  test_delete_network(users, false);

  test_transactions(users);

  elle::print("tests done.");

  return 0;
};
