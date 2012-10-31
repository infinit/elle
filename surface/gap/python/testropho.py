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

def FileTransferStatus(ts:'FileTransferStatus'):
  print("Transaction ", ts.transaction_id, "- status:", ts.status)

def Message(message):
  print(message.sender_id, ":", message.message)

class PollThread (threading.Thread):
  now = datetime.datetime.now()
  count = 0
  def run (self) :
    while True:
      time.sleep(0.05)
      self.count += (datetime.datetime.now() - self.now).microseconds
      if self.count > 1000000:
        state.poll()
        self.count -= 1000000
      self.now = datetime.datetime.now()

state.OnBite(BiteFunc)
state.OnFileTransfer(FileTransfer)
state.OnFileTransferStatus(FileTransferStatus)
state.OnMessage(Message)

def main():
  #register: fullname, email, hashed_password, device_name, activation_code
  state.register("", "", "", "", "")

  state.register("", "test@infinit.io", "", "pretty name", "bitebite")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "")

  state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")



  # state.login("test@infinit.io", "MEGABIET")

  # state.connect()

  # state.set_device_name("Bite")

  # str = "FistParty%s" % math.floor(random.random()*234567890);
  # state.create_network(str)

  # state.launch_watchdog()

#  PollThread().start()

print(">>>>>>Launching apply")
main()

# finally:
#   print("<<<<<<Closing watchdog")
#   state.stop_watchdog()

##########################################



# # state.send_file("juaodjaozj", "/home/dimrok/test")
# # state.send_file("jafoijaz", "/home/dimrok/test/main.cpp");
# # state.send_file("kjladkaj", "poijaoieazupihp");

# # now = datetime.datetime.now()
# # count1 = 0
# # count2 = 0

# # while True:
# #   time.sleep(0.05)
# #   count1 += (datetime.datetime.now() - now).microseconds
# #   count2 += (datetime.datetime.now() - now).microseconds
# #   if count1 > 50000:
# #     state.poll()
# #     count1 -= 50000
# #   if count2 > 2000000:
# #     state.ask_notif(math.floor(random.random() * 3))
# #     count2 -= 2000000
# #   now = datetime.datetime.now()
