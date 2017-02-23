#include <string>

#include <elle/UUID.hh>
#include <elle/serialization/json.hh>

#include <elle/das/model.hh>
#include <elle/das/serializer.hh>
#include <elle/das/printer.hh>
#include <elle/das/Symbol.hh>

/*
  This file shows:
  - Symbols declaration
  - Models declaration (nested, not nested)
  - Serialization
  - 'Printability'

  Expected output:
    ```
    records representation:
    Records(label = Favorites, records = [Record(title = Sandstorm, artist = Darude, id = 31900f3a-adaf-11e6-80f5-76304dec7eb7, release_date = 1999-Nov-26 00:00:00, tags = [Electronic, Dance]), Record(title = Never Gonna Give You Up, artist = Rick Astley, id = 1549b8a6-6cd7-453e-83de-49f3dc6b0e85, release_date = 1987-Jan-01 00:00:00, tags = ['80s Pop])])

    records (in json):
    {"label":"Favorites","records":[{"artist":"Darude","id":"31900f3a-adaf-11e6-80f5-76304dec7eb7","release_date":"1999-11-26T00:00:00","tags":["Electronic","Dance"],"title":"Sandstorm"},{"artist":"Rick Astley","id":"1549b8a6-6cd7-453e-83de-49f3dc6b0e85","release_date":"1987-01-01T00:00:00","tags":["'80s Pop"],"title":"Never Gonna Give You Up"}]}
    ```

  TODO:
  - Custom models (DasShortRecord = ... <Symbol_title, Symbol_artist>)
  - Named arguments: Record(title = "Darude")
  - Static capabilities: Symbol_title.method_has<Record>()
*/

// Declare symbols (Symbol_*) needed by the Record::Model.
ELLE_DAS_SYMBOL(title);
ELLE_DAS_SYMBOL(artist);
ELLE_DAS_SYMBOL(id);
ELLE_DAS_SYMBOL(release_date);
ELLE_DAS_SYMBOL(tags);

/// Declare a Record class with
struct Record
{
  std::string title;
  std::string artist;
  elle::UUID id;
  boost::posix_time::ptime release_date;
  std::vector<std::string> tags;

  // The Model can be declared in the class.
  using Model = elle::das::Model<Record,
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
// ELLE_DAS_MODEL_DEFAULT to define the default model for the class (this
// implies that you can more than one model per class).
ELLE_DAS_SYMBOL(label);
ELLE_DAS_SYMBOL(records);
using DasRecords = elle::das::Model<Records, elle::meta::List<Symbol_label,
                                                              Symbol_records>>;
ELLE_DAS_MODEL_DEFAULT(Records, DasRecords);

// Make all class::Model printable.
using elle::das::operator <<;

// Make class serializable.
ELLE_DAS_SERIALIZE(Record);
ELLE_DAS_SERIALIZE(DasRecords);

int
main()
{
  auto r = Records{
    "Favorites",
    {
      {
        "Sandstorm", "Darude",
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
  std::cout << "records representation: \n" << r << "\n\n";
  std::stringstream stream;
  {
    elle::serialization::json::SerializerOut serializer(stream, false);
    elle::das::serialize(r, serializer);
  }
  std::cout << "records (in json): \n" << stream.str() << '\n';
  return 0;
}
