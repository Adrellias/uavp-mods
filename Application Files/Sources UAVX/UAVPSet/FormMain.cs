// UAVPSet
// Copyright (C) 2007  Thorsten Raab
// Email: thorsten.raab@gmx.at
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Resources;
using System.Configuration;
using System.Reflection;
using System.Globalization;
using System.Threading;
using System.IO;

namespace UAVP.UAVPSet
{
    public partial class FormMain : Form
    {


        // Eigene Objekte erzeugen
        ParameterSets parameterSets = new ParameterSets(); //Beinhaltet alle Einstellungen
        PICConnect picConnect = new PICConnect(); //Verbindungsklasse mit PIC
        public Cursor cursor = Cursor.Current;
        // Texte f�r Mehrsprachigkeit
        public ResourceManager help;
        ResourceManager labels;
        public ResourceManager errorLabels;
        // bei write feldupdate auf gr�n ausschalten
        public bool writeUpdate = false;
        //Applikationspfad
        public string pfad;
        public bool picBootModus = false;

        public bool[] SenseButton = new bool[8];
        
        public FormMain(string args)
        {
            // sprache �bersteuern
            // wenn auto dann bestriebssystem einstellungen

          
            Thread.CurrentThread.CurrentUICulture = new CultureInfo("en");
 
            InitializeComponent();

            Properties.Settings.Default.language = "English";

            tabPageParameterSet1.BackColor = 
            tabPageParameterSet2.BackColor = 
            Properties.Settings.Default.Colour;

            //com Ports zuweisen zu pull-down
            string [] aviableComPorts = ComPorts.readPorts();
            foreach (string aviableComPort in aviableComPorts)
                comPortToolStripComboBox.Items.Add(aviableComPort);

            // sprachtexte laden
            help = new ResourceManager("UAVP.UAVPSet.Resources.hilfe", this.GetType().Assembly);
            labels = new ResourceManager("UAVP.UAVPSet.Resources.language", this.GetType().Assembly);
            errorLabels = new ResourceManager("UAVP.UAVPSet.Resources.error", this.GetType().Assembly);
            //Applikationspfad
            pfad = Application.StartupPath;
            // Comport aus Settings einstellen
            comPortToolStripComboBox.Text = Properties.Settings.Default.comPort;
            // version einstellen
            if (Properties.Settings.Default.version == "")
            {
                Version versionDialog = new Version();
                versionDialog.ShowDialog();
            }
            versionToolStripComboBox.Text = Properties.Settings.Default.version;
            // Logdatei neu erstellen
            Log.newLog(this);
            
            //Layout aus Einstellungen laden
            this.Height = Properties.Settings.Default.mainFormHeight;
            this.Width = Properties.Settings.Default.mainFormWidth;
            splitContainer1.SplitterDistance = Properties.Settings.Default.listViewWidth;

            LoadSettings(args);

            ChangeVersion();
            Log.write(this, "Version: " + Assembly.GetExecutingAssembly().GetName().Version.ToString(), 1);

          //  Version vrs = new Version(Application.ProductVersion);
          //  this.Text = this.Text + " v" + vrs.Major + "." + vrs.Minor + "." + vrs.Build;
         }

        public void ChangeVersion()
        {
            switch (Properties.Settings.Default.version)
            {
                case "UAVX":
                    linear1GroupBox.Visible = true;

                    groupBoxAltitude1.Visible = true;
                    groupBoxAltitude2.Visible = true;

                    groupBoxGPS1.Visible = true;
                    groupBoxGPS2.Visible = true;

                    GPSGain1Label.Visible = true;
                    GPSGain1NumericUpDown.Visible = true;

                    labelThrottleGS1.Visible = true;
                    ThrottleGS1NumericUpDown.Visible = true;

                    bit0Header1Label.Visible = true;
                    bit01CheckBox.Visible = true;
                    bit1Header1Label.Visible = true;
                    bit11CheckBox.Visible = true;
                    bit2Header1Label.Visible = true;
                    bit21CheckBox.Visible = true;
                    bit3Header1Label.Visible = true;
                    bit31CheckBox.Visible = true;
                    bit4Header1Label.Visible = true;
                    bit41CheckBox.Visible = true;
                    bit5Header1Label.Visible = true;
                    bit51CheckBox.Visible = true;
                    bit6Header1Label.Visible = true;
                    bit61CheckBox.Visible = true;

                    PWMSenseLabel.Visible = true;
                    Sense01Button.Visible = true;
                    Sense11Button.Visible = true;
                    Sense21Button.Visible = true;
                    Sense31Button.Visible = true;
                    Sense41Button.Visible = true;
                    Sense51Button.Visible = true;
                    PPMPolarityButton.Visible = true;

                    labelNavMagVar1.Visible = true;
                    NavMagVar1NumericUpDown.Visible = true;

                    CompassOffset1Label.Visible = true;
                    CompassOffset1NumericUpDown.Visible = true;

                    Compass1Label.Visible = true;
                    Compass1NumericUpDown.Visible = true;
                    Compass2label.Visible = true;
                    Compass2NumericUpDown.Visible = true;

                    GPSGain1Label.Visible = true;
                    GPSGain1NumericUpDown.Visible = true;

                    DescDelayLabel1.Visible = true;
                    DescDelay1NumericUpDown.Visible = true;

                    DescDelayLabel2.Visible = true;
                    DescDelay2NumericUpDown.Visible = true;

                    LabelHoverThrottle1.Visible = true;
                    HoverThrottle1NumericUpDown.Visible = true;

                    CameraRoll1Label.Visible = true;
                    CameraRoll1NumericUpDown.Visible = true;

                    CameraRollTrim1Label.Visible = true;
                    CameraRollTrim1NumericUpDown.Visible = true;

                    CameraPitch1Label.Visible = true;
                    CameraPitch1NumericUpDown.Visible = true;

                    DescDelayLabel1.Visible = true;
                    DescDelay1NumericUpDown.Visible = true;

                    labelThrottleGS1.Visible = true;
                    ThrottleGS1NumericUpDown.Visible = true;

                    labelESC1.Visible = true;
                    ESCComboBox1.Visible = true;

  
                    labelTelemetry1.Visible = true;
                    TelemetryComboBox1.Visible = true;

                    labelGyros1.Visible = true;
                    GyroComboBox1.Visible = true;

                    LabelHoverThrottle1.Visible = true;
                    HoverThrottle1NumericUpDown.Visible = true;

                    BatteryCapacity1Label.Visible = true;
                    BatteryCapacity1NumericUpDown.Visible = true;

                    Orientation1Label.Visible = true;
                    Orientation1NumericUpDown.Visible = true;

                    NavYawLimit1Label.Visible = true;
                    NavYawLimit1NumericUpDown.Visible = true;

                    NavYawLimit2Label.Visible = true;
                    NavYawLimit2NumericUpDown.Visible = true;

                    Ch9NumericUpDown.Visible = false;
                    Ch9Label.Visible = false;

                    Ch8NumericUpDown.Visible = false;
                    Ch8Label.Visible = false;


              //      YawDiff1NumericUpDown.Visible = false;
              //      YawDiff1Label.Visible = false;

              //      YawDiff2NumericUpDown.Visible = false;
              //      YawDiff2Label.Visible = false;

                    break;
                case "UAVXLight":
                    linear1GroupBox.Visible = false;

                    groupBoxAltitude1.Visible = false;
                    groupBoxAltitude2.Visible = false;

                    groupBoxGPS1.Visible = false;
                    groupBoxGPS2.Visible = false;

                    GPSGain1Label.Visible = false;
                    GPSGain1NumericUpDown.Visible = false;

                    labelThrottleGS1.Visible = false;
                    ThrottleGS1NumericUpDown.Visible = false;

                    bit0Header1Label.Visible = true;
                    bit01CheckBox.Visible = true; 
                    bit1Header1Label.Visible = false;
                    bit11CheckBox.Visible = false;
                    bit2Header1Label.Visible = true;
                    bit21CheckBox.Visible = true;
                    bit3Header1Label.Visible = true;
                    bit31CheckBox.Visible = true;
                    bit4Header1Label.Visible = false;
                    bit41CheckBox.Visible = false;
                    bit5Header1Label.Visible = false;
                    bit51CheckBox.Visible = false;
                    bit6Header1Label.Visible = false;
                    bit61CheckBox.Visible = false;

                    bit4Header1Label.Visible = false;
                    bit41CheckBox.Visible = false;

                    PWMSenseLabel.Visible = true;
                    Sense01Button.Visible = true;
                    Sense11Button.Visible = true;
                    Sense21Button.Visible = true;
                    Sense31Button.Visible = true;
                    Sense41Button.Visible = true;
                    Sense51Button.Visible = true;

                    labelNavMagVar1.Visible = false;
                    NavMagVar1NumericUpDown.Visible = false;

                    CompassOffset1Label.Visible = false;
                    CompassOffset1NumericUpDown.Visible = false;
                    Compass2label.Visible = false;
                    Compass2NumericUpDown.Visible = false;

                    Compass1Label.Visible = false;
                    Compass1NumericUpDown.Visible = false;

                    GPSGain1Label.Visible = false;
                    GPSGain1NumericUpDown.Visible = false;

                    DescDelayLabel1.Visible = true;
                    DescDelay1NumericUpDown.Visible = true;

                    LabelHoverThrottle1.Visible = true;
                    HoverThrottle1NumericUpDown.Visible = true;

                    CameraRoll1Label.Visible = false;
                    CameraRoll1NumericUpDown.Visible = false;

                    CameraRollTrim1Label.Visible = false;
                    CameraRollTrim1NumericUpDown.Visible = false;

                    CameraPitch1Label.Visible = false; 
                    CameraPitch1NumericUpDown.Visible = false;

                    DescDelayLabel1.Visible = false;
                    DescDelay1NumericUpDown.Visible = false;

                    labelThrottleGS1.Visible = false;
                    ThrottleGS1NumericUpDown.Visible = false;

                    labelESC1.Visible = true;
                    ESCComboBox1.Visible = true;

                    labelTelemetry1.Visible = true;
                    TelemetryComboBox1.Visible = true;

                    labelGyros1.Visible = true;
                    GyroComboBox1.Visible = true;

                    LabelHoverThrottle1.Visible = false;
                    HoverThrottle1NumericUpDown.Visible = false;

                    BatteryCapacity1Label.Visible = false;
                    BatteryCapacity1NumericUpDown.Visible = false;

                 //   HorizDamping1NumericUpDown.Visible = false;
                 //   HorizDamping1Label.Visible = false;

                //    HorizDampingDecay1NumericUpDown.Visible = false;
                //    HorizDampingDecay1Label.Visible = false;

                //    YawDiff1NumericUpDown.Visible = false;
                 //   YawDiff1Label.Visible = false;

                //    YawDiff2NumericUpDown.Visible = false;
                //    YawDiff2Label.Visible = false;

                    break;
            }
            versionToolStripComboBox.Text = Properties.Settings.Default.version;
        }
       
        private void LoadSettings(string args)
        {
            if (File.Exists(args) == true)
            {
                try
                {
                    parameterSets.laden(args, this);
                }
                catch (Exception exp)
                { 
                
                }
            }
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.mainFormHeight = this.Height;
            Properties.Settings.Default.mainFormWidth = this.Width;
            Properties.Settings.Default.listViewWidth = splitContainer1.SplitterDistance;
            Properties.Settings.Default.Save();
        }

        private void listViewJobs_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listViewJobs.SelectedItems.Count == 0) return;

            // Verbinden
            if (listViewJobs.SelectedItems[0].Index == 0) 
                picConnect.connect(this, false, true);
            // Set 1 oder 2 aktiv?
            //if (listViewJobs.SelectedItems[0].Index == 1) 
                picConnect.parameterSet(this);
            //lesen der Parameter
            if (listViewJobs.SelectedItems[0].Index == 1) 
                picConnect.readParameters(this, parameterSets);
            //schreiben der Parameter
            if (listViewJobs.SelectedItems[0].Index == 2) 
                picConnect.writeParameters(this, parameterSets);
            //Neutralwerte
            if (listViewJobs.SelectedItems[0].Index == 3) 
                picConnect.neutral(this);
            //Empf�ngerwerte
            if (listViewJobs.SelectedItems[0].Index == 4) 
                picConnect.receiver(this);

        }

        private void bitTextWechsel(Object changeBoxObject)
        {
            CheckBox changeBox = (CheckBox)changeBoxObject;
            if (changeBox.Checked == false) 
            {
                switch (changeBox.Name.Substring(0, 4)) 
                {
                    case "bit0": 
                        changeBox.Text = labels.GetString("bit0");
                        break;
                    case "bit1": 
                        changeBox.Text = labels.GetString("bit1");
                        break;
                    case "bit2": 
                        changeBox.Text = labels.GetString("bit2");
                        break;
                    case "bit3": 
                        changeBox.Text = labels.GetString("bit3");
                        break;
                    case "bit4": 
                        changeBox.Text = labels.GetString("bit4");
                        break;
                    case "bit5": 
                        changeBox.Text = labels.GetString("bit5");
                        break;
                    case "bit6":
                        changeBox.Text = labels.GetString("bit6");
                        break;
                    case "bit7":
                        changeBox.Text = labels.GetString("bit7");
                        break;
                }
            } 
            else 
            {
                switch (changeBox.Name.Substring(0, 4)) 
                {
                    case "bit0": 
                        changeBox.Text = labels.GetString("bit01");
                        break;
                    case "bit1": 
                        changeBox.Text = labels.GetString("bit11");
                        break;
                    case "bit2": 
                        changeBox.Text = labels.GetString("bit21");
                        break;
                    case "bit3": 
                        changeBox.Text = labels.GetString("bit31");
                        break;
                    case "bit4": 
                        changeBox.Text = labels.GetString("bit41");
                        break;
                    case "bit5": 
                        changeBox.Text = labels.GetString("bit51");
                        break;
                    case "bit6":
                        changeBox.Text = labels.GetString("bit61");
                        break;
                }
            }
        }

        //reaYawen auf �nderung in den Werten
        private void bit01CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit11CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit21CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit31CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (bit31CheckBox.Checked)
            {

                Ch9NumericUpDown.Visible = true;
                Ch9Label.Visible = true;

                Ch8NumericUpDown.Visible = true;
                Ch8Label.Visible = true;
            }
            else
            {
                Ch9NumericUpDown.Visible = false;
                Ch9Label.Visible = false;

                Ch8NumericUpDown.Visible = false;
                Ch8Label.Visible = false;
            }
            bitTextWechsel(sender);
        }

        private void bit41CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit51CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit61CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            //if (bit61CheckBox.Checked)
            if (false )
            {
                bit01CheckBox.Visible = false;
                bit0Header1Label.Visible = false;

                bit11CheckBox.Visible = false;
                bit1Header1Label.Visible = false;

                NavRTHAlt1NumericUpDown.Visible = false;
                labelNavRTHAlt1.Visible = false;
 
                NavRTHAlt2NumericUpDown.Visible = false;
                labelNavRTHAlt2.Visible = false;

                DescDelay1NumericUpDown.Visible = false;
                DescDelayLabel1.Visible = false;

                groupBoxGPS1.Visible = false;
                groupBoxGPS2.Visible = false;
            }
           // else
            if (false )
            {
                bit01CheckBox.Visible = true;
                bit0Header1Label.Visible = true;

                bit11CheckBox.Visible = true;
                bit1Header1Label.Visible = true;

                NavRTHAlt1NumericUpDown.Visible = true;
                labelNavRTHAlt1.Visible = true;

                NavRTHAlt2NumericUpDown.Visible = true;
                labelNavRTHAlt2.Visible = true;

                DescDelay1NumericUpDown.Visible = true;
                DescDelayLabel1.Visible = true;

                groupBoxGPS1.Visible = true;
                groupBoxGPS2.Visible = true;
            }

            bitTextWechsel(sender);
        }

        private void bit62CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }


        private void bit72CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit02CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit12CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit22CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit32CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void bit42CheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bitTextWechsel(sender);
        }

        private void cOMPortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Configuration configwindow = new Configuration(this);
            configwindow.ShowDialog();
        }

        private void configLadenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //Filter auf Setupdateien
            parameterOpenFileDialog.Filter = "auv - Setupdateien (*.auv)|*.auv|alle Dateien (*.*)|*.*";
            //Letzten Pfad aus den Settings holen
            parameterOpenFileDialog.InitialDirectory = Properties.Settings.Default.openFolder;
            if (parameterOpenFileDialog.ShowDialog() == DialogResult.OK) 
            {
                parameterSets.laden(parameterOpenFileDialog.FileName, this);
                // Pfad f�r n�chtes mal speichern
                Properties.Settings.Default.openFolder = parameterOpenFileDialog.InitialDirectory;
            }
        }


        private void feldUpdaten_Click_KeyDown(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
            BatteryValue1Label.Text = (Decimal.Round(Battery1NumericUpDown.Value * 0.2m, 1)).ToString(); // 0.2V units
        }

        private void feldUpdaten_KeyDown(object sender, KeyEventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
            BatteryValue1Label.Text = (Decimal.Round(Battery1NumericUpDown.Value * 0.2m, 1)).ToString(); // 0.2V units
        }

        private void configSpeichernToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //Filter auf Setupdateine
            parameterSaveFileDialog.Filter = "auv - Setupdateien (*.auv)|*.auv";
            //Vorbelegen des Pfades aus den Settings
            parameterSaveFileDialog.InitialDirectory = Properties.Settings.Default.saveFolder;
            if (parameterSaveFileDialog.ShowDialog() == DialogResult.OK) 
            {
                parameterSets.speichern(parameterSaveFileDialog.FileName, this);
                //speichern des Pfades
                Properties.Settings.Default.saveFolder = parameterSaveFileDialog.InitialDirectory;
            }
        }
 
        private void infoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Info info = new Info();
            info.ShowDialog();
        }

        private void infoGetFocus(object sender, EventArgs e)
        {
            Hilfe.info(this);
            if (sender.GetType().Name == "NumericUpDown")
            {
                NumericUpDown temp = (NumericUpDown)sender;
                temp.Select(0, 3);
            }
        }

        private void logLevelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Configuration configwindow = new Configuration(this);
            configwindow.configurationTabControl.SelectedIndex = 1;
            configwindow.ShowDialog();
        }

        /// <summary>
        /// Aufruf �ber Men�
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void connectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.connect(this, false, true);
        }

        private void parameterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.parameterSet(this);
        }

        private void readConfigToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.readParameters(this, parameterSets);
        }

        private void writeConfigToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.writeParameters(this, parameterSets);
        }

        private void neutralToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.neutral(this);
        }

        private void peceiverToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.receiver(this);
        }

        private void BurnPICToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // sicherheitsabfrage kann �ber die Config eingestellt werden
            if (Properties.Settings.Default.AskBurnPIC == true) 
            {
                if (MessageBox.Show(labels.GetString("savePICBurn"), "Flash PIC?", MessageBoxButtons.OKCancel) == 
                    DialogResult.OK) 
                    burn();
            } 
            else 
                burn();
        }
        
        void burn()
        {
            //Hex laden
            hexOpenFileDialog.Filter = "hex - Firmware (*.hex)|*.hex|alle Dateien (*.*)|*.*";
            //letzten Pfad aus Settings holen
            hexOpenFileDialog.InitialDirectory = Properties.Settings.Default.pathHex;
            if (hexOpenFileDialog.ShowDialog() == DialogResult.OK) 
            {
                picConnect.BurnPIC(this, hexOpenFileDialog.FileName);
                //Pfad f�r n�chstes mal speichern
                Properties.Settings.Default.pathHex = hexOpenFileDialog.InitialDirectory;
            }
        }

        private void miscellaneousToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Configuration configwindow = new Configuration(this);
            //Configurationswindow im richtigen Tab �ffnen
            configwindow.configurationTabControl.SelectedIndex = 2;
            configwindow.ShowDialog();
        }

        private void comPortToolStripComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //neuen Comport den settings zuweisen und abspeichern
            Properties.Settings.Default.comPort = comPortToolStripComboBox.Text;
            Properties.Settings.Default.Save();
        }

        private void versionToolStripComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.version = versionToolStripComboBox.Text;
            ChangeVersion();
        }

        private void parameterToolStripMenuItem1_Click_1(object sender, EventArgs e)
        {
            ViewParameter viewParameter = new ViewParameter(parameterSets, this);
            viewParameter.ShowDialog();
        }

        private void testsoftwareToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.testSoftware(this);
        }

        private void splitContainer2_SplitterMoved(object sender, SplitterEventArgs e)
        {

        }

        private void debugToolStripMenuItem_Click(object sender, EventArgs e)
        {
            picConnect.debugSoftware(this);
        }

        private void set1Set2ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            parameterSets.copySet1ToSet2(this);
        }

        private void set2Set1ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            parameterSets.copySet2ToSet1(this);
        }

        private void FormMain_Leave(object sender, EventArgs e)
        {
            
        }

        private void FormMain_Deactivate(object sender, EventArgs e)
        {
            // special for wolfgang :-)
            if (Properties.Settings.Default.focus == true)
                picConnect.closeConnection(this);
        }

        private void TelemetryComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
        } 

        private void ESCComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);     
        }

       
        private void GyroComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
        }

        private void TxRxComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
        }

        private void tabControlParameter_SelectedIndexChanged(object sender, EventArgs e)
        {
            picConnect.setparamset(this);
        }

        private void General2GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void SenseButton0_Click(object sender, EventArgs e)
        {
            SenseButton[0] = !SenseButton[0];
            infoTextBox.Text = "Throttle sense - INACTIVE.";
            parameterSets.feldUpdaten(sender, this);
        }

        private void SenseButton1_Click(object sender, EventArgs e)
        {
            SenseButton[1] = !SenseButton[1];
            infoTextBox.Text = "Aileron or Right Elevon servo sense - not for multicopter use - grey is reversed.";
            parameterSets.feldUpdaten(sender, this);
        }

        private void SenseButton2_Click(object sender, EventArgs e)
        {
            SenseButton[2] = !SenseButton[2];
            infoTextBox.Text = "Elevator or Left Elevon servo sense - not for multicopter use - grey is reversed.";
            parameterSets.feldUpdaten(sender, this);
        }

        private void SenseButton3_Click(object sender, EventArgs e)
        {
            SenseButton[3] = !SenseButton[3];
            infoTextBox.Text = "Rudder servo sense - not for multicopter use - grey is reversed.";
            parameterSets.feldUpdaten(sender, this);
        }

        private void SenseButton4_Click(object sender, EventArgs e)
        {
            SenseButton[4] = !SenseButton[4];
            infoTextBox.Text = "Camera roll servo sense - grey is reversed.";
            parameterSets.feldUpdaten(sender, this);
        }

        private void SenseButton5_Click(object sender, EventArgs e)
        {
            SenseButton[5] = !SenseButton[5];
            infoTextBox.Text = "Camera pitch servo sense -  grey is reversed.";
            parameterSets.feldUpdaten(sender, this);
        }

        private void PPMPolarityButton_Click(object sender, EventArgs e)
        {
            SenseButton[6] = !SenseButton[6];
            infoTextBox.Text = "Blue is negative polarity";
            parameterSets.feldUpdaten(sender, this);
        }

        private void ColourButton_Click(object sender, System.EventArgs e)
        {
            ColorDialog MyDialog = new ColorDialog();
            // Keeps the user from selecting a custom color.
            //MyDialog.AllowFullOpen = false;
            // Allows the user to get help. (The default is false.)
            MyDialog.ShowHelp = true;
            // Sets the initial color select to the current text color.
            MyDialog.Color = tabPageParameterSet1.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                tabPageParameterSet1.BackColor = MyDialog.Color;
                tabPageParameterSet2.BackColor = MyDialog.Color;
                Properties.Settings.Default.Colour = MyDialog.Color;
            }
        }

      
     
      
    }
}
