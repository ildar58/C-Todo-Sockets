#pragma once

struct Session
{
	int m_ID;

	vector<Message> m_Messages;
	CCriticalSection m_CS;

	Session(int ID)
		:m_ID(ID)
	{
	}

	~Session()
	{
	}

	void Add(Message& m)
	{
		CSingleLock sl(&m_CS, TRUE);
		m_Messages.push_back(m);
	}

	void Send(CSocket& s)
	{
		CSingleLock sl(&m_CS, TRUE);
		if (m_Messages.empty())
		{
			Message::Send(s, m_ID, M_BROKER, M_NODATA);
		}
		else
		{
			Message::Send(s, m_ID, M_STORAGE, M_DATA, to_string((int)m_Messages.size()));
			for (Message m : m_Messages)
				Message::Send(s, m.m_Header.m_To, m.m_Header.m_From, M_DATA, m.m_Data);
		}
	}
};
