#include <hole/Holeable.hh>

#include <nucleus/proton/Network.hh>

#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>

namespace hole
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Holeable::Holeable(Hole& hole,
                     const nucleus::proton::Network& network):
    network(network),
    _hole(hole)
  {}

  ///
  /// destructor.
  ///
  Holeable::~Holeable()
  {
  }

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the implementation.
  ///
  elle::Status          Holeable::Dump(const elle::Natural32    margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Holeable]" << std::endl;

    // dump the network.
    if (this->network.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the network");

    return elle::Status::Ok;
  }

}
