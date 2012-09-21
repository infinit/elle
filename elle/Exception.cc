#include <elle/Exception.hh>
#include <elle/standalone/Report.hh>

namespace elle
{

  elle::String
  Exception::_add_report()
  {
    std::stringstream ss;
    for (auto const& entry: standalone::Report::report.Get().container)
      {
        ss << entry->location << ": " << entry->message << '\n';
      }
    if (ss.str().size() > 0)
      return "\nReport: (most recent call last)\n" + ss.str();
    return "";
  }

}

