#include <elle/log.hh>

#include <etoile/depot/Depot.hh>

#include <hole/Hole.hh>
//#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/slug/Implementation.hh>

#include <horizon/Horizon.hh>

#include <lune/Descriptor.hh>
#include <lune/Set.hh>

#include <nucleus/Nucleus.hh>
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
  /*--------.
  | Factory |
  `--------*/

  std::unique_ptr<Hole>
  factory(hole::storage::Storage& storage)
  {
    lune::Descriptor descriptor(Infinit::Network);

    lune::Set set;
    if (lune::Set::exists(Infinit::Network) == true)
      set.load(Infinit::Network);

    switch (descriptor.meta().model().type)
      {
        // case Model::TypeLocal:
        // {
        //   this->_implementation =
        //     new implementations::local::Implementation(*this, network);
        //   break;
        // }
        case Model::TypeRemote:
        {
          // Retrieve the locus.
          if (set.loci.size() != 1)
            {
              static boost::format fmt("there should be a single locus "
                                       "in the network's set (%u)");
              throw std::runtime_error(str(fmt % set.loci.size()));
            }
          elle::network::Locus locus = *set.loci.begin();
          return std::unique_ptr<Hole>(
            new implementations::remote::Implementation(storage, locus));
        }
        case Model::TypeSlug:
        {
          std::vector<elle::network::Locus> members;
          for (elle::network::Locus const& locus: set.loci)
            members.push_back(locus);
          int port = Infinit::Configuration["hole"].Get("slug.port", 0);
          return std::unique_ptr<Hole>(
            new implementations::slug::Implementation(storage, members, port));
        }
        case Model::TypeCirkle:
        {
          /* XXX
          // allocate the instance.
          this->_implementation =
          new implementations::cirkle::Implementation(network);
          */
          elle::abort("Cirkle implementation is disabled for now");
          break;
        }
        default:
        {
          static boost::format fmt("unknown or not-yet-supported model '%u'");
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   str(fmt % descriptor.meta().model().type));
        }
      }

    elle::unreachable();
  }

  /*-------------.
  | Construction |
  `-------------*/

  Hole::Hole(storage::Storage& storage)
    : _storage(storage)
  {
    // Disable the meta logging.
    if (elle::radix::Meta::Disable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to disable the meta logging");

    // Retrieve the passport.
    {
      if (lune::Passport::exists() == false)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "the device passport does not seem to exist");
      _passport.load();
      if (_passport.Validate(Infinit::Authority) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to validate the passport");
    }

    // Enable the meta logging.
    if (elle::radix::Meta::Enable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to enable the meta logging");

    etoile::depot::hole(this);
    horizon::hole(this);
  }

  Hole::~Hole()
  {
    etoile::depot::hole(nullptr);
    horizon::hole(nullptr);
  }

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

  /*----------.
  | Utilities |
  `----------*/

  bool
  Hole::conflict(nucleus::proton::Address const& address,
                 nucleus::proton::MutableBlock const& block) const
  {
    // If the block doesn't exist locally, this is the first version
    // we know of and there is no conflict.
    if (!this->_storage.exist(address))
      {
        ELLE_DEBUG("This is the first revision of the block")
          return false;
      }

    ELLE_DEBUG_SCOPE("the mutable block seems to exist "
                     "locally: make sure it derives the "
                     "current revision");

    nucleus::proton::MutableBlock* current;

    // build a block according to the component.
    if (nucleus::factory().Build(
          address.component(),
          current) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to build the block");

    std::unique_ptr<nucleus::proton::MutableBlock> guard(current);

    // load the latest revision.
    current->load(this->_storage.path(address));

    ELLE_DEBUG("Current block revision %s and given block revision is %s",
               current->revision(), block.revision());

    if (block.revision() != current->revision())
      return !block.derives(*current);

    ELLE_DEBUG("Block have same revision, we need to distinguish them "
               "as the latest");

    // We check if contents are the same.
    {
      elle::Buffer b1;
      b1.writer() << block;

      elle::Buffer b2;
      b2.writer() << *current;

      /// We need to choose arbitrarly one block as the latest. For that we
      /// decide to compare their content, so we got the same result on
      /// every hosts.
      if (b1 == b2)
        {
          ELLE_TRACE("Both block contents match. Let's say the remote one "
                     "doesn't derive the local one");
          return false;
        }

      ELLE_WARN("conflict detected: blocks have the same revision but"
                " different content; assuming the latest revision is %s",
                (b1 < b2 ? block : *current));

      return (b1 < b2);
    }
  }
}
