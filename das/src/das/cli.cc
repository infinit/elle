#include <das/cli.hh>

namespace das
{
  namespace cli
  {
    void
    print_help(std::ostream& s,
               std::string const& name,
               bool with_argument,
               char short_name,
               std::string const& help)
    {
      if (short_name)
        elle::fprintf(s, "  -%s, ", short_name);
      else
        elle::fprintf(s, "      ");
      elle::fprintf(s, "--%-18s",
                    das::cli::option_name_from_c(name)
                    + (with_argument ? " arg" : ""));
      if (!help.empty())
        elle::fprintf(s, "  %s", help);
    }
  }
}
