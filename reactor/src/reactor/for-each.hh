#ifndef REACTOR_FOR_EACH_HH
# define REACTOR_FOR_EACH_HH

# include <elle/Exception.hh>
# include <elle/compiler.hh>
# include <elle/With.hh>
# include <reactor/scheduler.hh>
# include <reactor/Scope.hh>

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
  for_each_parallel(C& c, F const& f);

  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  break_parallel();
}

# include <reactor/for-each.hxx>

#endif
