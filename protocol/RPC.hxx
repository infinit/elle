#ifndef INFINIT_PROTOCOL_RPC_HXX
# define INFINIT_PROTOCOL_RPC_HXX

# include <lune/Passport.hh> // FIXME

# include <type_traits>

# include <elle/printf.hh>

# include <protocol/Channel.hh>
# include <protocol/ChanneledStream.hh>
# include <protocol/Packet.hh>

namespace infinit
{
  namespace protocol
  {
    template <typename ISerializer,
              typename OSerializer,
              typename R,
              typename ... Args>
    Procedure<ISerializer, OSerializer, R, Args ...>::
    ~Procedure()
    {}

    template <typename ISerializer,
              typename OSerializer,
              typename R,
              typename ... Args>
    Procedure<ISerializer, OSerializer, R, Args ...>::
    Procedure (RPC<ISerializer, OSerializer>& owner,
               uint32_t id,
               boost::function<R (Args...)> const& f)
      : _owner(owner)
      , _id(id)
      , _function(f)
    {}

    template <typename ISerializer, typename OSerializer>
    template <typename R, typename ... Args>
    RPC<ISerializer, OSerializer>::RemoteProcedure<R, Args ...>::
    RemoteProcedure(RPC<ISerializer, OSerializer>& owner)
      : RemoteProcedure<R, Args...>(owner.add<R, Args...>())
    {}

    template <typename ISerializer, typename OSerializer>
    template <typename R, typename ... Args>
    RPC<ISerializer, OSerializer>::RemoteProcedure<R, Args ...>::
    RemoteProcedure(RPC<ISerializer, OSerializer>& owner, uint32_t id)
      : _owner(owner)
      , _id(id)
    {}

    template <typename ISerializer, typename OSerializer>
    template <typename R, typename ... Args>
    void
    RPC<ISerializer, OSerializer>::RemoteProcedure<R, Args ...>::
    operator = (boost::function<R (Args...)> const& f)
    {
      auto proc = _owner._procedures.find(_id);
      assert(proc != _owner._procedures.end());
      assert(proc->second == nullptr);
      typedef Procedure<ISerializer, OSerializer, R, Args...> Proc;
      Proc* res = new Proc(_owner, _id, f);
      _owner._procedures[_id] = res;
      BaseProcedure<ISerializer, OSerializer>* fuck = res;
    }

    template <typename OSerializer>
    static void put_args(OSerializer&)
    {}

    template <typename OSerializer, typename T, typename ...Args>
    static void
    put_args(OSerializer& output, T a, Args ... args)
    {
      output << a;
      put_args<OSerializer, Args...>(output, args...);
    }


    template <typename ISerializer, typename R>
    static R get_res(ISerializer& input)
    {
      R res;
      input >> res;
      return res;
    }

    template <typename ISerializer, typename OSerializer>
    template <typename R, typename ... Args>
    R
    RPC<ISerializer, OSerializer>::RemoteProcedure<R, Args...>::
    operator () (Args ... args)
    {
      Channel channel(_owner._channels);
      {
        Packet question;
        OSerializer output(question);
        output << _id;
        put_args<OSerializer, Args...>(output, args...);
        question.flush();
        channel.write(question);
      }
      {
        Packet response(channel.read());
        ISerializer input(response);
        return get_res<ISerializer, R>(input);
      }
    }

    template <typename Input, typename R, typename ... Types>
    struct Call;

    template <typename Input, typename R>
    struct Call<Input, R>
    {
      template <typename S, typename ... Given>
      static R call(Input&, S const& f, Given&... args)
      {
        return f(args...);
      }
    };

    template <typename Input, typename R, typename First, typename ... Types>
    struct Call<Input, R, First, Types...>
    {
      template <typename S, typename ... Given>
      static R call(Input& input, S const& f, Given... args)
      {
        typename std::remove_const<typename std::remove_reference<First>::type>::type a;
        input >> a;
        return Call<Input, R, Types...>::call(input, f, args..., a);
      }
    };

    template <typename ISerializer,
              typename OSerializer,
              typename R,
              typename ... Args>
    void
    Procedure<ISerializer, OSerializer, R, Args...>::
    _call(ISerializer& in, OSerializer& out)
    {
      out << Call<ISerializer, R, Args...>::template call<>(in, _function);
    }

    template <typename ISerializer, typename OSerializer>
    template <typename R, typename ... Args>
    RPC<ISerializer, OSerializer>::RemoteProcedure<R, Args...>
    RPC<ISerializer, OSerializer>::add(boost::function<R (Args...)> const& f)
    {
      uint32_t id = _id++;
      typedef Procedure<ISerializer, OSerializer, R, Args...> Proc;
      _procedures[id] = std::unique_ptr<Proc>(new Proc(*this, id, f));
      return RemoteProcedure<R, Args...>(*this, id);
    }

    template <typename ISerializer, typename OSerializer>
    template <typename R, typename ... Args>
    RPC<ISerializer, OSerializer>::RemoteProcedure<R, Args...>
    RPC<ISerializer, OSerializer>::add()
    {
      uint32_t id = _id++;
      _procedures[id] = nullptr;
      return RemoteProcedure<R, Args...>(*this, id);
    }

    template <typename ISerializer, typename OSerializer>
    RPC<ISerializer, OSerializer>::RPC(ChanneledStream& channels)
      : BaseRPC(channels)
    {}

    template <typename ISerializer, typename OSerializer>
    void
    RPC<ISerializer, OSerializer>::run()
    {
      using elle::sprintf;
      using reactor::Exception;
      while (true)
        {
          Channel c(_channels.accept());
          Packet question(c.read());
          ISerializer input(question);
          uint32_t id;
          input >> id;
          auto procedure = _procedures.find(id);
          reactor::Scheduler& sched = _channels.scheduler();
          if (procedure == _procedures.end())
            throw Exception(sched, sprintf
                            ("call to unknown procedure: %s", id));
          if (procedure->second == nullptr)
            throw Exception(sched, sprintf
                            ("remote call to non-local procedure: %s", id));
          Packet answer;
          OSerializer output(answer);
          procedure->second->_call(input, output);
          // FIXME: auto flush
          answer.flush();
          c.write(answer);
        }
    }

    template <typename ISerializer, typename OSerializer>
    void
    RPC<ISerializer, OSerializer>::add(BaseRPC& rpc)
    {
      _rpcs.push_back(&rpc);
    }

    template <typename ISerializer, typename OSerializer>
    BaseProcedure<ISerializer, OSerializer>::~BaseProcedure()
    {}
  }
}

#endif
