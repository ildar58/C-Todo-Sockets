// Dautov_Storage_Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "Dautov_Storage_Server.h"
#include "Msg.h"
#include "Session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

map<int, shared_ptr<Session>> gSessions;


void ProcessClient(SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);

    Message m;
    int nCode = m.Receive(s);
    cout << m.m_Header.m_From << ": " << nCode << endl;

    switch (nCode)
    {
    case M_INIT:
    {
        auto pSession = make_shared<Session>(m.m_Header.m_From);
        gSessions[pSession->m_ID] = pSession;
        Message::Send(M_CONFIRM);
        break;
    }
    case M_EXIT:
    {
        gSessions.erase(m.m_Header.m_To);
        Message::Send(M_CONFIRM);
        return;
    }
    case M_GET_ALL_DATA:
    {
        if (gSessions.find(m.m_Header.m_To) != gSessions.end())
        {
            gSessions[m.m_Header.m_To]->Send(s);
        }
        break;
    }
    default:
        if (gSessions.find(m.m_Header.m_From) != gSessions.end())
        {
            if (gSessions.find(m.m_Header.m_To) != gSessions.end())
            {
                gSessions[m.m_Header.m_To]->Add(m);
            }
            else if (m.m_Header.m_To == M_ALL)
            {
                for (auto& [id, Session] : gSessions)
                {
                    if (id != m.m_Header.m_From)
                    {
                        Session->Add(m);
                    }
                }
            }
        }
    }
}

void Server()
{
    AfxSocketInit();

    CSocket Server;
    Server.Create(54321);

    while (true)
    {
        if (!Server.Listen())
            break;
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
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
            Server();
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
