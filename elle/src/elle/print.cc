// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wdeprecated"
#include <boost/bind.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
// #pragma GCC diagnostic pop

#include <elle/assert.hh>
#include <elle/err.hh>
#include <elle/print.hh>

namespace elle
{
  namespace _details
  {
    /*--------.
    | Helpers |
    `--------*/

    static
    void
    push(std::shared_ptr<Composite>& res, std::shared_ptr<Expression> e)
    {
      if (!res)
        res = std::make_shared<Composite>();
      res->expressions.emplace_back(std::move(e));
    }

    static
    std::shared_ptr<Expression>
    make_fmt(std::shared_ptr<Expression> fmt,
             boost::optional<std::shared_ptr<Composite>> then = boost::none)
    {
      if (!then)
        return fmt;
      else
        return std::make_shared<Branch>(std::move(fmt), std::move(then.get()));
    }

    std::shared_ptr<Expression>
    parse(std::string const& input)
    {
      namespace phoenix = boost::phoenix;
      namespace qi = boost::spirit::qi;
      namespace ascii = boost::spirit::ascii;
      using qi::labels::_1;
      using qi::labels::_2;
      using qi::labels::_3;
      using Iterator = decltype(input.begin());
      qi::rule<Iterator, std::shared_ptr<Composite>()> phrase;
      qi::rule<Iterator, char()> escape =
        qi::char_('\\') >> qi::char_("\\{}");
      qi::rule<Iterator, std::string()> literal =
        *(escape | (qi::char_  - '{' - '}' - '\\') );
      qi::rule<Iterator, std::shared_ptr<Expression>()> plain =
        literal[qi::_val = phoenix::bind(&Literal::make, qi::_1)];
      qi::rule<Iterator, std::string()> identifier =
        ascii::alpha >> *ascii::alnum;
      qi::rule<Iterator, std::shared_ptr<Expression>()> var =
        identifier[qi::_val = phoenix::bind(&Name::make, _1)] |
        qi::int_[qi::_val = phoenix::bind(&Index::make, _1)] |
        qi::eps[qi::_val = phoenix::bind(&Next::make)];
      qi::rule<Iterator, std::shared_ptr<Expression>()> fmt =
        (var >> -("?" >> phrase))
        [qi::_val = phoenix::bind(&make_fmt, _1, _2)];
      phrase = plain[phoenix::bind(&push, qi::_val, _1)] >>
        *("{" >> fmt[phoenix::bind(&push, qi::_val, _1)] >> "}" >>
          plain[phoenix::bind(&push, qi::_val, _1)]);
      std::shared_ptr<Expression> res;
      auto first = input.begin();
      auto last = input.end();
      if (!qi::phrase_parse(first, last, phrase, qi::eoi, res))
        elle::err("invalid format: %s", input);
      ELLE_ASSERT(res);
      ELLE_ASSERT(first == last);
      return res;
    }
  }
}
