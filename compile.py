import os

def main():
    cmd = "make DEBUG=1 LINUX=1"
    os.system(cmd)

    cmd = "BOARD=esp32-wroom-32 make all flash term PORT=/dev/ttyUSB0 RIOT=1"
    os.system(cmd)

if __name__ == '__main__':
        main()
