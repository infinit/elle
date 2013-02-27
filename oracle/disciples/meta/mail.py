# -*- encoding: utf-8 -*-

from meta import conf

from email.mime.text import MIMEText
from email.header import Header
from email.Utils import formataddr
#from email.utils import parseaddr, formataddr
import smtplib

def send(mail, subject, content,
         from_="Infinit <no-reply@infinit.io>",
         reply_to=None,
         encoding='utf8'):
    msg = MIMEText(content, _charset=encoding)
    msg['Subject'] = Header(subject, encoding)
    msg['From'] = Header(from_, encoding)
    # Got troubles with Header for recipient.
    msg['To'] = mail #formataddr(("", mail))
    if reply_to is not None:
        msg['Reply-To'] = reply_to

    smtp_server = smtplib.SMTP(conf.MANDRILL_SMTP_HOST, conf.MANDRILL_SMTP_PORT)
    try:
        smtp_server.login(conf.MANDRILL_USERNAME, conf.MANDRILL_PASSWORD)
        smtp_server.sendmail(msg['From'], [msg['To']], msg.as_string())
    finally:
        smtp_server.quit()


###############################################################################
# Mail subjects and body
###############################################################################

# XXX: use mailchimp templates

# Admin invitation:
INVITATION_SUBJECT = "Invitation to test Infinit!"
INVITATION_CONTENT = """
Hi,

Infinit is back and we’ve got some fantastic news to share with you! But,
before we do, we should thank you for all the feedback and insights you’ve
given us on our first release. We know there were a lot of problems and yes, it
was hard to use, but as they say, ‘practice makes perfect.’ And we are striving
for perfection.

First things, first. You need to download Infinit again to get started. I know…
sad face. But it’s ok. From now on, Infinit will auto-update. When you download
Infinit you’ll need to re-register as well (double sad face) and you’ll need
access code below (triple sad face).

The updated version of Infinit has some major improvements in design and
functionality. Let’s get into it:

- Auto-updates are enabled

- Signing up now auto-generates a unique handle for you (you’ll be able to
  change it in a future - version)

- Login and password are now auto-filled

- General design is cleaned up

- Infinit picks up your Mac avatar as your default photo (you’ll be able to
  change it in a future version)

- Transfers and transfer history are shown in the notifications panel (however,
  there still may be small issues)

- Searching for users is improved and cleaner

- Arrow keys and the enter key can be used as shortcuts while searching

- Google Connect button is integrated, but not functional

- Resizing the Finder no longer impacts the interface view in most cases

- Pending notifications are represented by numbers on the notification icon

- Pop-ups now display the correct corresponding transfers accurately

Download Infinit here: http://download.infinit.io/macosx/10.8/Infinit-ia64.dmg

Your access code is: %(activation_code)s

As a reminder, here’s a list of the people that have Infinit at the moment:

    - Cyrille Vincey
    - Valentin Lautier
    - Lionel Tressens
    - Talal Mazroui
    - Juan Diosdado
    - Clement Cazalot
    - Julien Quintard

Let us know if you have any questions, or if you have feedback for us, don’t
hesitate to email us at support@infinit.io.

All the best,

--%(space)s
The Infinit Team
http://infinit.io
""".strip()


#user infitation
USER_INVITATION_SUBJECT = "%(inviter_mail)s wants to share a file through Infinit!"
USER_NEW_FILE_CONTENT = """
Dear user,

%(inviter_fullname)s wants to share %(file_name)s with you on Infinit.

To accept the file, launch Infinit now and accept the file through your Finder
or in the Infinit notifications panel! If you have any technical questions,
don't hesitate to get in touch with us at support@infinit.io.

All the best,

--%(space)s
The Infinit Team
http://infinit.io
""".strip()



USER_INVITATION_CONTENT = """

Dear user,

%(inviter_fullname)s (%(inviter_mail)s) wants to share %(file_name)s and make you discover Infinit.

%(message)s

XXXX
To get started, download Infinit here: http://infinit.io/download.

IMPORTANT:
XXX
blabla, use this email, blabla

If there are other people with whom you would like to use Infinit, please let
us know by sending an email to community@infinit.io.

Instructions:

 * MacOSX (only Mountain Lion):
    1) Install Infinit and create an account with your access code.

 * Linux:
    1) Wait for infinit to be integrated.

All the best,

--
The Infinit Team
http://infinit.io
""".strip()

# Netork invitation
NETWORK_INVITATION_NEWUSER_SUBJECT = u"%(added_by)s wants you to join the '%(network_name)s' network on Infinit!"
NETWORK_INVITATION_NEWUSER_CONTENT = u"""
Hi %(recipient)s,

%(added_by)s has added you to the '%(network_name)s' network on Infinit.
Infinit is an incredibly powerful tool that lets you create folders with the
members of your different communities, giving you the ability to easily share
and access files and folders collaboratively.

You’ll no longer have to worry about storage limits in the cloud, large email
attachments or downloading files again!
And best of all, it's completely free :)

Click the link below to download Infinit and access your new network!

Download here: http://infinit.io/download

All the best,

--%(space)s
The Infinit Team
infinit.io
""".strip()

NETWORK_INVITATION_SUBJECT = u"%(added_by)s has just added you to the '%(network_name)s' network on Infinit!"
NETWORK_INVITATION_CONTENT = u"""
Hi %(recipient)s,

%(added_by)s has added you to the '%(network_name)s' network. Just launch
Infinit to access it!

Networks are made up of your community’s combined storage space allowing
everyone to access more content than they can store on their own devices. The
more space you and your community add to a network, the more files and folders,
you can all access.

Sharing and accessing files has never been easier! And best of all, it's
completely free :)

All the best,

--%(space)s
The Infinit Team
infinit.io
""".strip()


BACKTRACE_SUBJECT = u"""Crash report: %(signal)s in %(module)s - %(user)s""".strip()
BACKTRACE_CONTENT = u"""
%(user)s

---------
BACKTRACE
---------
%(bt)s

---------
ENV
---------
%(env)s

---------
SPEC
---------
%(spec)s

---------
More
---------
%(more)s
""".strip()
