import os
import sys

def main(opt):
    """
    Methodology:
        Static analysis first, with the following tools: cppcheck.
        Run the unit tests.
        Check memory with valgrind: memcheck, masif.
        Stack smash protector.
        For RIOT, emulate the target hardware (MCU) if available.
    """
    # Remove files from valgrind-output/
    cmd = "rm valgrin-output/*"
    os.system(cmd)
    cmd = "rm code-analysis/*"
    # Compilation and analysis for linux
    # Compile with testing
    if opt == "test":
        cmd = "make clean all DEBUG=1 LINUX=1 TEST=1"
        os.system(cmd)
    if opt == "bdd":
        cmd = "make clean all DEBUG=1 LINUX=1 BDD=1"
        os.system(cmd)
    # Static analysis
    cmd = "cppcheck . -D__LINUX__ --enable=all --includes-file=MAC/include --includes-file=MAC/include/testsinclude/ --includes-file=utils/include --includes-file=utils/include/testsinclude --includes-file=ipc-queues/include --includes-file=ipc-queues/include/testinclude --language=c --std=c11 --cppcheck-build-dir=code-analysis 2> code-analysis/cpp_output.txt"
    os.system(cmd)
    # Memory leaks check with valgrin
    cmd = "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-output/memcheck-output.txt bin/linux-x86_64/wmn-iot"
    os.system(cmd)
    # Valgrind with massif
    cmd = "valgrind --tool=massif --stacks=yes --massif-out-file=valgrind-output/massif-output.txt bin/linux-x86_64/wmn-iot"
    os.system(cmd)

    # Compilation and analysis for RIOT
    if opt == "test":
        cmd = "make clean all RIOT=1 TEST=1"
        os.system(cmd)
    if opt == "bdd":
        cmd = "make clean all RIOT=1 BDD=1"
        os.system(cmd)
    # Static code analysis
    cmd = "cppcheck . -D__RIOT__ --enable=all --includes-file=MAC/include --includes-file=MAC/include/testsinclude/ --includes-file=utils/include --includes-file=utils/include/testsinclude --includes-file=ipc-queues/include --includes-file=ipc-queues/include/testinclude --language=c --std=c11 --cppcheck-build-dir=code-analysis 2> code-analysis/cpp_output_riot.txt"
    os.system(cmd)
    # Compile codee for using with valgrind
    cmd = "make -B clean all-valgrind RIOT=1 TEST=1"
    os.system(cmd)
    # Memory leaks check with valgrind
    cmd = "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-output/memcheck-output-riot.txt --track-origins=yes --fullpath-after=/home/phantom/CP_Systems/RTOS/RIOT/ --read-var-info=yes /home/phantom/CP_Systems/Implementations/WMN-IoT/bin/native/stack_test.elf /dev/ttyACM0"
    os.system(cmd)
    # Valrgind with massif
    cmd = "valgrind --tool=massif --stacks=yes --massif-out-file=valgrind-output/massif-output-riot.txt --fullpath-after=/home/phantom/CP_Systems/RTOS/RIOT/ --read-var-info=yes /home/phantom/CP_Systems/Implementations/WMNIoT/bin/native/stack_test.elf /dev/ttyACM0"
    os.system(cmd)
    # Compile for the target board, flash, and execute
    cmd = "BOARD=esp32-wroom-32 make all flash term RIOT=1 TEST=1 PORT=/dev/ttyUSB0"
    os.system(cmd)

if __name__ == '__main__':
    argc = len(sys.argv)
    if argc != 2:
        print("Incorrect number of parameters.")
        print("Execute python3 simply_build.py opt")
        print("Where opt = test or bdd")
        exit()
    opt = sys.argv[2]
    main(opt)
