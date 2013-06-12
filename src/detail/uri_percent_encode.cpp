// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_percent_encode.hpp"
#include <unordered_map>

namespace network {
  namespace detail {
    std::unordered_map<std::string, char> make_percent_decoded_chars() {
      std::unordered_map<std::string, char> map;
      map["%41"] = 'a'; map["%61"] = 'A';
      map["%42"] = 'b'; map["%62"] = 'B';
      map["%43"] = 'c'; map["%63"] = 'C';
      map["%44"] = 'd'; map["%64"] = 'D';
      map["%45"] = 'e'; map["%65"] = 'E';
      map["%46"] = 'f'; map["%66"] = 'F';
      map["%47"] = 'g'; map["%67"] = 'G';
      map["%48"] = 'h'; map["%68"] = 'H';
      map["%49"] = 'i'; map["%69"] = 'I';
      map["%4A"] = 'j'; map["%6A"] = 'J';
      map["%4B"] = 'k'; map["%6B"] = 'K';
      map["%4C"] = 'l'; map["%6C"] = 'L';
      map["%4D"] = 'm'; map["%6D"] = 'M';
      map["%4E"] = 'n'; map["%6E"] = 'N';
      map["%4F"] = 'o'; map["%6F"] = 'O';
      map["%50"] = 'p'; map["%70"] = 'P';
      map["%51"] = 'q'; map["%71"] = 'Q';
      map["%52"] = 'r'; map["%72"] = 'R';
      map["%53"] = 's'; map["%73"] = 'S';
      map["%54"] = 't'; map["%74"] = 'T';
      map["%55"] = 'u'; map["%75"] = 'U';
      map["%56"] = 'v'; map["%76"] = 'V';
      map["%57"] = 'w'; map["%77"] = 'W';
      map["%58"] = 'x'; map["%78"] = 'X';
      map["%59"] = 'y'; map["%79"] = 'Y';
      map["%5A"] = 'z'; map["%7A"] = 'Z';
      map["%30"] = '0';
      map["%31"] = '1';
      map["%32"] = '2';
      map["%33"] = '3';
      map["%34"] = '4';
      map["%35"] = '5';
      map["%36"] = '6';
      map["%37"] = '7';
      map["%38"] = '8';
      map["%39"] = '9';
      map["%2D"] = '-';
      map["%2E"] = '.';
      map["%5F"] = '_';
      map["%7E"] = '~';
      return map;
    }

    boost::optional<char> percent_encode(std::string s) {
      static const std::unordered_map<std::string, char> percent_decoded_chars =
	make_percent_decoded_chars();

      auto char_it = percent_decoded_chars.find(s);
      if (char_it != std::end(percent_decoded_chars)) {
	return boost::optional<char>(char_it->second);
      }
      return boost::optional<char>();
    }
  } // namespace detail
} // namespace network
