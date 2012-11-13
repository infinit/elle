import json
import web

from meta.page import Page
from meta import notifier
from meta import database
from meta import error
from meta import regexp

import meta.mail

import re

import metalib

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

        _id = self.user['_id']

        req = {'recipient_id': invitee_id,
               'sender_id': _id,
               'sender_device_id': device_id,
               'network_id': network_id,
               'status': -1, # just created.
        }

        if not database.transactions().find_one(req):
            transaction_id = database.transactions().insert(req)

        sent = first_filename + (files_count == 1 and "" or " and %i other files" % (files_count - 1))

        if not self.connected(invitee_id):
            if not invitee_email:
                invitee_email = database.users().find_one({'_id': id_or_email})['email']
            inviter_mail = self.user['email']

            subject = USER_INVITATION_SUBJECT % {
                'inviter_mail': inviter_mail,
            }

            content = (new_user and USER_INVITATION_CONTENT or USER_NEW_FILE_CONTENT) % {
                'inviter_mail': inviter_mail,
                'file_name': sent,
            }

            meta.mail.send(invitee_email, subject, content)

        self.notifier.notify_one(
            notifier.FILE_TRANSFER,
            invitee_id,
            {
                'first_filename': first_filename,
                'files_count': files_count,
                'total_size': total_size,
                'is_directory': is_dir,
                'network_id' : network_id,
                'sender_id' : _id,
                'sender_fullname' : self.user['fullname'],
                'transaction_id' : transaction_id,
            }
        )

        return self.success({
            'created_transaction_id': transaction_id
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
    """
    __pattern__ = "/transaction/update"

    _validators = {
        'transaction_id': regexp.TransactionValidator,
        'device_id': regexp.DeviceIDValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        transaction =  database.transactions().find_one(
            database.ObjectId(self.data['transaction_id'].strip()))

        if not transaction:
            return self.error(error.UNKNOWN, "This transaction doesn't exists.")

        if self.user['_id'] != transaction['recipient_id']:
            return self.error(error.UNKNOWN, "This transaction doesn't belong to you.")

        transaction.update({
            'recipient_device_id':self.data['device_id'],
            'status': status, # accept or declined.
        })

        # XXX: , should we push it in
        # This transaction should be move to a 'finished_transaction' collection.
        updated_transaction_id = database.transactions().save(transaction);

        self.notifier.notify_one(
            notifier.FILE_TRANSFER_STATUS,
            transaction['sender_id'],
            {
                'transaction_id': updated_transaction_id,
                'network_id': transaction['network_id'],
                'sender_device_id': transaction['sender_device_id'],
                'recipient_device_id': self.data['device_id'],
                'recipient_device_name': self.data['device_name'],
                'status': self.data['status'],
            }
        )

        return self.success({
            'updated_transaction_id': updated_transaction_id,
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
            return self.error(error.UNKNOWN, "This transaction doesn't exists.")

        if self.user['_id'] != transaction['sender_id']:
            return self.error(error.UNKNOWN, "This transaction doesn't belong to you.")

        transaction.update({
            'status': 333, # started.
        })

        updated_transaction_id = database.transactions().save(transaction);

        self.notifier.notify_one(
            notifier.FILE_TRANSFER_STATUS,
            transaction['recipient_id'],
            {
                'transaction_id': transaction['_id'],
                'network_id': transaction['network_id'],
                'sender_device_id': transaction['sender_device_id'],
                'recipient_device_id': transaction['recipient_device_id'],
                'recipient_device_name': '',
                'status': transaction['status'],
            }
        )

        return self.success({
            'updated_transaction_id': updated_transaction_id,
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
            return self.error(error.UNKNOWN, "This transaction doesn't exists.")

        if self.user['_id'] != transaction['recipient_id']:
            return self.error(error.UNKNOWN, "This transaction doesn't belong to you.")

        transaction.update({
            'status': 666, # finished.
        })

        # This transaction should be move to a 'finished_transaction' collection.
        updated_transaction_id = database.transactions().save(transaction);

        self.notifier.notify_one(
            notifier.FILE_TRANSFER_STATUS,
            transaction['sender_id'],
            {
                'transaction_id': transaction['_id'],
                'network_id': transaction['network_id'],
                'sender_device_id': transaction['sender_device_id'],
                'recipient_device_id': transaction['recipient_device_id'],
                'recipient_device_name': '',
                'status': transaction['status'],
            }
        )

        return self.success({
            'updated_transaction_id': updated_transaction_id,
        })
