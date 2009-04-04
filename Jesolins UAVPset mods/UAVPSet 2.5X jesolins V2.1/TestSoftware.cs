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
    public partial class TestSoftware : Form
    {
        FormMain mainForm;
        PICConnect picConnect = new PICConnect();
        public bool errorFlag = false;

        public TestSoftware(FormMain mainForm, PICConnect picConnect)
        {
            InitializeComponent();
            this.mainForm = mainForm;
            this.picConnect = picConnect;
            readFunctions();
        }

        /// <summary>
        /// einlesen aller möglichen Befehle mittels ?
        /// </summary>
        void readFunctions()
        {
            picConnect.connect(mainForm, true, true);
            ArrayList functions = picConnect.testComm(mainForm);

            if (functions.Count == 0)
            {
                errorFlag = true;
                return;
            }
            if (functions[1].ToString().Substring(0, 14) == "Profi-Ufo TEST")
            {
                label1.Text = "";
            }
            
            functions = picConnect.askPic(mainForm, "?");
            // nur ausführen wenn mehr als 4 Zeilen zurückkommen.
            // sonst gab es immer einen Fehler wenn keine Verbindung besteht - dann kommen 0 Zeile zurück
            if (functions.Count > 4)
            {
                functions.RemoveAt(0); // ersten beiden Zeilen löschen - sind nur Info Zeilen
                functions.RemoveAt(0);
                functions.RemoveAt(functions.Count - 1); //letzen beiden Zeilen löschen (nur info und >)
                functions.RemoveAt(functions.Count - 1);
                // alle Befehle durchgehen und in ListView eintragen
                foreach (string function in functions)
                {
                    ListViewItem listViewItem = new ListViewItem(function);
                    // wenn Befehl mit 1-8 Beginnt dann daraus 8 Einträge erstellen
                    if (listViewItem.Text.Substring(0, 2) == "1-")
                    {
                        int maxValue = Convert.ToInt16(listViewItem.Text.Substring(2, 2).Trim('.'));
                        string temp = listViewItem.Text;
                        for (int i = 1; i <= maxValue; i++)
                        {
                            listViewItem = new ListViewItem(function);
                            listViewItem.Text = i.ToString() + "." + temp.Substring(3, temp.Length - 4);
                            functionsListView.Items.Add(listViewItem);
                        }
                    }
                    else if (listViewItem.Text.Substring(0, 1) != "B")
                    { //B = Bootloader - nicht anzeigen!
                        listViewItem.Text = listViewItem.Text.Substring(0, listViewItem.Text.Length - 1);
                        functionsListView.Items.Add(listViewItem);
                    }
                }
            }
            else 
            {
                // aufrufende Funktion fragt Flag ab und zeigt form nicht an
                errorFlag = true;
            }
        }

        private void TestSoftware_FormClosing(object sender, FormClosingEventArgs e)
        {
            picConnect.closeConnection(mainForm);
        }

        /// <summary>
        /// reagieren wenn ein Befehl aus der ListView ausgewählt wurde
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void functionsListView_Click(object sender, EventArgs e)
        {
            if (functionsListView.SelectedItems.Count == 0) return;

            Cursor = Cursors.WaitCursor;
            //antworten vom Pic speichern
            ArrayList infos = picConnect.askPic(mainForm, functionsListView.SelectedItems[0].Text.Substring(0, 1));

            // wenn Loglevel auf debug - deshalb mit if damit foreach nicht so oft aufgerufen wird
            if (Properties.Settings.Default.logLevel > 0)
            {
                Log.write(mainForm, "Answer from PIC:", 1);
                foreach (string output in infos)
                {
                    Log.write(mainForm, output, 1);
                }
            } 

            // Letzte Zeile mit # löschen
            // rausgenommen für send any Key - sonst ist letzte Textzeile weg
            //infos.RemoveAt(infos.Count - 1);

            //ergebnisse ausgeben
            foreach (string info in infos) 
            {
                string tempInfo = info;
                if (tempInfo == "")
                    tempInfo = "\r";
                outputTextBox.Text += tempInfo + "\n";
                // auf das ende der Textbox scrollen
                outputTextBox.SelectionStart = outputTextBox.Text.Length;
                outputTextBox.ScrollToCaret();
            }
            Cursor = Cursors.Default;
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void anyKeyButton_Click(object sender, EventArgs e)
        {
            ArrayList infos = picConnect.anyKey(mainForm);
            //ergebnisse ausgeben
            foreach (string info in infos)
            {
                string tempInfo = info;
                if (tempInfo == "")
                    tempInfo = "\r";
                outputTextBox.Text += tempInfo + "\n";
                // auf das ende der Textbox scrollen
                outputTextBox.SelectionStart = outputTextBox.Text.Length;
                outputTextBox.ScrollToCaret();
            }
        }

    }
}