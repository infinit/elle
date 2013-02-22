#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import threading
import argparse
import time
import sys
import os

from functools import partial
from getpass import getpass

"""

This script let you send files through infinit.

You have to specify your user name in the INFINIT_USER env variable.

This script blocks until the end of the transfer.

"""

class Worker(threading.Thread):
    def __init__(self, state):
        super(Worker, self).__init__()
        self.state = state
        self.running = True

    def run (self):
        while self.running:
            time.sleep(0.5)
            state.poll()

    def stop(self):
        self.running = False


def login(state, email = None):
    sender_id = os.getenv("INFINIT_USER", None)
    if sender_id == None:
        raise Exception("you must provide INFINIT_USER")

    password = getpass("password: ")
    state.login(sender_id, password)
    state.connect()

def on_finished(worker, state, transaction, new):
    if state.transaction_status(transaction) == state.TransactionStatus.finished:
        print("Stop the worker")
        worker.stop()

def main(state, user, filepath):
    login(state)
    worker = Worker(state)

    worker.start()

    id = state.send_files(user, [filepath,])
    state.transaction_status_callback(partial(on_finished, worker, state))

    while True:
        time.sleep(0.5)
        status = state.process_status(id)
        if status == state.ProcessStatus.running:
            print(".", end="")
        if status == state.ProcessStatus.success:
            print("Sucess")
            print("The transfert will now begin")
            break
        if status == state.ProcessStatus.failure:
            print("Failure D:")
            break

    worker.join()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("user", help="to user to who you want to send a file")
    parser.add_argument("file", help="path to the file")
    args = parser.parse_args()

    import gap
    state = gap.State()

    main(state, args.user, args.file)

    del state
