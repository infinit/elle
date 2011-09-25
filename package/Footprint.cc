//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author           [sat sep 24 11:26:13 2011]
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
    /// note that both the low-level serialization mechanism and the
    /// Archive class record the type of the element being serialized,
    /// hence two bytes.
    ///
    const Natural32			Footprint::Meta =
      sizeof (Byte) + sizeof (Natural8);

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Footprint::Footprint():
      size(0)
    {
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method computes the footprint of the given archivable
    /// object.
    ///
    Status		Footprint::Compute(const Archivable&	archivable,
					   Natural32&		size)
    {
      Footprint		footprint;

      enter();

      // create the footprint i.e archive in order to set it in
      // serialization mode.
      if (footprint.Create() == StatusError)
	escape("unable to create the footprint");

      // serialize the archivable with the footprint as argument so
      // as to compute its footprint.
      if (archivable.Serialize(footprint) == StatusError)
	escape("unable to serialize the archivable");

      // return the footprint's size.
      size = footprint.size;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a footprint.
    ///
    Status		Footprint::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Archive		archive;

      enter();

      std::cout << alignment << "[Footprint] " << this->size << std::endl;

      leave();
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method returns the size of the footprint object.
    ///
    Status		Footprint::Imprint(Natural32&		size) const
    {
      enter();

      // return the size.
      size = sizeof (Footprint);

      leave();
    }

    ///
    /// this method clones the current footprint.
    ///
    Status		Footprint::Clone(Footprint*&		object) const
    {
      enter();

      // allocate the object.
      object = new Footprint(*this);

      leave();
    }

    ///
    /// this method copies a footprint.
    ///
    Footprint&		Footprint::operator=(const Footprint&	element)
    {
      enter();

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
    Boolean		Footprint::operator==(const Footprint&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attribute.
      if (this->size != element.size)
	false();

      true();
    }

    ///
    /// this method compares two footprints.
    ///
    Boolean		Footprint::operator!=(const Footprint&	element) const
    {
      return (!(*this == element));
    }

//
// ---------- archive ---------------------------------------------------------
//

    ///
    /// this method computes a null footprint.
    ///
    Status		Footprint::Store(const Null&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + 0;

      leave();
    }

    ///
    /// this method computes a boolean footprint.
    ///
    Status		Footprint::Store(const Boolean&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural8);

      leave();
    }

    ///
    /// this method computes a character footprint.
    ///
    Status		Footprint::Store(const Character&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural8);

      leave();
    }

    ///
    /// this method computes a real footprint.
    ///
    Status		Footprint::Store(const Real&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural64);

      leave();
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status		Footprint::Store(const Integer8&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer8);

      leave();
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status		Footprint::Store(const Integer16&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer16);

      leave();
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status		Footprint::Store(const Integer32&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer32);

      leave();
    }

    ///
    /// this method computes a integer footprint.
    ///
    Status		Footprint::Store(const Integer64&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Integer64);

      leave();
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status		Footprint::Store(const Natural8&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural8);

      leave();
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status		Footprint::Store(const Natural16&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural16);

      leave();
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status		Footprint::Store(const Natural32&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural32);

      leave();
    }

    ///
    /// this method computes a natural footprint.
    ///
    Status		Footprint::Store(const Natural64&)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + sizeof (Natural64);

      leave();
    }

    ///
    /// this method computes a large footprint.
    ///
    Status		Footprint::Store(const Large&		element)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + BN_num_bytes(&element);

      leave();
    }

    ///
    /// this method computes a string footprint.
    ///
    Status		Footprint::Store(const String&		element)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + element.length();

      leave();
    }

    ///
    /// this method computes a region footprint.
    ///
    Status		Footprint::Store(const Region&		element)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + element.size;

      leave();
    }

    ///
    /// this method computes a archive footprint.
    ///
    Status		Footprint::Store(const Archive&		element)
    {
      enter();

      // add the footprint.
      this->size += Footprint::Meta + element.size;

      leave();
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Null&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Boolean&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Character&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Real&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Integer8&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Integer16&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Integer32&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Integer64&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Natural8&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Natural16&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Natural32&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Natural64&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Large&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(String&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Region&)
    {
      enter();

      escape("unable to load from a footprint");
    }

    ///
    /// this method returns an error.
    ///
    Status		Footprint::Load(Archive&)
    {
      enter();

      escape("unable to load from a footprint");
    }

  }
}
