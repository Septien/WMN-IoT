"""
In this script, only compilation for linux and native riot is completed. No analysis
or other tests are make here.
"""

import os
import sys

def main(opt):
    # clean the object files
    cmd = "make clean LINUX=1 TEST=1"
    os.system(cmd)
    cmd = "make clean RIOT=1 TEST=1"
    os.system(cmd)

    if opt == "test":
        cmd = "make all DEBUG=1 LINUX=1 TEST=1"
        os.system(cmd)
        cmd = "make all term RIOT=1 TEST=1"
        os.system(cmd)
        print("\n\nTesting for LINUX\n\n")
        cmd = "bin/linux-x86_64/wmn-iot"
        os.system(cmd)
    
    if opt == "bdd":
        cmd = "make all DEBUG=1 LINUX=1 BDD=1"
        os.system(cmd)
        cmd = "make all term RIOT=1 BDD=1"
        os.system(cmd)
        cmd = "bin/linux-x86_64/wmn-iot"
        os.system(cmd)

if __name__ == '__main__':
    argc = len(sys.argv)
    if argc != 2:
        print("Incorrect number of parameters.")
        print("Execute python3 simply_build.py opt")
        print("Where opt = test | bdd")
        exit()
    opt = sys.argv[1]
    main(opt)
