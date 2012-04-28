#ifndef DIARY_MEMOIRS_HH
# define DIARY_MEMOIRS_HH

# include <elle/types.hh>
# include <elle/io/Fileable.hh>
# include <elle/io/Dumpable.hh>
# include <elle/utility/Buffer.hh>


namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the diary in its archived form.
  ///
  class Memoirs:
    public elle::radix::Object,
    public elle::io::Dumpable,
    public elle::io::Fileable<Memoirs>
  {
  public:
    //
    // interfaces
    //

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    //elle::Status        Load(const elle::Path&);
    //elle::Status        Store(const elle::Path&) const;

    //
    // attributes
    //
    //elle::Archive               archive;
    elle::utility::Buffer         archive;
  };

}

#endif
