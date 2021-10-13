import os

def main():
    """
    Methodology:
        Static analysis first, with the following tools: cppcheck.
        Run the unit tests.
        Check memory with valgrind: memcheck, masif.
        Stack smash protector.
        For RIOT, emulate the target hardware (MCU) if available.
    """
    # Compile with testing
    cmd = "make all DEBUG=1 LINUX=1 TEST=1"
    os.system(cmd)
    # Static analysis
    cmd = "cpcheck bin/wmnlora"
    os.system(cmd)
    # Memory check wiht valgrin
    cmd = "valgrind --tool=memcheck --leak-ckeck=yes bin/wmnlora"
    os.system(cmd)
    # Valgrind with massif
    cmd = "valgrin --tool=masif --stacks=yes bin/wmnlora"
    os.system(cmd)


    cmd = "BOARD=esp32-wroom-32 make all flash term PORT=/dev/ttyUSB0 RIOT=1"
    os.system(cmd)

if __name__ == '__main__':
        main()
