# -*- encoding: utf-8 -*-

from meta import conf

from email.mime.text import MIMEText
from email.header import Header
#from email.utils import parseaddr, formataddr
import smtplib

def send(mail, subject, content,
         from_="Infinit <no-reply@infinit.io>",
         encoding='utf8'):
        msg = MIMEText(content, _charset=encoding)
        msg['Subject'] = Header(subject, encoding)
        msg['From'] = Header(from_, encoding)
        msg['To'] = Header(mail, encoding)

        smtp_server = smtplib.SMTP(conf.MANDRILL_SMTP_HOST, conf.MANDRILL_SMTP_PORT)
        try:
            smtp_server.login(conf.MANDRILL_USERNAME, conf.MANDRILL_PASSWORD)
            smtp_server.sendmail(msg['From'], [msg['To']], msg.as_string())
        finally:
            smtp_server.quit()

