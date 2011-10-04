//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jul 29 12:54:19 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HXX
#define ETOILE_GEAR_SCOPE_HXX

namespace etoile
{
  namespace gear
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the context associated with the scope
    /// by casting it with the given type.
    ///
    template <typename T>
    elle::Status	Scope::Use(T*&				context)
    {
      enter();

      // first, if the scope's context is null, allocate one.
      if (this->context == NULL)
	{
	  // allocate a context according to the nature.
	  this->context = new T;
	}

      // return the context by dynamically casting it.
      if ((context = dynamic_cast<T*>(this->context)) == NULL)
	escape("invalid context nature");

      leave();
    }

    ///
    /// this method is called to indicate the operation being performed
    /// on the scope by the actor.
    ///
    /// note that since multiple actors operate on the same scope, one must
    /// assume that other actors may have modified or even destroy the
    /// scope's target.
    ///
    /// therefore, this method returns a callback on the final operation
    /// to be carried out. however, if actors are still working on the scope,
    /// the returned callback is null.
    ///
    template <typename X,
	      typename T>
    elle::Status	Scope::Operate(const Operation		operation,
				       elle::Callback<
				         elle::Status,
				         elle::Parameters<
				           T&
				         >
				       >&			callback)
    {
      enter();

      // first, update the context's closing operation according to
      // its given value and the given operation.
      switch (operation)
	{
	case OperationDiscard:
	  {
	    // depending on the current context's closing operation.
	    switch (this->context->operation)
	      {
	      case OperationUnknown:
		{
		  //
		  // in this case, the given closing operation is the first
		  // one to take place.
		  //
		  // thus, the context is marked as requiring to be discarded.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDiscard:
		{
		  //
		  // the given closing operation is the same as the current
		  // context's.
		  //
		  // thus, everything seems fine this way.
		  //

		  break;
		}
	      case OperationStore:
		{
		  //
		  // the context's modifications have been marked as requiring
		  // to be stored.
		  //
		  // therefore, the given operation does not imply any change
		  // of plan.
		  //

		  break;
		}
	      case OperationDestroy:
		{
		  //
		  // as for the OperationStore, in this case, the context
		  // has been marked for deletion.
		  //
		  // therefore, the discarding given operation does not
		  // change the scope's closing operation i.e Destroy.
		  //

		  break;
		}
	      }

	    break;
	  }
	case OperationStore:
	  {
	    // depending on the current context's closing operation.
	    switch (this->context->operation)
	      {
	      case OperationUnknown:
		{
		  //
		  // in this case, the given closing operation is the first
		  // one to take place.
		  //
		  // thus, the context is marked as requiring to be stored.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDiscard:
		{
		  //
		  // the given closing operation is of higher importance than
		  // the existing one.
		  //
		  // therefore, the closing operation is set to: Store.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationStore:
		{
		  //
		  // the context's modifications have been marked as requiring
		  // to be stored.
		  //
		  // since the given operation is identical, the context's
		  // closing operation does not need to be changed.
		  //

		  break;
		}
	      case OperationDestroy:
		{
		  //
		  // in this case, the context has been marked for deletion.
		  //
		  // since the storing given operation is of lower importance,
		  // the closing operation is maintained.
		  //

		  break;
		}
	      }

	    break;
	  }
	case OperationDestroy:
	  {
	    // depending on the current context's closing operation.
	    switch (this->context->operation)
	      {
	      case OperationUnknown:
		{
		  //
		  // in this case, the given closing operation is the first
		  // one to take place.
		  //
		  // thus, the context is marked as requiring to be destroyed.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDiscard:
		{
		  //
		  // the given closing operation is of higher importance than
		  // the existing one.
		  //
		  // therefore, the closing operation is set to: Destroy.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationStore:
		{
		  //
		  // in this case, although some actors may have modified
		  // the context---since the current closing operation is
		  // Store---, the operation is set to Destroy because the
		  // given operation superseeds the current one.
		  //

		  // set the context's closing operation.
		  this->context->operation = operation;

		  break;
		}
	      case OperationDestroy:
		{
		  //
		  // in this case, the context has already been marked
		  // for deletion.
		  //
		  // therefore, the closing operation is maintained.
		  //

		  break;
		}
	      }

	    break;
	  }
	case OperationUnknown:
	  {
	    escape("unable to process the closing operation '%u'\n",
		   operation);
	  }
	}

      //
      // finally, return a callback to be triggered by the closing actor
      // depending on both the context's closing operation but also on
      // the presence of concurrent actors.
      //

      // if actors remain i.e apart from the current one, return a null
      // callback.
      //
      // indeed, only the final actor will trigger the closing operation. this
      // way potential conflicts are prevents while expensive cryptographic
      // operations are performed only once.
      if (this->actors.size() > 1)
	{
	  // return a null callback.
	  callback =
	    elle::Callback<
	      elle::Status,
	      elle::Parameters<
		T&
		>
	      >::Null;
	}
      else
	{
	  //
	  // otherwise, the current actor is the last one and is responsible
	  // for triggering the closing operation.
	  //
	  // therefore, returning a callback depending on the context's
	  // closing operation.
	  //

	  // depending on the closing operation...
	  switch (this->context->operation)
	    {
	    case OperationDiscard:
	      {
		// return the callback on the Disard method.
		callback = elle::Callback<>::Infer(&X::Discard);

		break;
	      }
	    case OperationStore:
	      {
		// return the callback on the Store method.
		callback = elle::Callback<>::Infer(&X::Store);

		break;
	      }
	    case OperationDestroy:
	      {
		// return the callback on the Destroy method.
		callback = elle::Callback<>::Infer(&X::Destroy);

		break;
	      }
	    case OperationUnknown:
	      {
		escape("unable to return a callback on the closing "
		       "operation '%u'\n",
		       this->context->operation);
	      }
	    }
	}

      leave();
    }

  }
}

#endif
