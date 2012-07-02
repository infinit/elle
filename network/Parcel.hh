#ifndef ELLE_NETWORK_PARCEL_HH
# define ELLE_NETWORK_PARCEL_HH

# include <elle/types.hh>

# include <elle/radix/Entity.hh>

# include <elle/network/fwd.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this class is just a wrapper for packets freshly received.
    ///
    /// whenver a message is being receveid, a parcel is author . then
    /// the session associated with this message is recorded before extracting
    /// the header. finally knowing the message's constituent types, the
    /// data can be fetched and extracted.
    ///
    class Parcel:
      public radix::Entity
    {
    public:
      //
      // constructors & destructors
      //
      Parcel();
      Parcel(Header*,
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
      Header*           header;
      Data*             data;
    };

  }
}

#endif
