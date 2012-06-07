#ifndef DIARY_MEMOIRS_HH
# define DIARY_MEMOIRS_HH

# include <boost/noncopyable.hpp>

# include <elle/types.hh>
# include <elle/concept/Fileable.hh>
# include <elle/io/Dumpable.hh>
# include <elle/utility/Buffer.hh>


namespace satellite
{

  ///
  /// this class represents the diary in its archived form.
  ///
  class Memoirs
    : public elle::radix::Object
    , public elle::io::Dumpable
    , public elle::concept::Fileable<Memoirs>
    , private boost::noncopyable
  {
  public:
    Memoirs();
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // XXX
    elle::utility::Buffer         archive;
    size_t                        offset;
  };

}

#endif
#include <satellites/diary/Memoirs.hxx>
