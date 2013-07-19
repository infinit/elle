#ifndef LOGGER_HXX
# define LOGGER_HXX

# include <iostream>

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

    template <typename I>
    RegisterTag<I>::RegisterTag()
    {
      for (auto const& tag: Logger::_tags())
        if (tag->name() == I().name())
          return;
      Logger::_tags().push_back(std::unique_ptr<Tag>(new I()));
    };
  }
}

#endif
