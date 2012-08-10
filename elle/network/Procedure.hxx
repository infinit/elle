#ifndef ELLE_NETWORK_PROCEDURE_HXX
# define ELLE_NETWORK_PROCEDURE_HXX

# include <elle/log.hh>

# include <elle/network/Inputs.hh>
# include <elle/network/TCPSocket.hh>
# include <elle/network/Locus.hh>

# include <elle/radix/Arguments.hh>
# include <elle/radix/Variables.hh>

# include <elle/serialize/BufferArchive.hh>

# include <elle/standalone/Report.hh>

namespace elle
{
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
    Procedure<I, O, E>::Procedure(const concurrency::Callback<
                                    Status,
                                    R
                                    >                           routine,
                                  const concurrency::Callback<
                                    Status,
                                    radix::Parameters<>
                                    >                           prolog,
                                  const concurrency::Callback<
                                    Status,
                                    radix::Parameters<>
                                    >                           epilog):
      routine(routine),
      prolog(prolog),
      epilog(epilog)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    // XXX
    namespace {
      struct ProcedureSkeletonExtractor
      {
      private:
        serialize::InputBufferArchive& _archive;

      public:
        ProcedureSkeletonExtractor(serialize::InputBufferArchive& archive)
          : _archive(archive)
        {}

        Status Load()
        {
          return Status::Ok;
        }

        template<typename T1, typename ...T>
        Status Load(T1& v1, T&... values)
        {
          ELLE_LOG_COMPONENT("elle.network.Procedure");
          ELLE_TRACE("Skeleton extract %p", &v1)
          try
            {
              _archive >> v1;
              return this->Load(values...);
            }
          catch (std::exception const& err)
            {
              ELLE_TRACE(
                "An exception occured while extracting skeleton: %s",
                err.what());
              return Status::Error;
            }
          assert(false && "unreachable");
          return (Status::Ok);
        }
      };
    }

    ///
    /// this method is called by the network manager whenever a message
    /// of tag I is received.
    ///
    template <const Tag I,
              const Tag O,
              const Tag E>
    Status              Procedure<I, O, E>::Skeleton(TCPSocket* socket,
                                                     Locus& locus,
                                                     infinit::protocol::Stream& channel,
                                                     Parcel&   parcel) const
    {
      serialize::InputBufferArchive archive(*parcel.data);
      ProcedureSkeletonExtractor extractor(archive);

      concurrency::Callback<
          Status,
            typename radix::Trait::Reference<
              typename Message<I>::P
          >::Type
      >               extract(&ProcedureSkeletonExtractor::Load, &extractor);

      radix::Variables<
      typename radix::Trait::Bare<
          typename Message<I>::P
          >::Type
        >               inputs;
      radix::Variables<
        typename radix::Trait::Bare<
          typename Message<O>::P
          >::Type
        >               outputs;
      Status            status;

      // call the prolog.
      if (this->prolog.Call() == Status::Error)
        escape("an error occured in the procedure's prolog");

      // extract the values from the archive given the types required
      // for the callback.
      if (inputs.Call(extract) == Status::Error)
        escape("unable to extract from the archive");

      // check that the end of the archive has been reached i.e all
      // the information has been extracted. this step ensures that
      // the archive does not contain more variables that extracted.
      if (archive.Stream().BytesLeft() > 0)
        {
          std::string msg(
            elle::sprintf("the archive seems to contain additional "
                          "information: %s bytes left",
                          archive.Stream().BytesLeft()));
          escape("%s", msg.c_str());
        }

      // at this point, an Arguments is created which references both
      // the inputs and outputs. thus no copy is made while the
      // outputs can still be accessed through the _outputs_ variable.
      radix::Arguments<
        typename
          radix::Set::Union<
          typename
          radix::Trait::Bare<
            typename Message<I>::P
            >::Type,
              typename
            radix::Trait::Bare<
              typename Message<O>::P
              >::Type
          >::Type
          >               arguments = radix::Arguments<>::Union(inputs, outputs);

      // call the routine.
      Context ctx;
      ctx.socket = socket;
      ctx.channel = &channel;
      std::string host;
      if (locus.host.Convert(host) == Status::Error)
        escape("unable to convert the host name");
      ctx.host = host;
      network::current_context(ctx);

      status = arguments.Call(this->routine);

      // call the epilog.
      if (this->epilog.Call() == Status::Error)
        escape("an error occured in the procedure's epilog");

      //
      // send back the report to the client if an error occured.
      //
      if (status == Status::Error)
        {
          //
          // serialize the report and send it to the caller.
          //
          standalone::Report& report = standalone::Report::report.Get();

          // reply with the report.
          socket->reply(Inputs<E>(report));

          // flush the report since it has been sent
          // to the sender.
          report.Flush();

          return Status::Ok;
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

            // Reply with the output bundle.
            socket->reply(bundle);

            break;
          }
        }

      return Status::Ok;
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
      std::cout << alignment << io::Dumpable::Shift
                << "[Routine]" << std::endl;

      if (this->routine.Dump(margin + 2) == Status::Error)
        escape("unable to dump the callback");

      // dump the callback.
      std::cout << alignment << io::Dumpable::Shift
                << "[Prolog]" << std::endl;

      if (this->prolog.Dump(margin + 2) == Status::Error)
        escape("unable to dump the callback");

      // dump the epilog.
      std::cout << alignment << io::Dumpable::Shift
                << "[Epilog]" << std::endl;

      if (this->epilog.Dump(margin + 2) == Status::Error)
        escape("unable to dump the callback");

      return Status::Ok;
    }

  }
}

#endif
