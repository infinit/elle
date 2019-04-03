#include <fstream>

#include <boost/range/adaptor/transformed.hpp>

#include <elle/das/cli.hh>
#include <elle/das/named.hh>
#include <elle/filesystem.hh>
#include <elle/filesystem/path.hh>
#include <elle/nbd/Server.hh>

using boost::adaptors::transformed;

ELLE_DAS_SYMBOL(file);
ELLE_DAS_SYMBOL(help);
ELLE_DAS_SYMBOL(interface);
ELLE_DAS_SYMBOL(port);
ELLE_DAS_SYMBOL(port_file);

ELLE_LOG_COMPONENT("nbd-file");

class FileDevice
  : public elle::nbd::Server::Device
{
public:
  using Super = elle::nbd::Server::Device;
  FileDevice(elle::fs::path const& p)
    : Super(p.string(), elle::fs::file_size(p))
    , _f(p)
  {
    if (!this->_f)
      elle::err("unable to open {}", p);
  }

  elle::Buffer
  read(uint64_t offset, uint32_t length) override
  {
    elle::Buffer data(length);
    this->_f.seekg(offset);
    this->_f.read(reinterpret_cast<char*>(data.mutable_contents()), length);
    return data;
  }

  void
  write(uint64_t offset, elle::Buffer data) override
  {
    this->_f.seekp(offset);
    this->_f.write(
      reinterpret_cast<char*>(data.contents()), data.size());
    // Since the NBD protocol itself should be mindful of sending write requests
    // in a batched way, flushing on every write request seems reasonable.
    this->_f.flush();
  }

  void
  write_zeroes(uint64_t offset, uint32_t length) override
  {
    auto constexpr bs = 4096ul;
    char zeroes[bs] = {0};
    for (auto c = offset; c < offset + length; c += bs)
      this->_f.write(&zeroes[0],
                 std::min(bs, offset + length - c));
    this->_f.flush();
  }

  void
  sync() override
  {
    ::fdatasync(std::filesystem::get_native_handle(this->_f));
  }

  ELLE_ATTRIBUTE(std::fstream, f);
};

static
void
_nbd_file(elle::fs::path path,
          std::string interface,
          int port,
          boost::optional<std::string> port_file)
{
  auto addr = [&]
              {
                try
                {
                  return boost::asio::ip::address::from_string(interface);
                }
                catch (boost::system::system_error const& e)
                {
                  elle::err(
                    "invalid interface {}: {}", interface, e.code().message());
                }
              }();
  elle::nbd::Server server(addr, port);
  if (port_file)
    server.listening().connect(
      [path = *port_file] (int port)
      {
        std::fstream f(path, std::ios::out);
        if (!f)
          elle::err("unable to open port file: {}", path);
        elle::print(f, "{}\n", port);
        if (f.fail())
          elle::err("unable to write port to {}", path);
      });
  FileDevice d(path);
  server.add(d);
  server.run();
}

auto const nbd_file = elle::das::named::function(
  _nbd_file, file,
  interface = "0.0.0.0",
  port = 10809,
  port_file = boost::none);

int main(int argc, char** argv)
{
  auto const proto = nbd_file.prototype().extend(help = false);
  elle::reactor::Scheduler sched;
  elle::reactor::Thread main(
    sched, "nbd-file",
    [&]
    {
      auto opts = elle::das::cli::Options{
        {"file", {'f', "File to serve"}},
        {"interface", {'i', "Interface to serve on"}},
        {"port", {'p', "Port to serve on"}},
        {"port_file", {'o', "File to write the port we listen on to"}},
      };
      elle::das::cli::call(
        proto,
        [&] (bool help, auto&& ... rest) -> void
        {
          if (help)
             elle::print(std::cout,
                         "{}: export a file over NBD.\n{}",
                         argv[0],
                         elle::das::cli::help(proto, opts));
          else
            _nbd_file(std::forward<decltype(rest)>(rest)...);
        },
        elle::make_vector(
          elle::as_range(argv + 1, argv + argc) |
          transformed([](char const* s) { return std::string(s); })),
        opts);
    });
  try
  {
    sched.run();
  }
  catch (elle::Error const& e)
  {
    elle::print(std::cerr, "{}: fatal error: {}\n", argv[0], e);
    return 1;
  }
}
