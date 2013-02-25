#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from functools import partial

import threading
import argparse
import time
import os

from getpass import getpass

"""

This script let you recv file from infinit.

You have to specify your user name in the INFINIT_USER env variable.

"""

def auto_accept(state, transaction, new):
    state.update_transaction(transaction._id, state.TransactionStatus.accepted)

def on_canceled(state, transaction, new):
    if state.transaction_status(transaction) == state.TransactionStatus.canceled:
        print("Transaction canceled, check your log")
        state.running = False

def on_finished(state, transaction, new):
    if state.transaction_status(transaction) == state.TransactionStatus.finished:
        print("Transaction succeeded")
        cnt = state.transaction_files_count(transaction)
        if cnt == 1:
            filename = state.transaction_first_filename(transaction)
            print("File received at '{}'".format(
                os.path.join(state.get_output_dir(), filename)))
        else:
            print("{} files received in '{}'".format(
                cnt, state.get_output_dir()))
        state.running = False

def login(state, email = None):
    receiver_id = os.getenv("INFINIT_USER", None)
    if receiver_id == None:
        raise Exception("you must provide INFINIT_USER")
    password = getpass("password: ")
    state.login(receiver_id, password)
    state.connect()
    return receiver_id

def select_transactions(state, l_transactions, sender):
    if sender is not None:
        l_sender_id = state.search_users(sender)
        if not l_sender_id:
            raise Exception("no such user")
        if len(l_sender_id) > 1:
            raise Exception("ambiguous sender name")
        sender_id = l_sender_id[0]

    # select pending transactions matching the sender id (if not None)
    enumeration = list(enumerate(
            l for l in l_transactions
            if state.transaction_status(l) == state.TransactionStatus.pending
            and (sender is None or state.transaction_sender_id(l) == sender_id)
    ))

    if len(enumeration) == 1:
        # if there is only one match, then return the id now
        return (enumeration[0][1],)

    # ask for user input
    for index, t in enumeration:
        first_filename  = state.transaction_first_filename(t)
        fullname        = state.transaction_sender_fullname(t)
        file_number     = state.transaction_files_count(t)
        if file_number > 1:
            print("[{}] {} files from {}".format(index, file_number, fullname))
        else:
            print("[{}] {} from {}".format(index, first_filename, fullname))

    selected = input("transaction numbers [all]> ")
    if selected:
        if isinstance(selected, (list, tuple)):
            l_selected = selected
        elif isinstance(selected, (str, bytes)):
            try:
                l_selected = (int(selected),)
            except ValueError as e:
                print(e)
                return []
        elif isinstance(selected, int):
            l_selected = (selected,)
    else:
        l_selected = []

    if l_selected:
        return (enumeration[int(i)][1] for i in l_selected)
    else:
        return l_transactions
    return []


def main(state, sender):
    id = login(state)

    state.transaction_callback(partial(auto_accept, state));
    state.transaction_status_callback(partial(on_finished, state))
    state.transaction_status_callback(partial(on_canceled, state))

    state.running = True
    state.set_device_name(id + "device")

    transactions = state.transactions()

    if len(transactions) > 1:
        to_handle = select_transactions(state, transactions, sender)
    else:
        to_handle = transactions

    print(to_handle)

    if not to_handle:
        raise Exception("you must select a transaction to accept")

    for t_ids in to_handle:
        state.update_transaction(t_ids, state.TransactionStatus.accepted)

    while state.running:
        time.sleep(0.5)
        state.poll()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--sender", help="the user from whom you want to recv the file")
    parser.add_argument("-d", "--download_dir", help="the download dir")
    args = parser.parse_args()

    if args.download_dir:
        os.putenv("INFINIT_DOWNLOAD_DIR", args.download_dir)

    import gap
    state = gap.State()


    main(state, args.sender)

    del state
