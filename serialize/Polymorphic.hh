#ifndef ELLE_SERIALIZE_POLYMORPHIC_HH
# define ELLE_SERIALIZE_POLYMORPHIC_HH

# include "detail/wrapper.hh"

///
/// Polymorphic wrapper informs the serializer to use virtual serialization
/// methods (i.e. not the concrete type). It comes with an helper function
/// to infer templated types.
/// ----------------------------
/// struct A:
///   public elle::serialize::Serializable
/// { ... };
///
/// struct B:
///   public A,
///   public elle::serialize::SerializableMixin
/// { ... };
/// XXX doc
/// A* a = new B;
/// archive & elle::serialize::polymorphic(*a);
/// archive & elle::serialize::Polymorphic<A>(*a); // more verbose form
/// ----------------------------

_ELLE_SERIALIZE_WRAPPER(Polymorphic, polymorphic);

#endif
