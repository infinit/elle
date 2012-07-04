#ifndef ELLE_NETWORK_TCPSOCKET_HXX
#define ELLE_NETWORK_TCPSOCKET_HXX

#include <elle/idiom/Close.hh>
# include <elle/print.hh>
# include <reactor/scheduler.hh>
# include <reactor/thread.hh>
#include <elle/idiom/Open.hh>

#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Scheduler.hh>

#include <elle/standalone/Report.hh>

#include <elle/network/Parcel.hh>
#include <elle/network/Header.hh>
#include <elle/network/Data.hh>

#include <elle/Manifest.hh>

#include <elle/log.hh>
#include <elle/print.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

# include <protocol/Channel.hh>
# include <protocol/ChanneledStream.hh>
# include <protocol/Serializer.hh>

namespace elle
{
  namespace network
  {
    template <typename I>
    void
    TCPSocket::send(const I inputs)
    {
      send(inputs, *_channels);
    }

    template <typename I>
    void
    TCPSocket::send(const I inputs, infinit::protocol::Stream& channel)
    {
      ELLE_LOG_TRACE_COMPONENT("Infinit.Network");
      ELLE_LOG_TRACE_SCOPE("%s: send packet %s", *this, inputs.tag);

      Data body;
      body.Writer() << inputs;

      infinit::protocol::Packet packet;
      serialize::OutputBinaryArchive whole(packet);
      whole << inputs.tag << body;

      packet.flush();

      {
        reactor::Lock lock(elle::concurrency::scheduler(), _stream_write_lock);
        channel.write(packet);
      }
    }

    ///
    /// this method receives a packet by blocking.
    ///
    template <typename O>
    Status
    TCPSocket::receive(infinit::protocol::Stream& channel, O outputs)
    {
      ELLE_LOG_TRACE_COMPONENT("Infinit.Network");
      ELLE_LOG_TRACE_SCOPE("%s: wait for answer.", *this);

      infinit::protocol::Packet packet(channel.read());
      Parcel* parcel = _read_parcel(packet);

      // check the tag.
      if (parcel->header->tag != outputs.tag)
        {
          //
          // in this case, the blocked fiber received a message whose
          // tag does not match the expected one.
          //
          Tag           tag = parcel->header->tag;

          // first, test if the message received is an error, if so, append
          // the report to the local one.
          if (tag == TagError)
            {
              standalone::Report    report;

              parcel->data->Reader() >> report;

              // report the remote error.
              transpose(report);
            }
          else
            {
              delete parcel;
              parcel = nullptr;
              escape("fiber was awaken by a packet with the wrong tag");
            }

          delete parcel;
          // in any case, return an error from the Receive() method.
          escape("received a packet with an unexpected tag '%u'", tag);
        }

      try
        {
          ELLE_LOG_TRACE("%s: reading data parcel of size %u",
                         *this, parcel->data->Size());
          parcel->data->Reader() >> outputs;
          delete parcel;
        }
      catch (std::exception const& err)
        {
          delete parcel;
          escape(err.what());
        }

      return Status::Ok;
    }

    ///
    /// this method sends and waits for an appropriate response.
    ///
    template <typename I,
              typename O>
    Status              TCPSocket::Call(const I                 inputs,
                                        O                       outputs)
    {
      ELLE_LOG_TRACE_COMPONENT("Infinit.Network");

      infinit::protocol::Channel channel(*_channels);

      // Send the inputs.
      ELLE_LOG_TRACE("%s: call tag %s and await tag %s",
                     *this, inputs.tag, outputs.tag)
        {
          this->send(inputs, channel);
          if (this->receive(channel, outputs) == Status::Error)
            escape("unable to receive the outputs");
        }

      return Status::Ok;
    }

    ///
    /// this method replies to the message which has just been received i.e
    /// whose tag is specified in the current session.
    ///
    template <typename I>
    void
    TCPSocket::reply(const I inputs)
    {
      this->send(inputs, *current_context().channel);
    }
  }
}

#endif
