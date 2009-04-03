using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

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

        private void rollLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(rollLabel.Text) < -100) 
            {
                rollTrackBar.Value = -100;
            } 
            else if (Convert.ToInt16(rollLabel.Text) > 100) 
            {
                rollTrackBar.Value = 100;
            } 
            else 
            {
                rollTrackBar.Value = Convert.ToInt16(rollLabel.Text);
            }
        }

        private void nickLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(nickLabel.Text) < -100) 
            {
                nickTrackBar.Value = -100;
            } 
            else if (Convert.ToInt16(nickLabel.Text) > 100) 
            {
                nickTrackBar.Value = 100;
            } 
            else 
            {
                nickTrackBar.Value = Convert.ToInt16(nickLabel.Text);
            }
        }

        private void gierLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(gierLabel.Text) < -100) 
            {
                gierTrackBar.Value = -100;
            } 
            else if (Convert.ToInt16(gierLabel.Text) > 100) 
            {
                gierTrackBar.Value = 100;
            } 
            else 
            {
                gierTrackBar.Value = Convert.ToInt16(gierLabel.Text);
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
                    rollLabel.Text = temp[2].ToString().Substring(0, 4);
                    nickLabel.Text = temp[3].ToString().Substring(0, 4);
                    gierLabel.Text = temp[4].ToString().Substring(0, 4);
                    ch5Label.Text = temp[5].ToString().Substring(0, 3);
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

    }
}