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
    # Remove files from valgrind-output/
    cmd = "rm valgrin-output/*"
    os.system(cmd)
    cmd = "rm code-analysis/*"
    # Compilation and analysis for linux
    # Compile with testing
    cmd = "make all DEBUG=1 LINUX=1 TEST=1"
    os.system(cmd)
    # Static analysis
    cmd = "cppcheck . -D__LINUX__ --enable=all --includes-file=MAC/include --includes-file=MAC/include/testsinclude/ --includes-file=utils/include --includes-file=utils/include/testsinclude --includes-file=ipc-queues/include --includes-file=ipc-queues/include/testinclude --language=c --std=c11 --cppcheck-build-dir=code-analysis 2> code-analysis/cpp_output.txt"
    os.system(cmd)
    # Memory leaks check with valgrin
    cmd = "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-output/memcheck-output.txt bin/linux-x86_64/wmnlora"
    os.system(cmd)
    # Valgrind with massif
    cmd = "valgrind --tool=massif --stacks=yes --massif-out-file=valgrind-output/massif-output.txt bin/linux-x86_64/wmnlora"
    os.system(cmd)

    # Compilation and analysis for RIOT
    cmd = "make all RIOT=1 TEST=1"
    os.system(cmd)
    # Static code analysis
    cmd = "cppcheck . -D__RIOT__ --enable=all --includes-file=MAC/include --includes-file=MAC/include/testsinclude/ --includes-file=utils/include --includes-file=utils/include/testsinclude --includes-file=ipc-queues/include --includes-file=ipc-queues/include/testinclude --language=c --std=c11 --cppcheck-build-dir=code-analysis 2> code-analysis/cpp_output_riot.txt"
    os.system(cmd)
    # Compile codee for using with valgrind
    cmd = "make -B clean all-valgrind RIOT=1 TEST=1"
    os.system(cmd)
    # Memory leaks check with valgrind
    cmd = "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-output/memcheck-output-riot.txt --track-origins=yes --fullpath-after=/home/phantom/CP_Systems/RTOS/RIOT/ --read-var-info=yes /home/phantom/CP_Systems/Implementations/WMNLoRa/bin/native/main_test.elf /dev/ttyACM0"
    os.system(cmd)
    # Valrgind with massif
    cmd = "valgrind --tool=massif --stacks=yes --massif-out-file=valgrind-output/massif-output-riot.txt --fullpath-after=/home/phantom/CP_Systems/RTOS/RIOT/ --read-var-info=yes /home/phantom/CP_Systems/Implementations/WMNLoRa/bin/native/main_test.elf /dev/ttyACM0"
    os.system(cmd)
    # Compile for the target board, flash, and execute
    cmd = "BOARD=esp32-wroom-32 make all flash term RIOT=1 TEST=1 PORT=/dev/ttyUSB0"
    os.system(cmd)

if __name__ == '__main__':
        main()
