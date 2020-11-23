from enums import *
from msg import *
from utils import *
import threading
import time

default_border = '================================='


def listenServer():
    while True:
        m = Message.SendMessage(M_BROKER, M_GETDATA)
        if m.Header.Type != M_NODATA:
            save_print(f'Message from client - {m.Header.From}:\n{m.Data}', default_border)
        time.sleep(2)


def connect():
    Message.SendMessage(M_BROKER, M_INIT)
    save_print(f'Your id is {Message.ClientID}', default_border)
    messages_thread = threading.Thread(target=listenServer, daemon=True)
    messages_thread.start()


def process():
    m = Message()
    connect()

    while (True):
        save_print('1. Send message \n2. Exit')
        answer = int(input())

        if answer == 1:
            save_print('Enter ID of client: ', None)
            m.Header.To = int(input())

            save_print('Enter your message: ', None)
            m.Data = input()

            Message.SendMessage(m.Header.To, M_DATA, m.Data)
            save_print('Message was send \n\n')
            continue
        if answer == 2:
            Message.SendMessage(M_BROKER, M_EXIT)
            save_print('Session ended\n\n')
            return
        save_print('Press 0 or1 \n\n')


if __name__ == '__main__':
    process()
