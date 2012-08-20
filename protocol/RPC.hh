#ifndef INFINIT_PROTOCOL_RPC_HH
# define INFINIT_PROTOCOL_RPC_HH

# include <iostream>
# include <memory>
# include <unordered_map>

# include <boost/function.hpp>
# include <boost/noncopyable.hpp>

# include <protocol/fwd.hh>

namespace infinit
{
  namespace protocol
  {
    template <typename ISerializer, typename OSerializer>
    class BaseProcedure: public boost::noncopyable
    {
    public:
      virtual ~BaseProcedure();
    protected:
      template <typename I, typename O>
      friend class RPC;
      virtual void _call(ISerializer& in, OSerializer& out) = 0;
    };

    template <typename ISerializer,
              typename OSerializer,
              typename R,
              typename ... Args>
    class Procedure: public BaseProcedure<ISerializer, OSerializer>
    {
    public:
      virtual ~Procedure();
    protected:
      virtual void _call(ISerializer& in, OSerializer& out);
    private:
      template <typename I, typename O>
      friend class RPC;
      Procedure(RPC<ISerializer, OSerializer>& owner,
                uint32_t id,
                boost::function<R (Args...)> const& f);
      uint32_t _id;
      RPC<ISerializer, OSerializer>& _owner;
      boost::function<R (Args...)> _function;
    };

    class BaseRPC
    {
    public:
      BaseRPC(ChanneledStream& channels);
      virtual void run() = 0;
    protected:
      template <typename ISerializer,
                typename OSerializer,
                typename R,
                typename ... Args>
      friend class Procedure;
      ChanneledStream& _channels;
      uint32_t _id;
    };

    template <typename ISerializer, typename OSerializer>
    class RPC: public BaseRPC, public boost::noncopyable
    {
    public:
      template <typename R, typename ... Args>
      class RemoteProcedure
      {
      public:
        RemoteProcedure(RPC<ISerializer, OSerializer>& owner);
        R operator() (Args ...);
        void operator = (boost::function<R (Args...)> const& implem);
      private:
        template <typename I, typename O>
        friend class RPC;
        RemoteProcedure(RPC<ISerializer, OSerializer>& owner, uint32_t id);
        uint32_t _id;
        RPC<ISerializer, OSerializer>& _owner;
      };

    public:
      RPC(ChanneledStream& channels);
      template <typename R, typename ... Args>
      RemoteProcedure<R, Args...>
      add();
      template <typename R, typename ... Args>
      RemoteProcedure<R, Args...>
      add(boost::function<R (Args...)> const& f);
      void add(BaseRPC& rpc);
      virtual void run();
    protected:
      typedef BaseProcedure<ISerializer, OSerializer> LocalProcedure;
      // FIXME: unique_ptr
      std::unordered_map<uint32_t, LocalProcedure*> _procedures;
      std::vector<BaseRPC*> _rpcs;
    };
  }
}

# include <protocol/RPC.hxx>

#endif
