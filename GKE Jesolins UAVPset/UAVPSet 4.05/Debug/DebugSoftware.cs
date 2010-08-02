using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Collections;

namespace UAVP.UAVPSet.Debug
{
    public partial class DebugSoftware : Form
    {
        FormMain mainForm;
        PICConnect picConnect = new PICConnect();



        // Variablen für Debug
        Graphics graph1;
        Graphics graph2;
        Graphics graph3;
        int x = 0;
        bool run = true;
        int xAlt = 0;
        ArrayList werte = new ArrayList();
        string[] lastTeile = { "0", "0", "0", "0", "0", "0", "0", "0", 
                               "0", "0", "0", "0", "0", "0", "0", "0",
                               "0", "0", "0", "0", "0", "0", "0", "0", // extra 8
                               "0", "0", "0", "0", "0", "0", "0", "0", "0" };

        int[] last = { 210, 210, 210, 210, 210, 210, 210, 210, 210 };
        int[] aktuell = { 210, 210, 210, 210, 210, 210, 210, 210, 210 };


        public DebugSoftware(FormMain mainForm, PICConnect picConnect)
        {
            InitializeComponent();

            CheckForIllegalCrossThreadCalls = false;
            this.mainForm = mainForm;
            this.picConnect = picConnect;
        }


        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            debugAusgabe(false);
        }

        private void configurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mapping map = new mapping();
            map.ShowDialog();
        }


        private void connectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            sTOPToolStripMenuItem.Enabled = true;
            sTOPToolStripMenuItem.ForeColor = Color.Red;
            connectToolStripMenuItem.Enabled = false;
            fileToolStripMenuItem.Enabled = false;
            configurationToolStripMenuItem.Enabled = false;
            saveToolStripMenuItem.Enabled = false;
         
            debugAusgabe(true);
            
        }

        /// <summary>
        /// Ausgabe der debugkurve
        /// </summary>
        /// <param name="pic">bei True --> PIC bei False --> File</param>
        private void debugAusgabe(bool pic)
        {

            // für Ausgabe
            graph1 = dia1Panel.CreateGraphics();
            graph2 = dia2Panel.CreateGraphics();
            graph3 = dia3Panel.CreateGraphics();


            werte = new ArrayList();

            string temp = "";

            

            int[] mapping = {
                                    Properties.Settings.Default.map1 - 1,
                                    Properties.Settings.Default.map2 - 1,
                                    Properties.Settings.Default.map3 - 1,
                                    Properties.Settings.Default.map4 - 1,
                                    Properties.Settings.Default.map5 - 1,
                                    Properties.Settings.Default.map6 - 1,
                                    Properties.Settings.Default.map7 - 1,
                                    Properties.Settings.Default.map8 - 1,
                                    Properties.Settings.Default.map9 - 1
                                };

            int[] divisor = {
                                    Properties.Settings.Default.div1,
                                    Properties.Settings.Default.div2,
                                    Properties.Settings.Default.div3,
                                    Properties.Settings.Default.div4,
                                    Properties.Settings.Default.div5,
                                    Properties.Settings.Default.div6,
                                    Properties.Settings.Default.div7,
                                    Properties.Settings.Default.div8,
                                    Properties.Settings.Default.div9
                                };

            int[] offset = {
                                    Properties.Settings.Default.off1,
                                    Properties.Settings.Default.off2,
                                    Properties.Settings.Default.off3,
                                    Properties.Settings.Default.off4,
                                    Properties.Settings.Default.off5,
                                    Properties.Settings.Default.off6,
                                    Properties.Settings.Default.off7,
                                    Properties.Settings.Default.off8,
                                    Properties.Settings.Default.off9
                                };

            int[] basis = {
                                    Properties.Settings.Default.bas1,
                                    Properties.Settings.Default.bas2,
                                    Properties.Settings.Default.bas3,
                                    Properties.Settings.Default.bas4,
                                    Properties.Settings.Default.bas5,
                                    Properties.Settings.Default.bas6,
                                    Properties.Settings.Default.bas7,
                                    Properties.Settings.Default.bas8,
                                    Properties.Settings.Default.bas9
                                };

            //int[] divisor = {
            //                        div[mapping[0]],
            //                        div[mapping[1]],
            //                        div[mapping[2]],
            //                        div[mapping[3]],
            //                        div[mapping[4]],
            //                        div[mapping[5]],
            //                        div[mapping[6]],
            //                        div[mapping[7]],
            //                        div[mapping[8]]
            //                    };

            //int[] offset = {
            //                        off[mapping[0]],
            //                        off[mapping[1]],
            //                        off[mapping[2]],
            //                        off[mapping[3]],
            //                        off[mapping[4]],
            //                        off[mapping[5]],
            //                        off[mapping[6]],
            //                        off[mapping[7]],
            //                        off[mapping[8]]
            //                    };

            //int[] basis = {
            //                        bas[mapping[0]],
            //                        bas[mapping[1]],
            //                        bas[mapping[2]],
            //                        bas[mapping[3]],
            //                        bas[mapping[4]],
            //                        bas[mapping[5]],
            //                        bas[mapping[6]],
            //                        bas[mapping[7]],
            //                        bas[mapping[8]]
            //                    };


            // DebugWerte lesen 

            if (pic == true)
            {
                picConnect.connect(mainForm, true, false);

                while (run == true)
                {
                    try
                    {
                        temp = picConnect.sp.ReadLine();
                    }
                    catch
                    {
                        Application.DoEvents();
                        continue;
                    }
                    drawGraph(temp, last, aktuell, mapping, divisor, offset, basis, pic);
                }
                run = true;
                // StreamReader schließen
                try
                {
                    picConnect.sp.Close();
                }
                catch { 
                }
            }
            else
            {
                // StreamReader-Instanz für die Datei erzeugen
                StreamReader sr = null;

                DialogResult result = openFileDialog1.ShowDialog();
                if (result == DialogResult.OK)
                {


                    string fileName = openFileDialog1.FileName;
                    try
                    {
                        sr = new StreamReader(fileName, Encoding.GetEncoding("windows-1252"));
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Fehler beim Öffnen der Datei '" + fileName + "': " +
                            ex.Message, Application.ProductName, MessageBoxButtons.OK,
                            MessageBoxIcon.Error);
                        return;
                    }
                    // Datei zeilenweise einlesen
                    while ((temp = sr.ReadLine()) != null && run == true)
                    {
                        drawGraph(temp, last, aktuell, mapping, divisor, offset, basis, pic);
                        System.Threading.Thread.Sleep(10);
                    }

                    run = true;
                    // StreamReader schließen
                    sr.Close();
                }
            }

        }


        private void drawGraph(string temp, int [] last, int [] aktuell, int [] mapping, int [] divisor,
                                int [] offset, int [] basis, bool pic)
        {
            werte.Add(temp);

            string[] teile = temp.Split(';');
            if (teile.Length < 9)
                return;

            try
            {

                for (int i = 0; i < teile.Length; i++)
                {
                    if (teile[i] == "")
                        teile[i] = lastTeile[i];
                    else 
                        lastTeile[i] = teile[i];
                }
                teile[teile.Length - 1] = teile[teile.Length - 1].Replace("\r", "");

                aktuell.CopyTo(last, 0);

                try
                {
                    if (basis[0] == 16)
                        aktuell[0] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[0]], 16) / divisor[0] - offset[0]);
                    else
                        aktuell[0] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[0]], 16) / divisor[0] - offset[0]);
                }
                catch
                {
                    aktuell[0] = last[0];
                }
                try
                {
                    if (basis[1] == 16)
                        aktuell[1] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[1]], 16) / divisor[1] - offset[1]);
                    else
                        aktuell[1] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[1]], 16) / divisor[1] - offset[1]);
                }
                catch
                {
                    aktuell[1] = last[1];

                }
                try
                {
                    if (basis[2] == 16)
                        aktuell[2] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[2]], 16) / divisor[2] - offset[2]);
                    else
                        aktuell[2] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[2]], 16) / divisor[2] - offset[2]);
                }
                catch
                {
                    aktuell[2] = last[2];
                }
                try
                {
                    if (basis[3] == 16)
                        aktuell[3] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[3]], 16) / divisor[3] - offset[3]);
                    else
                        aktuell[3] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[3]], 16) / divisor[3] - offset[3]);
                }
                catch
                {
                    aktuell[3] = last[3];
                }
                try
                {
                    if (basis[4] == 16)
                        aktuell[4] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[4]], 16) / divisor[4] - offset[4]);
                    else
                        aktuell[4] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[4]], 16) / divisor[4] - offset[4]);
                }
                catch
                {
                    aktuell[4] = last[4];
                }
                try
                {
                    if (basis[5] == 16)
                        aktuell[5] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[5]], 16) / divisor[5] - offset[5]);
                    else
                        aktuell[5] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[5]], 16) / divisor[5] - offset[5]);
                }
                catch
                {
                    aktuell[5] = last[5];
                }
                try
                {
                    if (basis[6] == 16)
                        aktuell[6] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[6]], 16) / divisor[6] - offset[6]);
                    else
                        aktuell[6] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[6]], 16) / divisor[6] - offset[6]);
                }
                catch
                {
                    aktuell[6] = last[6];
                }
                try
                {
                    if (basis[7] == 16)
                        aktuell[7] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[7]], 16) / divisor[7] - offset[7]);
                    else
                        aktuell[7] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[7]], 16) / divisor[7] - offset[7]);
                }
                catch
                {
                    aktuell[7] = last[7];
                }
                try
                {
                    if (basis[8] == 16)
                        aktuell[8] = Convert.ToInt32(210 - Convert.ToInt16(teile[mapping[8]], 16) / divisor[8] - offset[8]);
                    else
                        aktuell[8] = Convert.ToInt32(210 - Convert.ToSByte(teile[mapping[8]], 16) / divisor[8] - offset[8]);
                }
                catch
                {
                    aktuell[8] = last[8];
                }


                //for (int i = 0; i < 9; i++)
                //{
                //    if (aktuell[i] == 210 && last[i] != 210)
                //    {
                //        aktuell[i] = last[i];
                //    }
                //}


                if (x < 900)
                {
                    graph1.FillRectangle(Brushes.WhiteSmoke, x + 1, 1, 1, 219);
                    graph2.FillRectangle(Brushes.WhiteSmoke, x + 1, 1, 1, 219);
                    graph3.FillRectangle(Brushes.WhiteSmoke, x + 1, 1, 1, 219);
                }

                if (x == 100 || x == 200 || x == 300 || x == 400 || x == 500 || x == 600 || x == 700 || x == 800)
                {
                    graph1.FillRectangle(Brushes.Gray, x, 0, 1, 220);
                    graph2.FillRectangle(Brushes.Gray, x, 0, 1, 220);
                    graph3.FillRectangle(Brushes.Gray, x, 0, 1, 220);
                }
                else
                {
                    graph1.FillRectangle(Brushes.Black, x, 0, 1, 220);
                    graph1.FillRectangle(Brushes.Gray, x, 10, 1, 1);
                    graph1.FillRectangle(Brushes.Gray, x, 60, 1, 1);
                    graph1.FillRectangle(Brushes.Gray, x, 110, 1, 1);
                    graph1.FillRectangle(Brushes.Gray, x, 160, 1, 1);
                    graph1.FillRectangle(Brushes.Gray, x, 210, 1, 1);

                    graph2.FillRectangle(Brushes.Black, x, 0, 1, 220);
                    graph2.FillRectangle(Brushes.Gray, x, 10, 1, 1);
                    graph2.FillRectangle(Brushes.Gray, x, 60, 1, 1);
                    graph2.FillRectangle(Brushes.Gray, x, 110, 1, 1);
                    graph2.FillRectangle(Brushes.Gray, x, 160, 1, 1);
                    graph2.FillRectangle(Brushes.Gray, x, 210, 1, 1);

                    graph3.FillRectangle(Brushes.Black, x, 0, 1, 220);
                    graph3.FillRectangle(Brushes.Gray, x, 10, 1, 1);
                    graph3.FillRectangle(Brushes.Gray, x, 60, 1, 1);
                    graph3.FillRectangle(Brushes.Gray, x, 110, 1, 1);
                    graph3.FillRectangle(Brushes.Gray, x, 160, 1, 1);
                    graph3.FillRectangle(Brushes.Gray, x, 210, 1, 1);

                }

                if (x != 0 && (x % 3) == 1)
                {
                    graph1.FillRectangle(Brushes.Yellow, x, 210 - offset[0], 1, 1);

                    graph2.FillRectangle(Brushes.Yellow, x, 210 - offset[3], 1, 1);

                    graph3.FillRectangle(Brushes.Yellow, x, 210 - offset[6], 1, 1);
                }


                if (x != 0 && (x % 3) == 2)
                {
                    graph1.FillRectangle(Brushes.Lime, x, 210 - offset[1], 1, 1);

                    graph2.FillRectangle(Brushes.Lime, x, 210 - offset[4], 1, 1);

                    graph3.FillRectangle(Brushes.Lime, x, 210 - offset[7], 1, 1);
                }


                if (x != 0 && (x % 3) == 0)
                {
                    graph1.FillRectangle(Brushes.HotPink, x, 210 - offset[2], 1, 1);

                    graph2.FillRectangle(Brushes.HotPink, x, 210 - offset[5], 1, 1);

                    graph3.FillRectangle(Brushes.HotPink, x, 210 - offset[8], 1, 1);
                }

                graph1.DrawLine(Pens.Yellow, xAlt, last[0], x, aktuell[0]);
                graph1.DrawLine(Pens.Lime, xAlt, last[1], x, aktuell[1]);
                graph1.DrawLine(Pens.HotPink, xAlt, last[2], x, aktuell[2]);

                graph2.DrawLine(Pens.Yellow, xAlt, last[3], x, aktuell[3]);
                graph2.DrawLine(Pens.Lime, xAlt, last[4], x, aktuell[4]);
                graph2.DrawLine(Pens.HotPink, xAlt, last[5], x, aktuell[5]);

                graph3.DrawLine(Pens.Yellow, xAlt, last[6], x, aktuell[6]);
                graph3.DrawLine(Pens.Lime, xAlt, last[7], x, aktuell[7]);
                graph3.DrawLine(Pens.HotPink, xAlt, last[8], x, aktuell[8]);



                xAlt = x++;

                if (x == 900)
                    x = xAlt = 0;

                Application.DoEvents();
            }
            catch (Exception ex)
            { }
        }


        private void sTOPToolStripMenuItem_Click(object sender, EventArgs e)
        {

            sTOPToolStripMenuItem.Enabled = false;
            sTOPToolStripMenuItem.ForeColor = Color.Black;
            connectToolStripMenuItem.Enabled = true;
            fileToolStripMenuItem.Enabled = true;
            configurationToolStripMenuItem.Enabled = true;
            saveToolStripMenuItem.Enabled = true; 

            run = false;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                StreamWriter sw = new StreamWriter(saveFileDialog1.FileName, false, Encoding.GetEncoding("windows-1252"));
                try
                {
                    foreach (string wert in werte)
                        sw.WriteLine(wert.ToString());
                }
                catch (Exception exc)
                {
                    MessageBox.Show(exc.ToString(), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);

                }
                finally
                {
                    // datei schließen
                    sw.Close();
                }

            }
        }
    }
}