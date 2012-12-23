# -*- encoding: utf-8 -*-

from meta import conf

from email.mime.text import MIMEText
from email.header import Header
#from email.utils import parseaddr, formataddr
import smtplib

def send(mail, subject, content,
         from_="Infinit <no-reply@infinit.io>",
         reply_to=None,
         encoding='utf8'):
    msg = MIMEText(content, _charset=encoding)
    msg['Subject'] = Header(subject, encoding)
    msg['From'] = Header(from_, encoding)
    msg['To'] = Header(mail, encoding)
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
Dear Alpha tester,

Welcome to Infinit! You are one of the first individuals to get to try it out!

Remember that the version you will be using is still quite basic and only
functions in a local network at the moment. It may still have bugs and the
interface is still not in its public version. For now, file sharing is limited
to 30 MB and only the network owner can edit the files. Don’t worry though!
We’ll be updating all of this over the coming weeks.

You can report bugs or simply tell us what you think by sending us an email at
feedback@infinit.io or by visiting feedback.infinit.io. If you have any
questions, you can reach us on Skype at infinitdotio.

To get started, download Infinit here: http://infinit.io/download.

Your access code is: %(activation_code)s

If there are other people with whom you would like to use Infinit, please let
us know by sending an email to community@infinit.io.

Instructions:

 * MacOSX (only Mountain Lion):
    1) Install Infinit and create an account with your access code.
    2) Create a network.
    3) Drag and drop other users into your network.
    4) Drag and drop files into your network giving everyone read-only access
       to them.

 * Linux:
    1) Register a new account and start the Infinit shell:
        $ infinit register shell
    2) Start Infinit instances (in the Infinit shell):
        infinit-shell$ start
    3) Create a new network:
        infinit-shell$ create_network MyNetwork
    4) You might need to refresh networks with:
        infinit-shell$ refresh_networks
    5) Invite a friend to a network:
        infinit-shell$ invite my@friend.com MyNetwork

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

%(invited_fullname) (%(inviter_mail)s) wants to share %(file_name)s and make you discover Infinit.

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
