#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <elle/reactor/asio.hh>
#include <boost/algorithm/string.hpp>

/* Listen on an unix socket, forward mount requests to fusermount
*/
using namespace boost::asio;
int main(int argc, char** argv)
{
  const char* server_socket_path = "/tmp/fnetusermount.sock";
  // for consistency with client side
  const char* sspenv = getenv("FNETUSERMOUNT_SOCKET_PATH");
  if (sspenv)
    server_socket_path = sspenv;
  bool debug = getenv("FNETUSERMOUNT_DEBUG");
  io_service io;
  local::stream_protocol::acceptor a(io, server_socket_path);
  while (true)
  {
    local::stream_protocol::socket s(io);
    a.accept(s);
    boost::asio::streambuf b;
    read_until(s, b, '\n');
    std::istream is(&b);
    std::string line;
    std::getline(is, line);
    if (debug)
      fprintf(stderr, "'%s'\n", line.c_str());
    std::vector<std::string> sargs;
    boost::algorithm::split(sargs, line, boost::algorithm::is_any_of(" "));

    std::vector<const char*> nargv;
    nargv.resize(sargs.size()+2);
    nargv[0] = "fusermount";
    for (unsigned int i=1; i<=sargs.size(); ++i)
      nargv[i] = sargs[i-1].c_str();
    nargv[nargv.size()-1] = nullptr;
    setenv("_FUSE_COMMFD", std::to_string(s.native_handle()).c_str(), 1);
    int child = fork();
    if (child == 0)
    {
      execvp("fusermount", (char**)&nargv[0]);
      return 0;
    }
    else
    {
      int status;
      waitpid(child, &status, 0);
    }
  }
}