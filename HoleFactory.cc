#include <memory>

#include <elle/Authority.hh>
#include <elle/Passport.hh>

#include <lune/Descriptor.hh>
#include <lune/Set.hh>

#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/slug/Implementation.hh>

#include <HoleFactory.hh>
#include <Infinit.hh>

namespace infinit
{
  std::unique_ptr<hole::Hole>
  hole_factory(hole::storage::Storage& storage,
               elle::Passport const& passport,
               elle::Authority const& authority)
  {
    lune::Descriptor descriptor(Infinit::Network);

    lune::Set set;
    if (lune::Set::exists(Infinit::Network) == true)
      set.load(Infinit::Network);


    switch (descriptor.meta().model().type)
      {
        case hole::Model::TypeLocal:
        {
          return std::unique_ptr<hole::Hole>(
            new hole::implementations::local::Implementation(
              storage, passport, authority));
          break;
        }
        case hole::Model::TypeRemote:
        {
          // Retrieve the locus.
          if (set.loci.size() != 1)
            {
              static boost::format fmt("there should be a single locus "
                                       "in the network's set (%u)");
              throw std::runtime_error(str(fmt % set.loci.size()));
            }
          elle::network::Locus locus = *set.loci.begin();
          return std::unique_ptr<hole::Hole>(
            new hole::implementations::remote::Implementation(
              storage, passport, authority, locus));
        }
        case hole::Model::TypeSlug:
        {
          std::vector<elle::network::Locus> members;
          for (elle::network::Locus const& locus: set.loci)
            members.push_back(locus);
          int port = Infinit::Configuration["hole"].Get("slug.port", 0);
          int timeout_int =
            Infinit::Configuration["hole"].Get("slug.timeout", 5000);
          reactor::Duration timeout =
            boost::posix_time::milliseconds(timeout_int);
          return std::unique_ptr<hole::Hole>(
            new hole::implementations::slug::Implementation(
              storage, passport, authority, members, port, timeout));
        }
        case hole::Model::TypeCirkle:
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
  }
}
