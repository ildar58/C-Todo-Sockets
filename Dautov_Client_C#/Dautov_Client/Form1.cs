using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Dautov_Client
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private Dictionary<int, List<string>> Messages = new Dictionary<int, List<string>>();
        private bool Connected = false;

        private void UpdateList()
        {
            listBox1.Items.Clear();
            foreach (int sender in Messages.Keys)
            {
                listBox1.Items.Add(sender);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Message.Send((int) MemberTypes.M_BROKER, (int) MessageTypes.M_INIT);
            Connected = true;
            label1.Text = "Ваш ID: " + Message.m_client_id;
            timer1.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Message.Send((int) MemberTypes.M_BROKER, (int) MessageTypes.M_EXIT);
            Connected = false;
            label1.Text = "Вы отключены от сервера";
            timer1.Stop();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (!Connected || richTextBox1.Text.Length == 0 || textBox1.Text.Length == 0) return;
            Message.Send(Convert.ToInt32(textBox1.Text), (int) MessageTypes.M_DATA, richTextBox1.Text);
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string allMessages = "";
            int index = 0;
            foreach (string message in Messages[Convert.ToInt32(listBox1.SelectedItem)])
            {
                allMessages = allMessages + "Сообщение#" + ++index + ":\n" + message + "\n";
            }

            richTextBox2.Text = allMessages;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Message m = new Message();
            m = Message.Send((int) MemberTypes.M_BROKER, (int) MessageTypes.M_GETDATA);
            if (m.m_header.m_type != (int) MessageTypes.M_NODATA)
            {
                string selectedItem = "";

                if (listBox1.SelectedItem != null)
                {
                    selectedItem = listBox1.SelectedItem.ToString();
                }

                if (Messages.ContainsKey(m.m_header.m_from))
                {
                    Messages[m.m_header.m_from].Add(m.m_data);
                    UpdateList();
                }
                else
                {
                    List<string> l = new List<string>();
                    l.Add(m.m_data);
                    Messages.Add(m.m_header.m_from, l);
                    UpdateList();
                }

                if (selectedItem == m.m_header.m_from.ToString())
                {
                    listBox1.SelectedIndex = listBox1.Items.IndexOf(Convert.ToInt32(selectedItem));
                }
            }
        }
    }
}