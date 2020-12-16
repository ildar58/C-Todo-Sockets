from threading import Lock

mutex = Lock()

def save_print(text, borders=''):
    mutex.acquire()
    if borders is not None:
        print(borders)
    print(text)
    if borders is not None:
        print(borders)
    mutex.release()


def connect(s):
    try:
        s.connect(('localhost', 12345))
    except ConnectionRefusedError:
        save_print('Ошибка подключения!')
        return False
    return True


