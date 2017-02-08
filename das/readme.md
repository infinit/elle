# Das, Model manipulator and generator.

Das is a compile time model generation system.

> Das is part of the [Elle](https://github.com/infinit/elle) set of libraries.

## Motivation

The less code you write, the happier you should be is the philosophy behind Das.

## How to compile

_See [Elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //das/build -j 2
```

## Example

```cpp
DAS_SYMBOL(title);
DAS_SYMBOL(artist);

/// Declare a Record class with a title and an artist.
struct Record
{
  std::string title;
  std::string artist;

  using Model = das::Model<Record, elle::meta::List<Symbol_title, Symbol_artist>>;
};

// ...

Record record{"Sandstorm", "Darube"};
// Default printer.
std::cout << record << std::endl; // Record(title = Sandstorm, artist = Darube)
// Default serialization.
elle::serialization::json::SerializerOut serializer(std::cout, false);
das::serialize(record, serializer); // {"title": "Sandstorm", "artist": "Darube"}
```

## Maintainers

 * Website: https://infinit.sh/open-source
 * Email: open+elle@infinit.sh
