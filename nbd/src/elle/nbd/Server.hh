#pragma once

#include <boost/signals2.hpp>

#include <elle/attribute.hh>
#include <elle/reactor/network/TCPServer.hh>

namespace elle
{
  namespace nbd
  {
    /// A network block device server that exposes registered devices and maps
    /// network calls to relevant `Device` methods.
    class Server
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a server listening on the given interface and port.
      ///
      /// @param host Interface to listen on.
      /// @param port Port to listen one.
      Server(boost::asio::ip::address host, int port = 10809);
      ELLE_ATTRIBUTE_R(boost::asio::ip::address, host);
      ELLE_ATTRIBUTE_R(int, port);

    /*--------.
    | Devices |
    `--------*/
    public:
      /// A block device to register and expose on a `Server`.
      class Device
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        Device(std::string name, std::size_t size);
        ELLE_ATTRIBUTE_R(std::string, name);
        ELLE_ATTRIBUTE_R(std::size_t, size);

      /*---------------.
      | Implementation |
      `---------------*/
      public:
        /// Read data from the device.
        ///
        /// @param offset Index of the first byte to be read.
        /// @param length Number of bytes to be read.
        /// @return Data located at `[offset, offset + length[` on the device.
        virtual
        elle::Buffer
        read(uint64_t offset, uint32_t length) = 0;
        /// Write data to the device.
        ///
        /// @param offset Index of the first byte to be written.
        /// @param data Data to write.
        virtual
        void
        write(uint64_t offset, elle::Buffer data) = 0;
        /// Write zeroes to the device.
        ///
        /// This is a subcase of the generic write to improve the common case
        /// where large portions of the device must be zero'd.
        ///
        /// @param offset Index of the first byte to be zero'd.
        /// @param length Number of bytes to be zero'd.
        virtual
        void
        write_zeroes(uint64_t offset, uint32_t length) = 0;
        /// Block until all written data have reached permanent storage.
        virtual
        void
        sync() = 0;
        /// Discard data from the block device.
        ///
        /// Clients will make no asumption about the state of bytes previously
        /// trimmed, hence the implementation may entirely discard it. Taking no
        /// action, which is the default implementation, is correct.
        ///
        /// @param offset Index of the first byte to discard.  @param length
        /// Number of bytes to discerd.
        virtual
        void
        trim(uint64_t offset, uint32_t length);
        /// Prepare data for reading.
        ///
        /// Hint that the specified area will be read in the future, and caching
        /// it to memory for instance could speed up things. Takin no action,
        /// which is the default implementation, is correct.
        ///
        /// @param offset Index of the first byte to cache.  @param length
        /// Number of bytes to cache.
        virtual
        void
        cache(uint64_t offset, uint32_t length);
      };
      /// Register a device to be exposed by the server.
      void
      add(Device& d);
      ELLE_ATTRIBUTE_R(std::vector<std::reference_wrapper<Device>>, devices);

    /*-------.
    | Server |
    `-------*/
    public:
      /// Synchronously accept connections and respond to requests.
      void
      run();
      /// Signal that we are extended on the given port.
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (int)>, listening);
    private:
      Device&
      _options_haggling(reactor::network::Socket& sock, bool zeroes);
      ELLE_ATTRIBUTE(elle::reactor::network::TCPServer, server);
    };
  }
}
