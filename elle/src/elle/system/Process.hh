#ifndef ELLE_SYSTEM_PROCESS_HH
# define ELLE_SYSTEM_PROCESS_HH

# include <memory>
# include <string>
# include <vector>

# include <elle/attribute.hh>
# include <elle/compiler.hh>

namespace elle
{
  namespace system
  {
    class ELLE_API Process
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Process Self;
      typedef std::vector<std::string> Arguments;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Process(Arguments args);
      ~Process();

    /*-----------.
    | Attributes |
    `-----------*/
    public:
      ELLE_ATTRIBUTE_R(Arguments, arguments);

    /*--------.
    | Control |
    `--------*/
    public:
      int wait();
      int pid();

    /*---------------.
    | Implementation |
    `---------------*/
    private:
      class Impl;
      ELLE_ATTRIBUTE(std::unique_ptr<Impl>, impl);
    };
  }
}

#endif
