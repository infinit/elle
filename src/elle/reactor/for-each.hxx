#include <elle/With.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    inline
    Break::Break()
      : elle::Exception("break")
    {}

    inline
    Continue::Continue()
      : elle::Exception("continue")
    {}



    template <typename C, typename F>
    auto
    _for_each_parallel(C&& c, F const& f, std::string const& name)
      -> decltype(details::for_each_parallel_result(f, std::forward<C>(c)))
    {
      using Type = decltype(f(*std::begin(c)));
      auto constexpr valued = !std::is_same<Type, void>::value;
      std::vector<std::conditional_t<valued, Type, bool>> res;
      elle::With<reactor::Scope>(name) << [&] (reactor::Scope& scope)
      {
        for (auto&& elt: std::forward<C>(c))
        {
          auto constexpr lvalue =
            std::is_lvalue_reference<decltype(elt)>::value;
          using Wrapper = std::conditional_t<
            lvalue,
            std::reference_wrapper<std::remove_reference_t<decltype(elt)>>,
            decltype(elt)>;
          scope.run_background(
            elle::print("{}: {}: %f",
                        reactor::scheduler().current()->name(),
                        name.empty() ? "for-each" : name,
                        elt),
            [
              e = Wrapper{std::forward<decltype(elt)>(elt)},
              &f, &scope, &res]
            {
              try
              {
                elle::meta::static_if<valued>(
                  [&] (auto& v)
                  {
                    elle::meta::static_if<lvalue>(
                      [&] (auto&& e)
                      {
                        v.emplace_back(f(e.get()));
                      },
                      [&] (auto&& e)
                      {
                        v.emplace_back(f(std::forward<decltype(e)>(e)));
                      })(std::forward<decltype(e) const>(e));
                  },
                  [&] (auto&)
                  {
                    elle::meta::static_if<lvalue>(
                      [&] (auto&& e)
                      {
                        f(e.get());
                      },
                      [&] (auto&& e)
                      {
                        f(std::forward<decltype(e)>(e));
                      })(std::forward<decltype(e) const>(e));
                  })(res);
              }
              catch (Break const&)
              {
                scope.terminate_now();
              }
              catch (Continue const&)
              {}
            });
        }
        reactor::wait(scope);
      };
      return elle::meta::static_if<valued>(
        [] (auto& res) { return res; },
        [] (auto& res) {})(res);
    }

    template <typename C, typename F>
    auto
    for_each_parallel(C&& c, F const& f, std::string const& name)
      -> decltype(details::for_each_parallel_result(f, std::forward<C>(c)))
    {
      return _for_each_parallel(c, f, name);
    }

    template <typename E, typename F>
    auto
    for_each_parallel(std::initializer_list<E> const& c,
                      F const& f, std::string const& name)
      -> decltype(details::for_each_parallel_result(f, c))

    {
      return _for_each_parallel<std::initializer_list<E> const&, F>(c, f, name);
    }

    inline
    void
    break_parallel()
    {
      throw Break();
    }

    inline
    void
    continue_parallel()
    {
      throw Continue();
    }
  }
}
