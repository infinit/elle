#include <string>

#include <elle/UUID.hh>
#include <elle/serialization/json.hh>

#include <das/model.hh>
#include <das/serializer.hh>
#include <das/printer.hh>
#include <das/Symbol.hh>

// Declare symbols (Symbol_*) needed by the Record::Model.
DAS_SYMBOL(title);
DAS_SYMBOL(artist);
DAS_SYMBOL(id);
DAS_SYMBOL(release_date);
DAS_SYMBOL(tags);

/// Declare a Record class with
struct Record
{
  std::string title;
  std::string artist;
  elle::UUID id;
  boost::posix_time::ptime release_date;
  std::vector<std::string> tags;

  // The Model can be declared in the class.
  using Model = das::Model<Record,
                           elle::meta::List<Symbol_title,
                                            Symbol_artist,
                                            Symbol_id,
                                            Symbol_release_date,
                                            Symbol_tags>>;
};

struct Records
{
  std::string label;
  std::vector<Record> records;
};

// The Model alse can be declared outside the class but will require
// DAS_MODEL_DEFAULT.
DAS_SYMBOL(label);
DAS_SYMBOL(records);
using DasRecords = das::Model<Records,
                              elle::meta::List<Symbol_label,
                                               Symbol_records>>;
DAS_MODEL_DEFAULT(Records, DasRecords);

// Make all class::Model printable.
using das::operator <<;

// Make class serializable.
DAS_SERIALIZE(Record);
DAS_SERIALIZE(DasRecords);

int
main()
{
  Records r{
    "Favorites",
    {
      {
        "Sandstorm", "Darube",
        elle::UUID("31900f3a-adaf-11e6-80f5-76304dec7eb7"),
        boost::posix_time::time_from_string("1999-11-26 00:00:00.000"),
        {"Electronic", "Dance"}
      },
      {
        "Never Gonna Give You Up", "Rick Astley",
        elle::UUID("1549b8a6-6cd7-453e-83de-49f3dc6b0e85"),
        boost::posix_time::time_from_string("1987-01-01 00:00:00.000"),
        {"'80s Pop"}
      }
    }
  };
  std::cout << "records representation: " << std::endl << r << std::endl
            << std::endl;
  std::stringstream stream;
  elle::serialization::json::SerializerOut serializer(stream, false);
  das::serialize(r, serializer);
  std::cout << "records (in json): " << std::endl << stream.str() << std::endl;

  return 0;
}
