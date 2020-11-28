// Dautov_Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "Dautov_Server.h"
#include "Msg.h"
#include "Session.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int gMaxID = M_USER;
map<int, shared_ptr<Session>> gSessions;

void CheckClients()
{
    while (true)
    {
        for (auto it = gSessions.begin(); it != gSessions.end();)
        {
            double  workTime = clock() - it->second->getTime();
            if (workTime > 100000)
            {
                cout << it->first << " disconnected" << endl;
                it = gSessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
        Sleep(1000);
    }
}

void ProcessClient(SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);
   
    Message m;
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
            Message::SendStorage(M_STORAGE, pSession->m_ID, M_INIT);
            break;
        }
        case M_EXIT:
        {
            gSessions.erase(m.m_Header.m_From);
            Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            Message::SendStorage(M_STORAGE, m.m_Header.m_From, M_EXIT);
            return;
        }
        case M_GETDATA:
        {
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                gSessions[m.m_Header.m_From]->Send(s);
                gSessions[m.m_Header.m_From]->SetTime(clock());
            }
            break;
        }
        case M_GET_ALL_DATA:
        {
            CSocket sock;
            sock.Create();
            if (!sock.Connect("127.0.0.1", 54321))
            {
                DWORD dwError = GetLastError();
                throw runtime_error("Connection error");
            }
            m.Send(sock);
            m.Receive(sock);
            if (m.m_Header.m_Type == M_NODATA)
                m.Send(s, m.m_Header.m_To, m.m_Header.m_From, M_NODATA);
            else
            {
                const unsigned int messagesLen = stoi(m.m_Data);
                m.Send(s, m.m_Header.m_To, m.m_Header.m_From, M_DATA, to_string(messagesLen));
                for (unsigned int i = 0; i < messagesLen; i++) {
                    m.Receive(sock);
                    m.Send(s, m.m_Header.m_To, m.m_Header.m_From, M_DATA, m.m_Data);
                }
            }
            sock.Close();
            break;
        }
        default:
        {
            if (gSessions.find(m.m_Header.m_From) != gSessions.end())
            {
                if (gSessions.find(m.m_Header.m_To) != gSessions.end())
                {
                    Message::SendStorage(m.m_Header.m_To, m.m_Header.m_From, M_DATA, m.m_Data);
                    gSessions[m.m_Header.m_To]->Add(m);
                }
                else if (m.m_Header.m_To == M_ALL)
                {
                    for (auto& [id, Session] : gSessions)
                    {
                        if (id != m.m_Header.m_From)
                        {
                            Message::SendStorage(m.m_Header.m_To, m.m_Header.m_From, M_DATA, m.m_Data);
                            Session->Add(m);
                        }
                    }
                }
                Message::Send(s, m.m_Header.m_From, M_BROKER, M_CONFIRM);
            }
            break;
    }   
    }
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
