//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Packet.hxx
//
// created       julien quintard   [sun nov 29 20:31:28 2009]
// updated       julien quintard   [sun nov 29 22:11:18 2009]
//

#ifndef ELLE_NETWORK_PACKET_HXX
#define ELLE_NETWORK_PACKET_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Tag.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class stores the type of the template itself and is used
    /// by the PacketMap template.
    ///
    template <typename T>
    class PacketType
    {
    public:
      typedef T			Type;
    };

    ///
    /// this class makes the relation between a tag and a packet type.
    ///
    template <const Tag G>
    class PacketMap:
      public PacketType<void>
    {
    };

//
// ---------- macro-functions -------------------------------------------------
//

    ///
    /// this macro-function generates PacketMaps.
    ///
#define PacketDeclare(_tag_, _T_)					\
  template <>								\
  class PacketMap<_tag_>:						\
    public PacketType<_T_>						\
  {									\
  public:								\
    typedef void	(Meta::*Method)(const Location&,		\
					const _T_&);			\
									\
    static void		Dispatch(Meta*			object,		\
				 const Method		method,		\
				 const Location&	location,	\
				 const Archive&		archive)	\
    {									\
      typedef void	(Meta::*Method)(const Location&,		\
					const typename			\
					PacketMap<G>::Type&);		\
									\
     printf("FORWARD 1\n");						\
      									\
      _T_ o;								\
      									\
      (object->*method)(location, o);					\
    }									\
  };

  }
}

#endif
