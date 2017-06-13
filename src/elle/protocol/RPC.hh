#pragma once

#include <ostream>
#include <memory>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include <elle/Printable.hh>

#include <elle/reactor/Thread.hh>

#include <elle/protocol/fwd.hh>

namespace elle
{
  namespace protocol
  {
    /// Thrown by handler to ask the RPC server to exit from run.
    class LastMessageException
      : public elle::Exception
    {
    public:
      LastMessageException(std::string const& what);
    };

    using ExceptionHandler = std::function<void(std::exception_ptr)>;

    template <typename ISerializer, typename OSerializer>
    class BaseProcedure
      : public boost::noncopyable
    {
    public:
      BaseProcedure(std::string const& name);
      virtual
      ~BaseProcedure();

    protected:
      template <typename I, typename O>
      friend class RPC;

      virtual
      void
      _call(ISerializer& in, OSerializer& out) = 0;

    private:
      ELLE_ATTRIBUTE(std::string, name);
    };

    template <typename ISerializer,
              typename OSerializer,
              typename R,
              typename ... Args>
    class Procedure
      : public BaseProcedure<ISerializer, OSerializer>
    {
    public:
      using Owner = RPC<ISerializer, OSerializer>;
      using Function = std::function<R (Args...)>;

    public:
      virtual
      ~Procedure();

    protected:
      void
      _call(ISerializer& in, OSerializer& out) override;

    private:
      template <typename I, typename O>
      friend class RPC;

      Procedure(std::string const& name,
                RPC<ISerializer, OSerializer>& owner,
                uint32_t id,
                std::function<R (Args...)> const& f);

    private:
      ELLE_ATTRIBUTE(uint32_t, id);
      ELLE_ATTRIBUTE(Owner&, owner);
      ELLE_ATTRIBUTE(Function, function);
    };

    class BaseRPC
    {
    public:
      BaseRPC(ChanneledStream& channels);
      /// Run forever until one of the following:
      /// - Connection gets closed
      /// - Thread gets terminated
      /// - handler throws a LastMessageException.
      ///
      /// @param handler gets fed with any exception thrown by a RPC procedure.
      ///        handler can throw an exception, in which case that exception gets
      ///        used in place of the one thrown by RPC procedure as base for
      ///        the error reply.
      virtual
      void
      run(ExceptionHandler handler = {}) = 0;

    protected:
      template <typename ISerializer,
                typename OSerializer,
                typename R,
                typename ... Args>
      friend class Procedure;

      ELLE_ATTRIBUTE(ChanneledStream&, channels, protected);
      ELLE_ATTRIBUTE(uint32_t, id, protected);
    };

    template <typename ISerializer, typename OSerializer>
    class RPC
      : public BaseRPC
      , public elle::Printable
      , public boost::noncopyable
    {
    public:
      template <typename R, typename ... Args>
      class RemoteProcedure
      {
      public:
        using ReturnType = R;
        using Owner = RPC<ISerializer, OSerializer>;

      public:
        RemoteProcedure(std::string const& name,
                        RPC<ISerializer, OSerializer>& owner);
        R operator() (Args ...);
        void operator = (std::function<R (Args...)> const& implem);
        template <typename I, typename O>
        friend class RPC;
        RemoteProcedure(std::string const& name,
                        RPC<ISerializer, OSerializer>& owner,
                        uint32_t id);
      private:
        ELLE_ATTRIBUTE(uint32_t, id);
        ELLE_ATTRIBUTE(std::string, name);
        ELLE_ATTRIBUTE(Owner&, owner);
      };

    public:
      RPC(ChanneledStream& channels);

      template <typename R, typename ... Args>
      RemoteProcedure<R, Args...>
      add(std::string const& name);

      template <typename R, typename ... Args>
      RemoteProcedure<R, Args...>
      add(std::function<R (Args...)> const& f);

      void
      add(BaseRPC& rpc);

      void
      run(ExceptionHandler = {}) override;

      virtual
      void
      parallel_run();

    protected:
      using LocalProcedure = BaseProcedure<ISerializer, OSerializer>;
      using NamedProcedure = std::pair<std::string,
                        std::unique_ptr<LocalProcedure>>;
      using Procedures = std::unordered_map<uint32_t, NamedProcedure>;

      ELLE_ATTRIBUTE(Procedures, procedures, protected);
      ELLE_ATTRIBUTE(std::vector<BaseRPC*>, rpcs, protected);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const override;
    };
  }
}

#include <elle/protocol/RPC.hxx>
