#pragma once

# include <elle/Exception.hh>
# include <elle/With.hh>
# include <elle/compiler.hh>
# include <elle/reactor/Scope.hh>
# include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    class Break
      : public elle::Exception
    {
    public:
      Break();
    };

    template <typename C, typename F>
    void
    for_each_parallel(C& c, F const& f, std::string const& name = std::string{});

    ELLE_COMPILER_ATTRIBUTE_NORETURN
    void
    break_parallel();
  }
}
# include <elle/reactor/for-each.hxx>
