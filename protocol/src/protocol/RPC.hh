#ifndef INFINIT_PROTOCOL_RPC_HH
# define INFINIT_PROTOCOL_RPC_HH

# include <ostream>
# include <memory>
# include <unordered_map>

# include <boost/function.hpp>
# include <boost/noncopyable.hpp>

# include <elle/Printable.hh>

# include <reactor/thread.hh>

# include <protocol/fwd.hh>

namespace infinit
{
  namespace protocol
  {
    /// Thrown by handler to ask the RPC server to exit from run
    class LastMessageException: public elle::Exception
    {
    public:
      LastMessageException(std::string const& what);
      virtual ~LastMessageException();
    };

    typedef std::function<void(std::exception_ptr)> ExceptionHandler;

    template <typename ISerializer, typename OSerializer>
    class BaseProcedure: public boost::noncopyable
    {
    public:
      BaseProcedure(std::string const& name);
      virtual ~BaseProcedure();
    protected:
      template <typename I, typename O>
      friend class RPC;
      virtual void _call(ISerializer& in, OSerializer& out) = 0;
    private:
      std::string _name;
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
      Procedure(std::string const& name,
                RPC<ISerializer, OSerializer>& owner,
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
      /** Run forever until one of the following:
      *   - Connection gets closed
      *   - Thread gets terminated
      *   - handler throws a LastMessageException.
      *
      *   @param handler gets fed with any exception thrown by a RPC procedure.
      *          handler can throw an exception, in which case that exception gets
      *          used in place of the one thrown by RPC procedure as base for
      *          the error reply.
      */
      virtual void run(ExceptionHandler handler = {}) = 0;
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
    class RPC:
      public BaseRPC,
      public elle::Printable,
      public boost::noncopyable
    {
    public:
      template <typename R, typename ... Args>
      class RemoteProcedure
      {
      public:
        typedef R ReturnType;

      public:
        RemoteProcedure(std::string const& name,
                        RPC<ISerializer, OSerializer>& owner);
        R operator() (Args ...);
        void operator = (boost::function<R (Args...)> const& implem);
        template <typename I, typename O>
        friend class RPC;
        RemoteProcedure(std::string const& name,
                        RPC<ISerializer, OSerializer>& owner,
                        uint32_t id);
      private:
        uint32_t _id;
        std::string _name;
        RPC<ISerializer, OSerializer>& _owner;
      };

    public:
      RPC(ChanneledStream& channels);
      template <typename R, typename ... Args>
      RemoteProcedure<R, Args...>
      add(std::string const& name);
      template <typename R, typename ... Args>
      RemoteProcedure<R, Args...>
      add(boost::function<R (Args...)> const& f);
      void add(BaseRPC& rpc);
      virtual void run(ExceptionHandler = {});
      virtual void parallel_run();
    protected:
      typedef BaseProcedure<ISerializer, OSerializer> LocalProcedure;
      typedef std::pair<std::string,
                        std::unique_ptr<LocalProcedure>> NamedProcedure;
      typedef std::unordered_map<uint32_t, NamedProcedure> Procedures;
      Procedures _procedures;
      std::vector<BaseRPC*> _rpcs;

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const;
    };
  }
}

# include <protocol/RPC.hxx>

#endif
