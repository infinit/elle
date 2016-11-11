#ifndef DROPBOX_DROPBOX_HH
# define DROPBOX_DROPBOX_HH

# include <boost/filesystem.hpp>

# include <elle/Buffer.hh>
# include <elle/Error.hh>
# include <elle/attribute.hh>
# include <elle/optional.hh>

# include <das/Symbol.hh>
# include <das/model.hh>
# include <das/printer.hh>
# include <das/serializer.hh>

# include <reactor/http/Request.hh>

namespace dropbox
{
  namespace symbols
  {
    DAS_SYMBOL(backoff);
    DAS_SYMBOL(bytes);
    DAS_SYMBOL(changes);
    DAS_SYMBOL(client_mtime);
    DAS_SYMBOL(contents);
    DAS_SYMBOL(cursor);
    DAS_SYMBOL(display_name);
    DAS_SYMBOL(entries);
    DAS_SYMBOL(has_more);
    DAS_SYMBOL(is_deleted);
    DAS_SYMBOL(is_dir);
    DAS_SYMBOL(modified);
    DAS_SYMBOL(path);
    DAS_SYMBOL(read_only);
    DAS_SYMBOL(reset);
    DAS_SYMBOL(uid);
  }

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
    using Model = das::Model<AccountInfo,
                             elle::meta::List<
                               symbols::Symbol_uid,
                               symbols::Symbol_display_name>>;

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
      using Model = das::Model<
        Content,
        decltype(elle::meta::list(symbols::path,
                                  symbols::is_dir,
                                  symbols::bytes,
                                  symbols::client_mtime,
                                  symbols::modified))>;
    };
    boost::optional<std::vector<Content>> contents;

    using Model = das::Model<
      Metadata,
      decltype(elle::meta::list(symbols::is_dir,
                                symbols::path,
                                symbols::client_mtime,
                                symbols::modified,
                                symbols::is_deleted,
                                symbols::read_only,
                                symbols::contents))>;
  };

  struct Longpoll
  {
    bool changes;
    boost::optional<int> backoff;
    using Model = das::Model<
      Longpoll,
      decltype(elle::meta::list(symbols::changes,
                                symbols::backoff))>;
  };

  struct Delta
  {
    bool reset;
    std::string cursor;
    bool has_more;
    std::unordered_map<std::string, boost::optional<Metadata>> entries;
    using Model = das::Model<
      Delta,
      decltype(elle::meta::list(symbols::reset,
                                symbols::cursor,
                                symbols::has_more,
                                symbols::entries))>;
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

    boost::optional<Metadata>
    local_metadata(boost::filesystem::path const& path) const;

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
               reactor::http::Request::Configuration(),
             elle::ConstWeakBuffer const& payload = {},
             std::string const& op = "",
             std::vector<reactor::http::StatusCode> expected_codes =
               {} /* OK is automaticalyl added*/
             ) const;

    reactor::http::Request
    _fileop(boost::filesystem::path const& path,
            std::string const& op,
            std::vector<reactor::http::StatusCode> expected_codes = {},
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


namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<dropbox::AccountInfo>
      : public das::Serializer<dropbox::AccountInfo>
    {};

    template <>
    struct Serialize<dropbox::Metadata::Content>
      : public das::Serializer<dropbox::Metadata::Content>
    {};

    template <>
    struct Serialize<dropbox::Metadata>
      : public das::Serializer<dropbox::Metadata>
    {};
  }
}

#endif
