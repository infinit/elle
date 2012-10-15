#ifndef HOLE_HOLE_HH
# define HOLE_HOLE_HH

# include <elle/idiom/Close.hh>
# include <boost/signals.hpp>

# include <elle/attribute.hh>
# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>

# include <elle/Passport.hh>

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
    Hole(storage::Storage& storage,
         elle::Passport const& passport,
         elle::Authority const& authority);
    virtual ~Hole();

  /*------.
  | Ready |
  `------*/
  public:
    void
    ready_hook(boost::function<void ()> const& f);
    /// Signal other components the storage layer is ready.
    void
    ready(); // FIXME: protected
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
  protected:
    virtual
    void
    _join() = 0;
    virtual
    void
    _leave() = 0;

  /*----------.
  | Utilities |
  `----------*/
  public:
    /// Whether the given block conflicts with the one stored locally.
    bool
    conflict(nucleus::proton::Address const& address,
             nucleus::proton::MutableBlock const& block) const;

  /*--------.
  | Storage |
  `--------*/
  public:
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

  /*---------------.
  | Implementation |
  `---------------*/
  protected:
    virtual
    void
    _push(const nucleus::proton::Address& address,
         const nucleus::proton::ImmutableBlock& block) = 0;
    virtual
    void
    _push(const nucleus::proton::Address& address,
         const nucleus::proton::MutableBlock& block) = 0;
    virtual
    std::unique_ptr<nucleus::proton::Block>
    _pull(const nucleus::proton::Address&) = 0;
    virtual
    std::unique_ptr<nucleus::proton::Block>
    _pull(const nucleus::proton::Address&, const nucleus::proton::Revision&) = 0;
    virtual
    void
    _wipe(const nucleus::proton::Address& address) = 0;

  /*-----------.
  | Attributes |
  `-----------*/
  private:
    ELLE_ATTRIBUTE_R(elle::Passport, passport);
    ELLE_ATTRIBUTE_R(elle::Authority, authority);

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
