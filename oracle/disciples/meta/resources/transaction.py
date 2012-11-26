import json
import web

from meta.page import Page
from meta import notifier
from meta import database
from meta import error
from meta import regexp
from meta import mail

import re

import metalib

NONE = 0
PENDING = 1
REJECTED = 2
ACCEPTED = 3
READY = 4
DELETED = 5
STARTED = 6
FINISHED = 7

class Create(Page):
    """
    Send a file to a specific user.
    If you pass an email and the user is not registred in infinit,
    create a 'ghost' in the database, waiting for him to register.

    POST {
              'recipient_id_or_email': "email@pif.net", #required
              'first_filename': "The first file name",
              'files_count': 32
              'total_size': 42 (ko)
              'is_directory': bool
              'device_id': The device from where the file is get
              'network_id': "The network name", #required
         }
         -> {
                'created_transaction_id'
            }


    Errors:
        Using an id that doesn't exist.

    """
    __pattern__ = "/transaction/create"

    _validators = {
        'recipient_id_or_email': regexp.NonEmptyValidator,
        'first_filename': regexp.FilenameValidator,
        'network_id': regexp.NetworkValidator,
        'device_id': regexp.DeviceIDValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        id_or_email = self.data['recipient_id_or_email'].strip()
        first_filename = self.data['first_filename'].strip()
        network_id = self.data['network_id'].strip()
        device_id = self.data['device_id'].strip()
        files_count = int(self.data['files_count'])
        total_size = int(self.data['total_size'])
        is_dir = bool(self.data['is_directory'])

        invitee_id = 0
        invitee_email = ""
        new_user = False
        invitee = 0

        # Determine if user sent a mail or an id.
        if re.match(regexp.Email, id_or_email):
            invitee_email = id_or_email
            # Check is user is in database.
            invitee = database.users().find_one({'email': id_or_email})
            # if the user doesn't exist, create a ghost and invite.
            if not invitee:
                new_user = True
                invitee_id = database.users().save({})
                self.registerUser(
                    _id = invitee_id,
                    email = invitee_email,
                    register_status = 'ghost',
                    notifications = [],
                    accounts=[{'type':'email', 'id':invitee_email}]
                )
            else:
                invitee_id = invitee['_id']
        elif not re.match(regexp.ID, id_or_email):
            return self.error(error.USER_ID_NOT_VALID)
        else:
             invitee_id = database.ObjectId(id_or_email)

        _id = self.user['_id']

        req = {
            'recipient_id': database.ObjectId(invitee_id),
            'sender_id': database.ObjectId(_id),
            'sender_device_id': database.ObjectId(device_id),
            'network_id': database.ObjectId(network_id),
            'status': PENDING,
            'first_filename' : first_filename,
            'files_count' : files_count,
            'total_size' : total_size,
            'is_directory' : is_dir,
            'sender_fullname' : self.user['fullname'],
            'sender_device_id' : device_id,

            # Empty while accepted or rejected.
            'recipient_fullname' : '',
            'recipient_device_id' : '',
            'recipient_device_name' : '',
        }

        if not database.transactions().find_one(req):
            transaction_id = database.transactions().insert(req)

        sent = first_filename + (files_count == 1 and "" or " and %i other files" % (files_count - 1))

        if not self.connected(invitee_id):
            if not invitee_email:
                invitee_email = database.users().find_one({'_id': database.ObjectId(id_or_email)})['email']
            inviter_mail = self.user['email']

            subject = mail.USER_INVITATION_SUBJECT % {
                'inviter_mail': inviter_mail,
            }

            content = (new_user and mail.USER_INVITATION_CONTENT or mail.USER_NEW_FILE_CONTENT) % {
                'inviter_mail': inviter_mail,
                'file_name': sent,
            }

            mail.send(invitee_email, subject, content)

        self.notifier.notify_some(
            notifier.FILE_TRANSFER,
            [database.ObjectId(invitee_id), database.ObjectId(_id)], # sender and recipient.
            {
                'transaction_id' : transaction_id,

                # Sender.
                'sender_id' : _id,
                'sender_fullname' : self.user['fullname'],
                'sender_device_id': device_id,

                # Recipient.
                'recipient_id': invitee_id,

                # Network.
                'network_id' : network_id,

                # File info.
                'first_filename': first_filename,
                'files_count': files_count,
                'total_size': total_size,
                'is_directory': is_dir,
            }
        )

        return self.success({
            'created_transaction_id': transaction_id,
        })

class Update(Page):
    """
    Use to accept or deny a file transfer.
    Maybe more in the future but be careful, for the moment, user MUST be the recipient.
    POST {
           'transaction_id' : the id of the transaction.
           'device_id': the device id on which the user accepted the transaction.
           'device_name': the device name on which the user accepted the transaction.
           'status': status
         }
         -> {
                 'updated_transaction_id': the network id or empty string if refused.
            }

    Errors:
        The transaction doesn't exists.
        The use is not the recipient.
        Recipient and sender devices are the same.
    """
    __pattern__ = "/transaction/update"

    _validators = {
        'transaction_id': regexp.TransactionValidator,
        'device_id': regexp.DeviceIDValidator,
        'device_name': regexp.NonEmptyValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        transaction =  database.transactions().find_one(
            database.ObjectId(self.data['transaction_id'].strip()))

        if not transaction:
            return self.error(error.TRANSACTION_DOESNT_EXIST)

        if self.user['_id'] != transaction['recipient_id']:
            return self.error(error.TRANSACTION_DOESNT_BELONG_TO_YOU)

        if self.data['device_id'] == transaction['sender_device_id']:
            return self.error(error.TRANSACTION_CANT_BE_ACCEPTED, "Sender and recipient devices are the same.")

        transaction.update({
            'recipient_device_name' : self.data['device_name'],
            'recipient_device_id': database.ObjectId(self.data['device_id']),
            'status': self.data['status'], # accept or declined.
        })

        sender = database.users().find_one(database.ObjectId(transaction['sender_id']))
        recipient = database.users().find_one(database.ObjectId(transaction['recipient_id']))

        # If transfer is accepted, increase popularity of each user.
        if self.data['status'] == ACCEPTED and recipient['_id'] != sender['_id']:
            # XXX: probably not optimized, we should maybe use database.find_and_modify and increase
            # the value.
            sender['swaggers'][recipient['_id']] = sender['swaggers'].setdefault(recipient['_id'], 0) + 1;
            recipient['swaggers'][sender['_id']] = recipient['swaggers'].setdefault(sender['_id'], 0) + 1;
            database.users().save(sender);
            database.users().save(recipient);

        updated_transaction_id = database.transactions().save(transaction);

        self.notifier.notify_some(
            notifier.FILE_TRANSFER_STATUS,
            [transaction['sender_id'], transaction['recipient_id']],
            {
                'transaction_id': str(updated_transaction_id),

                # Sender.
                'sender_id': str(transaction['sender_id']),
                'sender_device_id': str(transaction['sender_device_id']),

                # Recipient.
                'recipient_id': str(transaction['recipient_id']),
                'recipient_device_id': self.data['device_id'],
                'recipient_device_name': self.data['device_name'],

                # Network.
                'network_id': str(transaction['network_id']),

                # Status.
                'status': self.data['status'],
            }
        )

        return self.success({
            'updated_transaction_id': str(updated_transaction_id),
        })

class Start(Page):
    """
    Notify the user that everything is ok and he can start downloading file.
    POST {
             'transaction_id' : the id
         }
         -> {
                 'updated_transaction_id' : the (new) id
            }

    Errors:
        The transaction doesn't exist.
        The user is not the sender.
        The transaction is not in a state in which it can be started.
    """
    __pattern__ = "/transaction/start"

    _validators = {
        'transaction_id': regexp.TransactionValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        transaction =  database.transactions().find_one(
            database.ObjectId(self.data['transaction_id'].strip()))

        if not transaction:
            return self.error(error.TRANSACTION_DOESNT_EXIST)

        if self.user['_id'] != transaction['sender_id']:
            return self.error(error.TRANSACTION_DOESNT_BELONG_TO_YOU)

        if not transaction['status'] in (ACCEPTED, READY) :
            return self.error(error.TRANSACTION_OPERATION_NOT_PERMITTED,
                              "This transaction can't be started. Status : %i" % transaction['status'])

        transaction.update({
            'status': STARTED, # started.
        })

        updated_transaction_id = database.transactions().save(transaction)
        self.notifier.notify_some(
            notifier.FILE_TRANSFER_STATUS,
            [transaction['sender_id'], transaction['recipient_id']],
            {
                'transaction_id': str(transaction['_id']),

                # Sender.
                'sender_id': str(transaction['sender_id']),
                'sender_device_id': str(transaction['sender_device_id']),

                # Recipient.
                'recipient_id': str(transaction['recipient_id']),
                'recipient_device_id': str(transaction['recipient_device_id']),
                'recipient_device_name': transaction['recipient_device_name'],

                # Network.
                'network_id': str(transaction['network_id']),

                # Status.
                'status': transaction['status'],
            }
        )

        return self.success({
            'updated_transaction_id': str(updated_transaction_id),
        })

class Delete(Page):
    """
    Delete the transaction that has been rejected or you want to delete by your self.
    POST {
              'transaction_id': the id
         }
         -> {
                 'updated_transaction_id'
            }

    Errors:
        The transaction doesn't exist.
        The user is not the sender.
        The transaction is not in a state in which it can be deleted.
    """
    __pattern__ = "/transaction/delete"

    _validators = {
        'transaction_id': regexp.TransactionValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        transaction =  database.transactions().find_one(
            database.ObjectId(self.data['transaction_id'].strip()))

        if not transaction:
            return self.error(error.TRANSACTION_DOESNT_EXIST)

        if self.user['_id'] != transaction['sender_id']:
            return self.error(error.TRANSACTION_DOESNT_BELONG_TO_YOU)

        if not transaction['status'] in (REJECTED, PENDING, STARTED) :
            return self.error(error.TRANSACTION_OPERATION_NOT_PERMITTED,
                              "This transaction can't be started. Status : %i" % transaction['status'])

        transaction.update({
            'status': DELETED, # started.
        })

        updated_transaction_id = database.transactions().save(transaction)

        self.notifier.notify_some(
            notifier.FILE_TRANSFER_STATUS,
            [transaction['sender_id'], transaction['recipient_id']],
            {
                'transaction_id': str(transaction['_id']),

                # Sender.
                'sender_id': str(transaction['sender_id']),
                'sender_device_id': str(transaction['sender_device_id']),

                # Recipient.
                'recipient_id': str(transaction['recipient_id']),
                'recipient_device_id': str(transaction['recipient_device_id']),
                'recipient_device_name': transaction['recipient_device_name'],

                # Newtork.
                'network_id': str(transaction['network_id']),

                # Status.
                'status': transaction['status'],
            }
        )

        return self.success({
            'updated_transaction_id': str(updated_transaction_id),
        })


class Close(Page):
    """
    Notify the sender that the transaction is complete.
    POST {
             'transaction_id' : the id
         }
         -> {
                 'updated_transaction_id' : the (new) id
            }

    Errors:
        The transaction doesn't exist.
        The user is not the recipient.
    """
    __pattern__ = "/transaction/finish"

    _validators = {
        'transaction_id': regexp.TransactionValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        transaction =  database.transactions().find_one(
            database.ObjectId(self.data['transaction_id'].strip()))

        if not transaction:
            return self.error(error.TRANSACTION_DOESNT_EXIST)

        if self.user['_id'] != transaction['recipient_id']:
            return self.error(error.TRANSACTION_DOESNT_BELONG_TO_YOU)

        if not transaction['status'] in (STARTED) :
            return self.error(error.TRANSACTION_OPERATION_NOT_PERMITTED,
                              "This transaction can't be started. Status : %i" % transaction['status'])

        transaction.update({
            'status': FINISHED, # finished.
        })

        # This transaction should be move to a 'finished_transaction' collection.
        updated_transaction_id = database.transactions().save(transaction);

        self.notifier.notify_some(
            notifier.FILE_TRANSFER_STATUS,
            [transaction['sender_id'], transaction['recipient_id']],
            {
                'transaction_id': str(transaction['_id']),

                # Sender.
                'sender_id': str(transaction['sender_id']),
                'sender_device_id': str(transaction['sender_device_id']),

                # Recipient.
                'recipient_id': str(transaction['recipient_id']),
                'recipient_device_id': str(transaction['recipient_device_id']),
                'recipient_device_name': transaction['recipient_device_name'],

                # Network.
                'network_id': str(transaction['network_id']),

                # Status.
                'status': transaction['status'],
            }
        )

        return self.success({
            'updated_transaction_id': updated_transaction_id,
        })

class All(Page):
    """
    Get all transaction involving user (as sender or recipient).
    GET
         -> {
                 [id0, id1, ...]
            }
    """
    __pattern__ = "/transactions"

    def GET(self):
        self.requireLoggedIn()

        # XXX: Maybe we can merge thoose request using $or.
        transactions = list(database.transactions().find({'recipient_id' : self.user['_id']}, ["_id"]))
        transactions += list(database.transactions().find({'sender_id' : self.user['_id']}, ["_id"]))

        # turn transaction to a list of id as string, removing duplicates.
        transactions = list(set([str(t['_id']) for t in transactions]))

        return self.success({'transactions': transactions})

class One(Page):
    """
    Get details of a specific transaction.
        Get
            -> {
                    'transaction_id' : The id of the transaction.

                    'sender_id' :
                    'sender_fullname' :
                    'sender_device_id' :

                    'recipient_id' :
                    'recipient_fullname' :
                    'recipient_device_id' :
                    'recipient_device_name' :

                    'network_id' :

                    'first_filename' :
                    'files_count' :
                    'total_size' :
                    'is_directory' :

                    'status' :
            }
    """

    __pattern__ = "/transaction/(.+)/view"

    def GET(self, _id):
        self.requireLoggedIn()

        transaction = database.transactions().find_one(database.ObjectId(_id))

        if not transaction:
            return self.error(error.TRANSACTION_DOESNT_EXIST)

        if not self.user['_id'] in (transaction['sender_id'], transaction['recipient_id']):
            return self.error(error.TRANSACTION_DOESNT_BELONG_TO_YOU)

        res = {
            'transaction_id' : str(transaction['_id']),

            'first_filename' : transaction['first_filename'],
            'files_count' : transaction['files_count'],
            'total_size' : transaction['total_size'],
            'is_directory' : transaction['is_directory'],
            'network_id' : str(transaction['network_id']),
            'sender_id' : str(transaction['sender_id']),
            'sender_fullname' : transaction['sender_fullname'],
            'sender_device_id' : str(transaction['sender_device_id']),
            'recipient_id' : str(transaction['recipient_id']),
            'recipient_fullname' : transaction['recipient_fullname'],
            'recipient_device_id' : str(transaction['recipient_device_id']),
            'recipient_device_name' : str(transaction['recipient_device_name']),
            'status' : int(transaction['status']),
        }

        return self.success(res)
