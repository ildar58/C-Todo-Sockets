from enums import *
from msg import *
from utils import *
import utils
import threading
import time

default_border = '================================='


def listenServer():
    m = Message()
    while utils.connection:
        m = Message.Send(MemberTypes.M_BROKER.value, MessageTypes.M_GETDATA.value)
        if m.m_header.m_type != MessageTypes.M_NODATA.value:
            save_print(f'Message from client - {m.m_header.m_from}:\n{m.m_data}', default_border)
        time.sleep(3)


def connect():
    Message.Send(MemberTypes.M_BROKER.value, MessageTypes.M_INIT.value)
    save_print(f'Your id is {Message.m_client_id}', default_border)
    messages_thread = threading.Thread(target=listenServer, daemon=True)
    messages_thread.start()


def process():
    m = Message()
    connect()

    while (True):
        save_print('1. Send message \n2. Exit')
        answer = int(input())

        if answer == Answers.MESSAGE.value:
            save_print('Enter ID of client: ', None)
            m.m_header.m_to = int(input())

            save_print('Enter your message: ', None)
            m.m_data = input()

            Message.Send(m.m_header.m_to, MessageTypes.M_DATA.value, m.m_data)
            save_print('Message was send \n\n')
            continue
        if answer == Answers.EXIT.value:
            Message.Send(MemberTypes.M_BROKER.value, MessageTypes.M_EXIT.value)
            utils.connection = False
            save_print('Session ended\n\n')
            return
        save_print('Press 0 or1 \n\n')


if __name__ == '__main__':
    process()
