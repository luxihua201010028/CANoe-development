namespace TCP_Demo
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label2 = new System.Windows.Forms.Label();
            this.txtServerPort = new System.Windows.Forms.TextBox();
            this.btnStart = new System.Windows.Forms.Button();
            this.txtSend = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.btnSend = new System.Windows.Forms.Button();
            this.btnEnd = new System.Windows.Forms.Button();
            this.lstShow = new System.Windows.Forms.ListBox();
            this.txtReceive = new System.Windows.Forms.TextBox();
            this.txtServerIP = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtClientPort = new System.Windows.Forms.TextBox();
            this.txtClientIP = new System.Windows.Forms.TextBox();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(214, 12);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(31, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "端口";
            // 
            // txtServerPort
            // 
            this.txtServerPort.Location = new System.Drawing.Point(200, 30);
            this.txtServerPort.Name = "txtServerPort";
            this.txtServerPort.Size = new System.Drawing.Size(58, 20);
            this.txtServerPort.TabIndex = 3;
            this.txtServerPort.Text = "6565";
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(269, 25);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(78, 25);
            this.btnStart.TabIndex = 1;
            this.btnStart.Text = "开始";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // txtSend
            // 
            this.txtSend.Location = new System.Drawing.Point(91, 114);
            this.txtSend.Name = "txtSend";
            this.txtSend.Size = new System.Drawing.Size(167, 20);
            this.txtSend.TabIndex = 4;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 143);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(73, 13);
            this.label6.TabIndex = 0;
            this.label6.Text = "接收的消息：";
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(269, 111);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(78, 25);
            this.btnSend.TabIndex = 5;
            this.btnSend.Text = "发送";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
            // 
            // btnEnd
            // 
            this.btnEnd.Location = new System.Drawing.Point(269, 173);
            this.btnEnd.Name = "btnEnd";
            this.btnEnd.Size = new System.Drawing.Size(78, 25);
            this.btnEnd.TabIndex = 6;
            this.btnEnd.Text = "终止";
            this.btnEnd.UseVisualStyleBackColor = true;
            this.btnEnd.Click += new System.EventHandler(this.btnEnd_Click);
            // 
            // lstShow
            // 
            this.lstShow.FormattingEnabled = true;
            this.lstShow.Location = new System.Drawing.Point(353, 12);
            this.lstShow.Name = "lstShow";
            this.lstShow.Size = new System.Drawing.Size(211, 186);
            this.lstShow.TabIndex = 4;
            this.lstShow.TabStop = false;
            // 
            // txtReceive
            // 
            this.txtReceive.BackColor = System.Drawing.Color.White;
            this.txtReceive.Location = new System.Drawing.Point(91, 143);
            this.txtReceive.Name = "txtReceive";
            this.txtReceive.ReadOnly = true;
            this.txtReceive.Size = new System.Drawing.Size(167, 20);
            this.txtReceive.TabIndex = 3;
            this.txtReceive.TabStop = false;
            // 
            // txtServerIP
            // 
            this.txtServerIP.Location = new System.Drawing.Point(91, 28);
            this.txtServerIP.Name = "txtServerIP";
            this.txtServerIP.Size = new System.Drawing.Size(103, 20);
            this.txtServerIP.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(124, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "IP地址";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(16, 31);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "服务器：";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 117);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(73, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "发送的消息：";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(16, 67);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(49, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "客户端：";
            // 
            // txtClientPort
            // 
            this.txtClientPort.BackColor = System.Drawing.Color.White;
            this.txtClientPort.Location = new System.Drawing.Point(200, 64);
            this.txtClientPort.Name = "txtClientPort";
            this.txtClientPort.ReadOnly = true;
            this.txtClientPort.Size = new System.Drawing.Size(58, 20);
            this.txtClientPort.TabIndex = 1;
            this.txtClientPort.TabStop = false;
            // 
            // txtClientIP
            // 
            this.txtClientIP.BackColor = System.Drawing.Color.White;
            this.txtClientIP.Location = new System.Drawing.Point(91, 64);
            this.txtClientIP.Name = "txtClientIP";
            this.txtClientIP.ReadOnly = true;
            this.txtClientIP.Size = new System.Drawing.Size(103, 20);
            this.txtClientIP.TabIndex = 1;
            this.txtClientIP.TabStop = false;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(576, 208);
            this.Controls.Add(this.lstShow);
            this.Controls.Add(this.txtReceive);
            this.Controls.Add(this.txtSend);
            this.Controls.Add(this.btnEnd);
            this.Controls.Add(this.btnSend);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.txtClientIP);
            this.Controls.Add(this.txtClientPort);
            this.Controls.Add(this.txtServerIP);
            this.Controls.Add(this.txtServerPort);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label2);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form1";
            this.Text = "TCP/IP 服务器端";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtServerPort;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.TextBox txtSend;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.Button btnEnd;
        private System.Windows.Forms.ListBox lstShow;
        private System.Windows.Forms.TextBox txtReceive;
        private System.Windows.Forms.TextBox txtServerIP;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtClientPort;
        private System.Windows.Forms.TextBox txtClientIP;
        private System.Windows.Forms.Timer timer1;
    }
}

