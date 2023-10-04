import os, sys
import socket
import shelve
import settings


# 创建一个socket服务端
def createserver(addr):
    # print('crate server',addr)
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(addr)
    server.listen()
    return server


# 创建一个socket客户端
def createclient(addr):
    # print('crate client',addr)
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(addr)
    return client
