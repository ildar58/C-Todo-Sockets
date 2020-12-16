from msg import *
from utils import *
import threading
import time
from api import *

default_border = '================================='
server_api = Api()


def listenServer():
    while True:
        m = server_api.Get()
        if m['result'] != None:
            save_print(f'Message from client - {m["result"]["id"]}:\n{m["result"]["message"]}', default_border)
        time.sleep(2)


def connect():
    server_api.Init()
    save_print(f'Your id is {server_api.clientId}', default_border)
    messages_thread = threading.Thread(target=listenServer, daemon=True)
    messages_thread.start()


def process():
    connect()

    while (True):
        save_print('1. Send message \n2.Get All Messages \n3. Exit')
        answer = int(input())

        if answer == 1:
            save_print('Enter ID of client: ', None)
            c_id = int(input())

            save_print('Enter your message: ', None)
            message = input()

            server_api.Post(c_id, message)
            save_print('Message was send \n\n')
            continue
        if answer == 2:
            messages = server_api.GetAll()['result']
            for m in messages:
                save_print(f'Message from client - {m[0]}:\n{m[1]}', default_border)
            continue
        if answer == 3:
            server_api.Exit()
            save_print('Session ended\n\n')
            return
        save_print('Press 0 or1 \n\n')


if __name__ == '__main__':
    process()
