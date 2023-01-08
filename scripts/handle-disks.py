#!/bin/python3

import os, sys, stat
import subprocess
import re

verb, args = sys.argv[1], sys.argv[2:]

def run(str):
    for cmd in str.split("\n"):
        cmd = list(filter(('').__ne__, cmd.split(" ")))
        if len(cmd) > 0:
            subprocess.run(cmd)

def create_mode(name, *drives, bs=65536):
    drives_str = re.sub("\(|\)|'|,", "", str(drives))

    cmds = f"""
    wipefs -a {drives_str}
    pvcreate -a {drives_str}
    vgcreate vg_{name} {drives_str}
    lvcreate -n lv_{name} -l 100%FREE vg_{name}
    mkfs.xfs /dev/vg_{name}/lv_{name}
    """

    run(cmds)

def mount_and_own_mode(device, location):
    cmds = f"""
    umount {location}
    mount /dev/vg_{device}/lv_{device} {location}
    chown -R {os.getlogin()} {location}
    """
    
    run(cmds)

def delete_mode(name):
    cmds = f"""
    lvremove /dev/vg_{name}/lv_{name}
    vgremove vg_{name}
    """

    run(cmds)

for i in range(len(args)):
    if not '=' in args[i]:
        args[i] = "'" + args[i] + "'"

if verb == "create":
    eval("create_mode(" + ", ".join(args) + ")")
elif verb == "delete":
    eval("delete_mode("+args[0]+")")
elif verb == "mount":
    eval(f"mount_and_own_mode({args[0]}, {args[1]})")