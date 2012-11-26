import gap
import time
import datetime
import random
import math
import threading
import re


state = gap.State()

def introspect(obj):
    for attr in dir(obj):
        if not re.match('^__', attr):
            print("%s = %s" % (attr, getattr(obj, attr)))

def BiteFunc(bite:'Bite'):
    print("Notification bite reçue", bite.debug)

def TransactionHandler(t):
    print(">>>>New transaction")
    if t.new and t.recipient_id == state._id :
        print(">>>>>>It's a new one, it for me so accept it !")
        state.update_transaction(t.transaction_id, state.TransactionStatus.accepted)
    elif t.new and t.sender_id == state._id:
        print(">>>>>>It's a new one but i'm the sender, so i do my shit alone.")
    else:
        print(">>>>>>This transaction is an old one, I don't give a shit")

def TransactionStatusHandler(ts:'FileTransferStatus'):
    print("TransactionStatusHandler :", ts.status)

    if ts.status == state.TransactionStatus.none or ts.status == state.TransactionStatus.pending :
        print(">>Do nothing")

    elif ts.status == state.TransactionStatus.accepted and ts.sender_id == state._id:
        print('[Accepted on', ts.recipient_device_name, "]")
        print(">>Invite user to network, start copying, giving rights, ...")
        state.update_transaction(ts.transaction_id, state.TransactionStatus.started)

    elif ts.status == state.TransactionStatus.rejected:
        print(">>The transaction has been canceled, destroy network, delete files...")
        state.update_transaction(ts.transaction_id, state.TransactionStatus.deleted)

    elif ts.status == state.TransactionStatus.finished:
        print(">>The transaction is finished.")

def TransactionStart(t):
    print("\n\n=== Transaction Start")
    introspect(t)

def Message(message):
    print(message.sender_id, ":", message.message)

class PollThread (threading.Thread):
    bite = False

    def run (self) :
        self.bite = False
        while True:
            time.sleep(0.05)
            state.poll()
            if self.bite == True:
                return

    def stop(self):
        self.bite = True

state.OnBite(BiteFunc)
state.OnTransaction(TransactionHandler)
state.OnTransactionStatus(TransactionStatusHandler)
state.OnMessage(Message)

#register: fullname, email, hashed_password, device_name, activation_code
def __register(name, mail):
    state.register(name, mail, "mdptmp", "device", "bitebite")
def __login(mail):
    state.login(mail, "mdptmp")

def __connect():
    state.connect()

def __send_file(mail):
    files = ["/home/dimrok/.zsh", "/home/dimrok/.emacs"]
    state.send_files(mail, files)

def __create_network():
    str = "FistParty%s" % math.floor(random.random()*234567890);
    state.create_network(str)

def __launch_watchdog():
    state.launch_watchdog()

def __pullNotifs():
    state.get_notifications(10)

def main():
    bad_fullname = ""
    good_fullname = "MrDick"

    bad_email = "abc@bi"
    good_email = "%s@a.aa" % math.floor(random.random()*234567890)

    password = "mdptmp"

    state.scratch_db()

    try:
      state.register(bad_fullname, good_email, password, "device", "bitebite")
    except Exception as e:
      print("Bad fullname")

    try:
      state.register(good_fullname, bad_email, password, "device", "bitebite")
    except Exception  as e:
      print ("Bad mail")

#    state.login(good_email, password)
    state.register(good_fullname, good_email, password, "device", "bitebite")

    # state.set_device_name("device")

#    __connect();

    state.logout()

    state.login(good_email, password)

    state.set_device_name("device")

    try :
        __connect();
        polling = PollThread()
        polling.start()

        time.sleep(2)

        __launch_watchdog()

        time.sleep(4)

        print('\n\nPulling notifs')
        __pullNotifs()

        # Send file to me.
        print('Send file to myself')
        __send_file(good_email)
        # time.sleep(10)
        # print('Send file to myself')
        # __send_file(good_email)
        # time.sleep(10)
        # print('Send file to myself')
        # __send_file(good_email)
        # time.sleep(10)
        # print('Send file to myself')
        # __send_file(good_email)

        # Sleep 1sec to wait for transaction
        time.sleep(5)

#        print('\n\nPulling notifs')
#        __pullNotifs()

        time.sleep(3)

        # Read notif and reset process.
        state.notifications_red()
        time.sleep(3)

        # Send file to me.
        print('Send a new file')
        __send_file(good_email)

        # Sleep 1sec to wait for transaction
        time.sleep(3)

#        print('\n\nPulling notifs')
#        __pullNotifs()

        polling.stop()

    except KeyboardInterrupt as e:
        polling.stop()


def main2():
    try:
        m = input('\n[i]nfinit or [g]mail?\n> ')

        if (m != 'i') and (m != 'g'):
            return

        r = (input('\n[r]egister or [l]ogin?\n> '))

        if (r != 'r') and (r != 'l'):
            return

        q = PollThread()
        q.start()

        if m == 'i':
            mail = "a@a.aa"
            recipient = "antony.mechin@gmail.com"
        elif m == 'g':
            mail = "antony.mechin@gmail.com"
            recipient = "a@a.aa"

        if r == 'r':
            __register("Testing Program", mail)
        elif r == 'l':
            __login(mail)

        state.set_device_name("castor")

        __connect()

        state.notifications_red()
        __send_file(mail)

        if input('\npull nofis? y/N\n> ') == 'y':
            __pullNotifs()

        # state.answer_transaction('bite', 1)
        # state.answer_transaction('bite', 0)

        if input('\nwatchdog? y/N\n> ') == 'y':
            __launch_watchdog()

        if input('\nsend_file? y/N\n> ') == 'y':
            __send_file(recipient)

        input('break infinit wait? y/N\n> ')
        q.stop()
    except KeyboardInterrupt:
        q.stop()


print(">>>>>>Launching apply")
main()
