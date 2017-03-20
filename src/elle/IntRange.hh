#pragma once

#include <set>
#include <vector>

#include <elle/attribute.hh>
#include <elle/fwd.hh>

namespace elle
{
  /// A range of integers, defined by its beginning and size.
  ///
  /// \code{.cc}
  ///
  /// IntRange x{10, 100}; // 10..109 (from 10 with 100 entries)
  /// assert(x.begin() == 10);
  /// assert(x.end() == 109);
  /// assert(x.contains(50));
  /// assert(x.contains({20, 30})); // Contains the range {20, 21, ..., 48, 49};
  ///
  /// \endcode
  class IntRange
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create a Range from a given start point and size.
    ///
    /// N.B. Second argument is not the end of the range but the size of the
    ///      range. Therefore, Range(0, 5) is [0, 1, 2, 3, 4].
    ///
    /// @param start Where the range starts.
    /// @param size The number of elements in the range.
    IntRange(int start, int size);

  /*----------.
  | Observers |
  `----------*/
  public:
    /// Return whether the IntRange compares less than the given IntRange.
    ///
    /// A IntRange is considered less if one of the following condition is
    /// fulfilled:
    /// - our start is less than their start.
    /// - our end is less then their end and our starts are equal.
    ///
    /// @param rhs The IntRange to compare us with.
    /// @returns Whether we are less than the given IntRange.
    bool
    operator <(IntRange const& rhs) const;
    /// Return whether the IntRange compares greater than the given IntRange.
    ///
    /// A IntRange is considered greater if one of the following condition is
    /// fulfilled:
    /// - our end is greater than their end.
    /// - our start is greater then their start and our ends are equal.
    ///
    /// @param rhs The IntRange to compare us with.
    /// @returns Whether we are greater than the given IntRange.
    bool
    operator >(IntRange const& rhs) const;
    /// Return whether the given IntRange is strictly equal to our IntRange.
    ///
    /// A IntRange are considered equal if both starts and ends are equal.
    ///
    /// @param rhs The IntRange to compare us with.
    /// @returns Whether we are equal to the given IntRange.
    bool
    operator ==(IntRange const& rhs) const;
    /// Check if the IntRange is empty.
    ///
    /// An IntRange is considered empty if start() >= end().
    ///
    /// @returns Whether the IntRange is empty.
    bool
    empty() const;
    /// Check if the IntRange contains the given IntRange.
    ///
    /// @param other The other IntRange.
    /// @returns Whether we are other IntRange is contained by our IntRange.
    bool
    contains(IntRange const& other) const;
    /// Check if the given value is contained by our IntRange.
    ///
    /// @param pos The value.
    /// @returns Whether if the value is contained.
    bool
    contains(int pos) const;
    /// Check if part of the given IntRange overlaps our IntRange.
    ///
    /// @param other The other IntRange.
    /// @returns Whether the other IntRange overlaps us.
    bool
    overlaps(IntRange const& other) const;
    ELLE_ATTRIBUTE_RW(int, start);
    ELLE_ATTRIBUTE_RW(int, end);
    ELLE_ATTRIBUTE_rw(int, size);

  /*-----------.
  | Operations |
  `-----------*/
  public:
    /// Return an IntRanges (a collection of up to two IntRange), resulting
    /// from subtracting the given IntRange from our IntRange.
    ///
    /// If the both are disjoint, return {us}.
    /// If we are a superset of the given IntRange, return {}.
    ///
    IntRanges
    operator -(IntRange const& rhs) const;
  };

  /// Print an IntRange, formatted as (<start>, <size>).
  std::ostream&
  operator <<(std::ostream& output, IntRange const& range);

  /// A collection of `IntRange`s.
  ///
  /// \code{.cc}
  ///
  /// auto ranges = elle::IntRanges{{
  ///   elle::IntRange{0, 5},
  ///   elle::IntRange{10, 5},
  /// }}; // 0, 1, 2, 3, 4, 10, 11, 12, 13, 14
  /// assert(ranges.contains(4));
  /// assert(!ranges.contains(8));
  /// assert(ranges.contains({1, 3}));
  /// for (auto range: ranges)
  ///   std::cout << range;
  /// // Result: (0, 5)(10, 5)
  ///
  /// \endcode
  class IntRanges
  {
  public:
    /// Create an empty IntRanges.
    IntRanges();
    /// Create an IntRanges containing the given IntRange.
    ///
    /// @param range An IntRange.
    IntRanges(IntRange range);
    /// Create an IntRanges containing the given collection of `IntRange`s.
    ///
    /// @param ranges A vector containing `IntRange`s.
    IntRanges(std::vector<IntRange> const& ranges);
    /// Add an IntRange.
    ///
    /// @param range The IntRange to add.
    /// @returns A reference to the updated IntRanges.
    IntRanges&
    operator += (IntRange range);
    /// Get the smallest integer superior of equal to the given integer, not
    /// contained in any IntRange of the IntRanges.
    ///
    /// @param position The minimum position.
    /// @returns The next free position.
    int
    next_free(int position) const;
    /// Complete the current IntRanges with a given IntRange and merge
    /// underlying `IntRange`s.
    ///
    /// @param range The IntRange to complete IntRanges with.
    /// @returns XXX[doc].
    IntRanges
    complete(IntRange range);
    /// Check if the given IntRange is contained in any of the underlying
    /// `IntRange`s.
    ///
    /// @param range The IntRange.
    /// @returns Whether the given range is contained.
    bool
    contains(IntRange const& range) const;
    /// Check if the given value is contained in any of the underlying
    /// `IntRange`s.
    ///
    /// @param pos The value.
    /// @returns Whether the given value is contained.
    bool
    contains(int pos) const;

  /*---------.
  | Iterable |
  `---------*/
  public:
    using iterator = std::set<IntRange>::iterator;
    using const_iterator = std::set<IntRange>::const_iterator;
    /// Return an iterator to the begin of the underlying `IntRange`s.
    iterator
    begin();
    /// Return a const iterator to the begin of the underlying `IntRange`s.
    const_iterator
    begin() const;
    /// Return an iterator to the end of the underlying `IntRange`s.
    iterator
    end();
    /// Return a const iterator to the end of the underlying `IntRange`s.
    const_iterator
    end() const;
  public:
    /// Check if we are equal to the IntRanges.
    bool
    operator ==(IntRanges const& rhs) const;

  public:
    friend
    std::ostream&
    operator <<(std::ostream& output, IntRanges const& ranges);

  private:
    ELLE_ATTRIBUTE(std::set<IntRange>, ranges);
  };

  /// Print an IntRanges, formatted as {<range>, <range>, ...}.
  std::ostream&
  operator <<(std::ostream& output, IntRanges const& ranges);
}
