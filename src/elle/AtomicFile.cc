#include <fstream>

#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/AtomicFile.hh>

ELLE_LOG_COMPONENT("elle.AtomicFile");

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  namespace
  {
    bfs::path
    mitigate(bfs::path const& path, std::string const& salt)
    {
      auto filename = "." + path.filename().string() + "." + salt;
      return path.parent_path() / filename;
    }
  }

  AtomicFile::AtomicFile(bfs::path const& path)
    : _path(path)
    , _exists()
    , _reading(false)
    , _writing(false)
    , _path_new(mitigate(path, "new"))
    , _path_old(mitigate(path, "old"))
  {
    this->_exists = this->_tidy();
  }

  AtomicFile::~AtomicFile()
  {
    ELLE_ASSERT(!this->reading());
    ELLE_ASSERT(!this->writing());
  }

  /*--------.
  | Reading |
  `--------*/

  AtomicFile::Read::Read(AtomicFile* owner):
    _owner(owner),
    _stream(new std::ifstream(owner->_path.string()))
  {
    this->_owner->_reading = true;
  }

  AtomicFile::Read::Read(Read&& source):
    _owner(std::move(source._owner)),
    _stream(std::move(source._stream))
  {
    source._owner = nullptr;
  }

  AtomicFile::Read::~Read()
  {
    if (this->_owner)
    {
      ELLE_TRACE_SCOPE("%s: close for reading", *this->_owner);
      this->_owner->_reading = false;
    }
  }

  std::istream&
  AtomicFile::Read::stream()
  {
    return *this->_stream;
  }

  elle::With<AtomicFile::Read>
  AtomicFile::read() const
  {
    ELLE_TRACE_SCOPE("%s: open for reading", *this);
    ELLE_ASSERT(this->exists());
    ELLE_ASSERT(!this->reading());
    ELLE_ASSERT(!this->writing());
    auto self = const_cast<AtomicFile*>(this);
    return elle::With<Read>(self);
  }

  /*--------.
  | Writing |
  `--------*/

  AtomicFile::Write::Write(AtomicFile* owner)
    : _owner(owner)
    , _stream(std::make_unique<std::ofstream>(owner->_path_new.string()))
  {
    this->_owner->_writing = true;
  }

  AtomicFile::Write::Write(Write&& source)
    : _owner(std::move(source._owner))
    , _stream(std::move(source._stream))
  {
    source._owner = nullptr;
  }

  AtomicFile::Write::~Write() noexcept(false)
  {
    if (this->_owner)
    {
      ELLE_TRACE_SCOPE("%s: close for writing", *this->_owner);
      this->_owner->_writing = false;
      this->_stream.reset();
      this->_owner->_commit();
    }
  }

  std::ostream&
  AtomicFile::Write::stream()
  {
    return *this->_stream;
  }

  elle::With<AtomicFile::Write>
  AtomicFile::write()
  {
    ELLE_TRACE_SCOPE("%s: open for writing", *this);
    ELLE_ASSERT(!this->reading());
    ELLE_ASSERT(!this->writing());
    auto self = const_cast<AtomicFile*>(this);
    return elle::With<Write>(self);
  }

  /*------------.
  | Transaction |
  `------------*/

  bool
  AtomicFile::_tidy() const
  {
    ELLE_DEBUG_SCOPE("%s: cleanup transaction state", *this);
    if (bfs::exists(this->_path))
    {
      ELLE_DEBUG_SCOPE("%s: file exists", *this);
      // If the base file exists, it is the current valid version, remove any
      // potential stray transaction file.
      if (bfs::exists(this->_path_new))
      {
        ELLE_WARN("%s: discard aborted new content", *this);
        remove(this->_path_new);
      }
      if (bfs::exists(this->_path_old))
      {
        ELLE_WARN("%s: remove stray old content", *this);
        remove(this->_path_old);
      }
      return true;
    }
    else if (bfs::exists(this->_path_old))
    {
      ELLE_DEBUG_SCOPE("%s: transaction failed", *this);
      // Otherwise if an old file exists, a transaction failed, restore it.
      {
        ELLE_WARN("%s: restore old content of failed transaction", *this);
        rename(this->_path_old, this->_path);
      }
      if (bfs::exists(this->_path_new))
      {
        ELLE_WARN("%s: discard aborted new content", *this);
        remove(this->_path_new);
      }
      return true;
    }
    else
    {
      ELLE_DEBUG_SCOPE("%s: file does not exist", *this);
      // Otherwise, the file never had any valid data written to it, remove any
      // potential stray transaction file.
      if (bfs::exists(this->_path_new))
      {
        ELLE_WARN("%s: discard aborted new content", *this);
        remove(this->_path_new);
      }
      return false;
    }
  }

  void
  AtomicFile::_commit()
  {
    bool const present = bfs::exists(this->_path);
    if (present)
    {
      ELLE_DEBUG("%s: save old version", *this);
      rename(this->_path, this->_path_old);
    }
    ELLE_DEBUG("%s: rename new version", *this);
    rename(this->_path_new, this->_path);
    this->_exists = true;
    if (present)
    {
      ELLE_DEBUG("%s: remove old version", *this);
      remove(this->_path_old);
    }
  }

  /*----------.
  | Printable |
  `----------*/

  void
  AtomicFile::print(std::ostream& stream) const
  {
    stream << "AtomicFile(\"" << this->_path << "\")";
  }
}
