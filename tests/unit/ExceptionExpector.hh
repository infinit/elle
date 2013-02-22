#ifndef TEST_UNIT_EXCEPTIONEXPECTOR_HH
# define TEST_UNIT_EXCEPTIONEXPECTOR_HH

namespace test
{
  namespace unit
  {
    /// VExceptionExpector is a function wrapper used to handle Exception and
    /// compare it to a given instance of this Exception. It means that
    /// Exception must have operator == overloaded.
    ///
    /// The overloaded operator () allow to call your function with exactly the
    /// same args, except the first one, which is the instance of Exception to
    /// compare to.
    ///
    /// If the expected instance of Expected instance of exception appears, the
    /// call returns normaly.
    /// If nothing is thrown, an std::runtime_error is thrown. The function was
    /// expected to throw.
    /// If an another instance of Exception or an other exception is thrown,
    /// this exception is rethrown.
    template <typename Exception, typename Func>
    class VExceptionExpector
    {
    public:
      /// Build a VExceptionExpector around a given Func f.
      VExceptionExpector(Func&& f);

    private:
      /// Run the function with given args and compare the thrown Exception with
      /// the given one.
      template <typename... Args>
      void
      _run(Exception const& expected, Args&&... args);

    public:
      /// Sugar to _run.
      template <typename... Args>
      void
      operator () (Exception const& expected, Args&&... args);

    private:
      Func _f;
    };

    /// Sugar to avoid passing Func as template parameter of VExceptionExpector.
    template <typename Exception, typename Func>
    VExceptionExpector<Exception, Func>
    build_vexception_expector(Func&& f);
  }
}

#endif
