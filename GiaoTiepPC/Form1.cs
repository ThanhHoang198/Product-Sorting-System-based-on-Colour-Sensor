using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;


namespace GiaoTiepPC
{
    public partial class window : Form
    {
        string speed, freq, spStop, timeStop, ON_OFF;
        delegate void SetTextCallBack(string text);
        string Data2Send = String.Empty;
        string FeedbackData = String.Empty;
        public window()
        {
            InitializeComponent();
            UART.DataReceived += new SerialDataReceivedEventHandler(Datareceive);
        }

        private void Datareceive(object sender, SerialDataReceivedEventArgs e)
        {
            FeedbackData = UART.ReadLine();
            SetTextFeedBack(FeedbackData);
        }

        
      
       private void SerialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
          /* if(txtData.IsHandleCreated)
            {
                this.Invoke((MethodInvoker)delegate
                 
                    {
                        string[] Data2Rec = UART.ReadExisting().Split('|');
                           txtRed.Text = Data2Rec[0];
                           txtBlue.Text = Data2Rec[1];
                           txtGreen.Text = Data2Rec[2];
                          txtData.Text = UART.ReadExisting();
                    }
                
                );
            }*/
        }


        private void SetTextFeedBack(string Text)
        {
            if(this.txtData.InvokeRequired)
            {
                SetTextCallBack d = new SetTextCallBack(SetTextFeedBack);
                this.BeginInvoke(d, new object[] { Text });
            }

            else
            {
                this.txtData.Clear();
                string[] Data2Rec = Text.Split('|');
                txtRed.Text = Data2Rec[0];
                txtBlue.Text = Data2Rec[1];
                txtGreen.Text = Data2Rec[2];
                txtSum.Text = Data2Rec[3];
                txtData.Text = Text;
            }


        }

        private void ComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void UART_Load(object sender, EventArgs e)
        {
            cbxCom.DataSource = SerialPort.GetPortNames();
            if (cbxCom.Items.Count > 0)
                cbxCom.SelectedIndex = 0;         
            grbAdvance.Enabled = false;
            lblStatus.Text = "Port is closed !";
            speed = "L";
            freq = "m";         //Default setup
            spStop = "0";
            timeStop = "0";
        }

        private void BtnConnect_Click(object sender, EventArgs e)
        {
            try
            {
                if (UART.IsOpen == true)
                {
                    UART.DiscardInBuffer();
                    UART.Close();

                }
                UART.PortName = cbxCom.Text;
                UART.Open();
                lblStatus.Text = "PC is connecting with " + UART.PortName.ToString();
                if (UART.IsOpen == true)
                {
                    btnConnect.Enabled = false;
                    btnDisconnect.Enabled = true;
                    btnStart.Enabled = true;
                }
            }
            catch 
            {
                MessageBox.Show("Bạn chưa chọn cổng COM !");
            }

        }

        private void BtnDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                if (UART.IsOpen)
                {   while (UART.BytesToWrite > 0) { }
                    UART.DiscardInBuffer();
                    UART.Dispose();
                    UART.Close();

                    btnConnect.Enabled = true;
                    btnDisconnect.Enabled = false;
                    lblStatus.Text = "Port is closed !";
                }              
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);

            }

        }
        int temp;
        private void BtnStart_Click(object sender, EventArgs e)
        {


            if (chbSetup.CheckState == CheckState.Checked)
            {
                if ((txtSPstop.Text != null && int.TryParse(txtSPstop.Text, out temp)) && (txtTimeStop.Text != null && int.TryParse(txtTimeStop.Text, out temp)))
                {
                    ON_OFF = "1";
                    spStop = txtSPstop.Text;
                    timeStop = txtTimeStop.Text;
                    sendData();
                    btnStart.Enabled = false;
                    btnStop.Enabled = true;
                }
                else
                    MessageBox.Show("Bạn phải nhập chữ số !");
            }
            else
            {
                ON_OFF = "1";
                sendData();
                btnStart.Enabled = false;
                btnStop.Enabled = true;
            }
        }

        private void BtnStop_Click(object sender, EventArgs e)
        {
            ON_OFF = "0";
            chbSetup.Checked = false;
            btnStart.Enabled = true;
            btnStop.Enabled = false;
            sendData();
        }
        int first_touch;
        private void CheckBox1_CheckedChanged(object sender, EventArgs e)
        {
            first_touch++;
            if (chbSetup.CheckState == CheckState.Checked)
            {
                if (first_touch == 1)
                {
                    cbxSpeed.SelectedItem = "Chậm";
                    cbxFreq.SelectedItem = "20 %";                    
                }
                grbAdvance.Enabled = true;
                switch (cbxSpeed.SelectedIndex)
                {
                    case 0: speed = "L"; break;

                    case 1: speed = "M"; break;

                    case 2: speed = "H"; break;

                }
                switch (cbxFreq.SelectedIndex)
                {
                    case 0: freq = "l"; break;

                    case 1: freq = "m"; break;

                    case 2: freq = "h"; break;

                }
            }
                if (chbSetup.CheckState == CheckState.Unchecked)
            {
                grbAdvance.Enabled = false;
                speed = "L";
                freq = "m";         //Default setup
                spStop = "0";
                timeStop = "0";

            }
        }
        private void sendData()
        {

            Data2Send = "<" + ON_OFF + "-" + speed + "-" + freq + "-" + spStop + "-" + timeStop + ">";
            if (UART.IsOpen)
            {
                UART.Write(Data2Send);
                txtData.Text = Data2Send;
            }
            else
                MessageBox.Show("Port is Closed !!!");
            

        }
        private void CbxSpeed_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch(cbxSpeed.SelectedIndex)
            {
                case 0: speed = "L"; break;

                case 1: speed = "M"; break;

                case 2: speed = "H"; break;

            }               
        }
        
        private void CbxFreq_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (cbxFreq.SelectedIndex)
            {
                case 0: freq = "l"; break;

                case 1: freq = "m"; break;

                case 2: freq = "h"; break;

            }

        }
    }
}
