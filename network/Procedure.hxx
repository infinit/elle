//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Procedure.hxx
//
// created       julien quintard   [mon jul 18 17:40:44 2011]
// updated       julien quintard   [fri sep  2 11:29:14 2011]
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
    /// XXX
    ///
    template <const Tag I,
	      const Tag O,
	      const Tag E>
    Procedure<I, O, E>::Procedure(const Callback< Status,
				                  R >&		routine,
				  const Callback< Status,
				                  Parameters<> >* prolog,
				  const Callback< Status,
					          Parameters<> >* epilog):
      routine(routine),
      prolog(NULL),
      epilog(NULL)
    {
      // clone the prolog, if necessary.
      if (prolog != NULL)
	this->prolog = new Callback< Status,
				     Parameters<> >(*prolog);

      // clone the epilog, if necessary.
      if (epilog != NULL)
	this->epilog = new Callback< Status,
				     Parameters<> >(*epilog);
    }

    ///
    /// XXX
    ///
    template <const Tag I,
	      const Tag O,
	      const Tag E>
    Procedure<I, O, E>::Procedure(const Procedure<I, O, E>&	procedure):
      Object(procedure),

      routine(procedure.routine),
      prolog(NULL),
      epilog(NULL)
    {
      // clone the prolog, if necessary.
      if (prolog != NULL)
	this->prolog = new Callback< Status,
				     Parameters<> >(*procedure.prolog);

      // clone the epilog, if necessary.
      if (epilog != NULL)
	this->epilog = new Callback< Status,
				     Parameters<> >(*procedure.epilog);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <const Tag I,
	      const Tag O,
	      const Tag E>
    Status		Procedure<I, O, E>::Skeleton(Archive&	archive) const
    {
      Callback<
	Status,
	typename Trait::Reference<
	  typename Message<I>::P
	  >::Type
	>		extract(&Archive::Extract, &archive);
      Variables<
	typename Trait::Bare<
	  typename Message<I>::P
	  >::Type
	>		inputs;
      Variables<
	typename Trait::Bare<
	  typename Message<O>::P
	  >::Type
	>		outputs;
      Status		status;
      Session*		session;

      enter();

      // retrieve the session.
      if (Session::Instance(session) == StatusError)
	escape("unable to retrieve the session instance");

      // check the session.
      if (session == NULL)
	escape("unable to proceed with a null session");

      // call the prolog, if required.
      if (this->prolog != NULL)
	{
	  if (this->prolog->Call() == StatusError)
	    escape("an error occured in the procedure's prolog");
	}

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
	>		arguments = Arguments<>::Union(inputs, outputs);

      // call the routine.
      status = arguments.Call(this->routine);

      // call the epilog, if required.
      if (this->epilog != NULL)
	{
	  if (this->epilog->Call() == StatusError)
	    escape("an error occured in the procedure's epilog");
	}

      //
      // send back the report to the client if an error occured.
      //
      if (status == StatusError)
	{
	  // depending on the error tag.
	  switch (E)
	    {
	    case TagError:
	      {
		//
		// nothing to do in this case i.e just ignore the error
		// though log it.
		//

		log("an error occured in this procedure");

		break;
	      }
	    default:
	      {
		//
		// in the other cases, serialize the report and send it
		// to the caller.
		//
		Report*		report;

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
	      }
	    }

	  leave();
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
	case TagOk:
	  {
	    //
	    // in this case, a TagOk message is sent back in order to
	    // acknowledge the success of the request.
	    //

	    // check the socket.
	    if (session->socket == NULL)
	      escape("unable to reply with a null socket");

	    // reply with the report.
	    if (session->socket->Reply(
		  Inputs<TagOk>(),
		  session) == StatusError)
	      escape("unable to reply with the status");

	    break;
	  }
	default:
	  {
	    //
	    // in this case the procedure's outputs are serialized and
	    // sent back to the caller.
	    //

	    typename
	      Message<O>::B::Inputs	bundle(outputs);

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

      leave();
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
    Status		Procedure<I, O, E>::Dump(const Natural32 margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Procedure]" << std::endl;

      // dump the callback.
      if (this->routine.Dump(margin + 2) == StatusError)
	escape("unable to dump the callback");

      // dump the callback.
      if (this->prolog != NULL)
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Prolog]" << std::endl;

	  if (this->prolog->Dump(margin + 2) == StatusError)
	    escape("unable to dump the callback");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Prolog] " << none << std::endl;
	}

      // dump the callback.
      if (this->epilog != NULL)
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Epilog]" << std::endl;

	  if (this->epilog->Dump(margin + 2) == StatusError)
	    escape("unable to dump the callback");
	}
      else
	{
	  std::cout << alignment << Dumpable::Shift
		    << "[Epilog] " << none << std::endl;
	}

      leave();
    }

  }
}

#endif
