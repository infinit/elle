#ifndef ELLE_FACTORY_HH
 # define ELLE_FACTORY_HH

#include <elle/Error.hh>
#include <elle/memory.hh>
#include <unordered_map>

namespace elle
{
  namespace _details
  {
    template <typename F>
    class Factory
    {
    public:
      using Type = decltype(std::declval<F>()());

      Factory(F f)
        : _f(f)
      {}

      template <typename C>
      operator C() const
      {
        return this->_f();
      }

      ELLE_ATTRIBUTE(F, f);
    };
  }

  template <typename F>
  _details::Factory<F>
  factory(F f)
  {
    return _details::Factory<F>(std::move(f));
  }

  /// A generic Factory.
  ///
  /// \code{.cc}
  ///
  /// struct Foo;
  /// {
  ///   Foo() = default;
  ///   virtual
  ///     ~Foo() = default;
  /// };
  ///
  /// struct FooA
  ///   : public Foo
  /// {
  ///   FooA() = default;
  ///   std::string v = {"I'm a A"};
  /// };
  ///
  /// struct FooB
  ///   : public Foo
  /// {
  ///   FooB() = default;
  ///   std::string u = {"I'm a B"};
  /// };
  ///
  /// elle::Factory<Foo>::register_(
  ///   "A", [] (elle::Factory<Foo>::Arguments const&)
  ///   {
  ///     return std::make_unique<FooA>();
  ///   });
  /// elle::Factory<Foo>::register_(
  ///   "B", [] (elle::Factory<Foo>::Arguments const&)
  ///   {
  ///     return std::make_unique<FooB>();
  ///   });
  /// auto a = elle::cast<FooA>::runtime(
  ///   std::move(elle::Factory<Foo>::instantiate("A", {})));
  /// std::cout << a->v << ", ";
  /// auto _b = elle::Factory<Foo>::instantiate("B", {});
  /// auto b = elle::cast<FooB>::compiletime(_b);
  /// std::cout << b->u;
  ///
  /// // Result: I'm a A, I'm a B.
  ///
  /// \endcode
  ///
  template<typename T>
  class Factory
  {
  public:
    class KeyError
      : public Error
    {
    public:
      KeyError(std::string const& key)
        : Error("No such key: " + key)
      {}
    };
    using Arguments = std::vector<std::string>;
    using Builder = std::function<std::unique_ptr<T>(Arguments const&)>;
    /// Register a Builder for a given name.
    ///
    /// @param name The name of the Builder.
    /// @param builder The builder.
    /// @returns 0
    static
    int
    register_(std::string const& name, Builder builder);
    /// Instantiate a std::unique_ptr<\T> using the requested Builder.
    ///
    /// @param name The name of the Builder to use.
    /// @param args The Arguments.
    /// @returns An std::unique_ptr.
    ///
    /// @throw KeyError If no Builder was registered under the given name.
    static
    std::unique_ptr<T>
    instantiate(std::string const& name, Arguments const& args);
  private:
    using Items = std::unordered_map<std::string, Builder>;
    static
    Items&
    _items();
  };

#define FACTORY_REGISTER(type, name, builder)                       \
  static int __attribute__((unused))                                \
  unused = elle::Factory<type>::register_(name, builder)

}

#include <elle/factory.hxx>


#endif
