#ifndef SATELLITES_DIARY_MEMOIRS_HXX
# define SATELLITES_DIARY_MEMOIRS_HXX

# include <elle/utility/Buffer.hh>

ELLE_SERIALIZE_SIMPLE(satellite::Memoirs,
                      archive,
                      memoirs,
                      version)
{
  enforce(version == 0);
  archive & memoirs.archive;
}

#endif
