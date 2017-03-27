#include <memory>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <elle/das/serializer.hh>
#include <iostream>
#include <elle/das/printer.hh>
#include <elle/das/Symbol.hh>
#include <elle/das/flatten.hh>
#include <elle/das/model.hh>
#include <elle/das/cli.hh>

#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>
#include <elle/reactor/http/Request.hh>

namespace symbols
{
  ELLE_DAS_SYMBOL(address_components);
  ELLE_DAS_SYMBOL(formatted_address);
  ELLE_DAS_SYMBOL(geometry);
  ELLE_DAS_SYMBOL(location);
  ELLE_DAS_SYMBOL(long_name);
  ELLE_DAS_SYMBOL(results);
  ELLE_DAS_SYMBOL(short_name);
  ELLE_DAS_SYMBOL(location_type);
  ELLE_DAS_SYMBOL(place_id);
  ELLE_DAS_SYMBOL(types);
  ELLE_DAS_SYMBOL(status);
}

using elle::das::operator <<;

struct Response
{
  struct Result
  {
    struct AddressComponents
    {
      std::string long_name;
      std::string short_name;
      std::vector<std::string> types;

      using Model = elle::das::Model<
        AddressComponents,
        decltype(elle::meta::list(symbols::long_name,
                                  symbols::short_name,
                                  symbols::types))>;
    };

    struct Geometry
    {
      std::unordered_map<std::string, double> location;
      std::string location_type;
      using Model = elle::das::Model<
        Geometry,
        decltype(elle::meta::list(symbols::location,
                                  symbols::location_type))>;
    };

    std::vector<AddressComponents> address_components;
    std::string formatted_address;
    Geometry geometry;
    std::string place_id;
    std::vector<std::string> types;
    using Model = elle::das::Model<
      Result,
      decltype(elle::meta::list(symbols::address_components,
                                symbols::formatted_address,
                                symbols::geometry,
                                symbols::place_id,
                                symbols::types))>;
  };

  std::vector<Result> results;
  std::string status;

  using Model = elle::das::Model<
    Response,
    decltype(elle::meta::list(symbols::results, symbols::status))>;
};

ELLE_DAS_SERIALIZE(Response::Result::AddressComponents);
ELLE_DAS_SERIALIZE(Response::Result::Geometry);
ELLE_DAS_SERIALIZE(Response::Result);
ELLE_DAS_SERIALIZE(Response);

namespace cli_symbols
{
  ELLE_DAS_CLI_SYMBOL(sensor, 0, "Use sensor", false);
  ELLE_DAS_CLI_SYMBOL(address, 0, "The address", false);
  // ELLE_DAS_CLI_SYMBOL(format, 0, "The format", false);
}

static
void
display(Response const& response,
        std::string const& format = "")
{
  /// TODO: Extract fields according to format.
  std::cout << response << std::endl;
}

static
void
search(std::string const& address,
       bool sensor)
{
  auto r = elle::reactor::http::Request{
    "http://maps.googleapis.com/maps/api/geocode/json",
    elle::reactor::http::Method::GET,
    "application/json"};
  std::stringstream sensor_str;
  sensor_str << std::boolalpha << sensor;
  r.query_string({
      {"address", address},
      {"sensor", sensor_str.str()}
    });
  // Finalize the operation.
  r.finalize();
  display(elle::serialization::json::deserialize<Response>(r));
};

int
main(int argc, char** argv)
{
  auto args = std::vector<std::string>(argv, argv + argc);
  elle::reactor::Scheduler s;
  elle::reactor::Thread main(
    s, "main",
    [&]
    {
      auto const proto = elle::das::named::prototype(cli_symbols::address,
                                                     cli_symbols::sensor
                                                     //cli_symbols::format
        );
      args.erase(args.begin());
      elle::das::cli::call(proto, search, args);
    });
  try
  {
    s.run();
    return 0;
  }
  catch (elle::reactor::Terminate const&)
  {
    std::cerr << "Interrupted..." << std::endl;
    return -1;
  }
  catch (std::runtime_error const&)
  {
    std::cerr << elle::exception_string() << std::endl;
    return -1;
  }
}
