#ifndef ELLE_NETWORK_PARCEL_HH
# define ELLE_NETWORK_PARCEL_HH

# include <elle/types.hh>

# include <elle/radix/Entity.hh>

# include <elle/network/Data.hh>

namespace elle
{

  using namespace radix;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// these classes need to be forward declared to avoid conflicts.
    ///
    class Session;
    class Header;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is just a wrapper for packets freshly received.
    ///
    /// whenver a message is being receveid, a parcel is author . then
    /// the session associated with this message is recorded before extracting
    /// the header. finally knowing the message's constituent types, the
    /// data can be fetched and extracted.
    ///
    class Parcel:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Parcel();
      Parcel(Session*,
             Header*,
             Data*);
      ~Parcel();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Session*          session;

      Header*           header;
      Data*             data;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Session.hh>
#include <elle/network/Header.hh>

#endif
