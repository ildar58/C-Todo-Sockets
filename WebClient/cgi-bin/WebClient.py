# -*- coding: utf-8 -*-

import os, sys, re, codecs, binascii, cgi, cgitb, datetime, pickle
import http.cookies
from msg import *

cgitb.enable()
sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())


class Messenger:
    id_input = ''
    message_input = ''
    messages = []

    def __init__(self, q):
        self.q = q
        Message.ClientID = int(q.getvalue('ClientID', 0))
        if Message.ClientID == 0:
            Message.SendMessage(M_BROKER, M_INIT)
        else:
            self.MsgGet()
        self.message_input = ''

    def MsgSend(self):
        Message.SendMessage(int(self.q.getvalue('user-id')), M_DATA, self.q.getvalue('message'))
        self.message_input = ''
        self.id_input = ''

    def MsgGet(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(('localhost', 12345))
            m = Message(Message.ClientID, M_BROKER, M_GET_ALL_DATA)
            m.Send(s)
            m.Receive(s)
            if (m.Header.Type != M_NODATA):
                messages_len = int(m.Data)
                i = 0
                while i < messages_len:
                    m.Receive(s);
                    self.messages.append((m.Header.From, m.Data))
                    i += 1
#################################################

    def printPage(self):
        print(f"""Content-type: text/html; charset=utf-8

<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>Messenger</title>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css" integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2" crossorigin="anonymous">
</head>
<body>
<div class="container" style="height: calc(100vh - 40px); margin-top: 20px;">
    <div class="row">
        <div class="col">
            <form class="needs-validation" action=/cgi-bin/WebClient.py name=msgform novalidate>
                <div class="card">
                    <div class="card-header">
                        Ваш ID: {Message.ClientID}
                    </div>
                </div>
                <div class="form-group">
                    <label for="inputMessage">Сообщение</label>
                    <input type="text" name="message" class="form-control" id="inputMessage" value={Messenger.message_input} required>
                    <div class="invalid-feedback">
                        Введите сообщение!
                    </div>
                </div>
                <div class="form-group">
                    <label for="inputId">Id адресата</label>
                    <input type="number" name="user-id" class="form-control" id="inputId" value="{Messenger.id_input}" required> 
                    <div class="invalid-feedback">
                        Введите id адресата!
                    </div>
                </div>
                <input type=hidden name=ClientID value="{Message.ClientID}">
                <input type=hidden name=type value="send">
                <button type=submit class="btn btn-primary">Отправить</button>
                <button class="btn btn-primary" onclick="document.forms.msgform.type.value='get'; document.forms.msgform.submit();">Загрузить</button>
            </form>
        </div>
        <div class="vl" style="border-left: 1px solid grey; height: calc(100vh - 40px);"></div>
        <div class="col">
            <div class="overflow-auto" style="height: 500px" id="messages">
""")
        for element in self.messages:
            self.printCard(element[0], element[1])
        print(f"""
</div>
        </div>
    </div>
</div>
<script src="https://code.jquery.com/jquery-3.5.1.slim.min.js" integrity="sha384-DfXdz2htPH0lsSSs5nCTpuj/zy4C+OGpamoFVy38MVBnE+IbbVYUew+OrCXaRkfj" crossorigin="anonymous"></script>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-ho+j7jyWK8fNQe+A12Hb8AhRq26LrZ/JpcUGGOn+Y7RsweNrtN/tE3MoK7ZeZDyx" crossorigin="anonymous"></script>
<script src="scripts.js"></script>
</body>
</html>
""")

###########################################

    def printCard(self, user_id, text):
        print(f"""
                <div class="card" style="margin-bottom: 10px">
                    <div class="card-header">
                        Сообщение от {user_id}
                    </div>
                    <div class="card-body">
                        <p class="card-text">{text}</p>
                    </div>
                </div>
""")


def main():
    q = cgi.FieldStorage()
    m = Messenger(q)

    MENU = {
        'send': m.MsgSend,
        'get': m.MsgGet,
    }

    try:
        MENU[q.getvalue('type')]()
    except Exception as e:
        pass

    m.printPage()

main()
