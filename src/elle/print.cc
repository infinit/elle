#include <memory>

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wdeprecated"
#include <boost/bind.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
// #pragma GCC diagnostic pop

#include <elle/Printable.hh>
#include <elle/assert.hh>
#include <elle/err.hh>
#include <elle/finally.hh>
#include <elle/print.hh>

namespace elle
{
  namespace _details
  {

    /*----.
    | AST |
    `----*/

    class Expression
      : public elle::Printable
    {};

    class Composite
      : public Expression
    {
    public:
      Composite(std::vector<std::shared_ptr<Expression>> expressions = {})
        : expressions(std::move(expressions))
      {}

      void
      print(std::ostream& s) const override
      {
        bool first = true;
        s << "Composite(";
        for (auto const& e: this->expressions)
        {
          if (first)
            first = false;
          else
            s << ", ";
          s << *e;
        }
        s << ')';
      }

      std::vector<std::shared_ptr<Expression>> expressions;
    };

    class Branch
      : public Expression
    {
    public:
      Branch(std::shared_ptr<Expression> cond,
             std::shared_ptr<Expression> then)
        : cond(std::move(cond))
        , then(std::move(then))
      {}

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Branch(";
        this->cond->print(s);
        s << ", ";
        this->then->print(s);
        s << ")";
      }

      std::shared_ptr<Expression> cond;
      std::shared_ptr<Expression> then;
    };

    class Index
      : public Expression
    {
    public:
      Index(int n)
        : n(n)
      {}

      static
      std::shared_ptr<Index>
      make(int n)
      {
        return std::make_shared<Index>(n);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Index(" << n << ")";
      }

      int n;
    };

    class Next
      : public Expression
    {
    public:
      Next()
      {}

      static
      std::shared_ptr<Next>
      make()
      {
        return std::make_shared<Next>();
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Next()";
      }
    };

    class Legacy
      : public Expression
    {
    public:
      Legacy(char fmt)
        : fmt(fmt)
      {}

      static
      std::shared_ptr<Legacy>
      make(char fmt)
      {
        return std::make_shared<Legacy>(fmt);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Legacy(" << this->fmt << ")";
      }

      char fmt;
    };

    class Name
      : public Expression
    {
    public:
      Name(std::string n)
        : n(std::move(n))
      {}

      static
      std::shared_ptr<Name>
      make(std::string& n)
      {
        return std::make_shared<Name>(n);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Name(" << n << ')';
      }

      std::string n;
    };

    class Literal
      : public Expression
    {
    public:
      Literal(std::string text)
        : text(std::move(text))
      {}

      static
      std::shared_ptr<Literal>
      make(std::string& text)
      {
        return std::make_shared<Literal>(text);
      }

      virtual
      void
      print(std::ostream& s) const override
      {
        s << "Literal(" << text << ')';
      }

      std::string text;
    };

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
      if (then)
        return std::make_shared<Branch>(std::move(fmt), std::move(then.get()));
      else
        return fmt;
    }

    static
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
        (qi::char_('\\') >> qi::char_("\\{}%"))[qi::_val = qi::_2];
      qi::rule<Iterator, std::string()> literal =
        *(escape | (qi::char_  - '{' - '}' - '\\' - '%') );
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
      qi::rule<Iterator, std::shared_ptr<Expression>()> legacy =
        ("%" >> qi::char_("cdefgiopsuxCEGSX%")[
          qi::_val = phoenix::bind(&Legacy::make, _1)]);
      phrase = plain[phoenix::bind(&push, qi::_val, _1)] >>
        *(("{" >> fmt[phoenix::bind(&push, qi::_val, _1)] >> "}" |
           legacy[phoenix::bind(&push, qi::_val, _1)]) >>
          plain[phoenix::bind(&push, qi::_val, _1)]);
      std::shared_ptr<Expression> res;
      auto first = input.begin();
      auto last = input.end();
      if (!qi::phrase_parse(first, last, phrase, qi::eoi, res) || first != last)
        elle::err("invalid format: %s", input);
      ELLE_ASSERT(res);
      return res;
    }

    /*------.
    | Print |
    `------*/

    static
    void
    print(std::ostream& s,
          Expression const& ast,
          std::vector<Argument> const& args,
          int& count,
          bool p,
          NamedArguments const& named,
          bool& full_positional)
    {
      auto const nth = [&] (int n) -> Argument const& {
        if (n >= signed(args.size()))
          elle::err(
            "too few arguments for format: %s, expected at least %s",
            args.size(), n + 1);
        return args[n];
      };
      auto* id = &typeid(ast);
      if (id == &typeid(Composite))
        for (auto const& e:static_cast<Composite const&>(ast).expressions)
          print(s, *e, args, count, p, named, full_positional);
      else if (id == &typeid(Literal))
      {
        if (p)
          s.write(static_cast<Literal const&>(ast).text.c_str(),
                  static_cast<Literal const&>(ast).text.size());
      }
      else if (id == &typeid(Next))
      {
        if (p)
          nth(count)(s);
        ++count;
      }
      else if (id == &typeid(Legacy))
      {
        if (p)
        {
          auto&& state = std::ios(nullptr);
          state.copyfmt(s);
          switch (static_cast<Legacy const&>(ast).fmt)
          {
            case 'p':
            case 'x':
              s << std::hex;
              break;
            case 'o':
              s << std::oct;
              break;
            case 'e':
              s << std::scientific;
              break;
            case 'f':
              s << std::fixed;
              break;
            case 'd':
            case 'i':
            case 'u':
              s << std::dec;
              break;
            case '%':
              s << '%';
              s.copyfmt(state);
              return;
            default:
              // FIXME
              break;
          }
          nth(count)(s);
          s.copyfmt(state);
        }
        ++count;
      }
      else if (id == &typeid(Index))
      {
        full_positional = false;
        if (p)
          nth(static_cast<Index const&>(ast).n)(s);
      }
      else if (id == &typeid(Name))
      {
        auto const& name = static_cast<Name const&>(ast).n;
        auto const it = named.find(name);
        if (it == named.end())
          elle::err("missing named format argument: %s", name);
        else if (p)
          it->second(s);
      }
      else if (id == &typeid(Branch))
      {
        auto const& branch = static_cast<Branch const&>(ast);
        auto const& cond = *branch.cond;
        auto const* cid = &typeid(cond);
        if (cid == &typeid(Next))
        {
          p = p && bool(nth(count));
          ++count;
        }
        else if (cid == &typeid(Index))
          p = p && bool(nth(static_cast<Index const&>(cond).n));
        else if (cid == &typeid(Name))
        {
          auto const& name = static_cast<Name const&>(cond).n;
          auto it = named.find(name);
          if (it != named.end())
            p = p && bool(it->second);
          else
            elle::err("missing named format argument: %s", name);
        }
        else
          elle::err("unexpected condition: %s", elle::type_info(cond));
        print(s, *branch.then, args, count, p, named, full_positional);
      }
    }

    void
    print(std::ostream& s,
          std::string const& fmt,
          std::vector<Argument> const& args,
          NamedArguments const& named)
    {
      auto const ast = _details::parse(fmt);
      int count = 0;
      bool full_positional = true;
      _details::print(s, *ast, args, count, true, named, full_positional);
      if (full_positional && count < signed(args.size()))
        elle::err("too many arguments for format: %s", fmt);
    }
  }
}
