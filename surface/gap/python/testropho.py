import gap
import time
import datetime
import random
import math
import threading

state = gap.State()

def BiteFunc(bite:'Bite'):
    print("Notification bite reçue", bite.debug)

def FileTransfer(transfer):
    print("User:", transfer.sender_id, "sent you:", transfer.file_name, "[", transfer.file_size, "]")
    answer = int(input("\nAccept incomming file ?\n> "))
    state.answer_transaction(transfer.transaction_id, answer)

def FileTransferStatus(ts:'FileTransferStatus'):
    print("Transaction ", ts.network_id, "- status:", ts.status)

def Message(message):
    print(message.sender_id, ":", message.message)

class PollThread (threading.Thread):
    now = datetime.datetime.now()
    count = 0
    bite = False

    def run (self) :
        self.bite = False
        while True:
            time.sleep(0.05)
            self.count += (datetime.datetime.now() - self.now).microseconds
            if self.count > 1000000:
                state.poll()
                self.count -= 1000000
            self.now = datetime.datetime.now()
            if self.bite == True:
                return

    def stop(self):
        self.bite = True

state.OnBite(BiteFunc)
state.OnFileTransfer(FileTransfer)
state.OnFileTransferStatus(FileTransferStatus)
state.OnMessage(Message)

#register: fullname, email, hashed_password, device_name, activation_code
def __register(name, mail):
    state.register(name, mail, "MEGABIET", "pretty name", "bitebite")

def __login(mail):
    state.login(mail, "MEGABIET")

def __connect():
    state.connect()

def __send_file(mail):
    files = ["/home/dimrok/test/main.cpp", "/home/dimrok/test/main2.cpp", "/home/dimrok/test/main3.cpp"]
    state.send_files(mail, files)

def __create_network():
    str = "FistParty%s" % math.floor(random.random()*234567890);
    state.create_network(str)

def __launch_watchdog():
    state.set_device_name("pretty name")
    state.launch_watchdog()

def main():
    try:
        m = input('\n[i]nfinit or [g]mail?\n> ')

        if (m != 'i') and (m != 'g'):
            return

        r = (input('\n[r]egister or [l]ogin?\n> '))

        if (r != 'r') and (r != 'l'):
            return

        q = PollThread()
        q.start()

        if m == 'i':
            mail = "antony.mechin@infinit.io"
            recipient = "antony.mechin@gmail.com"
        elif m == 'g':
            mail = "antony.mechin@gmail.com"
            recipient = "antony.mechin@infinit.io"

        if r == 'r':
            __register("Testing Program", mail)
        elif r == 'l':
            __login(mail)

        state.set_device_name("castor")

        __connect()

        # state.answer_transaction('bite', 1)
        # state.answer_transaction('bite', 0)


        if input('\nwatchdog? y/N\n> ') == 'y':
            __launch_watchdog()

        if input('\nsend_file? y/N\n> ') == 'y':
            __send_file(recipient)

        time.sleep(100000)
        input('break infinit wait? y/N\n> ')

        q.stop()
    except KeyboardInterrupt:
        q.stop()


print(">>>>>>Launching apply")
main()

# finally:
#     print("<<<<<<Closing watchdog")
#     state.stop_watchdog()

##########################################



# # state.send_file("juaodjaozj", "/home/dimrok/test")
# # state.send_file("jafoijaz", "/home/dimrok/test/main.cpp");
# # state.send_file("kjladkaj", "poijaoieazupihp");

# # now = datetime.datetime.now()
# # count1 = 0
# # count2 = 0

# # while True:
# #     time.sleep(0.05)
# #     count1 += (datetime.datetime.now() - now).microseconds
# #     count2 += (datetime.datetime.now() - now).microseconds
# #     if count1 > 50000:
# #         state.poll()
# #         count1 -= 50000
# #     if count2 > 2000000:
# #         state.ask_notif(math.floor(random.random() * 3))
# #         count2 -= 2000000
# #     now = datetime.datetime.now()
