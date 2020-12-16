# -*- coding: utf-8 -*-
import json
import os, sys, re, codecs, binascii, cgi, cgitb, datetime, pickle
import http.cookies
from msg import *

cgitb.enable()
sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())


def PrintJson(json_data):
    print("Content-type: application-json; charset=utf-8\n\n")
    print(json.dumps(json_data))


class Messenger:
    def __init__(self, q):
        self.q = q

    def MsgInit(self):
        Message.SendMessage(M_BROKER, M_INIT)
        json_data = {'result': Message.ClientID}
        PrintJson(json_data)

    def MsgPost(self):
        c_id = int(self.q.getvalue('id'))
        message = self.q.getvalue('message')
        Message.SendMessage(c_id, M_DATA, message)
        json_data = {'result': True}
        PrintJson(json_data)

    def MsgGet(self):
        m = Message.SendMessage(M_BROKER, M_GETDATA)
        if m.Header.Type != M_NODATA:
            json_data = {
                'result': {
                    'id': m.Header.From,
                    'message': m.Data
                }
            }
        else:
            json_data = {'result': None}
        PrintJson(json_data)

    def MsgGetAll(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            messages = []
            s.connect(('localhost', 12345))
            m = Message(Message.ClientID, M_BROKER, M_GET_ALL_DATA)
            m.Send(s)
            m.Receive(s)
            if (m.Header.Type != M_NODATA):
                messages_len = int(m.Data)
                i = 0
                while i < messages_len:
                    m.Receive(s)
                    messages.append((m.Header.From, m.Data))
                    i += 1
        json_data = {'result': messages}
        PrintJson(json_data)

    def MsgExit(self):
        Message.SendMessage(M_BROKER, M_EXIT)
        json_data = {'result': True}
        PrintJson(json_data)


def main():
    q = cgi.FieldStorage()
    if (q.getvalue('clientId') != None):
        Message.ClientID = int(q.getvalue('clientId'))
    m = Messenger(q)

    MENU = {
        'init': m.MsgInit,
        'exit': m.MsgExit,
        'post': m.MsgPost,
        'get': m.MsgGet,
        'getAll': m.MsgGetAll
    }

    try:
        MENU[q.getvalue('type')]()
    except Exception as e:
        pass


main()
