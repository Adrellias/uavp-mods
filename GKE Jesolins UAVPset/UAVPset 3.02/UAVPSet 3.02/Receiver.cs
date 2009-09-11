using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace UAVP.UAVPSet
{
    /// <summary>
    /// forumular für die Neutralwerte
    /// </summary>
    public partial class Receiver : Form
    {
        FormMain mainForm;
        PICConnect picConnect = new PICConnect();
        ArrayList ret;
        bool work = false;

        public Receiver(FormMain mainForm)
        {
            InitializeComponent();
            this.mainForm = mainForm;
        }

        //es werden nur die Labels upgedatet und über Event auf die TrackBars übertragen
        //weiters werden die Werte begrenzt
        private void gasLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(gasLabel.Text) < 0)
            {
                gasTrackBar.Value = 0;
            }
            else if (Convert.ToInt16(gasLabel.Text) > 200)
            {
                gasTrackBar.Value = 200;
            }
            else
            {
                gasTrackBar.Value = Convert.ToInt16(gasLabel.Text);
            }
        }

        private void RollLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(RollLabel.Text) < -100)
            {
                RollTrackBar.Value = -100;
            }
            else if (Convert.ToInt16(RollLabel.Text) > 100)
            {
                RollTrackBar.Value = 100;
            }
            else
            {
                RollTrackBar.Value = Convert.ToInt16(RollLabel.Text);
            }
        }

        private void PitchLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(PitchLabel.Text) < -100)
            {
                PitchTrackBar.Value = -100;
            }
            else if (Convert.ToInt16(PitchLabel.Text) > 100)
            {
                PitchTrackBar.Value = 100;
            }
            else
            {
                PitchTrackBar.Value = Convert.ToInt16(PitchLabel.Text);
            }
        }

        private void YawLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(YawLabel.Text) < -100)
            {
                YawTrackBar.Value = -100;
            }
            else if (Convert.ToInt16(YawLabel.Text) > 100)
            {
                YawTrackBar.Value = 100;
            }
            else
            {
                YawTrackBar.Value = Convert.ToInt16(YawLabel.Text);
            }
        }

        private void ch5Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(ch5Label.Text) < 0)
            {
                ch5TrackBar.Value = 0;
            }
            else if (Convert.ToInt16(ch5Label.Text) > 200)
            {
                ch5TrackBar.Value = 200;
            }
            else
            {
                ch5TrackBar.Value = Convert.ToInt16(ch5Label.Text);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            getReceiver();

        }

        private void Receiver_FormClosing(object sender, FormClosingEventArgs e)
        {
            picConnect.closeConnection(mainForm);
            timer1.Enabled = false;
        }

        private bool getReceiver()
        {
            // sollte der timer sich überholen wird die Abfrage nur ausgeführt wenn die 
            // vorherige bereits abgearbeitet ist
            if (work == false)
            {
                work = true; // auf true setzten damit der timer die funktion nicht paralell ausführt
                ret = picConnect.askPic(mainForm, "R");
                if (ret.Count == 0)
                {
                    timer1.Enabled = false;
                    this.Close();
                    return false;
                }
                else
                {
                    if (Properties.Settings.Default.logLevel > 0)
                    {
                        Log.write(mainForm, "R: " + ret[1].ToString(), 1);
                    }
                    // teilen der rückgabe
                    string[] temp = ret[1].ToString().Split(':');
                    // setzen der textboxen
                    gasLabel.Text = temp[1].ToString().Substring(0, 3);
                    RollLabel.Text = temp[2].ToString().Substring(0, 4);
                    PitchLabel.Text = temp[3].ToString().Substring(0, 4);
                    YawLabel.Text = temp[4].ToString().Substring(0, 4);
                    ch5Label.Text = temp[5].ToString().Substring(0, 3);
                    ch6Label.Text = temp[6].ToString().Substring(0, 3);
                    ch7Label.Text = temp[7].ToString().Substring(0, 3);
                    Application.DoEvents();
                }
                work = false;
            }

            return true;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        /// <summary>
        /// der timer wird aktiviert sobald das Form sichtbar ist
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Receiver_VisibleChanged(object sender, EventArgs e)
        {
            if (this.Visible == true)
            {
                if (getReceiver() == true)
                {
                    timer1.Enabled = true;
                }
            }


        }

        private void ch6Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(ch6Label.Text) < 0)
            {
                ch6TrackBar.Value = 0;
            }
            else if (Convert.ToInt16(ch6Label.Text) > 200)
            {
                ch6TrackBar.Value = 200;
            }
            else
            {
                ch6TrackBar.Value = Convert.ToInt16(ch6Label.Text);
            }
        }

        private void ch5Label_Click(object sender, EventArgs e)
        {

        }

        private void ch7Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(ch7Label.Text) < 0)
            {
                ch7TrackBar.Value = 0;
            }
            else if (Convert.ToInt16(ch7Label.Text) > 200)
            {
                ch7TrackBar.Value = 200;
            }
            else
            {
                ch7TrackBar.Value = Convert.ToInt16(ch7Label.Text);
            }
       

        }

    }
}