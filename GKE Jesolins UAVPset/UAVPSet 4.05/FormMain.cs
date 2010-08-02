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
        // Texte für Mehrsprachigkeit
        public ResourceManager help;
        ResourceManager labels;
        public ResourceManager errorLabels;
        // bei write feldupdate auf grün ausschalten
        public bool writeUpdate = false;
        //Applikationspfad
        public string pfad;
        public bool picBootModus = false;

        public bool[] SenseButton = new bool[8];
        
        public FormMain(string args)
        {
            // sprache übersteuern
            // wenn auto dann bestriebssystem einstellungen

            switch (Properties.Settings.Default.language)
            { 
                case "english":
                    Thread.CurrentThread.CurrentUICulture = new CultureInfo("en");
                    break;
                case "german":
                    Thread.CurrentThread.CurrentUICulture = new CultureInfo("de");
                    break;
                case "french":
                    Thread.CurrentThread.CurrentUICulture = new CultureInfo("fr");
                   break;
           }

            InitializeComponent();
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
         }

        /// <summary>
        /// Umstellen der Oberfläche je Version
        /// </summary>
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

                    labelGPSGain1.Visible = true;
                    GPSGain1NumericUpDown.Visible = true;

                    labelBaroScale1.Visible = true;
                    BaroScale1NumericUpDown.Visible = true;

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

                    labelNavMagVar1.Visible = true;
                    NavMagVar1NumericUpDown.Visible = true;

                    CompassOffset1Label.Visible = true;
                    CompassOffset1NumericUpDown.Visible = true;

                    Compass1Label.Visible = true;
                    Compass1NumericUpDown.Visible = true;
                    Compass2label.Visible = true;
                    Compass2NumericUpDown.Visible = true;

                    labelGPSGain1.Visible = true;
                    GPSGain1NumericUpDown.Visible = true;

                    labelDescDelay1.Visible = true;
                    DescDelay1NumericUpDown.Visible = true;

                    LabelHoverThrottle1.Visible = true;
                    HoverThrottle1NumericUpDown.Visible = true;

                    CameraRoll1Label.Visible = true;
                    CameraRoll1NumericUpDown.Visible = true;
                    CameraRoll2Label.Visible = true;
                    CameraRoll2NumericUpDown.Visible = true;
                    CameraRollTrim1Label.Visible = true;
                    CameraRollTrim1NumericUpDown.Visible = true;
                    CameraRollTrim2Label.Visible = true;
                    CameraRollTrim2NumericUpDown.Visible = true;

                    CameraPitch1Label.Visible = true;
                    CameraPitch1NumericUpDown.Visible = true;
                    CameraPitch2Label.Visible = true;
                    CameraPitch2NumericUpDown.Visible = true;

                    labelDescDelay1.Visible = true;
                    DescDelay1NumericUpDown.Visible = true;

                    labelBaroScale1.Visible = true;
                    BaroScale1NumericUpDown.Visible = true;

                    labelESC1.Visible = true;
                    ESCComboBox1.Visible = true;

                    LabelTxRx1.Visible = true;
                    TxRxComboBox1.Visible = true;

                    labelTelemetry1.Visible = true;
                    TelemetryComboBox1.Visible = true;

                    labelGyroRollPitch1.Visible = true;
                    GyroRollPitchComboBox1.Visible = true;

                    labelGyroYaw1.Visible = true;
                    GyroYawComboBox1.Visible = true;

                    LabelHoverThrottle1.Visible = true;
                    HoverThrottle1NumericUpDown.Visible = true;

                    BatteryCapacity1Label.Visible = true;
                    BatteryCapacity1NumericUpDown.Visible = true;

                    break;
                case "UAVXLight":
                    linear1GroupBox.Visible = false;

                    groupBoxAltitude1.Visible = false;
                    groupBoxAltitude2.Visible = false;

                    groupBoxGPS1.Visible = false;
                    groupBoxGPS2.Visible = false;

                    labelGPSGain1.Visible = false;
                    GPSGain1NumericUpDown.Visible = false;

                    labelBaroScale1.Visible = false;
                    BaroScale1NumericUpDown.Visible = false;

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

                    labelGPSGain1.Visible = false;
                    GPSGain1NumericUpDown.Visible = false;

                    labelDescDelay1.Visible = true;
                    DescDelay1NumericUpDown.Visible = true;

                    LabelHoverThrottle1.Visible = true;
                    HoverThrottle1NumericUpDown.Visible = true;

                    CameraRoll1Label.Visible = false;
                    CameraRoll1NumericUpDown.Visible = false;
                    CameraRoll2Label.Visible = false;
                    CameraRoll2NumericUpDown.Visible = false;
                    CameraRollTrim1Label.Visible = false;
                    CameraRollTrim1NumericUpDown.Visible = false;
                    CameraRollTrim2Label.Visible = false;
                    CameraRollTrim2NumericUpDown.Visible = false;

                    CameraPitch1Label.Visible = false; 
                    CameraPitch1NumericUpDown.Visible = false;
                    CameraPitch2Label.Visible = false;
                    CameraPitch2NumericUpDown.Visible = false;

                    labelDescDelay1.Visible = false;
                    DescDelay1NumericUpDown.Visible = false;

                    labelBaroScale1.Visible = false;
                    BaroScale1NumericUpDown.Visible = false;

                    labelESC1.Visible = true;
                    ESCComboBox1.Visible = true;

                    LabelTxRx1.Visible = true;
                    TxRxComboBox1.Visible = true;

                    labelTelemetry1.Visible = true;
                    TelemetryComboBox1.Visible = true;

                    labelGyroRollPitch1.Visible = true;
                    GyroRollPitchComboBox1.Visible = true;

                    labelGyroYaw1.Visible = true;
                    GyroYawComboBox1.Visible = true;

                    LabelHoverThrottle1.Visible = false;
                    HoverThrottle1NumericUpDown.Visible = false;

                    BatteryCapacity1Label.Visible = false;
                    BatteryCapacity1NumericUpDown.Visible = false;

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


        /// <summary>
        /// Speichern des Layouts wenn Form geschlossen
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.mainFormHeight = this.Height;
            Properties.Settings.Default.mainFormWidth = this.Width;
            Properties.Settings.Default.listViewWidth = splitContainer1.SplitterDistance;
            Properties.Settings.Default.Save();
        }

        /// <summary>
        /// wenn in der ListView ein element angeklickt wurde
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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
            //Empfängerwerte
            if (listViewJobs.SelectedItems[0].Index == 4) 
                picConnect.receiver(this);

        }
   
        /// <summary>
        /// Funktion um die Labels bei den Checkboxen Zentral zu ändern
        /// </summary>
        /// <param name="changeBox">zu ändernde Checkbox als Object</param>
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

        //reaYawen auf änderung in den Werten
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

        
        /// <summary>
        /// Laden der Parameterwerte
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configLadenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //Filter auf Setupdateien
            parameterOpenFileDialog.Filter = "auv - Setupdateien (*.auv)|*.auv|alle Dateien (*.*)|*.*";
            //Letzten Pfad aus den Settings holen
            parameterOpenFileDialog.InitialDirectory = Properties.Settings.Default.openFolder;
            if (parameterOpenFileDialog.ShowDialog() == DialogResult.OK) 
            {
                parameterSets.laden(parameterOpenFileDialog.FileName, this);
                // Pfad für nächtes mal speichern
                Properties.Settings.Default.openFolder = parameterOpenFileDialog.InitialDirectory;
            }
        }

        /// <summary>
        /// allgemeine Funktion um alle Felder upzudaten
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void feldUpdaten_Click_KeyDown(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
            BatteryValue1Label.Text = (Decimal.Round(Battery1NumericUpDown.Value / 4.6m, 1)).ToString();
        }


        /// <summary>
        /// allgemeine Funktion um alle Felder upzudaten
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void feldUpdaten_KeyDown(object sender, KeyEventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
            BatteryValue1Label.Text = (Decimal.Round(Battery1NumericUpDown.Value / 4.6m, 1)).ToString();
        }

        /// <summary>
        /// speichern der Parameter
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
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

        /// <summary>
        /// anzeige von Programminfos
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void infoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Info info = new Info();
            info.ShowDialog();
        }

        /// <summary>
        /// anzeige der Hilfe für die Werte
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void infoGetFocus(object sender, EventArgs e)
        {
            Hilfe.info(this);
            if (sender.GetType().Name == "NumericUpDown")
            {
                NumericUpDown temp = (NumericUpDown)sender;
                temp.Select(0, 3);
            }
        }

        /// <summary>
        /// anzeige der Configuration
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void logLevelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Configuration configwindow = new Configuration(this);
            configwindow.configurationTabControl.SelectedIndex = 1;
            configwindow.ShowDialog();
        }

        /// <summary>
        /// Aufruf über Menü
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
            // sicherheitsabfrage kann über die Config eingestellt werden
            if (Properties.Settings.Default.AskBurnPIC == true) 
            {
                if (MessageBox.Show(labels.GetString("savePICBurn"), "Flash PIC?", MessageBoxButtons.OKCancel) == 
                    DialogResult.OK) 
                    burn();
            } 
            else 
                burn();
        }
        
        /// <summary>
        /// auslager für brunPicToolStripMenuItem_Click
        /// </summary>
        void burn()
        {
            //Hex laden
            hexOpenFileDialog.Filter = "hex - Firmware (*.hex)|*.hex|alle Dateien (*.*)|*.*";
            //letzten Pfad aus Settings holen
            hexOpenFileDialog.InitialDirectory = Properties.Settings.Default.pathHex;
            if (hexOpenFileDialog.ShowDialog() == DialogResult.OK) 
            {
                picConnect.BurnPIC(this, hexOpenFileDialog.FileName);
                //Pfad für nächstes mal speichern
                Properties.Settings.Default.pathHex = hexOpenFileDialog.InitialDirectory;
            }
        }

        private void miscellaneousToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Configuration configwindow = new Configuration(this);
            //Configurationswindow im richtigen Tab öffnen
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

       
        private void GyroRollPitchComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
        }

        private void GyroYawComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
        }


        private void TxRxComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
        }


        private void RollProp1Label_Click(object sender, EventArgs e)
        {

        }

        private void tabControlParameter_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void General2GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void groupBoxGPS1_Enter(object sender, EventArgs e)
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

     
       
    }
}
