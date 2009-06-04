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
            {
                comPortToolStripComboBox.Items.Add(aviableComPort);
            }
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
                //TODO: Hier die Versionsfelder ergänzen!
                case "3.12":
                    rollLimit1Label.Visible = true;
                    rollLimit1NumericUpDown.Visible = true;
                    nickLimit1Label.Visible = true;
                    nickLimit1NumericUpDown.Visible = true;
                    rollLimit2Label.Visible = true;
                    rollLimit2NumericUpDown.Visible = true;
                    nickLimit2Label.Visible = true;
                    nickLimit2NumericUpDown.Visible = true;
                    baro1Label.Visible = false;
                    baro1NumericUpDown.Visible = false;
                    baroTemp1Label.Visible = false;
                    baroTemp1NumericUpDown.Visible = false;
                    baro2Label.Visible = false;
                    baro2NumericUpDown.Visible = false;
                    baroTemp2Label.Visible = false;
                    baroTemp2NumericUpDown.Visible = false;
                    baroInt1Label.Visible = false;
                    baroInt1NumericUpDown.Visible = false;
                    baroInt2Label.Visible = false;
                    baroInt2NumericUpDown.Visible = false;
                    rollMa1label.Visible = true;
                    rollMa1NumericUpDown.Visible = true;
                    nickMa1Label.Visible = true;
                    nickMa1NumericUpDown.Visible = true;
                    gierMa1Label.Visible = true;
                    gierMa1NumericUpDown.Visible = true;
                    rollMa2Label.Visible = true;
                    rollMa2NumericUpDown.Visible = true;
                    nickMa2Label.Visible = true;
                    nickMa2NumericUpDown.Visible = true;
                    gierMa2Label.Visible = true;
                    gierMa2NumericUpDown.Visible = true;
                    kameraNick1Label.Visible = false;
                    kameraRoll1Label.Visible = false;
                    kameraRoll1NumericUpDown.Visible = false;
                    kameraNick2Label.Visible = false;
                    kameraRoll2Label.Visible = false;
                    kameraRoll2NumericUpDown.Visible = false;
                    groupBoxGPS1.Visible = false;
                    groupBoxGPS2.Visible = false;
                    labelGPSD1.Visible = false;
                    labelGPSD2.Visible = false;
                    labelGPSP1.Visible = false;
                    labelGPSP2.Visible = false;
                    labelGPSSmooth1.Visible = false;
                    labelGPSSmooth2.Visible = false;
                    numericUpDownGPSD1.Visible = false;
                    numericUpDownGPSD2.Visible = false;
                    numericUpDownGPSP1.Visible = false;
                    numericUpDownGPSP2.Visible = false;
                    numericUpDownGPSSmooth1.Visible = false;
                    numericUpDownGPSSmooth2.Visible = false;
                    break;
                case "3.13":
                    rollLimit1Label.Visible = false;
                    rollLimit1NumericUpDown.Visible = false;
                    nickLimit1Label.Visible = false;
                    nickLimit1NumericUpDown.Visible = false;
                    rollLimit2Label.Visible = false;
                    rollLimit2NumericUpDown.Visible = false;
                    nickLimit2Label.Visible = false;
                    nickLimit2NumericUpDown.Visible = false;
                    baro1Label.Visible = false;
                    baro1NumericUpDown.Visible = false;
                    baroTemp1Label.Visible = false;
                    baroTemp1NumericUpDown.Visible = false;
                    baro2Label.Visible = false;
                    baro2NumericUpDown.Visible = false;
                    baroTemp2Label.Visible = false;
                    baroTemp2NumericUpDown.Visible = false;
                    baroInt1Label.Visible = false;
                    baroInt1NumericUpDown.Visible = false;
                    baroInt2Label.Visible = false;
                    baroInt2NumericUpDown.Visible = false;
                    rollMa1label.Visible = true;
                    rollMa1NumericUpDown.Visible = true;
                    nickMa1Label.Visible = true;
                    nickMa1NumericUpDown.Visible = true;
                    gierMa1Label.Visible = true;
                    gierMa1NumericUpDown.Visible = true;
                    rollMa2Label.Visible = true;
                    rollMa2NumericUpDown.Visible = true;
                    nickMa2Label.Visible = true;
                    nickMa2NumericUpDown.Visible = true;
                    gierMa2Label.Visible = true;
                    gierMa2NumericUpDown.Visible = true;
                    kameraNick1Label.Visible = false;
                    kameraRoll1Label.Visible = false;
                    kameraRoll1NumericUpDown.Visible = false;
                    kameraNick2Label.Visible = false;
                    kameraRoll2Label.Visible = false;
                    kameraRoll2NumericUpDown.Visible = false;
                    groupBoxGPS1.Visible = false;
                    groupBoxGPS2.Visible = false;
                    labelGPSD1.Visible = false;
                    labelGPSD2.Visible = false;
                    labelGPSP1.Visible = false;
                    labelGPSP2.Visible = false;
                    labelGPSSmooth1.Visible = false;
                    labelGPSSmooth2.Visible = false;
                    numericUpDownGPSD1.Visible = false;
                    numericUpDownGPSD2.Visible = false;
                    numericUpDownGPSP1.Visible = false;
                    numericUpDownGPSP2.Visible = false;
                    numericUpDownGPSSmooth1.Visible = false;
                    numericUpDownGPSSmooth2.Visible = false;
                    break;
                case "3.14":
                    rollLimit1Label.Visible = false;
                    rollLimit1NumericUpDown.Visible = false;
                    nickLimit1Label.Visible = false;
                    nickLimit1NumericUpDown.Visible = false;
                    rollLimit2Label.Visible = false;
                    rollLimit2NumericUpDown.Visible = false;
                    nickLimit2Label.Visible = false;
                    nickLimit2NumericUpDown.Visible = false;
                    baro1Label.Visible = true;
                    baro1NumericUpDown.Visible = true;
                    baroTemp1Label.Visible = true;
                    baroTemp1NumericUpDown.Visible = true;
                    baro2Label.Visible = true;
                    baro2NumericUpDown.Visible = true;
                    baroTemp2Label.Visible = true;
                    baroTemp2NumericUpDown.Visible = true;
                    baroInt1Label.Visible = true;
                    baroInt1NumericUpDown.Visible = true;
                    baroInt2Label.Visible = true;
                    baroInt2NumericUpDown.Visible = true;
                    rollMa1label.Visible = true;
                    rollMa1NumericUpDown.Visible = true;
                    nickMa1Label.Visible = true;
                    nickMa1NumericUpDown.Visible = true;
                    gierMa1Label.Visible = true;
                    gierMa1NumericUpDown.Visible = true;
                    rollMa2Label.Visible = true;
                    rollMa2NumericUpDown.Visible = true;
                    nickMa2Label.Visible = true;
                    nickMa2NumericUpDown.Visible = true;
                    gierMa2Label.Visible = true;
                    gierMa2NumericUpDown.Visible = true;
                    kameraNick1Label.Visible = false;
                    kameraRoll1Label.Visible = false;
                    kameraRoll1NumericUpDown.Visible = false;
                    kameraNick2Label.Visible = false;
                    kameraRoll2Label.Visible = false;
                    kameraRoll2NumericUpDown.Visible = false;
                    groupBoxGPS1.Visible = false;
                    groupBoxGPS2.Visible = false;
                    labelGPSD1.Visible = false;
                    labelGPSD2.Visible = false;
                    labelGPSP1.Visible = false;
                    labelGPSP2.Visible = false;
                    labelGPSSmooth1.Visible = false;
                    labelGPSSmooth2.Visible = false;
                    numericUpDownGPSD1.Visible = false;
                    numericUpDownGPSD2.Visible = false;
                    numericUpDownGPSP1.Visible = false;
                    numericUpDownGPSP2.Visible = false;
                    numericUpDownGPSSmooth1.Visible = false;
                    numericUpDownGPSSmooth2.Visible = false;   
                    break;
                case "3.15":
                    rollLimit1Label.Visible = false;
                    rollLimit1NumericUpDown.Visible = false;
                    nickLimit1Label.Visible = false;
                    nickLimit1NumericUpDown.Visible = false;
                    rollLimit2Label.Visible = false;
                    rollLimit2NumericUpDown.Visible = false;
                    nickLimit2Label.Visible = false;
                    nickLimit2NumericUpDown.Visible = false;
                    baro1Label.Visible = true;
                    baro1NumericUpDown.Visible = true;
                    baroTemp1Label.Visible = true;
                    baroTemp1NumericUpDown.Visible = true;
                    baro2Label.Visible = true;
                    baro2NumericUpDown.Visible = true;
                    baroTemp2Label.Visible = true;
                    baroTemp2NumericUpDown.Visible = true;
                    baroInt1Label.Visible = true;
                    baroInt1NumericUpDown.Visible = true;
                    baroInt2Label.Visible = true;
                    baroInt2NumericUpDown.Visible = true;
                    rollMa1label.Visible = false;
                    rollMa1NumericUpDown.Visible = false;
                    nickMa1Label.Visible = false;
                    nickMa1NumericUpDown.Visible = false;
                    gierMa1Label.Visible = true;
                    gierMa1NumericUpDown.Visible = true;
                    rollMa2Label.Visible = false;
                    rollMa2NumericUpDown.Visible = false;
                    nickMa2Label.Visible = false;
                    nickMa2NumericUpDown.Visible = false;
                    gierMa2Label.Visible = true;
                    gierMa2NumericUpDown.Visible = true;
                    kameraNick1Label.Visible = true;
                    kameraRoll1Label.Visible = true;
                    kameraRoll1NumericUpDown.Visible = true;
                    kameraNick2Label.Visible = true;
                    kameraRoll2Label.Visible = true;
                    kameraRoll2NumericUpDown.Visible = true;
                    groupBoxGPS1.Visible = false;
                    groupBoxGPS2.Visible = false;
                    labelGPSD1.Visible = false;
                    labelGPSD2.Visible = false;
                    labelGPSP1.Visible = false;
                    labelGPSP2.Visible = false;
                    labelGPSSmooth1.Visible = false;
                    labelGPSSmooth2.Visible = false;
                    numericUpDownGPSD1.Visible = false;
                    numericUpDownGPSD2.Visible = false;
                    numericUpDownGPSP1.Visible = false;
                    numericUpDownGPSP2.Visible = false;
                    numericUpDownGPSSmooth1.Visible = false;
                    numericUpDownGPSSmooth2.Visible = false;
                    break;
                case "3.16":
                    rollLimit1Label.Visible = false;
                    rollLimit1NumericUpDown.Visible = false;
                    nickLimit1Label.Visible = false;
                    nickLimit1NumericUpDown.Visible = false;
                    rollLimit2Label.Visible = false;
                    rollLimit2NumericUpDown.Visible = false;
                    nickLimit2Label.Visible = false;
                    nickLimit2NumericUpDown.Visible = false;
                    baro1Label.Visible = true;
                    baro1NumericUpDown.Visible = true;
                    baroTemp1Label.Visible = true;
                    baroTemp1NumericUpDown.Visible = true;
                    baro2Label.Visible = true;
                    baro2NumericUpDown.Visible = true;
                    baroTemp2Label.Visible = true;
                    baroTemp2NumericUpDown.Visible = true;
                    baroInt1Label.Visible = true;
                    baroInt1NumericUpDown.Visible = true;
                    baroInt2Label.Visible = true;
                    baroInt2NumericUpDown.Visible = true;
                    rollMa1label.Visible = false;
                    rollMa1NumericUpDown.Visible = false;
                    nickMa1Label.Visible = false;
                    nickMa1NumericUpDown.Visible = false;
                    gierMa1Label.Visible = true;
                    gierMa1NumericUpDown.Visible = true;
                    rollMa2Label.Visible = false;
                    rollMa2NumericUpDown.Visible = false;
                    nickMa2Label.Visible = false;
                    nickMa2NumericUpDown.Visible = false;
                    gierMa2Label.Visible = true;
                    gierMa2NumericUpDown.Visible = true;
                    kameraNick1Label.Visible = true;
                    kameraRoll1Label.Visible = true;
                    kameraRoll1NumericUpDown.Visible = true;
                    kameraNick2Label.Visible = true;
                    kameraRoll2Label.Visible = true;
                    kameraRoll2NumericUpDown.Visible = true;
                    groupBoxGPS1.Visible = true;
                    groupBoxGPS2.Visible = true;
                    labelGPSD1.Visible = true;
                    labelGPSD2.Visible = true;
                    labelGPSP1.Visible = true;
                    labelGPSP2.Visible = true;
                    labelGPSSmooth1.Visible = true;
                    labelGPSSmooth2.Visible = true;
                    numericUpDownGPSD1.Visible = true;
                    numericUpDownGPSD2.Visible = true;
                    numericUpDownGPSP1.Visible = true;
                    numericUpDownGPSP2.Visible = true;
                    numericUpDownGPSSmooth1.Visible = true;
                    numericUpDownGPSSmooth2.Visible = true;
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
            {
                picConnect.connect(this, false, true);
            }
            // Set 1 oder 2 aktiv?
            //if (listViewJobs.SelectedItems[0].Index == 1) 
            {
                picConnect.parameterSet(this);
            }
            //lesen der Parameter
            if (listViewJobs.SelectedItems[0].Index == 1) 
            {
                picConnect.readParameters(this, parameterSets);
            }
            //schreiben der Parameter
            if (listViewJobs.SelectedItems[0].Index == 2) 
            {
                picConnect.writeParameters(this, parameterSets);
            }
            //Neutralwerte
            if (listViewJobs.SelectedItems[0].Index == 3) 
            {
                picConnect.neutral(this);
            }
            //Empfängerwerte
            if (listViewJobs.SelectedItems[0].Index == 4) 
            {
                picConnect.receiver(this);
            }

        }

        /// <summary>
        /// Funktion um die Volt bei der Akku Unterspannung zu errechnen
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void akku1NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            double akkuValue1 = Math.Round((double)akku1NumericUpDown.Value * 4.6d,0,MidpointRounding.AwayFromZero);
            akkuValue1Label.Text = akkuValue1.ToString();
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
                    case "bit0": //Flugmodus
                        changeBox.Text = labels.GetString("bit0");
                        break;
                    case "bit1": //Graupner/Robbe
                        changeBox.Text = labels.GetString("bit1");
                        break;
                    case "bit2": //Integrierzustand
                        changeBox.Text = labels.GetString("bit2");
                        break;
                    case "bit3": //Specky Bit
                        changeBox.Text = labels.GetString("bit3");
                        break;
                    case "bit4": //Empfängerpulse
                        changeBox.Text = labels.GetString("bit4");
                        break;
                }
            } 
            else 
            {
                switch (changeBox.Name.Substring(0, 4)) 
                {
                    case "bit0": //Flugmodus
                        changeBox.Text = labels.GetString("bit01");
                        break;
                    case "bit1": //Graupner/Robbe
                        changeBox.Text = labels.GetString("bit11");
                        break;
                    case "bit2": //Integrierzustand
                        changeBox.Text = labels.GetString("bit21");
                        break;
                    case "bit3": //Specky Bit
                        changeBox.Text = labels.GetString("bit31");
                        break;
                    case "bit4": //Empfängerpulse
                        changeBox.Text = labels.GetString("bit41");
                        break;
                }
            }
        }



        //reagieren auf änderung in den Werten
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
            akkuValue1Label.Text = (Decimal.Round(akku1NumericUpDown.Value / 4.6m, 2)).ToString();
            akkuValue2Label.Text = (Decimal.Round(akku2NumericUpDown.Value / 4.6m, 2)).ToString();
        }


        /// <summary>
        /// allgemeine Funktion um alle Felder upzudaten
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void feldUpdaten_KeyDown(object sender, KeyEventArgs e)
        {
            parameterSets.feldUpdaten(sender, this);
            akkuValue1Label.Text = (Decimal.Round(akku1NumericUpDown.Value / 4.6m, 2)).ToString();
            akkuValue2Label.Text = (Decimal.Round(akku2NumericUpDown.Value / 4.6m, 2)).ToString();
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

        private void burnPicToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // sicherheitsabfrage kann über die Config eingestellt werden
            if (Properties.Settings.Default.askBurnPic == true) 
            {
                if (MessageBox.Show(labels.GetString("savePicBurn"), "Flash PIC?", MessageBoxButtons.OKCancel) == 
                    DialogResult.OK) 
                {
                    burn();
                }
            } 
            else 
            {
                burn();
            }
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
                picConnect.burnPic(this, hexOpenFileDialog.FileName);
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

        private void versionToolStripComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.version = versionToolStripComboBox.Text;
            //if (Properties.Settings.Default.version == "3.12" ||
            //    Properties.Settings.Default.version == "3.13")
            //{
            //    Properties.Settings.Default.baudRate = 38400;
            //}
            //else
            //{
            //    Properties.Settings.Default.baudRate = 115200;
            //}
            //Properties.Settings.Default.Save();
            ChangeVersion();
        }

        private void FormMain_Leave(object sender, EventArgs e)
        {
            
        }

        private void FormMain_Deactivate(object sender, EventArgs e)
        {
            // special for wolfgang :-)
            if (Properties.Settings.Default.focus == true)
            {
                picConnect.closeConnection(this);
            }
        }

        private void labelGPSD2_Click(object sender, EventArgs e)
        {

        }

        private void label4_Click(object sender, EventArgs e)
        {
        
        }

        private void listViewJobs_SelectedIndexChanged_1(object sender, EventArgs e)
        {

        }

        private void roll2GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void nick1GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void roll1GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void leerlaufgas2Label_Click(object sender, EventArgs e)
        {

        }

        private void sonst2GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void groupBoxGPS2_Enter(object sender, EventArgs e)
        {

        }

        private void linear2GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void infoTextBox_TextChanged(object sender, EventArgs e)
        {

        }

        private void rollMa2Label_Click(object sender, EventArgs e)
        {

        }

        private void nick2GroupBox_Enter(object sender, EventArgs e)
        {

        }

        private void baroInt1Label_Click(object sender, EventArgs e)
        {

        }

        private void baro1Label_Click(object sender, EventArgs e)
        {

        }

        private void baroTemp1Label_Click(object sender, EventArgs e)
        {

        }

        private void gierMa2Label_Click(object sender, EventArgs e)
        {

        }

        private void compass1Label_Click(object sender, EventArgs e)
        {

        }

        
    }
}
