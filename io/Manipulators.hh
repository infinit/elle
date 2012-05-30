#ifndef ELLE_IO_MANIPULATORS_HH
# define ELLE_IO_MANIPULATORS_HH

# include <elle/types.hh>

# include <elle/io/Unique.hh>

# include <elle/idiom/Close.hh>
#  include <iostream>
#  include <sstream>
#  include <string>
# include <elle/idiom/Open.hh>

namespace std
{

  elle::String    chop(const elle::String&,
                             const elle::Natural32 = 50);
  //template <const elle::io::Format F>
  //elle::String    chop(const elle::io::Uniquable<F>&,
  //                           const elle::Natural32 = 50);

}

#endif
