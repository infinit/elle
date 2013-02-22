#ifndef EXCEPTIONEXPECTOR_HXX
# define EXCEPTIONEXPECTOR_HXX

# include "ExceptionExpector.hh"


namespace test
{
  namespace unit
  {
    template <typename Exception, typename Func>
    VExceptionExpector<Exception, Func>
    build_vexception_expector(Func&& f)
    {
      return VExceptionExpector<Exception, Func>{f};
    }

    template <typename Exception, typename Func>
    VExceptionExpector<Exception, Func>::VExceptionExpector(Func&& f)
      : _f{f}
    {}

    template <typename Exception, typename Func>
    template <typename... Args>
    void
    VExceptionExpector<Exception, Func>::_run(Exception const& expected,
                                             Args&&... args)
    {
      try
      {
        this->_f(std::forward<Args>(args)...);

        // If we reach this code, it means we didn't throw.
        throw std::runtime_error("Exception expector didn't throw.");
      }
      catch (Exception const& e)
      {
        if (!(e == expected))
          throw;

      }
    }

    template <typename Exception, typename Func>
    template <typename... Args>
    void
    VExceptionExpector<Exception, Func>::operator () (
      Exception const& expected,
      Args&&... args)
    {
      this->_run(expected, std::forward<Args>(args)...);
    }

  }
}

#endif
