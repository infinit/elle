import gap
import time

state = gap.State()
#state.register("Testing Program", "test@infinit.io", "MEGABIET", "pretty name", "bitebite")

state.login("test@infinit.io", "MEGABIET")

time.sleep(1)

state.connect()

time.sleep(1)

def BiteFunc(bite:'Bite'):
  print("bite")
  print(bite.debug)

state.bind_bite_notification(BiteFunc)

while True:
  time.sleep(1)
  state.poll()
