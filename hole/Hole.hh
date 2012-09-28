#ifndef HOLE_HOLE_HH
# define HOLE_HOLE_HH

# include <boost/signals.hpp>

# include <elle/attribute.hh>
# include <elle/types.hh>
# include <elle/concurrency/Signal.hh>

# include <nucleus/proton/fwd.hh>

# include <lune/Descriptor.hh>
# include <lune/Set.hh>
# include <lune/Passport.hh>

# include <hole/fwd.hh>
# include <hole/storage/Storage.hh>

namespace hole
{
  /// The storage layer of an Infinit filesystem.
  class Hole
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    Hole(storage::Storage& storage);
    virtual ~Hole();

  /*------.
  | Ready |
  `------*/
  public:
    /// Signal other components the storage layer is ready.
    void
    ready();
    void
    ready_hook(boost::function<void ()> const& f);
  private:
    boost::signal<void ()> _ready;

  /*------------.
  | Join, leave |
  `------------*/
  public:
    void
    join();
    void
    leave();

  /*--------.
  | Storage |
  `--------*/
  public:
    /// Addresse of the root block.
    nucleus::proton::Address
    origin();
    /// Store a block.
    void
    push(const nucleus::proton::Address& address,
         const nucleus::proton::Block& block);
    /// Retreive a block.
    std::unique_ptr<nucleus::proton::Block>
    pull(const nucleus::proton::Address& address,
         const nucleus::proton::Revision& revision);
    /// Remove a block.
    void
    wipe(const nucleus::proton::Address& address);
  private:
    ELLE_ATTRIBUTE_R(storage::Storage&, storage);

  /*----------.
  | Utilities |
  `----------*/
  public:
    /// Whether the given block conflicts with the one stored locally.
    bool
    conflict(nucleus::proton::Address const& address,
             nucleus::proton::MutableBlock const& block) const;

  /*-----------.
  | Attributes |
  `-----------*/
  public:
    nucleus::proton::Network const& network() const;
  private:
    ELLE_ATTRIBUTE_R(lune::Descriptor, descriptor);
    ELLE_ATTRIBUTE_R(lune::Set, set);
    ELLE_ATTRIBUTE_R(lune::Passport, passport);

  /*---------------.
  | Implementation |
  `---------------*/
  private:
    Holeable* _implementation;

  /*------.
  | State |
  `------*/
  public:
    enum struct State
      {
        offline,
        online
      };
    ELLE_ATTRIBUTE_R(State, state);
  };

}

#endif
