#pragma once

#include <elle/attribute.hh>
#include <elle/reactor/network/TCPServer.hh>

namespace elle
{
  namespace nbd
  {
    class Server
    {
    public:
      Server(boost::asio::ip::address host, int port = 10809);
      void
      run();


    public:
      class Device
      {
      public:
        Device(std::string name, std::size_t size);

        virtual
        elle::Buffer
        read(uint64_t offset, uint32_t length) = 0;
        virtual
        void
        write(uint64_t offset, elle::Buffer data) = 0;
        virtual
        void
        write_zeroes(uint64_t offset, uint32_t length) = 0;
        virtual
        void
        sync() = 0;
        virtual
        void
        trim(uint64_t offset, uint32_t length);
        virtual
        void
        cache(uint64_t offset, uint32_t length);
        ELLE_ATTRIBUTE_R(std::string, name);
        ELLE_ATTRIBUTE_R(std::size_t, size);
      };
      void
      add(Device& d);

    private:
      Device&
      _options_haggling(reactor::network::Socket& sock, bool zeroes);

    private:
      ELLE_ATTRIBUTE_R(boost::asio::ip::address, host);
      ELLE_ATTRIBUTE_R(int, port);
      ELLE_ATTRIBUTE(elle::reactor::network::TCPServer, server);
      ELLE_ATTRIBUTE_R(std::vector<std::reference_wrapper<Device>>, devices);
    };
  }
}
