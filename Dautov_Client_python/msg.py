from sys import getsizeof
from enums import *
import socket
from utils import *
import struct


class MsgHeader:
    def __init__(self, m_to, m_from, m_type, m_size):
        self.m_to = m_to
        self.m_from = m_from
        self.m_type = m_type
        self.m_size = m_size


class Message:
    m_client_id = 0

    def __init__(self, m_to=None, m_from=None, m_type=None, m_data=''):
        if m_to is None:
            self.m_header = MsgHeader(0, 0, 0, 0)
            self.m_data = m_data
        else:
            self.m_header = MsgHeader(m_to, m_from, m_type, getsizeof(m_data))
            self.m_data = m_data

    def send(self, s):
        s.send(struct.pack('iiii', self.m_header.m_to, self.m_header.m_from, self.m_header.m_type, self.m_header.m_size))
        if self.m_header.m_size:
            s.send(struct.pack(str(self.m_header.m_size)+'s', self.m_data.encode('cp1251')))

    def receive(self, s):
        self.m_header.m_to, self.m_header.m_from, self.m_header.m_type, self.m_header.m_size = struct.unpack('iiii',
                                                                                                             s.recv(
                                                                                                                 4 * 4))
        if not self.m_header:
            return MessageTypes.M_NODATA.value
        if self.m_header.m_size:
            self.m_data = s.recv(self.m_header.m_size + 1).decode('cp1251')[:self.m_header.m_size]

    @staticmethod
    def Send(m_to, m_type=MessageTypes.M_DATA.value, m_data=''):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if connect(s):
            m = Message(m_to, Message.m_client_id, m_type, m_data)
            m.send(s)
            m.receive(s)
            if m.m_header.m_type == MessageTypes.M_INIT.value:
                Message.m_client_id = m.m_header.m_to
            s.close()
        return m
