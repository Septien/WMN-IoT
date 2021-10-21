"""
In this script, only compilation for linux and native riot is completed. No analysis
or other tests are make here.
"""

import os

def main():
    cmd = "make clean LINUX=1 TEST=1"
    os.system(cmd)
    
    cmd = "make all DEBUG=1 LINUX=1 TEST=1"
    print(cmd)
    os.system(cmd)
    
    cmd = "make clean RIOT=1 TEST=1"
    print(cmd)
    os.system(cmd)

    cmd = "make all term RIOT=1 TEST=1"
    print(cmd)
    os.system(cmd)
    
    print("\n\nTesting for LINUX\n\n")
    cmd = "bin/linux-x86_64/wmnlora"
    print(cmd)
    os.system(cmd)

if __name__ == '__main__':
    main()
