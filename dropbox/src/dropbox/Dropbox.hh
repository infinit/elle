#ifndef DROPBOX_DROPBOX_HH
# define DROPBOX_DROPBOX_HH

# include <boost/filesystem.hpp>

# include <elle/Buffer.hh>
# include <elle/Error.hh>
# include <elle/attribute.hh>
# include <elle/optional.hh>

# include <das/model.hh>
# include <das/serializer.hh>

# include <reactor/http/Request.hh>

namespace dropbox
{
  struct Error
    : public elle::Error
  {
    Error(boost::filesystem::path const& path, std::string const& reason);
  };

  struct DestinationExists
    : public Error
  {
    DestinationExists(boost::filesystem::path const& path);
  };

  struct NoSuchFile
    : public Error
  {
    NoSuchFile(boost::filesystem::path const& path);
  };

  struct AccountInfo
  {
    int uid;
    std::string display_name;
  };

  struct Metadata
  {
    bool is_dir;
    std::string path;
    int64_t bytes;
    boost::optional<std::string> client_mtime;
    boost::optional<std::string> modified;
    boost::optional<bool> is_deleted;
    boost::optional<bool> read_only;

    struct Content
    {
      std::string path;
      bool is_dir;
      int64_t bytes;
      boost::optional<std::string> client_mtime;
      boost::optional<std::string> modified;
    };
    boost::optional<std::vector<Content>> contents;
  };

  struct Longpoll
  {
    bool changes;
    boost::optional<int> backoff;
  };

  struct Delta
  {
    bool reset;
    std::string cursor;
    bool has_more;
    std::unordered_map<std::string, boost::optional<Metadata>> entries;
  };

  class Dropbox
  {
  public:
    Dropbox(std::string token, int block_size = 1048576);
    ~Dropbox();

    AccountInfo
    account_info();

    static
    std::string
    escape_path(boost::filesystem::path const& path);

    Metadata
    metadata(boost::filesystem::path const& path) const;

    elle::Buffer
    get(boost::filesystem::path const& path) const;

    elle::Buffer
    get(boost::filesystem::path const& path, int offset, int size) const;

    bool
    put(boost::filesystem::path const& path,
        elle::WeakBuffer const& content,
        bool overwrite = true);

    void
    create_folder(boost::filesystem::path const& path);

    void
    delete_(boost::filesystem::path const& path);

    void
    move(boost::filesystem::path const& from,
         boost::filesystem::path const& to);

    Delta
    delta(std::string cursor = "");

    std::string
    delta_latest_cursor();

    Longpoll
    longpoll_delta(std::string cursor);

  private:
    reactor::http::Request
    _request(std::string url,
             reactor::http::Method method = reactor::http::Method::GET,
             reactor::http::Request::QueryDict query =
               reactor::http::Request::QueryDict(),
             reactor::http::Request::Configuration =
               reactor::http::Request::Configuration()) const;

    reactor::http::Request
    _fileop(boost::filesystem::path const& path,
            std::string const& op,
            std::string const& path_arg = "path",
            reactor::http::Request::QueryDict query =
            reactor::http::Request::QueryDict());

    void
    _check_status(std::string const& op, reactor::http::Request& r) const;

    void
    _check_path(boost::filesystem::path const& path) const;

    elle::Buffer
    _get(boost::filesystem::path const& path,
         reactor::http::Request::Configuration conf) const;

    bool
    _ignored(boost::filesystem::path const& path) const;

    ELLE_ATTRIBUTE_R(std::string, token);
    ELLE_ATTRIBUTE_R(int, block_size);

  public:
    class Cache;
    class FileCache;
    ELLE_ATTRIBUTE(std::unique_ptr<Cache>, cache);
  };

  using das::operator <<;
}

DAS_MODEL_FIELDS(
  dropbox::AccountInfo,
  (uid, display_name));
DAS_MODEL_FIELDS(
  dropbox::Metadata::Content,
  (path, is_dir, bytes, client_mtime, modified));
DAS_MODEL_FIELDS(
  dropbox::Metadata,
  (is_dir, path, bytes, client_mtime, modified, is_deleted, read_only,
   contents));

namespace dropbox
{
  DAS_MODEL_DEFINE(
    AccountInfo,
    (uid, display_name),
    DasAccountInfo);
  DAS_MODEL_DEFINE(
    Metadata::Content,
    (path, is_dir, bytes, client_mtime, modified),
    DasMetadataContent);
  DAS_MODEL_DEFINE(
    Metadata,
    (is_dir, path, bytes, client_mtime, modified, is_deleted, read_only,
     contents),
    DasMetadata);
}

DAS_MODEL_DEFAULT(dropbox::AccountInfo, dropbox::DasAccountInfo);
DAS_MODEL_SERIALIZE(dropbox::AccountInfo);
DAS_MODEL_DEFAULT(dropbox::Metadata::Content, dropbox::DasMetadataContent);
DAS_MODEL_SERIALIZE(dropbox::Metadata::Content);
DAS_MODEL_DEFAULT(dropbox::Metadata, dropbox::DasMetadata);
DAS_MODEL_SERIALIZE(dropbox::Metadata);

#endif
