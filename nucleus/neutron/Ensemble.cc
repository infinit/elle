#include <nucleus/neutron/Ensemble.hh>
#include <nucleus/Exception.hh>

#include <elle/cryptography/PrivateKey.hh>
#include <elle/log.hh>

ELLE_LOG_TRACE_COMPONENT("infinit.nucleus.neutron.Ensemble");

namespace nucleus
{
  namespace neutron
  {

//
// ---------- construction ----------------------------------------------------
//

    Ensemble::Ensemble():
      proton::ContentHashBlock(ComponentEnsemble)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    void
    Ensemble::add(std::unique_ptr<Fellow>&& fellow)
    {
      ELLE_LOG_TRACE_SCOPE("add(%s)", fellow);

      if (this->exists(fellow->subject()) == true)
        throw Exception("a fellow with this subject already exists"); // XXX[remove in the future]

      this->_container.push_back(fellow.release());

      this->state = proton::StateDirty;
    }

    elle::Boolean
    Ensemble::empty() const
    {
      return (this->_container.empty());
    }

    elle::Boolean
    Ensemble::exists(Subject const& subject) const
    {
      Ensemble::Scoutor scoutor;

      ELLE_LOG_TRACE_SCOPE("exists(%s)", subject);

      try
        {
          scoutor = this->_locate(subject);
        }
      catch (...)
        {
          return (false);
        }

      return (true);
    }

    Fellow const*
    Ensemble::locate(Subject const& subject) const
    {
      Ensemble::Scoutor scoutor;
      Fellow* fellow;

      ELLE_LOG_TRACE_SCOPE("locate(%s)", subject);

      // XXX[consider the owner]

      scoutor = this->_locate(subject);
      fellow = *scoutor;

      return (fellow);
    }

    Fellow const*
    Ensemble::locate(Index const& index) const
    {
      Ensemble::Scoutor scoutor;
      Index i(0);

      ELLE_LOG_TRACE_SCOPE("locate(%s)", index);

      // XXX[consider the owner]

      for (scoutor = this->_container.begin();
           scoutor != this->_container.end();
           ++scoutor, ++i)
        {
          Fellow* fellow = *scoutor;

          if (index == i)
            return (fellow);
        }

      throw Exception(
              elle::sprintf(
                "unable to locate the fellow at the given index %s",
                index));
    }

    Index
    Ensemble::seek(Subject const& subject) const
    {
      Ensemble::Scoutor scoutor;
      Index i(0);

      ELLE_LOG_TRACE_SCOPE("seek(%s)", subject);

      // XXX[consider the owner]

      for (scoutor = this->_container.begin();
           scoutor != this->_container.end();
           ++scoutor, ++i)
        {
          Fellow* fellow = *scoutor;

          if (fellow->subject() == subject)
            return (i);
        }

      throw Exception(
              elle::sprintf(
                "unable to locate the fellow at the given index %s",
                subject));
    }

    Range<Fellow> const
    Ensemble::consult(Index const& index,
                      Size const& size) const
    {
      Ensemble::Scoutor scoutor;
      Range<Fellow> range;
      Index i(0);

      ELLE_LOG_TRACE_SCOPE("consult(%s, %s)", index, size);

      // XXX[add the owner to the list]

      if (range.Detach() == elle::Status::Error)
        throw Exception("unable to detach the data from the range"); // XXX[to remove in the future]

      for (scoutor = this->_container.begin();
           scoutor != this->_container.end();
           ++scoutor)
        {
          Fellow* fellow = *scoutor;

          // XXX
          printf("FELLOW TO CONSULT\n");

          // if this record lies in the selected range [index, index + size[
          if ((i >= index) && (i < (index + size)))
            {
              if (range.Add(fellow) == elle::Status::Error)
                throw Exception("unable to add the record to the given range"); // XXX[to remove in the future]
            }
        }

      return (range);
    }

    void
    Ensemble::update(elle::cryptography::PrivateKey const& k)
    {
      Ensemble::Iterator iterator;

      ELLE_LOG_TRACE_SCOPE("update(%s)", k);

      for (iterator = this->_container.begin();
           iterator != this->_container.end();
           ++iterator)
        {
          Fellow* fellow = *iterator;

          switch (fellow->subject().type())
            {
            case Subject::TypeUser:
              {
                Token token(fellow->subject().user(), k);

                ELLE_LOG_TRACE_SCOPE("update fellow user '%s'", fellow->subject());

                // Update the fellow's token with the freshly constructed
                // token which embeds the new private key encrypted with the
                // user's public key so that only he can decrypt it.
                fellow->token(token);

                break;
              }
            case Subject::TypeGroup:
              {
                // XXX
                throw Exception("not yet implemented");

                break;
              }
            case Subject::TypeUnknown:
            default:
              {
                throw Exception(
                        elle::sprintf("unknown subject type '%s'",
                                      fellow->subject().type()));
              }
            }
        }

      this->state = proton::StateDirty;
    }

    void
    Ensemble::remove(Subject const& subject)
    {
      Iterator iterator;
      Fellow* fellow;

      iterator = this->_locate(subject);
      fellow = *iterator;

      delete fellow;

      this->_container.erase(iterator);

      this->state = proton::StateDirty;
    }

    Ensemble::Scoutor const
    Ensemble::_locate(Subject const& subject) const
    {
      Ensemble::Scoutor scoutor;

      for (scoutor = this->_container.begin();
           scoutor != this->_container.end();
           ++scoutor)
        {
          Fellow* fellow = *scoutor;

          if (fellow->subject() == subject)
            return (scoutor);
        }

      throw Exception(
              elle::sprintf(
                "unable to locate the given subject %s",
                subject));
    }

    Ensemble::Iterator const
    Ensemble::_locate(Subject const& subject)
    {
      Ensemble::Iterator iterator;

      for (iterator = this->_container.begin();
           iterator != this->_container.end();
           ++iterator)
        {
          Fellow* fellow = *iterator;

          if (fellow->subject() == subject)
            return (iterator);
        }

      throw Exception(elle::sprintf("unable to locate the given subject %s",
                                    subject));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    Ensemble::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Ensemble] #"
                << this->_container.size() << std::endl;

      if (ContentHashBlock::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the underlying block");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Fellows]" << std::endl;

      for (auto fellow : this->_container)
        {
          if (fellow->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the fellow");
        }

      return (elle::Status::Ok);
    }

  }
}
