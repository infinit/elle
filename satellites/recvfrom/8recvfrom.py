#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from functools import partial

import threading
import argparse
import time
import os

from getpass import getpass

running = True

def auto_accept(state, transaction, new):
    state.update_transaction(transaction._id, state.TransactionStatus.accepted)

def on_finished(state, transaction, new):
    if state.transaction_status(transaction) == state.TransactionStatus.finished:
        print("Stop the worker")
        globals()["running"] = False

def login(state, email = None):
    sender_id = os.getenv("INFINIT_USER", None)
    password = getpass("password: ")
    state.login(sender_id, password)
    state.connect()

def main(state, user):
    login(state)

    state.transaction_callback(partial(auto_accept, state));
    state.transaction_status_callback(partial(on_finished, state))

    global running

    state.set_device_name("bibibiasdfadf")

    transactions = state.transactions()

    for t_ids in transactions:
        state.update_transaction(t_ids, state.TransactionStatus.accepted)

    while running:
        print(running)
        time.sleep(0.5)
        state.poll()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("user", help="the user from whom you want to recv the file")
    parser.add_argument("download_dir", help="the download dir")
    args = parser.parse_args()

    import gap
    state = gap.State()

    main(state, args.user)

    del state
