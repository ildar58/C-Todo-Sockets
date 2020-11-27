#include "pch.h"
#include "Msg.h"

void Message::Send(CSocket& s, unsigned int To, unsigned int From, unsigned int Type, const string& Data)
{
	Message m(To, From, Type, Data);
	m.Send(s);
}

Message Message::Send(unsigned int Type, const string& Data)
{
	CSocket s;
	s.Create();

	if (!s.Connect("127.0.0.1", 12345))
	{
		DWORD dwError = GetLastError();
		throw runtime_error("Connection error");
	}

	Message m(M_BROKER, M_STORAGE, Type, Data);
	m.Send(s);
	return m;
}
