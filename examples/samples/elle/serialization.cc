#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/UUID.hh>
#include <elle/Version.hh>
#include <elle/assert.hh>
#include <elle/serialization.hh>
#include <elle/serialization/json.hh>
#include <elle/serialization/binary.hh>

static const auto unknown_date = boost::posix_time::time_from_string(
  "1970-01-01 00:00:00.000");

namespace elle
{
  // The current serialization version.
  elle::Version serialization_tag::version(1, 0, 0);
}

// A Record class, that has evolved, from version 0.1.0 to version 0.3.0.
// 0.1.0
// + title (std::string)
// + artist (std::string)
// + id (elle::UUID°
// + unwanted (int)
// 0.2.0:
// + release_date (boost::posix_time::ptime)
// - unwanted (int)
// 0.3.0:
// + tags (std::vector<std::string>)

struct Record
  : public elle::Printable
{
  Record(std::string const& title,
         std::string const& artist,
         boost::posix_time::ptime const& release_date)
    : _title(title)
    , _artist(artist)
    , _id(elle::UUID::random())
    , _release_date(release_date)
  {
  }

  /// Add a deserialization constructor.
  Record(elle::serialization::SerializerIn& in,
         elle::Version const& version)
  {
    this->serialize(in, version);
  }

  /// The serialization interface.
  void
  serialize(elle::serialization::Serializer& s,
            elle::Version const& version)
  {
    // Serialize fields that didn't change.
    s.serialize("title", this->_title);
    s.serialize("artist", this->_artist);
    s.serialize("id", this->_id);
    // Simulate a field that use to be here at version 0.1.0 but is not wanted
    // anymore.
    if (version < elle::Version(0, 2, 0))
    {
      int unwanted = 3;
      s.serialize("unwanted", unwanted);
    }
    // Serialize new field 'release_date' introduced at version 0.2.0.
    if (version >= elle::Version(0, 2, 0))
      s.serialize("release_date", this->_release_date);
    else if (s.in())
      // Field was missing from 0.1.0 serialized objects, so give it the default
      // value we want.
      // boost::optional would also work.
      this->_release_date = unknown_date;
    // Serialize new field 'tags' introduced at version 0.3.0 and use the
    // default std::vector<std::string> if it didn't exist.
    if (version >= elle::Version(0, 3, 0))
      s.serialize("tags", this->_tags);
  }

  /// The implementation of the pure virtual elle::Printable::print method.
  void
  print(std::ostream& out) const override
  {
    out << "[" << this->_artist << "] " << this->_title;
    if (this->_release_date != unknown_date)
      out << " (" << this->_release_date << ")";
    if (!this->_tags.empty())
      out << " categories: " << this->_tags;
  }

private:
  ELLE_ATTRIBUTE_R(std::string, title);
  ELLE_ATTRIBUTE_R(std::string, artist);
  ELLE_ATTRIBUTE_R(elle::UUID, id);
  ELLE_ATTRIBUTE_R(boost::posix_time::ptime, release_date);
  ELLE_ATTRIBUTE_RX(std::vector<std::string>, tags);

public:
  typedef elle::serialization_tag serialization_tag;
};

int
main()
{
  Record record("Sandstorm", "Darube",
                boost::posix_time::time_from_string("1999-11-26 00:00:00.000"));
  record.tags().push_back("Dance");
  record.tags().push_back("Electronic");
  // Serialize Record as in version 0.1.0, where release_date & tags didn't
  // exist yet.
  {
    std::stringstream ss;
    elle::serialization::binary::serialize(record, ss, elle::Version(0, 1, 0));
    auto r = elle::serialization::binary::deserialize<Record>(ss, true);
    ELLE_ASSERT_EQ(r.title(), record.title());
    ELLE_ASSERT_EQ(r.artist(), record.artist());
    ELLE_ASSERT_EQ(r.id(), record.id());
    ELLE_ASSERT_NEQ(r.release_date(), record.release_date());
    ELLE_ASSERT_NEQ(r.tags(), record.tags());
    std::cout << "Record serialized at version 0.1.0 looks like: "
              << std::endl << "> " << r << std::endl << std::endl;
  }
  // Serialize Record as in version 0.2.0, where tags didn't exist.
  {
    std::stringstream ss;
    elle::serialization::binary::serialize(record, ss, elle::Version(0, 2, 0));
    auto r = elle::serialization::binary::deserialize<Record>(ss, true);
    ELLE_ASSERT_EQ(r.title(), record.title());
    ELLE_ASSERT_EQ(r.artist(), record.artist());
    ELLE_ASSERT_EQ(r.id(), record.id());
    ELLE_ASSERT_EQ(r.release_date(), record.release_date());
    ELLE_ASSERT_NEQ(r.tags(), record.tags());
    std::cout << "Record serialized at version 0.2.0 looks like: "
              << std::endl << "> " << r << std::endl << std::endl;
  }
  // Serialize Record as in version 0.3.0, the final version of the object.
  {
    std::stringstream ss;
    elle::serialization::binary::serialize(record, ss, elle::Version(0, 3, 0));
    auto r = elle::serialization::binary::deserialize<Record>(ss);
    ELLE_ASSERT_EQ(r.title(), record.title());
    ELLE_ASSERT_EQ(r.artist(), record.artist());
    ELLE_ASSERT_EQ(r.id(), record.id());
    ELLE_ASSERT_EQ(r.release_date(), record.release_date());
    ELLE_ASSERT_EQ(r.tags(), record.tags());
    std::cout << "Record serialized at version 0.3.0 looks like: "
              << std::endl << "> " << r << std::endl;
  }
  return 0;
}
