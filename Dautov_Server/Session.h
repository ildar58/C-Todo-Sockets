#pragma once

struct Session
{
	int m_ID;
	clock_t time;
	string m_Name;

	queue<Message> m_Messages;
	CCriticalSection m_CS;

	Session(int ID, string Name)
		:m_ID(ID), m_Name(Name), time(clock())
	{
	}

	~Session()
	{
	}

	void Add(Message& m)
	{
		CSingleLock sl(&m_CS, TRUE);
		m_Messages.push(m);
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
			m_Messages.front().Send(s);
			m_Messages.pop();
		}
	}

	void SetTime(clock_t t) {
		CSingleLock sl(&m_CS, TRUE);
		time = t;
	}

	clock_t getTime() {
		return time;
	}
};
