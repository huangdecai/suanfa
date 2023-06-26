import os
import sys
import time
import threading

import server
import client

if __name__ == '__main__':

    isServer =1# int(sys.argv[1])
    isDebug = True if sys.gettrace() else False
    if isDebug:
        isServer=0
    if isServer==1:
        server.start()
    else:
        client.start()

