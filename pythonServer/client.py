import binascii
import os, sys
import socket
import select
import time
import traceback
from ctypes import *
import json
from multiprocessing import Process, Queue
import threading
import struct

# coding: utf-8
import socket

def start():
    messages = ['This is the message ', 'It will be sent ', 'in parts ', ]

    server_address = ('localhost', 8090)

    # Create aTCP/IP socket

    socks = [socket.socket(socket.AF_INET, socket.SOCK_STREAM), socket.socket(socket.AF_INET, socket.SOCK_STREAM), ]

    # Connect thesocket to the port where the server is listening

    print ('connecting to %s port %s' % server_address)
    # 连接到服务器
    for s in socks:
      s.connect(server_address)

    for index, message in enumerate(messages):
      # Send messages on both sockets
      for s in socks:
        print ('%s: sending "%s"' % (s.getsockname(), message + str(index)))
        s.send(bytes(message + str(index),encoding='utf-8'))
      # Read responses on both sockets

    for s in socks:
      data = s.recv(1024)
      print ('%s: received "%s"' % (s.getsockname(), data))
      if data != "":
        print ('closingsocket', s.getsockname())
        s.close()


