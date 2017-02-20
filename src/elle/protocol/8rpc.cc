#include <iostream>

#include <boost/lexical_cast.hpp>

#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>

#include <elle/protocol/ChanneledStream.hh>
#include <elle/protocol/RPC.hh>
#include <elle/protocol/Serializer.hh>

reactor::Scheduler&
scheduler()
{
  static reactor::Scheduler scheduler;
  return scheduler;
}

std::string self;
#define CATCHES()                                                       \
  catch (std::runtime_error const& e)                                   \
    {                                                                   \
      std::cerr << self << ": fatal error: "                            \
                << e.what() << "." << std::endl;                        \
      return 1;                                                         \
    }                                                                   \
  catch (std::exception const& e)                                       \
    {                                                                   \
      std::cerr << self << ": internal error: "                         \
                << e.what() << "." << std::endl;                        \
      return 2;                                                         \
    }                                                                   \
  catch (...)                                                           \
    {                                                                   \
      std::cerr << self << ": internal unknown error." << std::endl;    \
      return 2;                                                         \
    }

int rpc(std::string const& host, int port, int id,
        std::vector<std::string> const& args)
{
  try
    {
      reactor::Scheduler& sched(scheduler());
      reactor::network::TCPSocket socket(sched, host, port);
      elle::protocol::Serializer serializer(sched, socket);
      elle::protocol::ChanneledStream channels(sched, serializer);
      typedef elle::protocol::RPC<elle::serialize::InputBinaryArchive,
                                     elle::serialize::OutputBinaryArchive>
        RPC;
      RPC rpcs(channels);
      // FIXME
      if (args.size() != 2)
        return 1;
      RPC::RemoteProcedure<void, std::string const&, int> rpc
        ("slug_connect", rpcs, id);
      rpc(args[0], boost::lexical_cast<int>(args[1]));
      return 0;
    }
  CATCHES();
}

int main(int argc, char** argv)
{
  self = argv[0];
  try
    {
      reactor::Scheduler& sched(scheduler());

      if (argc < 4)
        return 1;

      std::string host(argv[1]);
      int port(boost::lexical_cast<int>(argv[2]));
      int id(boost::lexical_cast<int>(argv[3]));
      std::vector<std::string> args;
      for (int i = 4; i < argc; ++i)
        args.push_back(argv[i]);
      reactor::VThread<int> main(sched, "8rpc", std::bind(&rpc, host, port,
                                                          id, args));
      sched.run();
      return main.result();
    }
  CATCHES();
}
