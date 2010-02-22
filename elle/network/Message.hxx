//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Message.hxx
//
// created       julien quintard   [sun feb 21 21:24:29 2010]
// updated       julien quintard   [mon feb 22 14:35:33 2010]
//

#ifndef ELLE_NETWORK_MESSAGE_HXX
#define ELLE_NETWORK_MESSAGE_HXX

namespace elle
{
  namespace network
  {

//
// ---------- methods ---------------------------------------------------------
//
/*
    ///
    /// these methods take a set of values and serialize them in
    /// a packet.
    ///
    /// \todo this method could be optimised as two archives are created.
    ///       it would be better to provide Archive.Seek() in order to first
    ///       write a zero size, then seek, and re-write the proper value.
    ///       in other words, we need a more flexible archive system that
    ///       allows the caller to find an already serialized item, change it
    ///       etc.
    ///

    ///
    /// the zero-parameter packing method.
    ///
    template <const Tag G>
    Status		Message::Pack(Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// single-parameter.
    ///
    template <const Tag G,
	      typename T1>
    Status		Message::Pack(const T1&			o1,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// two-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// three-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// four-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      const T4&			o4,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// five-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      const T4&			o4,
				      const T5&			o5,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// six-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      const T4&			o4,
				      const T5&			o5,
				      const T6&			o6,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// seven-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      const T4&			o4,
				      const T5&			o5,
				      const T6&			o6,
				      const T7&			o7,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// eight-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      const T4&			o4,
				      const T5&			o5,
				      const T6&			o6,
				      const T7&			o7,
				      const T8&			o8,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }

    ///
    /// nine-parameter.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		Message::Pack(const T1&			o1,
				      const T2&			o2,
				      const T3&			o3,
				      const T4&			o4,
				      const T5&			o5,
				      const T6&			o6,
				      const T7&			o7,
				      const T8&			o8,
				      const T9&			o9,
				      Packet&			packet)
    {
      Archive		archive;

      // create the inner archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the objects.
      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the objects");

      // create the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the archive with some metadata.
      if (packet.Serialize(Message::Name,
			   G,
			   archive.size,
			   archive) == StatusError)
	escape("unable to serialize the metadata");

      leave();
    }
*/
  }
}

#endif
