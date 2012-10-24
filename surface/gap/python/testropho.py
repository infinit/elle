import gap
import time
import datetime
import random
import math

state = gap.State()
#state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

state.login("test@infinit.io", "MEGABIET")

time.sleep(1)

state.connect()

time.sleep(1)

def BiteFunc(bite:'Bite'):
  print("Notification bite reçue", bite.debug)

def FileTransfer(transfer):
  print("User:", transfer.sender_id, "sent you:", transfer.file_name, "[", transfer.file_size, "]")

def FileTransferStatus(ts:'FileTransferStatus'):
  print("Transaction ", ts.transaction_id, "- status:", ts.status)

state.OnBite(BiteFunc)
state.OnFileTransfer(FileTransfer)
state.OnFileTransferStatus(FileTransferStatus)

now = datetime.datetime.now()
count1 = 0
count2 = 0

while True:
  time.sleep(0.05)
  count1 += (datetime.datetime.now() - now).microseconds
  count2 += (datetime.datetime.now() - now).microseconds
  if count1 > 50000:
    state.poll()
    count1 -= 50000
  if count2 > 2000000:
    state.ask_notif(math.floor(random.random() * 3))
    count2 -= 2000000
  now = datetime.datetime.now()
