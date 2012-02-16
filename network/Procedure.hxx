//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon jul 18 17:40:44 2011]
//

#ifndef ELLE_NETWORK_PROCEDURE_HXX
#define ELLE_NETWORK_PROCEDURE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Arguments.hh>
#include <elle/radix/Variables.hh>

#include <elle/network/Session.hh>
#include <elle/network/Bundle.hh>
#include <elle/network/Inputs.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace standalone;

  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor takes the routing callback along with optional
    /// prolog and epilog callbacks.
    ///
    template <const Tag I,
              const Tag O,
              const Tag E>
    Procedure<I, O, E>::Procedure(const Callback<
                                    Status,
                                    R
                                    >                           routine,
                                  const Callback<
                                    Status,
                                    Parameters<>
                                    >                           prolog,
                                  const Callback<
                                    Status,
                                    Parameters<>
                                    >                           epilog):
      routine(routine),
      prolog(prolog),
      epilog(epilog)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method is called by the network manager whenever a message
    /// of tag I is received.
    ///
    template <const Tag I,
              const Tag O,
              const Tag E>
    Status              Procedure<I, O, E>::Skeleton(Archive&   archive) const
    {
      Callback<
        Status,
        typename Trait::Reference<
          typename Message<I>::P
          >::Type
        >               extract(&Archive::Extract, &archive);
      Variables<
        typename Trait::Bare<
          typename Message<I>::P
          >::Type
        >               inputs;
      Variables<
        typename Trait::Bare<
          typename Message<O>::P
          >::Type
        >               outputs;
      Status            status;
      Session*          session;

      // retrieve the session.
      if (Session::Instance(session) == StatusError)
        escape("unable to retrieve the session instance");

      // check the session.
      if (session == NULL)
        escape("unable to proceed with a null session");

      // call the prolog.
      if (this->prolog.Call() == StatusError)
        escape("an error occured in the procedure's prolog");

      // extract the values from the archive given the types required
      // for the callback.
      if (inputs.Call(extract) == StatusError)
        escape("unable to extract from the archive");

      // check that the end of the archive has been reached i.e all
      // the information has been extracted. this step ensures that
      // the archive does not contain more variables that extracted.
      if (archive.offset != archive.size)
        escape("the archive seems to contain additional information");

      // at this point, an Arguments is created which references both
      // the inputs and outputs. thus no copy is made while the
      // outputs can still be accessed through the _outputs_ variable.
      Arguments<
        typename
          Set::Union<
            typename
              Trait::Bare<
                typename Message<I>::P
                >::Type,
            typename
              Trait::Bare<
                typename Message<O>::P
                >::Type
            >::Type
        >               arguments = Arguments<>::Union(inputs, outputs);

      // call the routine.
      status = arguments.Call(this->routine);

      // call the epilog.
      if (this->epilog.Call() == StatusError)
        escape("an error occured in the procedure's epilog");

      //
      // send back the report to the client if an error occured.
      //
      if (status == StatusError)
        {
          //
          // serialize the report and send it to the caller.
          //
          Report*               report;

          // check the socket.
          if (session->socket == NULL)
            escape("unable to reply with a null socket");

          // retrieve the report.
          if (Report::Instance(report) == StatusFalse)
            escape("unable to retrieve the report");

          // reply with the report.
          if (session->socket->Reply(
                Inputs<E>(*report),
                session) == StatusError)
            escape("unable to reply with the status");

          // flush the report since it has been sent
          // to the sender.
          report->Flush();

          return elle::StatusOk;
        }

      // reply according to the output tag.
      switch (O)
        {
        case TagNone:
          {
            //
            // nothing to do in this case.
            //

            break;
          }
        default:
          {
            //
            // in this case the procedure's outputs are serialized and
            // sent back to the caller.
            //

            typename
              Message<O>::B::Inputs     bundle(outputs);

            // check the socket.
            if (session->socket == NULL)
              escape("unable to reply with a null socket");

            // reply with the output bundle.
            if (session->socket->Reply(bundle,
                                       session) == StatusError)
              escape("unable to reply to the caller");

            break;
          }
        }

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(_(Procedure<I, O, E>),
          _(template <const Tag I,
                      const Tag O,
                      const Tag E>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the procedure.
    ///
    template <const Tag I,
              const Tag O,
              const Tag E>
    Status              Procedure<I, O, E>::Dump(const Natural32 margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Procedure]" << std::endl;

      // dump the routine.
      std::cout << alignment << Dumpable::Shift
                << "[Routine]" << std::endl;

      if (this->routine.Dump(margin + 2) == StatusError)
        escape("unable to dump the callback");

      // dump the callback.
      std::cout << alignment << Dumpable::Shift
                << "[Prolog]" << std::endl;

      if (this->prolog.Dump(margin + 2) == StatusError)
        escape("unable to dump the callback");

      // dump the epilog.
      std::cout << alignment << Dumpable::Shift
                << "[Epilog]" << std::endl;

      if (this->epilog.Dump(margin + 2) == StatusError)
        escape("unable to dump the callback");

      return elle::StatusOk;
    }

  }
}

#endif
