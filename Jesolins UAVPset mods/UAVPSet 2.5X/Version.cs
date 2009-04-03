using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace UAVP.UAVPSet
{
    public partial class Version : Form
    {
        public Version()
        {
            InitializeComponent();
            versionComboBox.Text = "3.16";
            Properties.Settings.Default.version = "3.16";
            Properties.Settings.Default.baudRate = 38400;
            Properties.Settings.Default.Save();
        }

        private void versionComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.version = versionComboBox.Text;
            //if (Properties.Settings.Default.version == "3.12" ||
            //    Properties.Settings.Default.version == "3.13")
            //{
            //    Properties.Settings.Default.baudRate = 38400;
            //}
            //else
            //{
            //    Properties.Settings.Default.baudRate = 115200;
            //}
            Properties.Settings.Default.Save();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
    }
}