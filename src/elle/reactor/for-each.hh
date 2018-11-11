#pragma once

#include <elle/Exception.hh>
#include <elle/compiler.hh>

namespace elle
{
  namespace reactor
  {
    namespace details
    {
      template <typename T>
      struct _for_each_parallel_result
      {
        using type = std::conditional_t<
          std::is_same<T, void>::value, void, std::vector<T>>;
      };

      template <typename F, typename C>
      auto
      for_each_iteration_result(F const& f, C&& c)
      {
        for (auto&& elt: std::forward<C>(c))
          return f(std::forward<decltype(elt)>(elt));
        elle::unreachable();
      }

      template <typename F, typename C>
      auto
      for_each_parallel_result(F const& f, C&& c)
        -> typename _for_each_parallel_result<
          decltype(for_each_iteration_result(f, std::forward<C>(c)))>::type;
    }

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
    auto
    for_each_parallel(C&& c, F const& f, std::string const& name = {})
      -> decltype(details::for_each_parallel_result(f, std::forward<C>(c)));
    /// Initializer list overload, for convenience.
    template <typename E, typename F>
    auto
    for_each_parallel(std::initializer_list<E> const& c,
                      F const& f, std::string const& name = {})
      -> decltype(details::for_each_parallel_result(f, c));

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

    /// Continue exception used to skip current iteration of for_each_parallel.
    class Continue
      : public elle::Exception
    {
    public:
      Continue();
    };

    /// Continue from a for_each_parallel loop by throwing a Continue exception.
    ELLE_COMPILER_ATTRIBUTE_NORETURN
    void
    continue_parallel();
  }
}

#include <elle/reactor/for-each.hxx>
