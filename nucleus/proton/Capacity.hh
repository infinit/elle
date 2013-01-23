#ifndef NUCLEUS_PROTON_CAPACITY_HH
# define NUCLEUS_PROTON_CAPACITY_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {
    /// Represent the number of elements in a branch of a hierarchical
    /// porcupine data structure.
    ///
    /// The example below illustrates the capacity which can be then used
    /// to seek an element according to its absolute sequential position
    /// in the elements (noted E) of the tree.
    ///
    ///                          root
    ///                       (capacity=6)
    ///                     '      .       `
    ///                   '        .        `
    ///                 '          .          `
    ///             value        value        value
    ///          (capacity=2) (capacity=1) (capacity=3)
    ///           |   |            |        |   |   |
    ///          E0  E1           E3       E4  E5  E6
    ///
    /// The capacity can therefore be used to know how many total elements are
    /// kept in the data structure while enabling one to locate a precise
    /// element, say E4, given its position only i.e 4.
    typedef elle::Natural64 Capacity;
  }
}

#endif
