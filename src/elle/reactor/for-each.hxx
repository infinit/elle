#pragma once

# include <elle/reactor/Scope.hh>
# include <elle/reactor/scheduler.hh>

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
    for_each_parallel(C& c, F const& f, std::string const& name)
    {
      elle::With<reactor::Scope>(name) << [&] (reactor::Scope& scope)
      {
        for (auto& elt: c)
          scope.run_background(
            elle::sprintf("%s: %s: %s",
                          reactor::scheduler().current()->name(),
                          name.empty() ? "for-each" : name,
                          elt),
            [&]
            {
              try
              {
                f(elt);
              }
              catch (Break const&)
              {
                scope.terminate_now();
              }
            });
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
