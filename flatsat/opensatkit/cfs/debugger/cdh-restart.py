#!gdb
import sys
import gdb
import os

number_restarts = 0

def gdb_print(s):
    gdb.execute("printf \"" + s + "\"\n")

def on_stop(sig):
    global number_restarts
  
    if isinstance(sig, gdb.SignalEvent):
        gdb_print("CDH stopped with signal % s" % str(sig.stop_signal))
        crash_file = "crash.file." + str( gdb.selected_inferior().pid)
        gdb.execute("set logging file " + crash_file)
        gdb.execute("set logging on")
        gdb.execute("where")
        gdb.execute("set logging off")
        number_restarts += 1
        gdb_print("Number of restarts: %d" % number_restarts)
        gdb.execute("set confirm off")
        gdb.execute("load")
        gdb.execute("run")
        gdb.execute("set pagination off")
    else:
        gdb_print("Stop instance: %s" % str(sig))
        gdb_print("WARNING: Need to manually recover")

gdb.events.stop.connect (on_stop)
