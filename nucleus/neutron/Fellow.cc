#include <nucleus/neutron/Fellow.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Fellow Fellow::Null;

//
// ---------- construction ----------------------------------------------------
//

    Fellow::Fellow()
    {
    }

    Fellow::Fellow(Subject const& subject):
      _subject(subject)
    {
    }

    Fellow::Fellow(Subject const& subject,
                   Token const& token):
      _subject(subject),
      _token(token)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    Subject const&
    Fellow::subject() const
    {
      return (this->_subject);
    }

    Subject&
    Fellow::subject()
    {
      return (this->_subject);
    }

    Token const&
    Fellow::token() const
    {
      return (this->_token);
    }

    void
    Fellow::token(Token const& token)
    {
      this->_token = token;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    Fellow::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Fellow]" << std::endl;

      if (this->_subject.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the subject");

      if (this->_token.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the token");

      return (elle::Status::Ok);
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Fellow::print(std::ostream& stream) const
    {
      stream << "fellow("
             << this->_subject
             << ")";
    }

//
// ---------- rangeable -------------------------------------------------------
//

    Subject&
    Fellow::symbol()
    {
      return (this->_subject);
    }

  }
}
