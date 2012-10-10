#!/usr/bin/env python3
# -*- python -*-

import os
import re
import sys
import stat
import copy
import time
import email
import runpy
import pprint
import argparse
import datetime
import threading
import email.mime
import smtplib as sml
import subprocess as sp
import pygooglechart as pgc

from higgs import pool
from higgs import infinitools
from collections import namedtuple
from collections import OrderedDict

MAIL_TARGETS = (
    "developers@infinit.io",
    "raphael.londeix@infinit.io",
    "fabien.pichot@infinit.io",
    "antony.mechin@infinit.io",
    "quentin.hocquet@infinit.io",
    "julien.quitard@infinit.io",
    "charles.guillot@infinit.io",
)

MANDRILL_USERNAME = 'infinitdotio'
MANDRILL_PASSWORD = 'ca159fe5-a0f7-47eb-b9e1-2a8f03b9da86'
MANDRILL_SMTP_HOST = 'smtp.mandrillapp.com'
MANDRILL_SMTP_PORT = 587

CHART_W_SIZE = 400
CHART_H_SIZE = 250

BASE = os.environ.get(
    'DIR_SOURCE',
    os.path.join(
        os.path.abspath(os.path.dirname(__file__)),
        '../../source/'
    )
)
mail_file = os.path.join(BASE, "farm", "mail_template.in")

with open(mail_file) as template:
    mail_template = template.read()

class StatusCode():
    SUCCESS = 0
    FAILURE = 1
    def __init__(self):
        self.status = StatusCode.FAILURE
        pass


class Watcher:
    def pre_run(self, environ={}): pass

    def post_run(self, res): pass

    def post_shutdown(self, all_reports): pass

class Timing(Watcher):
    def __init__(self, script_name, report):
        report[script_name]["timing"] = OrderedDict()
        self.timing_report = report[script_name]["timing"]
        pass

    def pre_run(self, environ={}):
        self.t0 = datetime.datetime.now()

    def post_run(self, status_code):
        t0 = self.t0
        t1 = datetime.datetime.now()

        self.timing_report["start time"] = str(t0)
        self.timing_report["end time"] = str(t1)
        self.timing_report["elapsed time"] = \
                "{0} sec".format((t1 - t0).total_seconds())

class Environ(Watcher):
    def __init__(self, script_name, report):
        report[script_name]["environ"] = OrderedDict()
        self.env_report = report[script_name]["environ"]

    def pre_run(self, environ={}):

        for key, val in environ.items():
            if key.startswith("MNT"):
                self.env_report[key] = val
            elif key.startswith("INFINIT"):
                self.env_report[key] = val

class Result(Watcher):
    def __init__(self, script_name, report):
        self.result_report = report[script_name]

    def post_run(self, status_code):
        if status_code.status == StatusCode.SUCCESS:
            self.result_report["result"] = "SUCCESS"
        elif status_code.status == StatusCode.FAILURE:
            self.result_report["result"] = "FAILURE"

class Memory(Watcher):

    def __init__(self, script_name, report):
        report[script_name]["memory"] = OrderedDict()
        self.memory_report = report[script_name]["memory"]

    def _watch(self, pid, curr_stat):
        cmd = "ps -o %cpu,%mem,rss,bsdtime,maj_flt,min_flt,vsz {0}";

        out = sp.check_output(cmd.format(pid).split())
        (header_val, data, *drop) = out.split(b'\n')
        for (type, val) in zip(header_val.split(), data.split()):
            if val not in curr_stat:
                dec_type = type.decode("utf8")
                dec_val = val.decode("utf8")
                curr_stat[dec_type] = []
                curr_stat[dec_type].append(dec_val)

        while True:
            try:
                out = sp.check_output(cmd.format(pid).split())
            except sp.CalledProcessError as e:
                break

            time.sleep(0.2)
            (header_val, data, *dump) = out.split(b'\n')
            for (type, val) in zip(header_val.split(), data.split()):
                dec_type = type.decode("utf8")
                dec_val = val.decode("utf8")
                curr_stat[dec_type].append(dec_val)

    def pre_run(self, environ={}):
        self.l_pids = []
        self.l_threads = []
        for (key, value) in environ.items():
            if key.startswith("INFINIT_PID"):
                self.l_pids.append(value)
                for pid in value.split(","):
                    pid_key = "pid({0})".format(pid)
                    self.memory_report[pid_key] = OrderedDict()
                    T = threading.Thread(target=self._watch,
                            args=(value,self.memory_report[pid_key]))
                    T.start()
                    self.l_threads.append(T)

    def post_shutdown(self, drop):
        for T in self.l_threads:
            T.join()

class Graph(Watcher):
    def __init__(self, script_name, report):
        self._report = report;
        self.script_name = script_name

    def render_memory_graphs(self):
        memory_report = self._report[self.script_name]["memory"]
        for pid, pid_stats in memory_report.items():
            for type, stats in pid_stats.items():
                if type == b'%CPU':
                    l_floatvals = [float(value) for value in stats]
                    chart = pgc.SimpleLineChart(CHART_W_SIZE,
                            CHART_H_SIZE,
                            y_range=(0, 100))
                    chart.set_axis_labels(pgc.Axis.LEFT, [0, 25, 50, 75, 100])
                    chart.set_axis_labels(pgc.Axis.BOTTOM, ["start", "end"])
                    chart.set_title("%CPU over time for {0}".format(
                        pid))
                    chart.set_legend([self.script_name, "bite" ,"botte"])
                    chart.add_data(l_floatvals)
                    chart.add_data(l_floatvals)
                    chart.add_data(l_floatvals)

    def post_shutdown(self, drop):
        self.render_memory_graphs()

class Mail(Watcher):
    def __init__(self, script_name="", report={}):
        self.smtp = sml.SMTP(MANDRILL_SMTP_HOST, MANDRILL_SMTP_PORT)

    def final(self, rep):
        if all(test["result"] == "SUCCESS" for test in rep["scripts"].values()):
            return "SUCCESS"
        else:
            return "FAILURE"


    def send_mail(self, all_reports, _enc="utf8"):
        from email.mime.multipart import MIMEMultipart
        from email.mime.text import MIMEText
        from email.header import Header
        from higgs import moody
        html = mail_template
        html = moody.render(html, **all_reports)

        s = sp.check_output("git rev-parse --short HEAD".split(), cwd=os.environ["DIR_SOURCE"])
        sub = s.decode("utf8").strip()
        full_subject = "[INFINIT] [build-farm] Test report (#{0}): {1}".format(sub, self.final(all_reports))

        msg = MIMEText(html, 'html', _charset=_enc)
        msg['Subject'] = Header(full_subject, _enc)
        msg['From'] = "admin@infinit.io"
        msg['To'] = ", ".join(MAIL_TARGETS)

        print("send mail to:", MAIL_TARGETS)
        try:
            self.smtp.login(MANDRILL_USERNAME, MANDRILL_PASSWORD)
            #self.smtp.sendmail(msg['From'], [msg['To']], msg.as_string())
            self.smtp.send_message(msg)
        finally:
            self.smtp.quit()

    def post_shutdown(self, all_reports):
        self.send_mail(all_reports)

