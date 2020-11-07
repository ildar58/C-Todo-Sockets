using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;

public enum MemberTypes
{
    M_BROKER = 0,
    M_ALL = 10,
    M_USER = 100
}

public enum MessageTypes
{
    M_INIT,
    M_EXIT,
    M_GETDATA,
    M_NODATA,
    M_DATA,
    M_CONFIRM
}

struct MsgHeader
{
    public MsgHeader(int to, int from, int type, int size)
    {
        this.m_to = to;
        this.m_from = from;
        this.m_type = type;
        this.m_size = size;
    }

    public static int GetSize()
    {
        return sizeof(int) * 4;
    }

    public byte[] TransformToBytes()
    {
        byte[] bytes = new byte[sizeof(int) * 4];
        int i = 0;

        Array.Copy(BitConverter.GetBytes(this.m_to), 0, bytes, i++ * sizeof(int), sizeof(int));
        Array.Copy(BitConverter.GetBytes(this.m_from), 0, bytes, i++ * sizeof(int), sizeof(int));
        Array.Copy(BitConverter.GetBytes(this.m_type), 0, bytes, i++ * sizeof(int), sizeof(int));
        Array.Copy(BitConverter.GetBytes(this.m_size), 0, bytes, i * sizeof(int), sizeof(int));

        return bytes;
    }

    public void TransformToMsgHeader(byte[] bytes)
    {
        int i = 0;
        this.m_to = BitConverter.ToInt32(bytes, i++ * sizeof(int));
        this.m_from = BitConverter.ToInt32(bytes, i++ * sizeof(int));
        this.m_type = BitConverter.ToInt32(bytes, i++ * sizeof(int));
        this.m_size = BitConverter.ToInt32(bytes, i * sizeof(int));
    }

    public int m_to;
    public int m_from;
    public int m_type;
    public int m_size;
};

namespace Dautov_Client
{
    class Message
    {
        public MsgHeader m_header;
        public string m_data;
        public static int m_client_id = 0;


        static private Encoding cp1251 = Encoding.GetEncoding("windows-1251");

        public Message()
        {
        }

        public Message(int to, int from, int type, string data = "")
        {
            m_header = new MsgHeader(to, from, type, data.Length);
            m_data = data;
        }

        public void Send(Socket s)
        {
            s.Send(m_header.TransformToBytes(), MsgHeader.GetSize(), SocketFlags.None);
            if (!string.IsNullOrEmpty(m_data))
            {
                s.Send(cp1251.GetBytes(m_data), m_data.Length, SocketFlags.None);
            }
        }

        public int Receive(Socket s)
        {
            byte[] bytes = new byte[MsgHeader.GetSize()];
            s.Receive(bytes, MsgHeader.GetSize(), SocketFlags.None);

            m_header.TransformToMsgHeader(bytes);

            if (m_header.m_size > 0)
            {
                bytes = new byte[m_header.m_size + 1];
                s.Receive(bytes, m_header.m_size + 1, SocketFlags.None);
                m_data = cp1251.GetString(bytes, 0, m_header.m_size);
            }

            return m_header.m_type;
        }

        public static Message Send(int to, int type = (int) MessageTypes.M_DATA, string data = "")
        {
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                s.Connect(endPoint);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                throw;
            }

            Message m = new Message(to, Message.m_client_id, type, data);
            m.Send(s);
            m.Receive(s);
            if (m.m_header.m_type == (int) MessageTypes.M_INIT)
                Message.m_client_id = m.m_header.m_to;
            s.Close();
            return m;
        }
    }
}