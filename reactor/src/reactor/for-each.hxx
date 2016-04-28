#ifndef REACTOR_FOR_EACH_HXX
# define REACTOR_FOR_EACH_HXX

namespace reactor
{
  inline
  Break::Break()
    : elle::Exception("break")
  {}

  template <typename C, typename F>
  void
  for_each_parallel(C& c, F const& f)
  {
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      for (auto& elt: c)
        scope.run_background(
          elle::sprintf("%s: for_each: %s",
                        reactor::scheduler().current()->name(),
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

#endif
