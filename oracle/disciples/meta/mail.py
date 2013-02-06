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


#################################################################################
# Mail subjects and body
#################################################################################

# XXX: use mailchimp templates

# Admin invitation:
INVITATION_SUBJECT = "Invitation to test Infinit!"
INVITATION_CONTENT = """
Hi,

We’re finally here! Welcome to the Infinit private beta.

Infinit is a powerful file sharing tool (with better performance than you or
your girlfriend), and you are one of the first people that will get to take it
for a spin! Exciting, right?

For the moment, we’ve restrained our community to a small number of users while
we work on stabilizing the application. For this reason, you won’t be able to
invite other people just yet. It takes time to organize an orgy!

Here is a list of other people on Infinit with whom you can begin sharing. All
guys I'm afraid...

- Cyrille Vincey
- Valentin Lautier
- Lionel Tressens
- Talal Mazroui
- Juan Diosdado
- Clement Cazalot
- Julien Quintard

Another 30 people will be invited to Infinit next week in order to offer you
more possibilities to share.

*You can download Infinit by clicking this link:*

    http://download.infinit.io/macosx/10.7/Infinit-ia64.dmg

Your access code is: %(activation_code)s

Just remember that this is a private beta version of Infinit, so you may
experience bugs, crashes or other impediments to the experience. If you do,
please let us know by sending a quick email to support@infinit.io or
idontgiveashit@infinit.io - both work ;). We’ll get back to you as soon as
possible to better understand and solve your problem unless your name is Talal,
in which case we can't help.

All the best,

--%(space)s
The Infinit Team
http://infinit.io
""".strip()


#user infitation
USER_INVITATION_SUBJECT = "%(inviter_mail)s wants to share a file through Infinit!"
USER_NEW_FILE_CONTENT = """
Dear user,

%(inviter_mail)s wants to share %(file_name)s.

--
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
