#ifndef INFINIT_PROTOCOL_RPC_HXX
# define INFINIT_PROTOCOL_RPC_HXX

# include <type_traits>

# include <elle/concurrency/Scheduler.hh>
# include <elle/printf.hh>

# include <reactor/backtrace.hh>
# include <reactor/exception.hh>

# include <protocol/Channel.hh>
# include <protocol/ChanneledStream.hh>
# include <protocol/Packet.hh>

namespace infinit
{
  namespace protocol
  {
    /*--------------.
    | BaseProcedure |
    `--------------*/

    template <typename IS, typename OS>
    BaseProcedure<IS, OS>::
    BaseProcedure(std::string const& name):
      _name(name)
    {}

    template <typename IS, typename OS>
    BaseProcedure<IS, OS>::
    ~BaseProcedure()
    {}

    /*----------.
    | Procedure |
    `----------*/

    template <typename IS,
              typename OS,
              typename R,
              typename ... Args>
    Procedure<IS, OS, R, Args ...>::
    Procedure (std::string const& name,
               RPC<IS, OS>& owner,
               uint32_t id,
               boost::function<R (Args...)> const& f):
      BaseProcedure<IS, OS>(name),
      _id(id),
      _owner(owner),
      _function(f)
    {}

    template <typename IS,
              typename OS,
              typename R,
              typename ... Args>
    Procedure<IS, OS, R, Args ...>::
    ~Procedure()
    {}

    /*------------------------.
    | RemoteProcedure helpers |
    `------------------------*/

    template <typename OS>
    static
    void
    put_args(OS&)
    {}

    template <typename OS,
              typename T,
              typename ...Args>
    static
    void
    put_args(OS& output, T a, Args ... args)
    {
      output << a;
      put_args<OS, Args...>(output, args...);
    }

    template <typename IS,
              typename R>
    struct GetRes
    {
      static inline R get_res(IS& input)
      {
        R res;
        input >> res;
        return res;
      }
    };

    template <typename IS>
    struct GetRes<IS, void>
    {
      static inline
      void
      get_res(IS& input)
      {
        char c;
        input >> c;
      }
    };

    /*----------------.
    | RemoteProcedure |
    `----------------*/

    template <typename IS,
              typename OS>
    template <typename R,
              typename ... Args>
    RPC<IS, OS>::RemoteProcedure<R, Args ...>::
    RemoteProcedure(std::string const& name,
                    RPC<IS, OS>& owner):
      RemoteProcedure<R, Args...>(owner.add<R, Args...>(name))
    {}

    template <typename IS,
              typename OS>
    template <typename R,
              typename ... Args>
    RPC<IS, OS>::RemoteProcedure<R, Args ...>::
    RemoteProcedure(std::string const& name,
                    RPC<IS, OS>& owner,
                    uint32_t id):
      _id(id),
      _name(name),
      _owner(owner)
    {}

    template <typename IS,
              typename OS>
    template <typename R,
              typename ... Args>
    void
    RPC<IS, OS>::RemoteProcedure<R, Args ...>::
    operator = (boost::function<R (Args...)> const& f)
    {
      auto proc = _owner._procedures.find(_id);
      assert(proc != _owner._procedures.end());
      assert(proc->second == nullptr);
      typedef Procedure<IS, OS, R, Args...> Proc;
      Proc* res = new Proc(_name, _owner, _id, f);
      _owner._procedures[_id] = res;
    }


    template <typename IS,
              typename OS>
    template <typename R,
              typename ... Args>
    R
    RPC<IS, OS>::RemoteProcedure<R, Args...>::
    operator () (Args ... args)
    {
      Channel channel(_owner._channels);
      {
        Packet question;
        OS output(question);
        output << _id;
        put_args<OS, Args...>(output, args...);
        channel.write(question);
      }
      {
        Packet response(channel.read());
        IS input(response);
        bool res;
        input >> res;
        if (res)
          return GetRes<IS, R>::get_res(input);
        else
          {
            std::string error;
            input >> error;
            uint16_t bt_size;
            input >> bt_size;
            reactor::Backtrace bt;
            for (int i = 0; i < bt_size; ++i)
              {
                reactor::StackFrame frame;
                input >> frame.symbol;
                input >> frame.symbol_mangled;
                input >> frame.symbol_demangled;
                input >> frame.address;
                input >> frame.offset;
                bt.push_back(frame);
              }
            reactor::Exception* inner =
              new reactor::Exception(elle::concurrency::scheduler(), error, bt);
            reactor::Exception e(
              elle::concurrency::scheduler(), "remote procedure failed");
            e.inner_exception(inner);
            throw e;
          }
      }
    }

    /*------------------.
    | Procedure helpers |
    `------------------*/

    template <typename Input,
              typename R,
              typename ... Types>
    struct Call;

    template <typename Input,
              typename R>
    struct Call<Input, R>
    {
      template <typename S, typename ... Given>
      static
      R
      call(Input&,
           S const& f,
           Given&... args)
      {
        return f(args...);
      }
    };

    template <typename Input,
              typename R,
              typename First,
              typename ... Types>
    struct Call<Input, R, First, Types...>
    {
      template <typename S,
                typename ... Given>
      static
      R
      call(Input& input,
           S const& f,
           Given... args)
      {
        typename std::remove_const<
          typename std::remove_reference<First>::type>::type a;
        input >> a;
        return Call<Input, R, Types...>::call(input, f, args..., a);
      }
    };

    namespace
    {
      template <typename IS,
                typename OS,
                typename R,
                typename ... Args>
      struct VoidSwitch
      {
        static
        void
        call(IS& in,
             OS& out,
             boost::function<R (Args...)> const& f)
        {
          R res(Call<IS, R, Args...>::template call<>(in, f));
          out << true;
          out << res;
        }
      };

      template <typename IS,
                typename OS,
                typename ... Args>
      struct VoidSwitch<IS, OS, void, Args ...>
      {
        static
        void
        call(IS& in,
             OS& out,
             boost::function<void (Args...)> const& f)
        {
          Call<IS, void, Args...>::template call<>(in, f);
          out << true;
          unsigned char c(42);
          out << c;
        }
      };
    }

    /*----------.
    | Procedure |
    `----------*/

    template <typename IS,
              typename OS,
              typename R,
              typename ... Args>
    void
    Procedure<IS, OS, R, Args...>::
    _call(IS& in,
          OS& out)
    {
      std::string err;
      try
        {
          VoidSwitch<IS, OS, R, Args ...>::call(
            in, out, _function);
        }
      catch (reactor::Exception& e)
        {
          out << false;
          out << std::string(e.what());
          out << uint16_t(e.backtrace().size());
          for (auto const& frame: e.backtrace())
            {
              out << frame.symbol;
              out << frame.symbol_mangled;
              out << frame.symbol_demangled;
              out << frame.address;
              out << frame.offset;
            }
        }
      catch (std::exception& e)
        {
          out << false;
          out << std::string(e.what());
          out << uint16_t(0);
        }
      catch (...)
        {
          out << false;
          out << std::string("unknown error");
          out << uint16_t(0);
        }
    }

    /*----.
    | RPC |
    `----*/

    template <typename IS,
              typename OS>
    template <typename R,
              typename ... Args>
    RPC<IS, OS>::RemoteProcedure<R, Args...>
    RPC<IS, OS>::
    add(boost::function<R (Args...)> const& f)
    {
      uint32_t id = _id++;
      typedef Procedure<IS, OS, R, Args...> Proc;
      _procedures[id] = std::unique_ptr<Proc>(new Proc(*this, id, f));
      return RemoteProcedure<R, Args...>(*this, id);
    }

    template <typename IS,
              typename OS>
    template <typename R,
              typename ... Args>
    RPC<IS, OS>::RemoteProcedure<R, Args...>
    RPC<IS, OS>::
    add(std::string const& name)
    {
      uint32_t id = _id++;
      _procedures[id] = nullptr;
      return RemoteProcedure<R, Args...>(name, *this, id);
    }

    template <typename IS,
              typename OS>
    RPC<IS, OS>::
    RPC(ChanneledStream& channels):
      BaseRPC(channels)
    {}

    template <typename IS,
              typename OS>
    void
    RPC<IS, OS>::
    run()
    {
      using elle::sprintf;
      using reactor::Exception;
      while (true)
        {
          Channel c(_channels.accept());
          Packet question(c.read());
          IS input(question);
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
          OS output(answer);
          procedure->second->_call(input, output);
          c.write(answer);
        }
    }

    template <typename IS,
              typename OS>
    void
    RPC<IS, OS>::
    add(BaseRPC& rpc)
    {
      _rpcs.push_back(&rpc);
    }
  }
}

#endif
