using System;
using System.Windows.Forms;
using System.Net;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.ComponentModel;

namespace TCP_Demo
{
    public partial class Form1 : Form
    {
        //定义一个TcpClient类
        static TcpClient client = null;         
        //定义一个NetworkStream类, 用于网络访问的基础流的数据操作
        static NetworkStream stream = null;
        //定义一个TcpServer类
        TcpListener server = null;
        //线程用于实时监测client是否接入，以及是否数据传过来
        public Thread SocketWatch; 
        delegate void SetTextCallback(string text);

        //0 - 断开状态； 1- 尝试连接中； 2- 已连接；3- 尝试断开中；
        public int socket_status = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //获取本地计算机的主机名
            string name = Dns.GetHostName();
            //获取本地计算机的Internet协议（IP）地址
            IPAddress[] ipadrlist = Dns.GetHostAddresses(name); 
            foreach (IPAddress ipa in ipadrlist)
            {
                //检索IPv4的IP地址
                if (ipa.AddressFamily == AddressFamily.InterNetwork)
                {
                    lstShow.Items.Add("检索到主机的IP地址：" + ipa.ToString());
                    //将第一个有效的IPv4地址作为服务器地址
                    if (txtServerIP.Text == "") 
                        txtServerIP.Text = ipa.ToString();
                }
            }
            btnSend.Enabled = false;
            btnEnd.Enabled = false;
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            btnStart.Enabled = false;
            int port = Convert.ToInt32(txtServerPort.Text);
            IPAddress IP = System.Net.IPAddress.Parse(txtServerIP.Text);
            IPEndPoint p = new IPEndPoint(IP, port);
            server = new TcpListener(p);
            server.Start();
            lstShow.Items.Add("服务器已启动！");
            lstShow.Items.Add("等待客户端连接....");
            Update();
            socket_status = 1; //等待client接入
            //开启新的线程，用于监控client接入和数据接收
            this.SocketWatch = new Thread(new ThreadStart(this.SocketTask));
            this.SocketWatch.Start();

            btnSend.Enabled = true;
            btnEnd.Enabled = true;

        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            byte[] sendmsg = new byte[512]; //定义发送的Byte数组
            int i = 0;
            //Encode转换为UTF8
            byte[] msg = Encoding.UTF8.GetBytes(txtSend.Text);
            for (i = 0; i < msg.Length;i++ )
            {
               sendmsg[i] = msg[i];
            }
            //CANoe根据最后字符是否为'\0'来判断接收的字符串长度
            sendmsg[msg.Length] = 0x00; 
            stream = client.GetStream();
            stream.Write(sendmsg, 0, sendmsg.Length);
            lstShow.Items.Add("发送数据成功！");
        }

        private void btnEnd_Click(object sender, EventArgs e)
        {
            if (this.SocketWatch != null) this.SocketWatch.Abort();
            if (stream != null) stream.Close();
            if (client != null) client.Close();
            if (server != null) server.Stop();
            socket_status = 0;
            btnStart.Enabled = true;
            btnSend.Enabled = false;
            btnEnd.Enabled = false;

        }

        //用于实时监测client是否接入，以及是否数据传过来
        public void SocketTask()
        {
            while (true)
            {
                if (socket_status==1)
                {
                    //接受挂起的连接请求。
                    client = server.AcceptTcpClient(); 
                    socket_status = 2;
                }
                
                Byte[] bytes = new Byte[256];
                //返回用于发送和接收数据的NetworkStream
                stream = client.GetStream(); 
                try
                {
                    stream.Read(bytes, 0, bytes.Length);//从读取Stream数据
                    //按照UTF8的编码方式得到字符串
                    string data = Encoding.UTF8.GetString(bytes, 0, bytes.Length);
                    if (bytes[0] != '\0') // 如果数据是有效的
                    {
                        this.SetText(data);//数据传递给主线程
                    }
                }
                catch (System.IO.IOException)
                {
                    if (client.Connected == true)
                    {
                        client.Close(); //释放此TcpClient实例
                        stream.Close(); //关闭当前流并释放与之关联的所有资源
                    }
                    socket_status = 3;
                    break;
                }
            }
        }

        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtReceive.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txtReceive.Text = text;
                lstShow.Items.Add("接收到数据：" + text);
            }
        }


        private void timer1_Tick(object sender, EventArgs e)
        {
            //处理界面的更新
            Socket s;
            if (client != null)
            {
                if (socket_status ==3) //尝试断开中
                {

                    server.Stop();
                    lstShow.Items.Add("客户端已断开！");
                    txtClientIP.Text = "";
                    txtClientPort.Text = "";
                    btnStart.Enabled = true;
                    btnSend.Enabled = false;
                    btnEnd.Enabled = false;
                    client = null;
                    socket_status = 0;
                }
                else
                {
                    if (txtClientIP.Text == "")
                    {
                        lstShow.Items.Add("客户端已连接！");
                        s = client.Client;
                        //显示远程客户端IP和Port端口
                        txtClientIP.Text = s.RemoteEndPoint.ToString().Split(':')[0];
                        txtClientPort.Text = s.RemoteEndPoint.ToString().Split(':')[1];
                    }
                }

            }
        }
    }

}
