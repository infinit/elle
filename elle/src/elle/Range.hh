#ifndef ELLE_RANGE_HH
# define ELLE_RANGE_HH

# include <set>
# include <vector>

# include <elle/attribute.hh>

namespace elle
{
  class Range;
  class Ranges;

  class Range
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    Range(int start, int size);

  /*----------.
  | Observers |
  `----------*/
  public:
    bool
    operator <(Range const& rhs) const;
    bool
    empty() const;
    bool
    contains(Range const& other) const;
    bool
    contains(int pos) const;
    bool
    overlaps(Range const& other) const;
    ELLE_ATTRIBUTE_RW(int, start);
    ELLE_ATTRIBUTE_RW(int, end);
    ELLE_ATTRIBUTE_rw(int, size);

  /*-----------.
  | Operations |
  `-----------*/
  public:
    Ranges
    operator -(Range const& rhs) const;
  };

  std::ostream&
  operator <<(std::ostream& output, Range const& range);

  class Ranges
  {
  public:
    Ranges();
    Ranges(Range range);
    Ranges(std::vector<Range> const& ranges);
    Ranges&
    operator += (Range range);
    int
    next_free(int position) const;
    Ranges
    complete(Range range);
    bool
    contains(Range range) const;

  /*---------.
  | Iterable |
  `---------*/
  public:
    typedef std::set<Range>::iterator iterator;
    typedef std::set<Range>::const_iterator const_iterator;
    iterator begin()       { return this->_ranges.begin(); }
    iterator begin() const { return this->_ranges.begin(); }
    iterator end()         { return this->_ranges.end();   }
    iterator end()   const { return this->_ranges.end();   }

    ELLE_ATTRIBUTE(std::set<Range>, ranges);
  };

  std::ostream&
  operator <<(std::ostream& output, Ranges const& ranges);
}

#endif
