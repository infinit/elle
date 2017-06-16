#pragma once

#include <elle/Exception.hh>
#include <elle/compiler.hh>

namespace elle
{
  namespace reactor
  {
    /// Apply a given function to every item of a given container in parallel.
    ///
    /// @code{.cc}
    ///
    /// // Consider a webservice that calculates the square of a given integer.
    /// // web_square makes the HTTP call and returns the value.
    ///
    /// int
    /// web_square(int t);
    ///
    /// auto c = std::vector<int>{1, 2, 3, 4};
    ///
    /// elle::reactor::for_each_parallel(
    ///   c,
    ///   [&] (int& e)
    ///   {
    ///     std::cout << web_square(e) << " ";
    ///   });
    /// // Result (arbitrary order): 9 16 1 4
    ///
    /// @endcode
    template <typename C, typename F>
    void
    for_each_parallel(C&& c, F const& f, std::string const& name = {});

    /// Break exception used to break for_each_parallel execution.
    class Break
      : public elle::Exception
    {
    public:
      Break();
    };

    /// Break from a for_each_parallel loop by throwing a Break exception.
    ELLE_COMPILER_ATTRIBUTE_NORETURN
    void
    break_parallel();
  }
}

#include <elle/reactor/for-each.hxx>
