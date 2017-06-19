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

    template <typename C, typename F>
    void
    for_each_parallel(C&& c, F const& f, std::string const& name)
    {
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
            elle::print("{}: {}: {}",
                        reactor::scheduler().current()->name(),
                        name.empty() ? "for-each" : name,
                        elt),
            [
              e = Wrapper{std::forward<decltype(elt)>(elt)},
              &f, &scope]
            {
              try
              {
                elle::meta::static_if<lvalue>(
                  [&f] (auto&& e)
                  {
                    f(e.get());
                  },
                  [&f] (auto&& e)
                  {
                    f(std::forward<decltype(e)>(e));
                  })(std::forward<decltype(e)>(e));
              }
              catch (Break const&)
              {
                scope.terminate_now();
              }
            });
        }
        reactor::wait(scope);
      };
    }

    inline
    void
    break_parallel()
    {
      throw Break();
    }
  }
}
