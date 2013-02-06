#include <surface/gap/MetricReporter.hh>
#include <common/common.hh>

#include <elle/print.hh>

#include <cstdlib>
#include <iostream>
#include <ctime>

int main(void)
{
  auto& server = surface::gap::metrics::google::server(
    common::metrics::google_server(),
    common::metrics::google_port(),
    "cd",
    surface::gap::metrics::google::retrieve_id(common::metrics::id_path()));

  // Initialization.
  std::srand(std::time(0));

  std::string base;

  std::pair<std::string, std::string> users{
    base + std::to_string(std::rand() % 1000),
    base + std::to_string(std::rand() % 1000),
  };

  int transaction_base = std::rand() % 100000000;

  std::vector<std::string> transactions;

  for (int i = 0; i < 8; ++i)
  {
    transactions.push_back(std::to_string(transaction_base + i));
  }

// Register failure.
  server.store("test:user:register:attempt");
  server.store("test:user:register:fail", "cd2", "email already taken.");
// Register success.
  server.store("test:user:register:attempt");
  server.store("test:user:register:succeed");

// // Login failure
//   server.store("test:user:login:attempt");
//   server.store("test:user:login:fail", "cd2", "login:password invalid.");

// // Login success.
//   server.store("test:user:login:attempt");
//   server.store("test:user:login:succeed");
//   server.update_user(users.first);

// // Logout failure.
//   server.update_user(users.first);
//   server.store("test:user:logout:attempt");
//   server.store("test:user:logout:fail", "cd2", "unknown error");

// //  Login success.
//   server.store("test:user:login:attempt");
//   server.store("test:user:login:succeed");
//   server.update_user(users.first);


// // Send file process.
//   server.store("test:transaction:create:attempt",
//                {{"cd2", transactions[3]},{"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:transaction:create:succeed",
//                {{"cd2", transactions[3]}, {"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:transaction:accept:attempt",  "cd2", transactions[3]);
//   server.store("test:transaction:accept:succeed",  "cd2", transactions[3]);
//   server.store("test:transaction:prepare:attempt", "cd2", transactions[3]);
//   server.store("test:transaction:prepare:succeed", "cd2", transactions[3]);
//   server.store("test:transaction:start:attempt",   "cd2", transactions[3]);
//   server.store("test:transaction:start:succeed",   "cd2", transactions[3]);
//   server.store("test:transaction:finish:attempt",  "cd2", transactions[3]);
//   server.store("test:transaction:finish:succeed",  "cd2", transactions[3]);

//   // Self share, cancel while downloading.
//   server.store("test:ux:drop:self");
//   server.store("test:network:create:attempt");
//   server.store("test:network:create:success");
//   server.store("test:transaction:create:attempt", {{"cd2", transactions[4]}, {"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:transaction:create:succeed", {{"cd2", transactions[4]}, {"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:accept");
//   server.store("test:transaction:accept:attempt", "cd2", transactions[4]);
//   server.store("test:transaction:accept:succeed", "cd2", transactions[4]);
//   server.store("test:ux:click:panel:transfer:close:nowhere");
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:close:button");
//   server.store("test:transaction:prepare:attempt", "cd2", transactions[4]);
//   server.store("test:network:user:add:attempt");
//   server.store("test:network:user:add:succeed");
//   server.store("test:transaction:prepare:succeed", "cd2", transactions[4]);
//   server.store("test:transaction:start:attempt", "cd2", transactions[4]);
//   server.store("test:transaction:start:succeed", "cd2", transactions[4]);
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:cancel");
//   server.store("test:transaction:cancel:attempt", "cd2", transactions[4]);
//   server.store("test:network:delete:attempt");
//   server.store("test:network:delete:succeed");
//   server.store("test:transaction:cancel:succeed", "cd2", transactions[4]);
//   server.store("test:ux:click:transfer:close:nowhere");

//   // Self share.
//   server.store("test:ux:drop:self");
//   server.store("test:network:create:attempt");
//   server.store("test:network:create:success");
//   server.store("test:transaction:create:attempt", {{"cd2", transactions[4]}, {"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:transaction:create:succeed", {{"cd2", transactions[4]}, {"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:accept");
//   server.store("test:transaction:accept:attempt", "cd2", transactions[4]);
//   server.store("test:transaction:accept:succeed", "cd2", transactions[4]);
//   server.store("test:ux:click:panel:transfer:close:nowhere");
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:close:button");
//   server.store("test:transaction:prepare:attempt", "cd2", transactions[4]);
//   server.store("test:network:user:add:attempt");
//   server.store("test:network:user:add:succeed");
//   server.store("test:transaction:prepare:succeed", "cd2", transactions[4]);
//   server.store("test:transaction:start:attempt", "cd2", transactions[4]);
//   server.store("test:transaction:start:succeed", "cd2", transactions[4]);
//   server.store("test:transaction:finish:attempt", "cd2", transactions[4]);
//   server.store("test:transaction:finish:succeed", "cd2", transactions[4]);
//   server.store("test:network:delete:attempt");
//   server.store("test:network:delete:succeed");

//   // P10.2:
//   server.update_user(users.first);
//   server.store("test:ux:drop:self");
//   server.store("test:network:create");
//   server.store("test:transaction:create", {{"cd2", transactions[5]}, {"cm1", "32402"}, {"cm2", "3"}});
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:accept");
//   server.store("test:transaction:accept", "cd2", transactions[5]);
//   server.store("test:ux:click:panel:transfer:close:nowhere");
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:close:button");
//   server.store("test:network:user:add");
//   server.store("test:transaction:start", "cd2", transactions[5]);
//   server.store("test:transaction:finish", "cd2", transactions[5]);
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:panel:transfer:access");
//   server.store("test:network:delete");
//   server.store("test:ux:click:panel:transfer:close:nowhere");


//   server.store("test:ux:drop:favorite");
//   server.store("test:ux:drop:bar");
//   server.store("test:ux:drop:nowhere");
//   server.store("test:ux:click:self");
//   server.store("test:ux:click:favorite");
//   server.store("test:ux:click:transfer:self");
//   server.store("test:ux:click:transfer:favorite");
//   server.store("test:ux:click:transfer:user");
//   server.store("test:ux:click:transfer:social");
//   server.store("test:ux:click:transfer:email");
//   server.store("test:ux:click:nowhere");
//   server.store("test:ux:click:panel:transfer:open");
//   server.store("test:ux:click:panel:transfer:close:button");
//   server.store("test:ux:click:panel:transfer:close:nowhere");
//   server.store("test:ux:click:panel:transfer:nowhere");
//   server.store("test:ux:click:panel:transfer:accept");
//   server.store("test:ux:click:panel:transfer:deny");
//   server.store("test:ux:click:panel:transfer:cancel");
//   server.store("test:ux:click:panel:transfer:access");
//   server.store("test:ux:click:bar:search:field");
//   server.store("test:ux:click:bar:search:dropzone:empty");
//   server.store("test:ux:click:bar:search:dropzone:open");
//   server.store("test:ux:click:bar:search:dropzone:close:nowhere");
//   server.store("test:ux:click:bar:search:dropzone:close:button");
//   server.store("test:ux:click:bar:search:dropzone:nowhere");
//   server.store("test:ux:click:bar:search:dropzone:remove:item");
//   server.store("test:ux:click:bar:search:dropzone:remove:all");
//   server.store("test:ux:keyboard:bar:search");
//   server.store("test:ux:click:bar:search:invite");
//   server.store("test:ux:click:bar:search:share");
//   server.store("test:ux:keyboard:bar:search:invite");
//   server.store("test:ux:keyboard:bar:search:share");
//   server.store("test:ux:click:panel:search:open");
//   server.store("test:ux:click:panel:search:close:nowhere");
//   server.store("test:ux:keyboard:panel:search:close:empty");
//   server.store("test:ux:click:panel:search:select:user");
//   server.store("test:ux:click:panel:search:select:social");
//   server.store("test:ux:click:panel:search:select:other");
//   server.store("test:ux:keyboard:panel:search:select:user");
//   server.store("test:ux:keyboard:panel:search:select:social");
//   server.store("test:ux:keyboard:panel:search:select:other");
//   server.store("test:ux:click:panel:notifications:open");
//   server.store("test:ux:click:panel:notifications:close:icon");
//   server.store("test:ux:click:panel:notifications:close:nowhere");
//   server.store("test:ux:click:panel:notifications:nowhere");
//   server.store("test:ux:click:panel:notifications:accept");
//   server.store("test:ux:click:panel:notifications:deny");
//   server.store("test:ux:click:panel:notifications:cancel");
//   server.store("test:ux:click:panel:notifications:access");

  elle::printf("Test done.");

  return 0;
}
