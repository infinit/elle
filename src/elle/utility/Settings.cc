#include <elle/utility/Settings.hh>

#include <elle/io/Dumpable.hh>

namespace elle
{
  namespace utility
  {

    Status              Settings::Dump(const Natural32          margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Settings]" << std::endl;

      auto section_it = this->begin(),
           section_end = this->end();
      for (; section_it != section_end; ++section_it)
        {
          std::cout << alignment << io::Dumpable::Shift
                    << "[Section] " << section_it->first << std::endl;

          auto pair_it = section_it->second->begin(),
               pair_end = section_it->second->end();
          for (; pair_it != pair_end; ++pair_it)
            {
              std::cout << alignment << io::Dumpable::Shift
                        << io::Dumpable::Shift
                        << "[" << pair_it->first << "] "
                        << pair_it->second << std::endl;
            }
        }

      return Status::Ok;
    }

  }
}
