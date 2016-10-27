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
      // If set_uid is true, set real uid/gid to effective uid/gid before exec
      Process(Arguments args, bool set_uid = false);
      ~Process();

    /*-----------.
    | Attributes |
    `-----------*/
    public:
      ELLE_ATTRIBUTE_R(Arguments, arguments);
      ELLE_ATTRIBUTE_R(bool, set_uid);

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
