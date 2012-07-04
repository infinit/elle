#include <hole/implementations/remote/Customer.hh>

#include <elle/network/TCPSocket.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this constant defines the time after which a customer should
      /// have authenticated.
      ///
      const elle::Natural32             Customer::Timeout = 180000;

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// default constructor.
      ///
      Customer::Customer(elle::network::TCPSocket*         socket)
        : state(Customer::StateUnknown)
        , socket(socket)
      {}

      ///
      /// destructor.
      ///
      Customer::~Customer()
      {
        // delete the socket.
        if (this->socket != NULL)
          delete this->socket;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the customer.
      ///
      elle::Status      Customer::Dump(const elle::Natural32    margin) const
      {
        elle::String    alignment(margin, ' ');

        std::cout << alignment << "[Customer]" << std::endl;

        // dump the state.
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[State] " << this->state << std::endl;

        return elle::Status::Ok;
      }

    }
  }
}
