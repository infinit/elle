#pragma once

#include <boost/filesystem.hpp>

#include <elle/attribute.hh>
#include <elle/Printable.hh>
#include <elle/With.hh>

namespace elle
{
  /// Atomic files ensure transactional writes to files.
  ///
  /// An atomic files ensures the previous version of the file is overwritten
  /// only if everything went well. Any unexpected shutdown of the program while
  /// writing new data will entail a rollback to the previous version.
  ///
  /// Use AtomicFile::read() to retrieve a stream to read the latest content
  /// successfully written to the file. Use the AtomicFile::write() to retrieve
  /// a stream to write new content to the file. The new data will be taken in
  /// account only when and if the returned stream is cleanly destroyed.
  ///
  /// @code{.cc}
  ///
  /// AtomicFile file("some/path");
  /// // Write initial content.
  /// {
  ///   Write write = file.write();
  ///   write.stream() << "data";
  ///   // The file will be effectively created when the Write object is
  ///   // destroyed here. If anything prevents this (SIGKILL, ...), the file
  ///   // will be considered inexistent upon the next run.
  /// }
  /// // Oneliner works too.
  /// file.write().stream() << "data";
  /// // Read data.
  /// std::string data;
  /// file.read().stream() >> data; // data == "data"
  /// // Write initial content.
  /// {
  ///   Write write = file.write();
  ///   write.stream() << "data2";
  ///   // The new content will be effectively commited when the Write object is
  ///   // destroyed here. If anything prevents this (SIGKILL, ...), the file
  ///   // will contain "data" upon the next run.
  /// }
  /// @endcode
  class ELLE_API AtomicFile:
    public elle::Printable
  {
  /*------.
  | Types |
  `------*/
  public:
    /// Self type.
    using Self = AtomicFile;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create an AtomicFile at the given path.
    AtomicFile(boost::filesystem::path const& path);
    /// Destruct an AtomicFile.
    ///
    /// @pre !this->reading()
    /// @pre !this->writing()
    ~AtomicFile();
  private:
    ELLE_ATTRIBUTE_R(boost::filesystem::path, path);

  /*-----------.
  | Attributes |
  `-----------*/
  public:
    /// Whether the file exists.
    ELLE_ATTRIBUTE_R(bool, exists);

  /*--------.
  | Reading |
  `--------*/
  public:
    /// A read operation to an atomic file.
    class Read
    {
    private:
      /// Move a read operation.
      Read(Read&& source);
      /// Destruct a read operation.
      ///
      /// @post !this->owner->reading()
      ~Read();
      /// Let With manage us.
      friend class elle::With<Read>;
    public:
      /// The stream to read content from.
      std::istream&
      stream();
      /// The AtomicFile this operation runs on.
      ELLE_ATTRIBUTE_R(AtomicFile*, owner);
    private:
      /// Create a Read for the given
      Read(AtomicFile* owner);
      /// Let the owner access us.
      friend class AtomicFile;
      /// The temporary file to read content from.
      std::unique_ptr<std::istream> _stream;
    };

    /// A ward containing stream to read content from.
    ///
    /// The file stays in read mode until the ward is destructed.
    ///
    /// @return A ward containing stream to read content from.
    /// @pre this->exists()
    /// @pre !this->reading()
    /// @pre !this->writing()
    /// @post this->reading()
    elle::With<Read>
    read() const;
    /// Whether the file is in read mode.
    ELLE_ATTRIBUTE_R(bool, reading);

  /*--------.
  | Writing |
  `--------*/
  public:
    /// A write operation to an atomic file.
    class Write
    {
    private:
      /// Move a write operation.
      Write(Write&& source);
      /// Destruct a write operation, commiting the changes.
      ///
      /// @post !this->owner->exists()
      /// @post !this->owner->writing()
      ~Write() noexcept(false);
      /// Let With manage us.
      friend class elle::With<Write>;
    public:
      /// The stream to write new content to.
      std::ostream&
      stream();
      /// The AtomicFile this operation runs on.
      ELLE_ATTRIBUTE_R(AtomicFile*, owner);
    private:
      /// Create a Write for the given
      Write(AtomicFile* owner);
      /// Let the owner access us.
      friend class AtomicFile;
      /// The temporary file to write content to.
      std::unique_ptr<std::ostream> _stream;
    };
    /// A ward containing stream to write new content to.
    ///
    /// The file stays in writing mode until the ward is destructed. The new
    /// content is effectively commited when leaving writing mode.
    ///
    /// @return A ward containing stream to write new content to.
    /// @pre !this->reading()
    /// @pre !this->writing()
    /// @post this->exists()
    /// @post this->writing()
    elle::With<Write>
    write();
    /// Whether the file is in write mode.
    ELLE_ATTRIBUTE_R(bool, writing);

  /*------------.
  | Transaction |
  `------------*/
  private:
    /// Clean up any stray transaction file and restore this in a clean state.
    bool _tidy() const;
    /// Commit the newly written content.
    void _commit();
    /// The path to store the new content being written.
    ELLE_ATTRIBUTE(boost::filesystem::path, path_new);
    /// The path to store the previous content while committing the new one.
    ELLE_ATTRIBUTE(boost::filesystem::path, path_old);

  /*----------.
  | Printable |
  `----------*/
  public:
    /// Print pretty representation to @a stream.
    void
    print(std::ostream& stream) const override;
  };
}
