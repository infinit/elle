#include <elle/nbd/Server.hh>

#include <chrono>
#include <fstream>

#include <boost/endian/conversion.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <elle/bitfield.hh>
#include <elle/enum.hh>
#include <elle/log.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/TCPSocket.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/utility/Move.hh>
#include <elle/Exit.hh>

ELLE_LOG_COMPONENT("elle.nbd.Server");

using boost::endian::endian_reverse;
using namespace std::chrono_literals;

namespace elle
{
  namespace nbd
  {
    ELLE_ENUM(
      Info,
      (export_, 0),
      (name, 1),
      (description, 2),
      (block_size, 3));

    ELLE_ENUM(
      Option,
      // Client wants to select a named export (is followed by name of export).
      (export_name, 1),
      // Client wishes to abort negotiation.
      (abort, 2),
      // Client request list of supported exports (not followed by data).
      (list, 3),
      // Client wishes to initiate TLS.
      (starttls, 5),
      // Client wants information about the given export.
      (info, 6),
      // Client wants to select the given and move to the transmission phase.
      (go, 7),
      // Client wishes to use structured replies during the transmission phase.
      (structured_replies, 8));

    // Replies the server can send during negotiation
    ELLE_ENUM(
      Response,
      // ACK a request. Data: option number to be acked
      (ack, 1),
      // Reply to NBD_OPT_LIST (one of these per server; must be followed by
      // NBD_REP_ACK to signal the end of the list
      (server, 2),
      // Reply to NBD_OPT_INFO
      (info, 3),
      // If the high bit is set, the reply is an error
      (flag_error, (1 << 31)),
      // Client requested an option not understood by this version of the server
      (err_unsup, (1 | flag_error)),
      // Client requested an option not allowed by server configuration. (e.g.,
      // the option was disabled)
      (err_policy, (2 | flag_error)),
      // Client issued an invalid request
      (err_invalid, (3 | flag_error)),
      // Option not supported on this platform
      (err_platform, (4 | flag_error)),
      // TLS required
      (err_tls_reqd, (5 | flag_error)),
      // NBD_OPT_INFO or ..._GO requested on unknown export
      (err_unknown, (6 | flag_error)),
      // Server is not willing to serve the export without the block size being
      // negotiated
      (err_block_size_reqd, (8 | flag_error)));

    ELLE_ENUM(
      HandshakeFlag,
      // new-style export that actually supports extending.
      (fixed_newstyle, 1 << 0),
      // we won't send the 128 bits of zeroes if the client sends
      // NBD_FLAG_C_NO_ZEROES.
      (no_zeroes, 1 << 1));

    ELLE_ENUM(
      Command,
      (read, 0),
      (write, 1),
      (disc, 2),
      (flush, 3),
      (trim, 4),
      (cache, 5),
      (write_zeroes, 6),
      (block_status, 7),
      (resize, 8));

    ELLE_ENUM(
      TransmissionFlag,
      (has_flags, 1 << 0),
      (read_only, 1 << 1),
      (send_flush, 1 << 2),
      (send_fua, 1 << 3),
      (rotational, 1 << 4),
      (send_trim, 1 << 5),
      (send_write_zeroes, 1 << 6),
      (send_df, 1 << 7),
      (can_multi_conn, 1 << 8),
      (send_resize, 1 << 9),
      (send_cache, 1 << 10));

    ELLE_ENUM(
      Error,
      (none, 0),
      (perm, 1),
      (io, 5),
      (nomem, 12),
      (inval, 22),
      (nospc, 28),
      (overflow, 75),
      (shutdown, 108));
  }
}

ELLE_BITFIELD(elle::nbd::HandshakeFlag);
ELLE_BITFIELD(elle::nbd::TransmissionFlag);

namespace elle
{
  namespace nbd
  {
    static
    uint16_t
    read_16(reactor::network::Socket& sock)
    {
      return endian_reverse(
        *reinterpret_cast<uint16_t*>(sock.read(2).contents()));
    }

    static
    uint32_t
    read_32(reactor::network::Socket& sock)
    {
      return endian_reverse(
        *reinterpret_cast<uint32_t*>(sock.read(4).contents()));
    }

    static
    uint64_t
    read_64(reactor::network::Socket& sock)
    {
      return endian_reverse(
        *reinterpret_cast<uint64_t*>(sock.read(8).contents()));
    }

    Server::Server(boost::asio::ip::address host, int port)
      : _host(host)
      , _port(port)
      , _server()
    {}

    void
    Server::run()
    {
      this->_server.listen(this->_host, this->_port);
      this->_listening(this->_server.port());
      elle::With<elle::reactor::Scope>() <<
        [&] (elle::reactor::Scope& scope)
        {
          while (true)
          {
            auto sock = std::make_shared(std::move(*this->_server.accept()));
            scope.run_background(
              elle::print("{}", sock->peer()),
              [&, sock] {
                try
                {
                  ELLE_TRACE_SCOPE("handle connection from {}", sock->peer());
                  bool zeroes = true;
                  ELLE_DEBUG("initiate handshake")
                  {
                    sock->write("NBDMAGIC");
                    sock->write("IHAVEOPT");
                    sock->write(endian_reverse(uint16_t(HandshakeFlag::fixed_newstyle |
                                                        HandshakeFlag::no_zeroes)));
                    {
                      auto flags = HandshakeFlag(read_32(*sock));
                      auto const consume = [&] (HandshakeFlag flag)
                                           {
                                             auto res = bool(flags & flag);
                                             flags = flags & ~flag;
                                             return res;
                                           };
                      if (consume(HandshakeFlag::fixed_newstyle))
                        ELLE_DEBUG("client supports fixed newstyle handshake");
                      else
                        ELLE_TRACE(
                          "client does not support fixed newstyle handshake");
                      if (consume(HandshakeFlag::no_zeroes))
                      {
                        zeroes = false;
                        ELLE_DEBUG("client requires no zeroes");
                      }
                      if (bool(flags))
                      {
                        ELLE_WARN("rejecting client with unknown flags: %x", flags);
                        return;
                      }
                    }
                  }
                  auto& device = this->_options_haggling(*sock, zeroes);
                  while (true)
                  {
                    // Consume magic number
                    {
                      static constexpr uint8_t magic_expected[4] =
                        {0x25, 0x60, 0x95, 0x13};
                      auto magic = sock->read(4);
                      if (magic != elle::ConstWeakBuffer(magic_expected))
                        ELLE_WARN("invalid request magic: {}", magic);
                    }
                    auto flags = read_16(*sock);
                    auto cmd = Command(read_16(*sock));
                    auto handle =
                      *reinterpret_cast<uint64_t*>(sock->read(8).contents());
                    auto offset = read_64(*sock);
                    auto length = read_32(*sock);
                    auto const resp =
                      [&] (Error error = Error::none)
                      {
                        static constexpr uint8_t magic[4] =
                          {0x67, 0x44, 0x66, 0x98};
                        sock->write(magic);
                        sock->write(uint32_t(error));
                        sock->write(handle);
                      };
                    ELLE_DUMP_SCOPE("received command: {}({}, {}, {}, {})",
                                    cmd, flags, handle, offset, length);
                    if (offset + length > device.size())
                      if (cmd == Command::write || cmd == Command::write_zeroes)
                      {
                        ELLE_TRACE("{} is out of bound", cmd);
                        resp(Error::nospc);
                        break;
                      }
                      else if (cmd == Command::read || cmd == Command::trim)
                      {
                        ELLE_TRACE("{} is out of bound", cmd);
                        resp(Error::inval);
                        break;
                      }
                    switch (cmd)
                    {
                      case Command::read:
                      {
                        ELLE_TRACE_SCOPE("read {} bytes at {}", length, offset);
                        resp();
                        auto data = device.read(offset, length);
                        sock->write(data);
                        break;
                      }
                      case Command::write:
                      case Command::write_zeroes:
                      {
                        ELLE_TRACE_SCOPE(
                          "write {} {} at {}", length,
                          cmd == Command::write ? "bytes" : "zeroes", offset);
                        if (cmd == Command::write)
                          // FIXME: use kernel level no-copy
                          device.write(offset, sock->read(length));
                        else
                          device.write_zeroes(offset, length);
                        resp();
                        break;
                      }
                      case Command::disc:
                      {
                        ELLE_TRACE_SCOPE("client requests disconnect");
                        throw elle::Exit();
                      }
                      case Command::flush:
                      {
                        ELLE_TRACE_SCOPE("flush");
                        device.sync();
                        resp();
                        break;
                      }
                      case Command::trim:
                      {
                        ELLE_TRACE_SCOPE("trim {} bytes at {}", length, offset);
                        device.trim(offset, length);
                        resp();
                        break;
                      }
                      case Command::cache:
                      {
                        ELLE_TRACE_SCOPE("cache {} bytes at {}", length, offset);
                        device.cache(offset, length);
                        resp();
                        break;
                      }
                      case Command::block_status:
                      case Command::resize:
                      {
                        ELLE_TRACE_SCOPE("unsupported {} request", cmd);
                        resp(Error::inval);
                        break;
                      }
                      default:
                      {
                        ELLE_WARN("unrecognized client request: {}", int(cmd));
                        resp(Error::inval);
                      }
                    }
                  }
                }
                catch (elle::Exit const&)
                {
                  ELLE_TRACE("gracefully ending session");
                }
                catch (elle::reactor::network::ConnectionClosed const&)
                {
                  ELLE_TRACE("client closed connection");
                }
                catch (elle::Error const& e)
                {
                  ELLE_WARN("client error: {}", e);
                }
              });
          }
        };
    }

    Server::Device&
    Server::_options_haggling(reactor::network::Socket& sock,
                              bool zeroes)
    {
      ELLE_DEBUG("initiate options haggling")
        while (true)
        {
          auto magic = sock.read(8);
          if (magic != elle::ConstWeakBuffer("IHAVEOPT"))
            elle::err("unknow magic during options haggling: {}", magic);
          auto option = Option(read_32(sock));
          ELLE_DUMP_SCOPE("received option: {}", option);
          auto data = [&]
                      {
                        auto length = read_32(sock);
                        ELLE_DUMP("option data length: {}", length);
                        auto data = sock.read(length);
                        if (length > 0)
                          ELLE_DUMP("option data: {}", data);
                        return data;
                      }();
          auto const resp_header =
            [&] (Option option, Response rep)
            {
              sock.write(endian_reverse(uint64_t(0x3e889045565a9)));
              sock.write(endian_reverse(uint32_t(option)));
              sock.write(endian_reverse(uint32_t(rep)));
            };
          auto const resp =
            [&] (Option option, Response rep)
            {
              ELLE_DUMP_SCOPE("send response: {}", rep);
              resp_header(option, rep);
              sock.write(endian_reverse(uint32_t(0)));
            };
          auto const resp_export =
            [&] (std::size_t size)
            {
              sock.write(endian_reverse(uint64_t(size)));
              sock.write(endian_reverse(
                            uint16_t(
                              TransmissionFlag::has_flags  |
                              TransmissionFlag::send_flush |
                              TransmissionFlag::send_fua   |
                              TransmissionFlag::send_trim  |
                              TransmissionFlag::send_df    |
                              TransmissionFlag::send_cache)));
            };
          switch (option)
          {
            case Option::export_name:
            {
              auto& device =
                [&] () -> Device&
                {
                  if (this->_devices.empty())
                    elle::err("no devices available");
                  if (data == elle::ConstWeakBuffer(""))
                    return this->_devices.front().get();
                  else
                  {
                    auto res = boost::range::find_if(
                      this->_devices,
                      [&] (Device const& d)
                      { return data == d.name(); });
                    if (res != this->_devices.end())
                      return (*res).get();
                    elle::err("no such device: {}", data);
                  }
                }();
              ELLE_TRACE_SCOPE("export volume {}", device.name());
              resp_export(device.size());
              if (zeroes)
              {
                uint8_t zeroes[124];
                memset(&zeroes, 0, sizeof(zeroes));
                sock.write(zeroes);
              }
              return device;
            }
            case Option::abort:
            {
              ELLE_TRACE("abort session upon client request")
                resp(Option::abort, Response::ack);
              throw elle::Exit();
            }
            case Option::list:
            {
              ELLE_TRACE_SCOPE("list exports");
              for (auto const& d: this->_devices)
              {
                auto const& name = d.get().name();
                resp_header(Option::list, Response::server);
                sock.write(
                  endian_reverse(uint32_t(4 + name.size())));
                sock.write(
                  endian_reverse(uint32_t(name.size())));
                sock.write(name);
              }
              resp(Option::list, Response::ack);
              break;
            }
            case Option::starttls:
            {
              resp(Option::abort, Response::err_unsup);
              break;
            }
            case Option::info:
            case Option::go:
            {
              // nbd-client doesn't use these, so I have no reliable
              // way of testing it for now.
              ELLE_ERR("OPT_INFO AND OPT_GO NOT IMPLEMENTED");
              resp(option, Response::err_unsup);
              // resp_header(Option::info, Response::info);
              // sock.write(endian_reverse(uint32_t(12)));
              // sock.write(endian_reverse(uint16_t(Info::export_)));
              // resp_export();
              // resp_header(Option::info, Response::info);
              // sock.write(endian_reverse(uint32_t(2 + path.size())));
              // sock.write(endian_reverse(uint16_t(Info::name)));
              // sock.write(path.string());
              // resp(Option::info, Response::ack);
              // if (option == Option::go)
              //   return XXX;
              break;
            }
            case Option::structured_replies:
            {
              if (data.size() == 0)
                // nbd-client doesn't seem to negotiate structured
                // replies, so I have no reliable way of testing it
                // for now.
                resp(Option::structured_replies,
                     Response::err_unsup);
              else
                resp(Option::structured_replies,
                     Response::err_invalid);
              break;
            }
            default:
            {
              ELLE_WARN("unknown client option: {}", int(option));
              resp(option, Response::err_unsup);
              break;
            }
          }
        }
    }

    void
    Server::add(Device& d)
    {
      this->_devices.emplace_back(d);
    }

    Server::Device::Device(std::string name, std::size_t size)
      : _name(std::move(name))
      , _size(size)
    {}

    void
    Server::Device::trim(uint64_t, uint32_t)
    {}

    void
    Server::Device::cache(uint64_t offset, uint32_t length)
    {}
  }
}
