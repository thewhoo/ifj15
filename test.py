#!/usr/bin/env python

import os
import subprocess


def print_test_head(source):
    print("-------------------------------")
    print("TEST {}: {}".format(x, source))


def check_ret_code(returned, expected):
    if returned == int(expected):
        print("Return value: OK")
        return True
    else:
        print("Expected return code: {}\nProgram returned: {}"
              .format(expected, returned))
        return False


def check_stdout(out):
    if out.strip():
        print("UNexpected output: "+out)
        print(out)
        return False
    return True


def check_stderr(err):
    if 'IFJ15' not in err:
        print("UNexpected error output: "+err)
        return False
    return True

def print_statistics():
    print("-------------------------------")
    print("Number of tests: {}\nSuccess tests: {}\nFailed: {}\n"
          .format(x, x_ok, x_fail))

    if x_fail != 0:
        print("Failed tests:")
        for k, v in fails.items():
            print("TEST {}: {}".format(k,v))

tests_dir = os.path.join(os.getcwd(), 'tests')

x = 0
x_ok = 0
x_fail = 0
fails = {}

print("\nIFJ15: Tests containing some error.\n")

for source in [f for f in os.listdir(tests_dir)
              if os.path.isfile(os.path.join(tests_dir, f))]:

    x = x + 1
    source_path = os.path.join(tests_dir, source)
    proc = subprocess.Popen(["./ifj", source_path],
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    out, err = proc.communicate()
    ret_code = proc.returncode
    
    print_test_head(source)
    c_ret = check_ret_code(ret_code, source[-1])
    c_out = check_stdout(out.decode('utf-8'))
    c_err = check_stderr(err.decode('utf-8'))

    if c_ret and c_out and c_err:
        x_ok = x_ok + 1
    else:
        x_fail = x_fail + 1
        fails[x] = source

print_statistics()
