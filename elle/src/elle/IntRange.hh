#pragma once

#include <set>
#include <vector>

#include <elle/attribute.hh>
#include <elle/fwd.hh>

namespace elle
{
  class IntRange
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    IntRange(int start, int size);

  /*----------.
  | Observers |
  `----------*/
  public:
    bool
    operator <(IntRange const& rhs) const;
    bool
    empty() const;
    bool
    contains(IntRange const& other) const;
    bool
    contains(int pos) const;
    bool
    overlaps(IntRange const& other) const;
    ELLE_ATTRIBUTE_RW(int, start);
    ELLE_ATTRIBUTE_RW(int, end);
    ELLE_ATTRIBUTE_rw(int, size);

  /*-----------.
  | Operations |
  `-----------*/
  public:
    IntRanges
    operator -(IntRange const& rhs) const;
  };

  std::ostream&
  operator <<(std::ostream& output, IntRange const& range);

  class IntRanges
  {
  public:
    IntRanges();
    IntRanges(IntRange range);
    IntRanges(std::vector<IntRange> const& ranges);
    IntRanges&
    operator += (IntRange range);
    int
    next_free(int position) const;
    IntRanges
    complete(IntRange range);
    bool
    contains(IntRange range) const;

  /*---------.
  | Iterable |
  `---------*/
  public:
    using iterator = std::set<IntRange>::iterator;
    using const_iterator = std::set<IntRange>::const_iterator;
    iterator begin()       { return this->_ranges.begin(); }
    iterator begin() const { return this->_ranges.begin(); }
    iterator end()         { return this->_ranges.end();   }
    iterator end()   const { return this->_ranges.end();   }

    ELLE_ATTRIBUTE(std::set<IntRange>, ranges);
  };

  std::ostream&
  operator <<(std::ostream& output, IntRanges const& ranges);
}
