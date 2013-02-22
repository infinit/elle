#ifndef LOGGER_HXX
# define LOGGER_HXX

namespace elle
{
  namespace log
  {
    template <typename I>
    RegisterIndenter<I>::RegisterIndenter()
    {
      std::function<std::unique_ptr<Indentation> ()>
        factory(Logger::_factory());
      Logger::_factory() =
        [factory]()
        {
          return std::unique_ptr<Indentation>(new I(factory));
        };
    };
  }
}

#endif
