#include <elle/IntRange.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.IntRange");

namespace elle
{
  IntRange::IntRange(int start, int size)
    : _start(start)
    , _end(start + size)
  {}

  bool
  IntRange::operator <(IntRange const& rhs) const
  {
    return this->start() < rhs.start() ||
           this->start() == rhs.start() && this->end() < rhs.end();
  }

  int
  IntRange::size() const
  {
    return this->_end - this->_start;
  }

  void
  IntRange::size(int size)
  {
    this->_end = this->_start + size;
  }

  bool
  IntRange::empty() const
  {
    return this->end() <= this->start();
  }

  bool
  IntRange::contains(IntRange const& other) const
  {
    return this->start() <= other.start() && this->end() >= other.end();
  }

  bool
  IntRange::contains(int pos) const
  {
    return pos >= this->start() && pos < this->end();
  }

  bool
  IntRange::overlaps(IntRange const& other) const
  {
    return this->contains(other.start()) || this->contains(other.end());
  }

  IntRanges
  IntRange::operator -(IntRange const& rhs) const
  {
    // Disjoint
    if (!rhs.overlaps(*this))
      return *this;
    // Superset
    if (rhs.contains(*this))
      return IntRanges();
    // Prefix
    if (rhs.start() <= this->start())
      return IntRange(rhs.end(), this->end());
    // Suffix
    if (rhs.end() >= this->end())
      return IntRange(this->start(), rhs.end());
    // Right in the middle
    return IntRanges
      ({IntRange(this->start(), rhs.start()), IntRange(rhs.end(), this->end())});
  }

  std::ostream&
  operator <<(std::ostream& output, IntRange const& range)
  {
    elle::fprintf(output, "(%s, %s)", range.start(), range.size());
    return output;
  }

  /*-------.
  | IntRanges |
  `-------*/

  IntRanges::IntRanges()
    : _ranges()
  {}

  static inline
  void
  emplace(std::set<IntRange>& set, IntRange range)
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
  emplace(std::set<IntRange>& set, Args&& ... args)
  {
#if GCC_VERSION_LTE(4, 7)
    set.insert(IntRange(std::forward<Args>(args)...));
#else
    set.emplace(std::forward<Args>(args)...);
#endif
  }

  IntRanges::IntRanges(IntRange range)
    : _ranges()
  {
    emplace(this->_ranges, std::move(range));
  }

  IntRanges::IntRanges(std::vector<IntRange> const& ranges)
    : _ranges()
  {
    for (auto const& range: ranges)
      emplace(this->_ranges, std::move(range));
  }

  IntRanges&
  IntRanges::operator += (IntRange range)
  {
    this->complete(range);
    return *this;
  }

  int
  IntRanges::next_free(int position) const
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
  IntRanges::contains(IntRange range) const
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

  IntRanges
  IntRanges::complete(IntRange range)
  {
    ELLE_TRACE_SCOPE("%s: complete with %s", *this, range);
    IntRanges res;
    boost::optional<IntRange> previous;
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
  operator <<(std::ostream& output, IntRanges const& ranges)
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
