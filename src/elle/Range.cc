#include <elle/Range.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.Range");

namespace elle
{
  Range::Range(int start, int size)
    : _start(start)
    , _end(start + size)
  {}

  bool
  Range::operator <(Range const& rhs) const
  {
    return this->start() < rhs.start() ||
           this->start() == rhs.start() && this->end() < rhs.end();
  }

  int
  Range::size() const
  {
    return this->_end - this->_start;
  }

  void
  Range::size(int size)
  {
    this->_end = this->_start + size;
  }

  bool
  Range::empty() const
  {
    return this->end() <= this->start();
  }

  bool
  Range::contains(Range const& other) const
  {
    return this->start() <= other.start() && this->end() >= other.end();
  }

  bool
  Range::contains(int pos) const
  {
    return pos >= this->start() && pos < this->end();
  }

  bool
  Range::overlaps(Range const& other) const
  {
    return this->contains(other.start()) || this->contains(other.end());
  }

  Ranges
  Range::operator -(Range const& rhs) const
  {
    // Disjoint
    if (!rhs.overlaps(*this))
      return *this;
    // Superset
    if (rhs.contains(*this))
      return Ranges();
    // Prefix
    if (rhs.start() <= this->start())
      return Range(rhs.end(), this->end());
    // Suffix
    if (rhs.end() >= this->end())
      return Range(this->start(), rhs.end());
    // Right in the middle
    return Ranges
      ({Range(this->start(), rhs.start()), Range(rhs.end(), this->end())});
  }

  std::ostream&
  operator <<(std::ostream& output, Range const& range)
  {
    elle::fprintf(output, "(%s, %s)", range.start(), range.size());
    return output;
  }

  /*-------.
  | Ranges |
  `-------*/

  Ranges::Ranges()
    : _ranges()
  {}

  static inline
  void
  emplace(std::set<Range>& set, Range range)
  {
#if GCC_VERSION_LTE(4, 7)
    set.insert(std::move(range));
#else
    set.emplace(std::move(range));
#endif
  }

  template <typename ... Args>
  static inline
  void
  emplace(std::set<Range>& set, Args&& ... args)
  {
#if GCC_VERSION_LTE(4, 7)
    set.insert(Range(std::forward<Args>(args)...));
#else
    set.emplace(std::forward<Args>(args)...);
#endif
  }

  Ranges::Ranges(Range range)
    : _ranges()
  {
    emplace(this->_ranges, std::move(range));
  }

  Ranges::Ranges(std::vector<Range> const& ranges)
    : _ranges()
  {
    for (auto const& range: ranges)
      emplace(this->_ranges, std::move(range));
  }

  Ranges&
  Ranges::operator += (Range range)
  {
    this->complete(range);
    return *this;
  }

  int
  Ranges::next_free(int position) const
  {
    for (auto const& chunk: this->_ranges)
      if (chunk.end() >= position)
        if (chunk.start() > position)
          return position;
        else
          return chunk.end();
    return position;
  }

  bool
  Ranges::contains(Range range) const
  {
    for (auto const& chunk: this->_ranges)
    {
      if (chunk.contains(range))
        return true;
      else if (chunk.start() >= range.start())
        return false;
    }
    return false;
  }

  Ranges
  Ranges::complete(Range range)
  {
    ELLE_TRACE_SCOPE("%s: complete with %s", *this, range);
    Ranges res;
    boost::optional<Range> previous;
    for (auto current: this->_ranges)
    {
      ELLE_DUMP_SCOPE("%s: consider %s", *this, current);
      if (current.start() > range.end())
      {
        ELLE_DUMP("%s: past the end, stop", *this);
        break;
      }
      if (current.overlaps(range))
      {
        ELLE_DUMP("%s: overlap", *this);
        if (current.start() > range.start())
        {
          emplace(res._ranges, range.start(), current.start());
          this->_ranges.erase(current);
          current.start(range.start());
          // Merge with previous if needed.
          if (previous && previous.get().end() == current.start())
          {
            this->_ranges.erase(previous.get());
            current.start(previous.get().start());
          }
          emplace(this->_ranges, current);
        }
        range.start(current.end());
        ELLE_DUMP("%s: leftover is %s", *this, range);
        if (range.empty())
        {
          ELLE_DUMP("%s: leftover empty, done", *this);
          return res;
        }
      }
      else
        ELLE_DUMP("%s: skip %s", *this, current);
      previous = current;
    }
    emplace(res._ranges, range);
    if (previous && previous.get().end() == range.start())
    {
      this->_ranges.erase(previous.get());
      range.start(previous.get().start());
    }
    emplace(this->_ranges, range);
    ELLE_DUMP("%s: added %s", *this, range);
    return res;
  }

  std::ostream&
  operator <<(std::ostream& output, Ranges const& ranges)
  {
    output << "[";
    bool first = true;
    for (auto range: ranges)
    {
      if (first)
        first = false;
      else
        output << ", ";
      output << range;
    }
    output << "]";
    return output;
  }
}
