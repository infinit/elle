#ifndef ETOILE_GEAR_TRANSCRIPT_HH
# define ETOILE_GEAR_TRANSCRIPT_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <etoile/gear/Action.hh>

# include <vector>

# include <boost/noncopyable.hpp>

namespace etoile
{
  namespace gear
  {

    /// This class represents a set of actions to be performed on the
    /// storage layer such as pushing a block, removing another one and so
    /// on.
    class Transcript:
      public elle::Printable,
      public elle::io::Dumpable,
      private boost::noncopyable
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::vector<Action const*> Container;
      typedef Container::iterator Iterator;
      typedef Container::const_iterator Scoutor;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      ~Transcript();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Record an action in the transcript.
      ///
      /// Note that the ownership of the given action is transferred to
      /// the transcript.
      void
      record(Action const* action);
      /// Return the number of actions recorded in the transcript.
      elle::Size
      size() const;
      /// Return true if the transcript is empty.
      elle::Boolean
      empty() const;
      /// Merge the given transcript's actions with the current's.
      void
      merge(Transcript&& other);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

  }
}

#endif
