//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat sep 24 11:26:13 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Footprint.hh>

namespace elle
{
  namespace package
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines the size of the meta information recorded
    /// before an element.
    ///
    /// note that the low-level serialization mechanism records the type
    /// of the element being serialized in a byte. noteworthy is that
    /// many cases are optimised. this is way the footprint functionality
    /// is not extremely precise in terms of prediction. this explains
    /// why a small Natural8 is actually serialized in only one byte,
    /// packing both the type and its value within a byte, while the
    /// footprint assumes a byte is used for the type and another byte
    /// is used for the value.
    ///
    const Natural32                     Footprint::Meta = sizeof (Byte);

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Footprint::Footprint():
      state(Footprint::StateUnknown),
      archivable(NULL),
      size(0)
    {
    }

    ///
    /// specific constructor.
    ///
    Footprint::Footprint(const Archivable&                      archivable):
      state(Footprint::StateUnknown),
      archivable(&archivable),
      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the footprint of the given archivable.
    ///
    Status              Footprint::Compute()
    {
      // check that an archivable has been assigned to the footprint.
      if (this->archivable == NULL)
        escape("no archivable has been assigned to this footprint");

      // operate depending on the state.
      switch (this->state)
        {
        case StateUnknown:
          {
            // create the footprint i.e archive in order to set it in
            // serialization mode.
            if (this->Create() == StatusError)
              escape("unable to create the footprint");

            // serialize the archivable with the footprint as argument so
            // as to compute its footprint.
            if (this->archivable->Serialize(*this) == StatusError)
              escape("unable to serialize the archivable");

            break;
          }
        case StateConsistent:
          {
            //
            // do not proceed since the footprint has already been computed
            // and the object has not been modified since.
            //

            break;
          }
        case StateInconsistent:
          {
            //
            // proceed with the computation.
            //

            // reset the size.
            this->size = 0;

            // serialize the archivable with the footprint as argument so
            // as to compute its footprint.
            if (this->archivable->Serialize(*this) == StatusError)
              escape("unable to serialize the archivable");

            break;
          }
        }

      // set the state as consistent.
      this->state = Footprint::StateConsistent;

      return StatusOk;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a footprint.
    ///
    Status              Footprint::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');
      Archive           archive;

      std::cout << alignment << "[Footprint] "
                << std::dec << this->size << std::endl;

      return StatusOk;
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method returns the size of the footprint object.
    ///
    Status              Footprint::Imprint(Natural32&           size) const
    {
      // return the size.
      size = sizeof (Footprint);

      return StatusOk;
    }

    ///
    /// this method clones the current footprint.
    ///
    Status              Footprint::Clone(Footprint*&            object) const
    {
      // allocate the object.
      object = new Footprint(*this);

      return StatusOk;
    }

    ///
    /// this method copies a footprint.
    ///
    Footprint&          Footprint::operator=(const Footprint&   element)
    {
      // test if the footprints are identical.
      if (this == &element)
        return (*this);

      // recycle the object.
      if (this->Recycle(&element) == StatusError)
        yield(*this, "unable to recycle the object");

      return (*this);
    }

    ///
    /// this method check if two footprints match.
    ///
    Boolean             Footprint::operator==(const Footprint&  element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return StatusTrue;

      // compare the attribute.
      if (this->size != element.size)
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method compares two footprints.
    ///
    Boolean             Footprint::operator!=(const Footprint&  element) const
    {
      return (!(*this == element));
    }

//
// ---------- archive ---------------------------------------------------------
//

    ///
    /// this method computes a null footprint.
    ///
    Status              Footprint::Store(const Null&)
    {
      // add the footprint.
      this->size += Footprint::Meta + 0;

      return StatusOk;
    }

    ///
    /// this method computes a boolean footprint.
    ///
    Status              Footprint::Store(const Boolean&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural8);

      return StatusOk;
    }

    ///
    /// this method computes a character footprint.
    ///
    Status              Footprint::Store(const Character&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural8);

      return StatusOk;
    }

    ///
    /// this method computes a real footprint.
    ///
    Status              Footprint::Store(const Real&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural64);

      return StatusOk;
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status              Footprint::Store(const Integer8&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer8);

      return StatusOk;
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status              Footprint::Store(const Integer16&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer16);

      return StatusOk;
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status              Footprint::Store(const Integer32&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer32);

      return StatusOk;
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status              Footprint::Store(const Integer64&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer64);

      return StatusOk;
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status              Footprint::Store(const Natural8&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural8);

      return StatusOk;
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status              Footprint::Store(const Natural16&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural16);

      return StatusOk;
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status              Footprint::Store(const Natural32&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural32);

      return StatusOk;
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status              Footprint::Store(const Natural64&)
    {
      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural64);

      return StatusOk;
    }

    ///
    /// this method computes a large footprint.
    ///
    Status              Footprint::Store(const Large&           element)
    {
      // add the footprint.
      this->size += Footprint::Meta + BN_num_bytes(&element);

      return StatusOk;
    }

    ///
    /// this method computes a string footprint.
    ///
    Status              Footprint::Store(const String&          element)
    {
      // add the footprint.
      this->size += Footprint::Meta + element.length();

      return StatusOk;
    }

    ///
    /// this method computes a region footprint.
    ///
    Status              Footprint::Store(const Region&          element)
    {
      // add the footprint.
      this->size += Footprint::Meta + element.size;

      return StatusOk;
    }

    ///
    /// this method computes a archive footprint.
    ///
    Status              Footprint::Store(const Archive&         element)
    {
      // add the footprint.
      this->size += Footprint::Meta + element.size;

      return StatusOk;
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Null&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Boolean&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Character&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Real&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Integer8&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Integer16&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Integer32&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Integer64&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Natural8&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Natural16&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Natural32&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Natural64&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Large&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(String&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Region&)
    {
      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status              Footprint::Load(Archive&)
    {
      escape("unable to load from a footprint");
    }

  }
}
