#ifndef HOLE_HOLEABLE_HH
# define HOLE_HOLEABLE_HH

# include <elle/attribute.hh>
# include <elle/radix/Entity.hh>
# include <elle/types.hh>

# include <hole/fwd.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Network.hh>

namespace hole
{

  ///
  /// this class defines the interface hole implementation should implement.
  ///
  class Holeable:
    public elle::radix::Entity
  {
  public:
    //
    // constructors & destructors
    //
    Holeable(Hole& hole,
             const nucleus::proton::Network&);
    virtual ~Holeable();

    //
    // interface
    //
    public:
    virtual void                Join() = 0;
    virtual elle::Status        Leave() = 0;

    virtual void
    Put(const nucleus::proton::Address&,
        const nucleus::proton::ImmutableBlock&) = 0;
    virtual void
    Put(const nucleus::proton::Address&,
        const nucleus::proton::MutableBlock&) = 0;
    virtual std::unique_ptr<nucleus::proton::Block>
    Get(const nucleus::proton::Address&) = 0;
    virtual std::unique_ptr<nucleus::proton::Block>
    Get(const nucleus::proton::Address&, const nucleus::proton::Revision&) = 0;
    virtual void Kill(const nucleus::proton::Address&) = 0;

    //
    // interfaces
    //

    // dumpable
    elle::Status                Dump(const elle::Natural32 = 0) const;

    //
    // attributes
    //
    nucleus::proton::Network            network;
  private:
    ELLE_ATTRIBUTE_RX(Hole&, hole);
  };

}

#endif
