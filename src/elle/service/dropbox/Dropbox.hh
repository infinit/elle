#pragma once

#include <boost/filesystem.hpp>

#include <elle/Buffer.hh>
#include <elle/Error.hh>
#include <elle/attribute.hh>
#include <elle/optional.hh>

#include <elle/das/Symbol.hh>
#include <elle/das/model.hh>
#include <elle/das/printer.hh>
#include <elle/das/serializer.hh>

#include <elle/reactor/http/Request.hh>

namespace elle
{
  namespace service
  {
    namespace dropbox
    {
      namespace symbols
      {
        ELLE_DAS_SYMBOL(backoff);
        ELLE_DAS_SYMBOL(bytes);
        ELLE_DAS_SYMBOL(changes);
        ELLE_DAS_SYMBOL(client_mtime);
        ELLE_DAS_SYMBOL(contents);
        ELLE_DAS_SYMBOL(cursor);
        ELLE_DAS_SYMBOL(display_name);
        ELLE_DAS_SYMBOL(entries);
        ELLE_DAS_SYMBOL(has_more);
        ELLE_DAS_SYMBOL(is_deleted);
        ELLE_DAS_SYMBOL(is_dir);
        ELLE_DAS_SYMBOL(modified);
        ELLE_DAS_SYMBOL(path);
        ELLE_DAS_SYMBOL(read_only);
        ELLE_DAS_SYMBOL(reset);
        ELLE_DAS_SYMBOL(uid);
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
        using Model = elle::das::Model<AccountInfo,
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
          using Model = elle::das::Model<
            Content,
            decltype(elle::meta::list(symbols::path,
                                      symbols::is_dir,
                                      symbols::bytes,
                                      symbols::client_mtime,
                                      symbols::modified))>;
        };
        boost::optional<std::vector<Content>> contents;

        using Model = elle::das::Model<
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
        using Model = elle::das::Model<
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
        using Model = elle::das::Model<
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
        elle::reactor::http::Request
        _request(std::string url,
                 elle::reactor::http::Method method = elle::reactor::http::Method::GET,
                 elle::reactor::http::Request::QueryDict query =
                   elle::reactor::http::Request::QueryDict(),
                 elle::reactor::http::Request::Configuration =
                   elle::reactor::http::Request::Configuration(),
                 elle::ConstWeakBuffer const& payload = {},
                 std::string const& op = "",
                 std::vector<elle::reactor::http::StatusCode> expected_codes =
                   {} /* OK is automaticalyl added*/
                 ) const;

        elle::reactor::http::Request
        _fileop(boost::filesystem::path const& path,
                std::string const& op,
                std::vector<elle::reactor::http::StatusCode> expected_codes = {},
                std::string const& path_arg = "path",
                elle::reactor::http::Request::QueryDict query =
                elle::reactor::http::Request::QueryDict());

        void
        _check_status(std::string const& op, elle::reactor::http::Request& r) const;

        void
        _check_path(boost::filesystem::path const& path) const;

        elle::Buffer
        _get(boost::filesystem::path const& path,
             elle::reactor::http::Request::Configuration conf) const;

        bool
        _ignored(boost::filesystem::path const& path) const;

        ELLE_ATTRIBUTE_R(std::string, token);
        ELLE_ATTRIBUTE_R(int, block_size);

      public:
        class Cache;
        class FileCache;
        ELLE_ATTRIBUTE(std::unique_ptr<Cache>, cache);
      };

      using elle::das::operator <<;
    }
  }

  namespace serialization
  {
    template <>
    struct Serialize<services::dropbox::AccountInfo>
      : public das::Serializer<services::dropbox::AccountInfo>
    {};

    template <>
    struct Serialize<services::dropbox::Metadata::Content>
      : public das::Serializer<services::dropbox::Metadata::Content>
    {};

    template <>
    struct Serialize<services::dropbox::Metadata>
      : public das::Serializer<services::dropbox::Metadata>
    {};
  }
}
