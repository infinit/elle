#ifndef ELLE_OS_LOCALE_HH
# define ELLE_OS_LOCALE_HH

# include <locale>

# include <boost/optional.hpp>

# include <elle/attribute.hh>

namespace elle
{
  namespace os
  {
    namespace locale
    {
      // XXX: Release 0.9.3.
      // Qt and boost filesystem conflict with locales on Windows, making boost
      // filesystem behave weirdly with utf8 paths.
      //
      // The problem occurs on
      // > elle.os.file.size
      // > elle.system.read_file_chunk
      // > every other places we open a stream.
      // -----------------------------------------------------------------------
      // Default:
      // boost::filesystem::exists(utf8_path); > True
      // boost::filesystem::ifstream f(utf8_path) > f.good() == False
      //
      // -----------------------------------------------------------------------
      // boost::filesystem::path::imbue(std::locale(""));
      // boost::filesystem::exists(utf8_path) > False
      // boost::filesystem::ifstream f(utf8_path) > f.good() == True.
      //
      // -----------------------------------------------------------------------
      // boost::filesystem::path::imbue(utf8_locale);
      // boost::filesystem::exists(utf8_path) > True
      // boost::filesystem::ifstream f(utf8_path) > f.good() == False.
      //
      // Can't reproduce it with a minimal test case, so to fix utf8 on Windows
      // for 0.9.3, let's imbue the right locale at the right time.
      // If INFINIT_WINDOWS is not defined, thoose classes constructors and
      // destructors are no-op.
      // XXX: Works with some utf8, not all of them... Further investigation
      // requiered.
      class Imbuer
      {
      protected:
        Imbuer(std::locale const&);
        Imbuer();
        virtual
        ~Imbuer();

        ELLE_ATTRIBUTE_R(boost::optional<std::locale>, previous_locale);
      };

      class DefaultImbuer:
        public Imbuer
      {
      public:
        DefaultImbuer();
        virtual
        ~DefaultImbuer() = default;
      };
    }
  }
}

#endif
