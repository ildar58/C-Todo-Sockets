// Dautov_Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "Dautov_Server.h"
#include "Msg.h"
#include "Session.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <shared_mutex>

int gMaxID = M_USER;
map<int, shared_ptr<Session>> gSessions;
shared_mutex hMutex;

void CheckClients()
{
    while (true)
    {
        hMutex.lock();
            for (auto& [id, Session] : gSessions)
            {
                if (Session->CheckClient())
                {
                    cout << id << " disconnected" << endl;
                    gSessions.erase(Session->m_ID);
                }
            }
         hMutex.unlock();
         Sleep(2000);
    }
}

void ProcessClient(SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);
    Message m;
    hMutex.lock();
    int nCode = m.Receive(s);
    if (nCode != M_GETDATA)
        cout << m.m_Header.m_From << ": " << nCode << endl;
    
    switch (nCode)
    {
        case M_INIT:
        {
            auto pSession = make_shared<Session>(++gMaxID, m.m_Data);
            gSessions[pSession->m_ID] = pSession;
            Message::Send(s, pSession->m_ID, M_BROKER, M_INIT);
            break;
        }
        case M_EXIT:
        {
            gSessions.erase(m.m_Header.m_From);
            Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            return;
        }
        case M_GETDATA:
        {
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                gSessions[m.m_Header.m_From]->Send(s);
                gSessions[m.m_Header.m_From]->ResetTimer();
            }
            break;
        }
        default:
        {
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
                            Session->Add(m);
                    }
                }
                Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            }
            break;
    }   
    }

    hMutex.unlock();
}


void Server()
{
    AfxSocketInit();

    CSocket Server;
    Server.Create(12345);

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
            thread s_t(CheckClients);
            s_t.detach();
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
