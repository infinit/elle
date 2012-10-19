#include <elle/cryptography/Cipher.hh>
#include <elle/log.hh>

#include <hole/Hole.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/Network.hh>

#include <Infinit.hh>

#include <elle/idiom/Close.hh>
# include <boost/format.hpp>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.hole.Hole");

namespace hole
{
  /*-------------.
  | Construction |
  `-------------*/

  Hole::Hole(storage::Storage& storage,
             elle::Passport const& passport,
             elle::Authority const& authority)
    : _storage(storage)
    , _passport(passport)
    , _authority(authority)
    , _state(State::offline)
  {
    // Disable the meta logging.
    if (elle::radix::Meta::Disable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to disable the meta logging");

    if (_passport.Validate(this->authority()) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to validate the passport");

    // Enable the meta logging.
    if (elle::radix::Meta::Enable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to enable the meta logging");
  }

  Hole::~Hole()
  {}

  /*-------------.
  | Join, leave |
  `-------------*/

  void
  Hole::join()
  {
    this->_join();
  }

  void
  Hole::leave()
  {
    this->_leave();
  }

  /*------.
  | Ready |
  `------*/

  void
  Hole::ready()
  {
    ELLE_LOG_SCOPE("ready");
    if (this->_state != State::online)
      {
        this->_ready();
        this->_state = Hole::State::online;
      }
  }

  void
  Hole::ready_hook(boost::function<void ()> const& f)
  {
    Hole::_ready.connect(f);
  }

  void
  Hole::push(const nucleus::proton::Address& address,
             const nucleus::proton::Block& block)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, address, block);
    // XXX check the block's footprint which should not exceed Extent

    // Forward the request depending on the nature of the block which
    // the address indicates.
    switch (address.family())
      {
      case nucleus::proton::Family::content_hash_block:
        {
          const nucleus::proton::ImmutableBlock*        ib;
          ib = static_cast<const nucleus::proton::ImmutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::ImmutableBlock*>(
                   &block) != nullptr);
          this->_push(address, *ib);
          break;
        }
      case nucleus::proton::Family::public_key_block:
      case nucleus::proton::Family::owner_key_block:
      case nucleus::proton::Family::imprint_block:
        {
          const nucleus::proton::MutableBlock*          mb;
          mb = static_cast<const nucleus::proton::MutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::MutableBlock*>(
                   &block) != nullptr);
          this->_push(address, *mb);
          break;
        }
      default:
        {
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   elle::sprintf("unknown block family '%u'",
                                                 address.family()));
        }
      }
  }

  std::unique_ptr<nucleus::proton::Block>
  Hole::pull(const nucleus::proton::Address& address,
             const nucleus::proton::Revision& revision)
  {
    ELLE_TRACE_SCOPE("%s(%s, %s)", __FUNCTION__, address, revision);

    // Forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family())
      {
      case nucleus::proton::Family::content_hash_block:
        return this->_pull(address);
      case nucleus::proton::Family::public_key_block:
      case nucleus::proton::Family::owner_key_block:
      case nucleus::proton::Family::imprint_block:
        return this->_pull(address, revision);
      default:
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 elle::sprintf("unknown block family '%u'",
                                               address.family()));
      }
  }

  void
  Hole::wipe(nucleus::proton::Address const& address)
  {
    ELLE_TRACE_SCOPE("%s(%s)", __FUNCTION__, address);
    this->_wipe(address);
  }
}
