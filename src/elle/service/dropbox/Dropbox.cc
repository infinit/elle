#include <elle/IntRange.hh>
#include <elle/find.hh>
#include <elle/log.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/Thread.hh>
#include <elle/reactor/http/url.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/serialization/json.hh>
#include <elle/service/dropbox/Dropbox.hh>

ELLE_LOG_COMPONENT("elle.services.dropbox.Dropbox");

namespace bfs = boost::filesystem;

std::string
to_lower(bfs::path const& p)
{
  return boost::to_lower_copy(p.string());
}

struct HashPath
{
  size_t
  operator()(bfs::path const& p) const
  {
    return std::hash<std::string>()(to_lower(p));
  }
};

struct ComparePath
{
  size_t
  operator()(bfs::path const& lhs,
             bfs::path const& rhs) const
  {
    return to_lower(lhs) == to_lower(rhs);
  }
};

/// A case insensitive map of paths.
template <typename Value>
using PathMap
  = std::unordered_map<bfs::path, Value, HashPath, ComparePath>;

ELLE_DAS_SERIALIZE(elle::service::dropbox::Longpoll);
ELLE_DAS_SERIALIZE(elle::service::dropbox::Delta);

namespace elle
{
  namespace service
  {
    namespace dropbox
    {
      class Dropbox::Cache
      {
      public:
        virtual
        ~Cache()
        {}

        virtual
        boost::optional<Metadata>
        metadata(bfs::path const& path)
        {
          return {};
        }

        virtual
        void
        metadata_update(bfs::path const&, Metadata)
        {}

        virtual
        void
        metadata_delete(bfs::path const& path)
        {}
      };

      class LongPollCache
        : public Dropbox::Cache
      {
      public:
        LongPollCache(Dropbox& dropbox, bool full = true)
          : _dropbox(dropbox)
          , _full(full)
          , _initialized()
          , _metadata()
          , _poll_thread(elle::sprintf("%s poll thread", *this),
                         [this] { this->_poll(); })
        {}

        ~LongPollCache() override
        {
          // FIXME: Add a thread mode that terminates upon destruction.
          this->_poll_thread.terminate_now();
        }

        boost::optional<Metadata>
        metadata(bfs::path const& path) override
        {
          if (auto it = elle::find(this->_metadata, path))
          {
            ELLE_DEBUG_SCOPE("%s: retreive metadata for %s", *this, path);
            ELLE_DUMP("%s: %s", *this, it->second);
            return it->second;
          }
          else
          {
            if (this->_full)
              throw NoSuchFile(path);
            else
              // FIXME: even if not full, if we have the parent directory we can
              // know if that file is missing.
              return {};
          }
        }

        void
        metadata_update(bfs::path const&,
                        Metadata value) override
        {
          bfs::path path(value.path);
          ELLE_DEBUG_SCOPE("%s: store metadata for %s", *this, path);
          ELLE_DUMP("%s: %s", *this, value);
          // Update metadata
          {
            // Preserve previously registered folder content.
            if (value.is_dir && !value.contents)
              if (auto it = elle::find(this->_metadata, path))
              {
                ELLE_DEBUG("%s: preserve contents: %s",
                           *this, it->second.contents);
                value.contents = std::move(it->second.contents);
              }
            this->_metadata[path] = value;
          }
          // Update parent metadata
          auto parent_path = path.parent_path();
          ELLE_DEBUG("%s: register in parent metadata: %s", *this, parent_path)
          {
            auto it = this->_metadata.find(parent_path);
            if (it == this->_metadata.end())
            {
              if (!this->_full || parent_path == "/")
                return;
              ELLE_DEBUG("%s: create parent folder metadata: %s", *this,
                         parent_path);
              Metadata m;
              m.is_dir = true;
              m.path = parent_path.string();
              m.bytes = 0;
              it = this->_metadata.emplace(parent_path, std::move(m)).first;
            }
            auto& parent_metadata = it->second;
            if (parent_metadata.is_dir)
            {
              if (!parent_metadata.contents)
                parent_metadata.contents.emplace();
              Metadata::Content content;
              content.path = std::move(value.path);
              content.is_dir = value.is_dir;
              content.bytes = value.bytes;
              content.client_mtime = value.client_mtime;
              content.modified = value.modified;
              for (auto& entry: parent_metadata.contents.get())
                if (entry.path == path)
                {
                  ELLE_DEBUG("%s: update metadata in %s", *this, parent_path);
                  entry = std::move(content);
                  return;
                }
              ELLE_DEBUG("%s: push metadata in %s", *this, parent_path);
              parent_metadata.contents.get().emplace_back(std::move(content));
            }
            else
              ELLE_ERR("%s: parent path %s metadata is not a directory: %s",
                       *this, parent_path, parent_metadata);
          }
        }

        void
        metadata_delete(bfs::path const& path) override
        {
          ELLE_DEBUG_SCOPE("%s: delete metadata for %s", *this, path);
          this->_metadata.erase(path);
          auto parent_path = path.parent_path();
          if (auto it = elle::find(this->_metadata, parent_path))
          {
            auto& parent_metadata = it->second;
            if (parent_metadata.is_dir && parent_metadata.contents)
            {
              auto& contents = parent_metadata.contents.get();
              for (auto it = contents.begin(); it != contents.end(); ++it)
                if (it->path == path)
                {
                  ELLE_DEBUG_SCOPE("%s: delete metadata in %s",
                                   *this, parent_path);
                  contents.erase(it);
                  break;
                }
            }
          }
        }

      private:
        void
        _poll()
        {
          if (this->_full)
          {
            this->_apply_delta();
            // Fetch "/" manually as it is not returned by the intial delta.
            this->_full = false;
            this->_dropbox.metadata("/");
            this->_full = true;
          }
          else
          {
            this->_cursor = this->_dropbox.delta_latest_cursor();
          }
          this->_initialized.open();
          while (true)
          {
            ELLE_TRACE_SCOPE("%s: longpoll", *this);
            auto longpoll = this->_dropbox.longpoll_delta(this->_cursor);
            if (longpoll.changes)
            {
              ELLE_TRACE_SCOPE("%s: longpoll signalled changes", *this);
              this->_apply_delta();
            }
            else
              ELLE_DEBUG("%s: longpoll returned with no change", *this);
            if (longpoll.backoff)
            {
              auto delay = boost::posix_time::seconds(longpoll.backoff.get());
              ELLE_DEBUG("%s: longpoll backoff of %s", *this, delay);
              elle::reactor::sleep(delay);
            }
          }
        }

        void
        _apply_delta()
        {
          ELLE_TRACE_SCOPE("%s: apply delta", *this);
          Delta delta;
          do
          {
            delta = this->_dropbox.delta(this->_cursor);
            if (!this->_cursor.empty() && delta.reset)
            {
              ELLE_ABORT("delta asked to reset the whole thing, "
                         "not implemented yet");
            }
            ELLE_DEBUG("%s: new cursor: %s", *this, delta.cursor);
            this->_cursor = delta.cursor;
            for (auto const& entry: delta.entries)
              if (entry.second)
                this->metadata_update(entry.first, entry.second.get());
              else
                this->metadata_delete(entry.first);
          }
          while (delta.has_more);
        }

        ELLE_ATTRIBUTE(Dropbox&, dropbox);
        ELLE_ATTRIBUTE(bool, full);
        ELLE_ATTRIBUTE_RX(elle::reactor::Barrier, initialized);
        ELLE_ATTRIBUTE(PathMap<Metadata>, metadata);
        ELLE_ATTRIBUTE(elle::reactor::Thread, poll_thread);
        ELLE_ATTRIBUTE(std::string, cursor);
      };

      class Dropbox::FileCache
      {
      public:
        FileCache(Dropbox& dropbox, bfs::path path, int size)
          : _dropbox(dropbox)
          , _path(std::move(path))
          , _contents(size)
          , _ranges_read()
        {}

        void
        sync(int offset, int size)
        {
          auto range = elle::IntRange(offset, size);
          if (range.end() > signed(this->_contents.size()))
            range.end(this->_contents.size());
          ELLE_DEBUG_SCOPE("%s: sync %s", *this, range);
          for (auto missing: this->_ranges_read.complete(range))
            this->_sync(missing);
          if (range.end() != signed(this->_contents.size()))
          {
            auto next = this->_ranges_fetching.next_free(range.start());
            ELLE_ASSERT_GT(next, range.start());
            auto look_ahead = next - range.start();
            if (look_ahead < this->_dropbox.block_size())
            {
              ELLE_TRACE("%s: look ahead is %s, cache more", *this, look_ahead);
              new elle::reactor::Thread(
                "preemptive cache",
                [this, next]
                {
                  auto next_range
                    = elle::IntRange(next, this->_dropbox.block_size());
                  if (next_range.end() > signed(this->_contents.size()))
                    next_range.end(this->_contents.size());
                  if (!this->_ranges_fetching.contains(next_range))
                    this->_get(next_range);
                },
                true);
            }
          }
        }

        void
        _sync(elle::IntRange range)
        {
          if (this->_ranges_fetching.contains(range))
          {
            if (!this->_ranges_fetched.contains(range))
            {
              ELLE_DEBUG_SCOPE("%s: already being fetched, wait", *this);
              ELLE_DUMP("%s: range being fetched: %s",
                        *this, this->_ranges_fetching);
              do
              {
                elle::reactor::sleep(100_ms);
              }
              while (!this->_ranges_fetched.contains(range));
              ELLE_DEBUG("%s: done waiting", *this);
            }
            else
              ELLE_DEBUG("%s: already fetched", *this);
          }
          else
          {
            ELLE_DUMP("%s: not being fetched (fetching: %s)",
                    *this, this->_ranges_fetching);
            // Always download at least a block
            range.size(std::max(range.size(), this->_dropbox.block_size()));
            this->_get(range);
          }
        }

        void
        _get(elle::IntRange range)
        {
          if (range.end() > signed(this->_contents.size()))
            range.end(this->_contents.size());
          ELLE_TRACE_SCOPE("%s: download %s", *this, range);
          this->_ranges_fetching += range;
          elle::reactor::http::Request::Configuration conf;
          conf.header_add(
            "Range",
            elle::sprintf("bytes=%s-%s", range.start(), range.end() - 1));
          auto data = this->_dropbox._get(this->_path, std::move(conf));
          if (signed(data.size()) != range.size())
          {
            ELLE_ERR("%s: fetched data size mismatch (%s instead of %s), "
                     "deadlock ahead", *this, data.size(), range.size());
          }
          memcpy(this->_contents.mutable_contents() + range.start(),
                 data.contents(), data.size());
          this->_ranges_fetched += elle::IntRange(range.start(), data.size());
        }

        ELLE_ATTRIBUTE(Dropbox&, dropbox);
        ELLE_ATTRIBUTE(bfs::path, path);
        ELLE_ATTRIBUTE(elle::Buffer, contents);
        ELLE_ATTRIBUTE(elle::IntRanges, ranges_read);
        ELLE_ATTRIBUTE(elle::IntRanges, ranges_fetching);
        ELLE_ATTRIBUTE(elle::IntRanges, ranges_fetched);
        friend class Dropbox;
      };

      // FIXME
      static auto _caches = PathMap<Dropbox::FileCache>{};

      Error::Error(bfs::path const& path,
                   std::string const& reason)
        : elle::Error(elle::sprintf("%s: %s", path, reason))
      {}

      DestinationExists::DestinationExists(bfs::path const& path)
        : Error(path, "destination already exists")
      {}

      NoSuchFile::NoSuchFile(bfs::path const& path)
        : Error(path, "no such file")
      {}

      Dropbox::Dropbox(std::string token, int block_size)
        : _token(std::move(token))
        , _block_size(block_size)
        , _cache(new LongPollCache(*this))
      {
        elle::reactor::wait(
          static_cast<LongPollCache*>(this->_cache.get())->initialized());
      }

      Dropbox::~Dropbox()
      {
        // Delete the cache first as it uses our fields
        this->_cache.reset();
      }

      AccountInfo
      Dropbox::account_info()
      {
        auto r = this->_request("https://api.dropbox.com/1/account/info");
        this->_check_status("getting account info", r);
        {
          // FIXME: deserialize json with helper everywhere
          elle::serialization::json::SerializerIn s(r, false);
          return s.deserialize<AccountInfo>();
        }
      }

      std::string
      Dropbox::escape_path(bfs::path const& path)
      {
        if (path == "/")
          return "/";
        else
        {
          std::string res;
          for (auto const& chunk: path)
          {
            res += "/";
            res += elle::reactor::http::url_encode(chunk.string());
          }
          return res;
        }
      }

      boost::optional<Metadata>
      Dropbox::local_metadata(bfs::path const& path) const
      {
        return this->_cache->metadata(path);
      }

      Metadata
      Dropbox::metadata(bfs::path const& path) const
      {
        ELLE_TRACE_SCOPE("%s: fetch metadata for %s", *this, path.string());
        if (auto metadata = this->_cache->metadata(path))
          return metadata.get();
        static auto const url_fmt = boost::format(
          "https://api.dropbox.com/1/metadata/auto%s");
        this->_check_path(path);
        auto r = this->_request(
          str(boost::format(url_fmt) % this->escape_path(path)),
          elle::reactor::http::Method::GET,
          elle::reactor::http::Request::QueryDict(), {}, {}, "metadata",
          {elle::reactor::http::StatusCode::Not_Found});
        if (r.status() == elle::reactor::http::StatusCode::OK)
        {
          // FIXME: deserialize json with helper everywhere
          elle::serialization::json::SerializerIn s(r, false);
          auto res = s.deserialize<Metadata>();
          ELLE_DUMP("%s: metadata: %s", *this, res);
          if (res.is_deleted && res.is_deleted.get())
          {
            ELLE_TRACE("%s: file was deleted", *this);
            throw NoSuchFile(path);
          }
          this->_cache->metadata_update(path, res);
          return res;
        }
        else if (r.status() == elle::reactor::http::StatusCode::Not_Found)
        {
          ELLE_TRACE("%s: file not found", *this);
          throw NoSuchFile(path);
        }
        else
        {
          this->_check_status("fetching metadata", r);
          elle::unreachable();
        }
      }

      elle::Buffer
      Dropbox::get(bfs::path const& path) const
      {
        return this->_get(path, elle::reactor::http::Request::Configuration());
      }

      elle::Buffer
      Dropbox::get(bfs::path const& path,
                   int offset, int size) const
      {
        auto it = _caches.find(path);
        if (it == _caches.end())
        {
          auto metadata = this->metadata(path);
          it = _caches.emplace(
            path, Dropbox::FileCache(const_cast<Dropbox&>(*this),
                                     path, metadata.bytes)).first;
        }
        it->second.sync(offset, size);
        return elle::Buffer(it->second._contents.contents() + offset, size);
      }

      elle::Buffer
      Dropbox::_get(bfs::path const& path,
                    elle::reactor::http::Request::Configuration conf) const
      {
        ELLE_TRACE_SCOPE("%s: fetch file %s", *this, path.string());
        this->_check_path(path);
        static auto const url_fmt = boost::format
          ("https://api-content.dropbox.com/1/files/auto%s");
        auto r = this->_request(
          str(boost::format(url_fmt) % this->escape_path(path)),
          elle::reactor::http::Method::GET,
          elle::reactor::http::Request::QueryDict(),
          std::move(conf),
          {}, "get",
          {elle::reactor::http::StatusCode::Partial_Content,
              elle::reactor::http::StatusCode::Not_Found});

        if (r.status() == elle::reactor::http::StatusCode::OK ||
            r.status() == elle::reactor::http::StatusCode::Partial_Content)
        {
          auto res = r.response();
          ELLE_DEBUG("%s: got %s bytes", *this, res.size());
          return res;
        }
        else if (r.status() == elle::reactor::http::StatusCode::Not_Found)
        {
          ELLE_TRACE("%s: file not found", *this);
          throw NoSuchFile(path);
        }
        else
        {
          this->_check_status("fetching file", r);
          elle::unreachable();
        }
      }

      bool
      Dropbox::put(bfs::path const& path,
                   elle::WeakBuffer const& content,
                   bool overwrite)
      {
        ELLE_TRACE_SCOPE("%s: put file: %s (overwrite: %s)",
                         *this, path.string(), overwrite);
        if (this->_ignored(path))
          return false;
        this->_check_path(path);
        auto query = elle::reactor::http::Request::QueryDict{};
        if (!overwrite)
          query.insert({{"overwrite", "false"},
                        {"autorename", "false"}});
        static auto const url_fmt = boost::format
          ("https://api-content.dropbox.com/1/files_put/auto%s");
        auto r = this->_request(
          str(boost::format(url_fmt) % this->escape_path(path)),
          elle::reactor::http::Method::PUT,
          std::move(query), {}, content, "write",
          {elle::reactor::http::StatusCode::Conflict});

        if (r.status() == elle::reactor::http::StatusCode::OK)
        {
          elle::serialization::json::SerializerIn s(r, false);
          auto metadata = s.deserialize<Metadata>();
          ELLE_DUMP("%s: metadata: %s", *this, metadata);
          this->_cache->metadata_update(path, metadata);
          return true;
        }
        else if (r.status() == elle::reactor::http::StatusCode::Conflict)
        {
          return false;
        }
        else
        {
          this->_check_status("putting file", r);
          elle::unreachable();
        }
      }

      void
      Dropbox::create_folder(bfs::path const& path)
      {
        auto r = this->_fileop(path, "create_folder");
        this->_check_status("create_folder", r);
        {
          elle::serialization::json::SerializerIn s(r, false);
          auto metadata = s.deserialize<Metadata>();
          ELLE_DUMP("%s: metadata: %s", *this, metadata);
          this->_cache->metadata_update(path, metadata);
        }
      }

      void
      Dropbox::delete_(bfs::path const& path)
      {
        auto r = this->_fileop(path, "delete",
                               {elle::reactor::http::StatusCode::Not_Found});
        if (r.status() == elle::reactor::http::StatusCode::Not_Found)
        {
          ELLE_TRACE("%s: file not found", *this);
          throw NoSuchFile(path);
        }
        this->_check_status("delete", r);
        {
          elle::serialization::json::SerializerIn s(r, false);
          auto metadata = s.deserialize<Metadata>();
          ELLE_DUMP("%s: metadata: %s", *this, metadata);
          this->_cache->metadata_delete(path);
        }
      }

      void
      Dropbox::move(bfs::path const& from,
                    bfs::path const& to)
      {
        ELLE_TRACE_SCOPE("%s: move %s to %s", *this, from, to);
        elle::reactor::http::Request::QueryDict query;
        query["to_path"] = to.string();
        auto r = this->_fileop(from, "move",
                               { elle::reactor::http::StatusCode::Forbidden},
                               "from_path", std::move(query));
        if (r.status() == elle::reactor::http::StatusCode::Forbidden)
          throw DestinationExists(to);
        this->_check_status("move", r);
        {
          elle::serialization::json::SerializerIn s(r, false);
          auto metadata = s.deserialize<Metadata>();
          ELLE_DUMP("%s: metadata: %s", *this, metadata);
          this->_cache->metadata_delete(from);
          this->_cache->metadata_update(to, std::move(metadata));
        }
      }

      Delta
      Dropbox::delta(std::string cursor)
      {
        ELLE_TRACE_SCOPE(
          "%s: get delta from %s",
          *this, cursor.empty() ? std::string("scratch") : cursor);
        elle::reactor::http::Request::QueryDict query;
        if (!cursor.empty())
          query["cursor"] = cursor;
        auto r = this->_request("https://api.dropbox.com/1/delta",
                                elle::reactor::http::Method::POST,
                                std::move(query),
                                {}, {}, "delta");
        this->_check_status("fetching delta", r);
        {
          elle::serialization::json::SerializerIn s(r, false);
          return s.deserialize<Delta>();
        }
      }

      std::string
      Dropbox::delta_latest_cursor()
      {
        auto r = this->_request("https://api.dropbox.com/1/delta/latest_cursor",
                                elle::reactor::http::Method::POST,
                                elle::reactor::http::Request::QueryDict(),
                                {}, {}, "delta_latest_cursor");
        this->_check_status("fetching latest cursor", r);
        {
          elle::serialization::json::SerializerIn s(r, false);
          return s.deserialize<std::string>("cursor");
        }
      }

      Longpoll
      Dropbox::longpoll_delta(std::string cursor)
      {
        elle::reactor::http::Request::QueryDict query;
        ELLE_ASSERT(!cursor.empty());
        query["cursor"] = cursor;
        auto r = this->_request(
          "https://api-notify.dropbox.com/1/longpoll_delta",
          elle::reactor::http::Method::GET,
          std::move(query),
          {}, {}, "longpoll_delta");
        this->_check_status("longpolling delta", r);
        {
          elle::serialization::json::SerializerIn s(r, false);
          return s.deserialize<Longpoll>();
        }
      }

      namespace
      {
        elle::reactor::Duration
        delay(int attempt)
        {
          unsigned int factor = pow(2, std::max(8, attempt));
          return boost::posix_time::milliseconds(factor * 100);
        }
      }

      elle::reactor::http::Request
      Dropbox::_request(
        std::string url,
        elle::reactor::http::Method method,
        elle::reactor::http::Request::QueryDict query,
        elle::reactor::http::Request::Configuration conf,
        elle::ConstWeakBuffer const& payload,
        std::string const& op,
        std::vector<elle::reactor::http::StatusCode> expected_codes) const
      {
        expected_codes.push_back(elle::reactor::http::StatusCode::OK);
        conf.timeout(elle::reactor::DurationOpt()); // Disable timeout
        int attempt = 0;
        while (true)
        {
          elle::reactor::http::Request r(url, method, conf);
          ELLE_TRACE_SCOPE("%s: request: %s", *this, r);
          for (auto const& entry: query)
            ELLE_DUMP("%s: %s = \"%s\"", *this, entry.first, entry.second);
          query["access_token"] = this->_token;
          r.query_string(query);
          if (payload.size())
            r.write(reinterpret_cast<const char*>(payload.contents()),
                    payload.size());
          r.finalize();
          if (std::find(expected_codes.begin(), expected_codes.end(), r.status())
              != expected_codes.end())
          {
            return r;
          }
          ELLE_WARN("Unexpected dropbox HTTP status on %s: %s, attempt %s",
                    op, r.status(), attempt+1);
          ++attempt;
          elle::reactor::sleep(delay(attempt));
        }
      }

      elle::reactor::http::Request
      Dropbox::_fileop(bfs::path const& path,
                       std::string const& op,
                       std::vector<elle::reactor::http::StatusCode> expected_codes,
                       std::string const& path_arg,
                       elle::reactor::http::Request::QueryDict query)
      {
        ELLE_TRACE_SCOPE("%s: %s: %s", *this, op, path.string());
        static auto const url_fmt = boost::format("https://api.dropbox.com/1/fileops/%s");
        query.insert({{"root", "auto"},
                      {path_arg, path.string()}});
        auto r =
          this->_request(str(boost::format(url_fmt) % op),
                         elle::reactor::http::Method::POST, std::move(query),
                         {}, {}, op, expected_codes);
        return r;
      }

      void
      Dropbox::_check_status(std::string const& op,
                             elle::reactor::http::Request& r) const
      {
        if (r.status() != elle::reactor::http::StatusCode::OK)
        {
          ELLE_ERR("unknown dropbox HTTP status on %s: %s", op, r.status());
          ELLE_ERR("body: %s", r.response());
          ELLE_ABORT("unknown dropbox HTTP status on %s: %s", op, r.status());
        }
      }

      bool
      Dropbox::_ignored(bfs::path const& path) const
      {
        auto const name = to_lower(path);
        return name == ".ds_store";
      }

      void
      Dropbox::_check_path(bfs::path const& path) const
      {
        // Dropbox doesn't support certain characters in file names and will
        // yield 400 bad requests if requests are attempted on those path.
        // Preemptively consider them as non existent.
        for (auto const& chunk: path)
          for (char const c: chunk.string())
            if (c == '\\')
            {
              ELLE_TRACE("%s: reject invalid path containing backslashes",
                         *this);
              throw NoSuchFile(path);
            }
      }
    }
  }
}
