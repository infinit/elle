#include "../State.hh"
#include "../MetricReporter.hh"

#include <common/common.hh>

#include <elle/utility/Time.hh>
#include <elle/idiom/Close.hh>

#include <boost/filesystem.hpp>

#include <QString>
#include <QByteArray>
#include <QLocalSocket>
#include <QProcess>

ELLE_LOG_COMPONENT("infinit.surface.gap.State");

namespace surface
{
  namespace gap
  {

    namespace fs = boost::filesystem;

    State::ProcessId
    State::send_files(std::string const& recipient_id_or_email,
                      std::unordered_set<std::string> const& files)
    {
      ELLE_TRACE("Sending files to %s", recipient_id_or_email);
      return this->_add_process(
        "send_files",
        std::bind(&State::_send_files, this, recipient_id_or_email, files)
      );
    }

    void
    State::_send_files(std::string const& recipient_id_or_email,
                       std::unordered_set<std::string> const& files)
    {
      ELLE_TRACE("_Sending file to '%s'.", recipient_id_or_email);

      if (files.empty())
        throw Exception(gap_no_file,
                        "no files to send");

      int size = 0;
      for (auto const& path : files)
        {
          if (!fs::exists(path))
            throw Exception(gap_file_not_found,
                            "file doesn't exist.");

          size += this->file_size(path);
        }

      std::string first_filename =
        fs::path(*(files.cbegin())).filename().string();

      ELLE_DEBUG("First filename '%s'.", first_filename);

      // Build timestamp.
      elle::utility::Time time;
      time.Current();

      std::string network_name = elle::sprintf(
          "%s-%s", recipient_id_or_email, time.nanoseconds
      );

      ELLE_DEBUG("Creating temporary network '%s'.", network_name);

      std::string network_id = this->create_network(network_name);

      ELLE_DEBUG("created network id is %s", network_id);
      if (this->_wait_portal(network_id) == false)
          throw Exception{gap_error, "Couldn't find portal to infinit instance"};

      ELLE_DEBUG("Retrieving 8transfert binary path...");
      auto transfer_binary = common::infinit::binary_path("8transfer");
      ELLE_DEBUG("Using 8transfert binary '%s'", transfer_binary);

      metrics::google::server().store("transaction:create:attempt",
                                    {{"cm1", std::to_string(files.size())},
                                     {"cm2", std::to_string(size)}});

      try
        {
          try
            {
              for (auto& file: files)
              {
                QStringList arguments;

                arguments << "-n" << network_id.c_str()
                          << "-u" << this->_me._id.c_str()
                          << "--path" << file.c_str()
                          << "--to";

                ELLE_DEBUG("LAUNCH: %s %s",
                           transfer_binary,
                           arguments.join(" ").toStdString());

                QProcess p;
                p.start(transfer_binary.c_str(), arguments);
                if (!p.waitForFinished(-1))
                  throw Exception(gap_internal_error, "8transfer binary failed");
                if (p.exitCode())
                  throw Exception(gap_internal_error, "8transfer binary exited with errors");
              }
            }
          catch (...)
            {
              // Something went wrong, we need to destroy the network.
              this->delete_network(network_id, false);
              throw;
            }

          plasma::meta::CreateTransactionResponse res;

          res = this->_meta->create_transaction(recipient_id_or_email,
                                                first_filename,
                                                files.size(),
                                                size,
                                                fs::is_directory(first_filename),
                                                network_id,
                                                this->device_id());

          metrics::google::server().store("transaction:create:succeed",
                                          {{"cd2", res.created_transaction_id},
                                           {"cm1", std::to_string(files.size())},
                                           {"cm2", std::to_string(size)}});

        }
      CATCH_FAILURE_TO_METRICS("transaction:create");
    }

    float
    State::transaction_progress(std::string const& transaction_id)
    {
      ELLE_TRACE("Retrieve progress of transaction %s", transaction_id);

      auto const& tr = this->transaction(transaction_id);

      if (tr.status == gap_transaction_status_finished)
        return 1.0f;
      else if (tr.status == gap_transaction_status_canceled)
        return 0.0f;
      else if (tr.status != gap_transaction_status_started)
        return 0.0f;
      else if (!this->infinit_instance_manager().has_network(tr.network_id))
        {
          throw Exception{
              gap_network_not_found,
              "Cannot launch 8progress without infinit instance"
          };
        }

      std::string const& progress_binary = common::infinit::binary_path("8progress");
      QStringList arguments;
      arguments << "-n" << tr.network_id.c_str()
                << "-u" << this->_me._id.c_str()
      ;
      ELLE_DEBUG("LAUNCH: %s %s", progress_binary,
                 arguments.join(" ").toStdString());

      QProcess p;
      p.start(progress_binary.c_str(), arguments);
      if (!p.waitForFinished())
        throw Exception{
            gap_internal_error, "8progress binary failed"
        };
      if (p.exitCode())
        throw Exception{
            gap_internal_error, "8progress binary exited with errors"
        };

      int progress = 0;
      std::stringstream ss{p.readAllStandardOutput().data()};
      ss >> progress;

      if (progress < 0)
        {
          ELLE_WARN("8progress returned a negative integer: %s", progress);
          progress = 0;
        }
      else if (progress > 100)
        {
          ELLE_WARN("8progress returned an integer greater than 100: %s", progress);
          progress = 100;
        }
      return ((float) progress) / 100.0f;
    }

    void
    State::_download_files(std::string const& transaction_id)
    {
      auto pair = State::transactions().find(transaction_id);

      ELLE_ASSERT(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      Transaction const& trans = pair->second;

      std::string const& transfer_binary = common::infinit::binary_path("8transfer");

      QStringList arguments;
      arguments << "-n" << trans.network_id.c_str()
                << "-u" << this->_me._id.c_str()
                << "--path" << this->_output_dir.c_str()
                << "--from";

      ELLE_DEBUG("LAUNCH: %s %s",
                 transfer_binary,
                 arguments.join(" ").toStdString());

      try
      {
        QProcess p;
        p.start(transfer_binary.c_str(), arguments);
        if (!p.waitForFinished(-1))
          throw Exception(gap_internal_error, "8transfer binary failed");
        if (p.exitCode())
          throw Exception(gap_internal_error, "8transfer binary exited with errors");

        ELLE_WARN("Download complete. Your file is at '%s'.",
                  this->_output_dir.c_str());

        update_transaction(transaction_id,
                         gap_TransactionStatus::gap_transaction_status_finished);
      }
      catch (...)
      {
        update_transaction(transaction_id,
                         gap_TransactionStatus::gap_transaction_status_canceled);
      }
    }

    static
    bool
    _check_action_is_available(std::string const& user_id,
                               Transaction const& transaction,
                               gap_TransactionStatus status)
    {
      typedef
        std::map<gap_TransactionStatus, std::set<gap_TransactionStatus>>
        StatusMap;

      static const StatusMap _sender_status_update{
        {gap_transaction_status_pending,
          {gap_transaction_status_canceled}},
        {gap_transaction_status_accepted,
          {gap_transaction_status_prepared, gap_transaction_status_canceled}},
        {gap_transaction_status_prepared,
          {gap_transaction_status_canceled}},
        {gap_transaction_status_started,
          {gap_transaction_status_canceled}},
        // {gap_transaction_status_canceled,
        //   {}},
        // {gap_transaction_status_finished,
        //   {}}
      };

      static StatusMap _recipient_status_update{
        {gap_transaction_status_pending,
          {gap_transaction_status_accepted, gap_transaction_status_canceled}},
        {gap_transaction_status_accepted,
          {gap_transaction_status_canceled}},
        {gap_transaction_status_prepared,
          {gap_transaction_status_started, gap_transaction_status_canceled}},
        {gap_transaction_status_started,
          {gap_transaction_status_canceled, gap_transaction_status_finished}},
        // {gap_transaction_status_canceled,
        //   {}},
        // {gap_transaction_status_finished,
        //   {}}
      };

      if (user_id != transaction.recipient_id &&
          user_id != transaction.sender_id)
      {
        throw Exception{gap_error, "You are neither recipient nor the sender."};
      }

      if (user_id == transaction.recipient_id)
      {
        auto const& status_list = _recipient_status_update.find(
          (gap_TransactionStatus) transaction.status);

        if (status_list == _recipient_status_update.end() ||
            status_list->second.find((gap_TransactionStatus) status) == status_list->second.end())
        {
          ELLE_WARN("You are not allowed to change status from %s to %s",
                    transaction.status, status);
          return false;
        }
      }
      else if (user_id == transaction.sender_id)
      {
         auto const& status_list = _sender_status_update.find(
           (gap_TransactionStatus) transaction.status);

        if (status_list == _sender_status_update.end() ||
            status_list->second.find((gap_TransactionStatus)status) == status_list->second.end())
        {
          ELLE_WARN("You are not allowed to change status from %s to %s",
                    transaction.status, status);
          return false;
        }
      }

      return true;
    }

    void
    State::update_transaction(std::string const& transaction_id,
                              gap_TransactionStatus status)
    {
      ELLE_DEBUG("Update transaction '%s': '%s'",
                 transaction_id, status);

      auto pair = State::transactions().find(transaction_id);

      ELLE_ASSERT(pair != State::transactions().end());

      if (pair == State::transactions().end())
        return;

      Transaction const& transaction = pair->second;

      if (!_check_action_is_available(this->_me._id, transaction, status))
        return;

      switch (status)
      {
        case gap_transaction_status_accepted:
          this->_accept_transaction(transaction);
          break;
        case gap_transaction_status_prepared:
          this->_prepare_transaction(transaction);
          break;
        case gap_transaction_status_started:
          this->_start_transaction(transaction);
          break;
        case gap_transaction_status_canceled:
          this->_cancel_transaction(transaction);
          break;
        case gap_transaction_status_finished:
          this->_close_transaction(transaction);
          break;
        default:
          ELLE_WARN("You are not able to change transaction status to '%s'.",
                    status);
          return;
      }

      // Send file request successful.
    } // !update_transaction()

    void
    State::_accept_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Accept transaction '%s'", transaction.transaction_id);

      if (transaction.recipient_id != this->_me._id)
      {
        throw Exception{gap_error, "Only recipient can accept transaction."};
      }

      metrics::google::server().store("transaction:accept:attempt",
                                      {{"cd2", transaction.transaction_id}});

      try
      {
        this->_meta->update_transaction(transaction.transaction_id,
                                        plasma::TransactionStatus::accepted,
                                        this->device_id(),
                                        this->device_name());
      }
      CATCH_FAILURE_TO_METRICS("transaction:accept");

      metrics::google::server().store("transaction:accept:succeed",
                                      {{"cd2", transaction.transaction_id}});

      // Could be improve.
      _swaggers_dirty = true;
    }

    void
    State::_on_transaction_accepted(Transaction const& transaction)
    {
      ELLE_TRACE("On transaction accepted '%s'", transaction.transaction_id);

      if (transaction.sender_device_id != this->device_id())
        return;

      ELLE_DEBUG("giving '%s' access to the network '%s'",
                transaction.recipient_id,
                transaction.network_id);

      this->_networks_dirty = true;
      this->network_add_user(transaction.network_id,
                             transaction.recipient_id);

      ELLE_DEBUG("Giving '%s' permissions on the network to '%s'.",
                transaction.recipient_id,
                transaction.network_id);

      this->set_permissions(transaction.recipient_id,
                            transaction.network_id,
                            nucleus::neutron::permissions::write);

      ELLE_DEBUG("update transaction");

      // When recipient has rights, allow him to start download.
      this->update_transaction(transaction.transaction_id,
                               gap_transaction_status_prepared);

      // XXX Could be improved.
      _swaggers_dirty = true;
    }

    void
    State::_prepare_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("prepare transaction '%s'", transaction.transaction_id);

      if (transaction.sender_device_id != this->device_id())
      {
        throw Exception{gap_error, "Only sender can prepare his network."};
      }

      metrics::google::server().store("transaction:prepare:attempt",
                                      {{"cd2", transaction.transaction_id}});

      try
      {
      this->_meta->update_transaction(transaction.transaction_id,
                                      plasma::TransactionStatus::prepared);
      }
      CATCH_FAILURE_TO_METRICS("transaction:prepare");

      metrics::google::server().store("transaction:prepare:succeed",
                                      {{"cd2", transaction.transaction_id}});
    }

    void
    State::_on_transaction_prepared(Transaction const& transaction)
    {
      ELLE_TRACE("prepared trans '%s'", transaction.transaction_id);

      if (transaction.recipient_device_id != this->device_id())
      {
        ELLE_DEBUG("transaction doesn't concern your device.");
        // grace à hanthhhony
        return;
      }

      this->_networks_dirty = true;
      this->prepare_network(transaction.network_id);
      this->_meta->network_add_device(
          transaction.network_id,
          this->device_id()
      );
      this->infinit_instance_manager().launch_network(transaction.network_id);
      // Ensure creation.
      this->_wait_portal(transaction.network_id);

      this->update_transaction(transaction.transaction_id,
                               gap_transaction_status_started);
    }

    void
    State::_start_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Start transaction '%s'", transaction.transaction_id);

      if (transaction.recipient_device_id != this->device_id())
      {
        throw Exception{gap_error, "Only recipient can start transaction."};
      }

      metrics::google::server().store("transaction:start:attempt",
                                      {{"cd2", transaction.transaction_id}});

      try
      {
        this->_meta->update_transaction(transaction.transaction_id,
                                        plasma::TransactionStatus::started);
      }
      CATCH_FAILURE_TO_METRICS("transaction:start");

      metrics::google::server().store("transaction:start:succeed",
                                      {{"cd2", transaction.transaction_id}});
    }

    void
    State::_on_transaction_started(Transaction const& transaction)
    {
      ELLE_TRACE("Started trans '%s'", transaction.transaction_id);

      if (transaction.recipient_device_id != this->device_id() &&
          transaction.sender_device_id != this->device_id())
      {
        ELLE_DEBUG("transaction doesn't concern your device.");
        return;
      }

      if (this->_wait_portal(transaction.network_id) == false)
        throw Exception{gap_error, "Couldn't find portal to infinit instance"};

      std::exception_ptr exception;
      {
        reactor::Scheduler sched;
        reactor::Thread sync{
            sched,
            "notify_8infinit",
            [&] () -> void {
                try {
                  this->_notify_8infinit(transaction, sched);
                } catch (std::runtime_error const&) {
                    exception = std::current_exception();
                }
            }
        };

        sched.run();
      }


      if (exception != std::exception_ptr{})
      {
        this->delete_network(transaction.network_id, true);
        std::rethrow_exception(exception); // XXX SCOPE OF EXCEPTION PTR
      }
      if (transaction.recipient_device_id == this->device_id())
        _download_files(transaction.transaction_id);
    }

    void
    State::_close_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Close transaction '%s'", transaction.transaction_id);

      if(transaction.recipient_device_id != this->device_id())
      {
        throw Exception{gap_error,
            "Only recipient can close transaction."};
      }

      metrics::google::server().store("transaction:finish:attempt",
                                      {{"cd2", transaction.transaction_id}});

      try
      {
        this->_meta->update_transaction(transaction.transaction_id,
                                        plasma::TransactionStatus::finished);
      }
      CATCH_FAILURE_TO_METRICS("transaction:finish");

      metrics::google::server().store("transaction:finish:succeed",
                                      {{"cd2", transaction.transaction_id}});

    }

    void
    State::_on_transaction_closed(Transaction const& transaction)
    {
      ELLE_DEBUG("Closed transaction '%s'", transaction.transaction_id);

      // Delete networks.
      this->delete_network(transaction.network_id, true /* force */);
    }

    void
    State::_cancel_transaction(Transaction const& transaction)
    {
      ELLE_DEBUG("Cancel transaction '%s'", transaction.transaction_id);

      //XXX: If download has started, cancel it, delete files, ...
      if (transaction.sender_id == this->_me._id)
      {
        metrics::google::server().store("transaction:cancel:sender:attempt",
                                        {{"cd1", std::to_string(transaction.status)},
                                         {"cd2", transaction.transaction_id}});

        try
        {
          this->_meta->update_transaction(transaction.transaction_id,
                                          plasma::TransactionStatus::canceled);
        }
        CATCH_FAILURE_TO_METRICS("transaction:cancel:sender");

        metrics::google::server().store("transaction:cancel:sender:succeed",
                                        {{"cd1", std::to_string(transaction.status)},
                                          {"cd2", transaction.transaction_id}});
      }
      else
      {
        metrics::google::server().store("transaction:cancel:recipient:attempt",
                                        {{"cd1", std::to_string(transaction.status)},
                                         {"cd2", transaction.transaction_id}});
        try
        {
          this->_meta->update_transaction(transaction.transaction_id,
                                          plasma::TransactionStatus::canceled);
        }
        CATCH_FAILURE_TO_METRICS("transaction:cancel:recipient");

        metrics::google::server().store("transaction:cancel:recipient:succeed",
                                        {{"cd1", std::to_string(transaction.status)},
                                         {"cd2", transaction.transaction_id}});
      }
    }

    void
    State::_on_transaction_canceled(Transaction const& transaction)
    {
      ELLE_DEBUG("Canceled transaction '%s'", transaction.transaction_id);

      // XXX: If some process are launch, such as 8transfer, 8progess for the
      // current transaction, cancel them.

      // Delete networks.
      this->delete_network(transaction.network_id, true);
    }

    State::TransactionsMap const&
    State::transactions()
    {
      if (_transactions != nullptr)
        return *_transactions;

      _transactions.reset(new TransactionsMap{});

      auto response = this->_meta->transactions();
      for (auto const& transaction_id: response.transactions)
        {
          auto transaction = this->_meta->transaction(transaction_id);
          (*this->_transactions)[transaction_id] = transaction;
        }

      return *(this->_transactions);
    }

    Transaction const&
    State::transaction(std::string const& id)
    {
      auto it = this->transactions().find(id);
      if (it != this->transactions().end())
        return it->second;
      return this->transaction_sync(id);
    }

    Transaction const&
    State::transaction_sync(std::string const& id)
    {
      ELLE_TRACE("Synching transaction %s from meta", id);
      try
        {
          auto transaction = this->_meta->transaction(id);
          ELLE_DEBUG("Synched transaction %s has status %d",
                     id, transaction.status);
          return ((*this->_transactions)[id] = transaction);
        }
      catch (std::runtime_error const& e)
        {
          throw Exception{
              gap_transaction_doesnt_exist,
              e.what()
          };
        }
    }

    size_t
    State::poll(size_t max)
    {
      if (!this->_trophonius)
        throw Exception{gap_error, "Trophonius is not connected"};

      size_t count = 0;
      while (count < max)
        {
          std::unique_ptr<Notification> notif{
              this->_trophonius->poll()
          };

          if (!notif)
            break;
          try
            {
              this->_handle_notification(*notif);
            }
          catch (reactor::Exception const& e)
            {
              ELLE_WARN("reactor exception %s while handling notification '%s'",
                        e, notif->notification_type);
              continue;
            }
          catch (std::runtime_error const& e)
            {
              ELLE_WARN("error %s while handling notification '%s'",
                        e.what(), notif->notification_type);
              continue;
            }
          ++count;
        }

      return count;
    }

    void
    State::transaction_callback(TransactionNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool is_new) -> void {
        return cb(static_cast<TransactionNotification const&>(notif), is_new);
      };

      this->_notification_handlers[NotificationType::transaction].push_back(fn);
    }

    void
    State::transaction_status_callback(TransactionStatusNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool is_new) -> void {
        return cb(static_cast<TransactionStatusNotification const&>(notif), is_new);
      };

      _notification_handlers[NotificationType::transaction_status].push_back(fn);
    }

    void
    State::message_callback(MessageNotificationCallback const& cb)
    {
      auto fn = [cb] (Notification const& notif, bool) -> void {
        return cb(static_cast<MessageNotification const&>(notif));
      };

      this->_notification_handlers[NotificationType::message].push_back(fn);
    }

    void
    State::_on_transaction(TransactionNotification const& notif,
                           bool is_new)
    {
      ELLE_TRACE("New transaction");

      if (!is_new)
        return;

      auto it = this->transactions().find(notif.transaction.transaction_id);

      if (it != this->transactions().end())
      {
        ELLE_WARN("you already have this transaction");
        return;
      }

      // Normal case, this is a new transaction, store it to match server.
      (*this->_transactions)[notif.transaction.transaction_id] = notif.transaction;
    }

    void
    State::_on_transaction_status(TransactionStatusNotification const& notif)
    {
      ELLE_TRACE("_on_notification: gap_TransactionStatusNotification");

      auto const pair = State::transactions().find(notif.transaction_id);

      if (pair == State::transactions().end())
      {
        // Something went wrong.
        auto transaction = this->_meta->transaction(notif.transaction_id);

        (*this->_transactions)[notif.transaction_id] = transaction;
      }

      (*this->_transactions)[notif.transaction_id].status = notif.status;

      auto const& transaction = this->transaction(notif.transaction_id);

      switch((plasma::TransactionStatus) notif.status)
      {
        case plasma::TransactionStatus::accepted:
          // We update the transaction from meta.
          (*_transactions)[notif.transaction_id] = this->_meta->transaction(
              notif.transaction_id
          );
          this->_on_transaction_accepted(transaction);
          break;
        case plasma::TransactionStatus::prepared:
          this->_on_transaction_prepared(transaction);
          break;
        case plasma::TransactionStatus::started:
          this->_on_transaction_started(transaction);
          break;
        case plasma::TransactionStatus::canceled:
          this->_on_transaction_canceled(transaction);
          break;
        case plasma::TransactionStatus::finished:
          this->_on_transaction_closed(transaction);
          break;
        default:
          ELLE_WARN("The status '%s' is unknown.", notif.status);
          return;
      }
    }

  }
}
