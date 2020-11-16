// Dautov_Client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "Dautov_Client.h"
#include "Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool connection = true;
mutex hMutex;

enum Answers {
    MESSAGE = 1,
    EXIT    = 2
};

void listenServer() {
    Message m;
    while (connection) {
        m = Message::Send(M_BROKER, M_GETDATA);
        if (m.m_Header.m_Type != M_NODATA)
        {
            hMutex.lock();
            cout << "===============================================" << endl;;
            cout << "Message from client - " << m.m_Header.m_From << ":" << endl;
            cout << m.m_Data << endl;
            cout << "===============================================" << endl;
            hMutex.unlock();
        }

        Sleep(2000);
    }
}

void Connect()
{
    Message::Send(M_BROKER, M_INIT);
    hMutex.lock();
    cout << "Your ID is " << Message::m_ClientID << endl;
    hMutex.unlock();
    thread t(listenServer);
    t.detach();
}

void Process() {
    Message m;
    unsigned int answer;

    Connect();
    
    while (true)
    {
        hMutex.lock();
        cout << "1. Send message \n2. Exit" << endl;
        hMutex.unlock();
        cin >> answer;

        switch (answer)
        {
        case MESSAGE: 
        {
            hMutex.lock();
            cout << "Enter ID of client: ";
            cin >> m.m_Header.m_To;

            cout << "Enter your message: ";
            cin >> m.m_Data;

            Message::Send(m.m_Header.m_To, M_DATA, m.m_Data);
            cout << "Message was send\n\n";
            hMutex.unlock();
            break;
        }
        case EXIT:
        {
            Message::Send(M_BROKER, M_EXIT);
            connection = false;
            hMutex.lock();
            cout << "Session ended\n\n";
            hMutex.unlock();
            return;
        }
        default:
            hMutex.lock();
            cout << "Press 0 or 1\n\n" << endl;
            hMutex.unlock();
        }
    }
}


// Единственный объект приложения

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            AfxSocketInit();
            Process();
            // TODO: вставьте сюда код для приложения.
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
