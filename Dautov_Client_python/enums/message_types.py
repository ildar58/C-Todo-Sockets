from enum import Enum


class MessageTypes(Enum):
    M_INIT = 0
    M_EXIT = 1
    M_GETDATA = 2
    M_NODATA = 3
    M_DATA = 4
    M_CONFIRM = 5
