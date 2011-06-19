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
    /// forumular f�r die Neutralwerte
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

        private void ThrottleLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(ThrottleValue.Text) < 0)
                ThrottleTrackBar.Value = 0;
            else if (Convert.ToInt16(ThrottleValue.Text) > 100)
                ThrottleTrackBar.Value = 100;
            else
                ThrottleTrackBar.Value = Convert.ToInt16(ThrottleValue.Text);
        }

        private void RollLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(RollValue.Text) < -100)
                RollTrackBar.Value = -100;
            else if (Convert.ToInt16(RollValue.Text) > 100)
                RollTrackBar.Value = 100;
            else
                RollTrackBar.Value = Convert.ToInt16(RollValue.Text);
        }

        private void PitchLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(PitchValue.Text) < -100)
                PitchTrackBar.Value = -100;
            else if (Convert.ToInt16(PitchValue.Text) > 100)
                PitchTrackBar.Value = 100;
            else
                PitchTrackBar.Value = Convert.ToInt16(PitchValue.Text);
        }

        private void YawLabel_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(YawValue.Text) < -100)
                YawTrackBar.Value = -100;
            else if (Convert.ToInt16(YawValue.Text) > 100)
                YawTrackBar.Value = 100;
            else
                YawTrackBar.Value = Convert.ToInt16(YawValue.Text);
        }

        private void ch5Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(RTHValue.Text) < 0)
                RTHTrackBar.Value = 0;
            else if (Convert.ToInt16(RTHValue.Text) > 100)
                RTHTrackBar.Value = 100;
            else
                RTHTrackBar.Value = Convert.ToInt16(RTHValue.Text);
        }

        private void ch6Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(CamPitchTrimValue.Text) < -100)
                CamPitchTrimTrackBar.Value = -100;
            else if (Convert.ToInt16(CamPitchTrimValue.Text) > 100)
                CamPitchTrimTrackBar.Value = 100;
            else
                CamPitchTrimTrackBar.Value = Convert.ToInt16(CamPitchTrimValue.Text);
        }

        private void ch7Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(NavSValue.Text) < 0)
                NavSTrackBar.Value = 0;
            else if (Convert.ToInt16(NavSValue.Text) > 100)
                NavSTrackBar.Value = 100;
            else
                NavSTrackBar.Value = Convert.ToInt16(NavSValue.Text);
        }

        private void Ch8Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(Ch8Value.Text) < 0)
                Ch8TrackBar.Value = 0;
            else if (Convert.ToInt16(Ch8Value.Text) > 100)
                Ch8TrackBar.Value = 100;
            else
                Ch8TrackBar.Value = Convert.ToInt16(Ch8Value.Text);
        }

        private void Ch9Label_TextChanged(object sender, EventArgs e)
        {
            if (Convert.ToInt16(Ch9Value.Text) < 0)
                Ch9TrackBar.Value = 0;
            else if (Convert.ToInt16(Ch9Value.Text) > 100)
                Ch9TrackBar.Value = 100;
            else
                Ch9TrackBar.Value = Convert.ToInt16(Ch9Value.Text);
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
            // sollte der timer sich �berholen wird die Abfrage nur ausgef�hrt wenn die 
            // vorherige bereits abgearbeitet ist
            if (work == false)
            {
                work = true; 
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
                    // teilen der r�ckgabe
                    string[] temp = ret[1].ToString().Split(':');
                    // setzen der textboxen
                    ThrottleValue.Text = temp[1].ToString().Substring(0, 3);
                    RollValue.Text = temp[2].ToString().Substring(0, 4);
                    PitchValue.Text = temp[3].ToString().Substring(0, 4);
                    YawValue.Text = temp[4].ToString().Substring(0, 4);
                    RTHValue.Text = temp[5].ToString().Substring(0, 3);
                    CamPitchTrimValue.Text = temp[6].ToString().Substring(0, 4);
                    NavSValue.Text = temp[7].ToString().Substring(0, 3);
                    Ch8Value.Text = temp[8].ToString().Substring(0, 3);
                    Ch9Value.Text = temp[9].ToString().Substring(0, 3);

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

        private void Receiver_VisibleChanged(object sender, EventArgs e)
        {
            if (this.Visible == true)
                if (getReceiver() == true)
                    timer1.Enabled = true;
        }

     
       
     

    }
}