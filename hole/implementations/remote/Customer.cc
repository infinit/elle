#include <hole/implementations/remote/Customer.hh>
#include <hole/implementations/remote/Remote.hh>

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
      Customer::Customer(elle::TCPSocket*         socket)
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
// ---------- callbacks -------------------------------------------------------
//

      ///
      /// this callback is triggered whenever the customer is considered
      /// disconnected.
      ///
      elle::Status      Customer::Disconnected()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Customer::Disconnected()\n");

        // set the customer's state as dead.
        this->state = Customer::StateDead;

        // Q_EMIT the signal.
        if (this->signal.dead.Emit(this) == elle::StatusError)
          escape("unable to Q_EMIT the signal");

        return elle::StatusOk;
      }

      ///
      /// this callback is triggered whenever an error occurs on the
      /// customer's socket.
      ///
      elle::Status      Customer::Error(elle::String     error)
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Customer::Error()\n");

        // log the error.
        log(error.c_str());

        // disconnect the socket, though that may be unecessary.
        this->socket->Disconnect();

        return elle::StatusOk;
      }

      ///
      /// this callback is triggered once the authentication timer times out.
      ///
      /// if the customer has not been authenticated, it is destroyed.
      ///
      elle::Status      Customer::Abort()
      {
        // debug.
        if (Infinit::Configuration.hole.debug == true)
          printf("[hole] implementations::remote::Customer::Abort()\n");

        // check if the customer has been authenticated.
        if (this->state != Customer::StateAuthenticated)
          {
            // Q_EMIT the signal.
            if (this->signal.dead.Emit(this) == elle::StatusError)
              escape("unable to Q_EMIT the signal");
          }

        return elle::StatusOk;
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
        std::cout << alignment << elle::Dumpable::Shift
                  << "[State] " << this->state << std::endl;

        return elle::StatusOk;
      }

    }
  }
}
