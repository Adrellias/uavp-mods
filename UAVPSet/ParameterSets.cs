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
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using System.Reflection;

namespace UAVP.UAVPSet
{
    /// <summary>
    /// zentrale klasse um alle Prameter zu speichern
    /// </summary>
    public class ParameterSets
    {
        /// <summary>
        /// initialisierung der Parameter
        /// </summary>
        public ParameterSets()
        {
            // alle Parameter werden vorbereitet
            //TODO: Erweitern wenn neue Parameter hinzukommen
            for (int i = 0; i < parameterForm1.Length; i++) 
            {
                parameterForm1[i].Chapter = "";
                parameterForm2[i].Chapter = "";
                parameterPic1[i].Chapter = "";
                parameterPic2[i].Chapter = "";
                // mindestwerte vorbelegen
                switch (i)
                {
                    case 3:
                    case 4:
                    case 8:
                    case 9:
                    case 13:
                    case 14:
                    case 16:
                        parameterForm1[i].Value = "1";
                        parameterForm2[i].Value = "1";
                        parameterPic1[i].Value = "1";
                        parameterPic2[i].Value = "1";
                        break;
                    case 22:
                        parameterForm1[i].Value = "5";
                        parameterForm2[i].Value = "5";
                        parameterPic1[i].Value = "5";
                        parameterPic2[i].Value = "5";
                        break;
                    default:
                        parameterForm1[i].Value = "0";
                        parameterForm2[i].Value = "0";
                        parameterPic1[i].Value = "0";
                        parameterPic2[i].Value = "0";
                        break;

                }
                parameterForm1[i].Command = "Register " + (i+1).ToString();
                parameterForm2[i].Command = "Register " + (i + 1).ToString();
                parameterPic1[i].Command = "Register " + (i + 1).ToString();
                parameterPic2[i].Command = "Register " + (i + 1).ToString();
                parameterForm1[i].Comment = kurzParameter[i];
                parameterForm2[i].Comment = kurzParameter[i];
                parameterPic1[i].Comment = kurzParameter[i];
                parameterPic2[i].Comment = kurzParameter[i];
            }

        }
        
        
        // farben um die Felder bei update zu markieren
        public enum Farbe {black, green, orange, red};

        /// <summary>
        /// allgemeine Struktur um die Parameter aufzunehmen
        /// </summary>
        public struct ParameterSetsStruc
        {
            public string Command;
            public string Value;
            public string Comment;
            public string Chapter;
        }

        //TODO: Array erweitern wenn neue Parameter hinzukommen
        // Parameter Sets erzeugen für Form
        public ParameterSetsStruc[] parameterForm1 = new ParameterSetsStruc[28];
        public ParameterSetsStruc[] parameterForm2 = new ParameterSetsStruc[28];
        // für PIC Register 1 und 2
        public ParameterSetsStruc[] parameterPic1 = new ParameterSetsStruc[28];
        public ParameterSetsStruc[] parameterPic2 = new ParameterSetsStruc[28];


        //namen der Parameter für kurz-Ausgabe
        //TODO: Array erweitern wenn neue Parameter hinzukommen
        string[] kurzParameter = { "RP","RI","RD","RL","RIL",
                                    "\r\nNP","NI","ND","NL","NIL",
                                    "\r\nGP","GI","GD","GL","GIL",
                                    "\r\nGK","IA","AU","LRL","LVZ",
                                    "\r\nLAA","LWG","LL","LWR","LWN","KRN","KO", "BD" };
  

        
        /// <summary>
        /// Laden einer Konfigurationsdatei
        /// </summary>
        /// <param name="pfad"></param>
        /// <param name="mainForm"></param>
        public void laden(string pfad, FormMain mainForm)
        {
            // try / catch wenn fehler beim öffnen oder interpretieren der datei
            try 
            {
                IniReader iniReader = new IniReader(pfad);
                // log schreiben bei debug
                Log.write(mainForm, "Load Parameterset: " + pfad, 1);
                ParameterSetsStruc[] registers = iniReader.GetChapter("ChannelSet");
                // alle Felder auf rot setzen
                updateForm(registers, mainForm);
            } 
            catch (Exception e) 
            {
                MessageBox.Show(e.ToString(), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.write(mainForm, "Wrong Parameter File!", 0);
            }
        }

        /// <summary>
        /// speichern der Parameter in eine Datei
        /// </summary>
        /// <param name="pfad"></param>
        /// <param name="mainForm"></param>
        public void speichern(string pfad, FormMain mainForm)
        {

            StreamWriter sw = new StreamWriter(pfad, false, Encoding.GetEncoding("windows-1252")); 
            try 
            {
                // header für Datei
                sw.WriteLine("[ChannelSet]");
                // es wird immer das aktive Tab gespeichert
                if (mainForm.tabControlParameter.SelectedIndex == 0) 
                {
                    foreach (ParameterSetsStruc register in parameterForm1) 
                    {
                        if (register.Command != null) 
                        {
                            sw.WriteLine(register.Command + "=" + register.Value);
                        }
                    }
                } 
                else 
                {
                    foreach (ParameterSetsStruc register in parameterForm2) 
                    {
                        if (register.Command != null) 
                        {
                            sw.WriteLine(register.Command + "=" + register.Value);
                        }
                    }
                }
            } 
            catch (Exception e) 
            {
                MessageBox.Show(e.ToString(), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.write(mainForm, e.ToString(), 1);
            } 
            finally 
            {
                // datei schließen
                sw.Close();
            }
            // log schreiben bei debug
            Log.write(mainForm, "Write Parameterset: " + pfad, 1);
        }

        /// <summary>
        /// ein einzelnes Feld updaten
        /// </summary>
        /// <param name="objekt"></param>
        /// <param name="mainForm"></param>
        public void feldUpdaten(Object objekt, FormMain mainForm)
        {
            // wenn es sich um ein Parameter feld handelt
            if (objekt.GetType().Name == "NumericUpDown") 
            {
                NumericUpDown feld = (NumericUpDown)objekt;
                // je nach Parameter TAB
                if (mainForm.tabControlParameter.SelectedIndex == 0) 
                {
                    //feld nur setzen wenn Integral Limit OK ist - sonst return ohne update
                    if (feld.Tag.ToString() == "5" || feld.Tag.ToString() == "10" || feld.Tag.ToString() == "15")
                    {
                        //integral Limit errechnen
                        int tempValue = Convert.ToInt32(parameterForm1[Convert.ToInt16(feld.Tag) - 4].Value);
                        if (tempValue < 0)
                            tempValue = tempValue * -1;
                        int limitTemp = (int)Math.Floor(127.0 / Convert.ToDouble(tempValue));
                        
                        if (limitTemp < Convert.ToInt32(feld.Value))
                        {
                            // *****TONY ADDED THIS LINE
                            if(limitTemp >= feld.Minimum && limitTemp <= feld.Maximum)
                            {
                                feld.Value = Convert.ToDecimal(parameterForm1[Convert.ToInt16(feld.Tag) - 1].Value);
                            }
                            return;
                        }
                        
                    }

                    
                    
                    parameterForm1[Convert.ToInt16(feld.Tag)-1].Value = feld.Value.ToString();
                    
                    //IntegralLimit berechnen wenn Integral geändert wurde
                    if (feld.Tag.ToString() == "2" || feld.Tag.ToString() == "7" || feld.Tag.ToString() == "12")
                    {
                        //integral Limit errechnen
                        int tempValue = Convert.ToInt32(parameterForm1[Convert.ToInt16(feld.Tag) - 1].Value);
                        if (tempValue < 0)
                            tempValue = tempValue * -1;
                        int limitTemp = (int) Math.Floor(127.0 / Convert.ToDouble(tempValue));
                        
                        if (limitTemp > 0 && limitTemp < Convert.ToInt32(parameterForm1[Convert.ToInt16(feld.Tag) + 2].Value))
                        { 
                            switch (feld.Tag.ToString())
                            {
                                case "2":
                                    mainForm.rollIntLimit1NumericUpDown.Value = limitTemp;
                                    break;
                                case "7":
                                    mainForm.nickIntLimit1NumericUpDown.Value = limitTemp;
                                    break;
                                case "12":
                                    mainForm.gierIntLimit1NumericUpDown.Value = limitTemp;
                                    break;
                            }
                        }
                    }
                    
                    
                    
                    // wenn es gleich dem PIC wert ist -> wieder grün
                    //Hier die Idee der Farbdarstellung:
                    //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                    //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                    //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                    //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind
                    
                    // wenn pic noch nicht gelesen wurde ist Chapter leer
                    if (parameterPic1[Convert.ToInt16(feld.Tag) - 1].Chapter == "ChannelSet")
                    {
                        // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                        if (parameterForm1[Convert.ToInt16(feld.Tag) - 1].Value ==
                            parameterPic1[Convert.ToInt16(feld.Tag) - 1].Value)
                        {
                            feld.ForeColor = Color.Green;
                        }
                        // wenn wert nicht mit PIC ident
                        else
                        { 
                            // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                            if (mainForm.writeUpdate == true)
                            {
                                feld.ForeColor = Color.Red;
                            }
                            else
                            { 
                                feld.ForeColor = Color.Orange;
                            }
                        }
                    }
                    else
                    // dann wird feld auf black gesetzt
                    {
                        feld.ForeColor = Color.Black;
                    }
                } 
                else
                {
                    //feld nur setzen wenn Integral Limit OK ist - sonst return ohne update
                    if (feld.Tag.ToString() == "5" || feld.Tag.ToString() == "10" || feld.Tag.ToString() == "15")
                    {
                        //integral Limit errechnen
                        int tempValue = Convert.ToInt32(parameterForm2[Convert.ToInt16(feld.Tag) - 4].Value);
                        if (tempValue < 0)
                            tempValue = tempValue * -1;
                        int limitTemp = (int)Math.Floor(127.0 / Convert.ToDouble(tempValue));
                        if (limitTemp < Convert.ToInt32(feld.Value))
                        {
                            feld.Value = Convert.ToDecimal(parameterForm2[Convert.ToInt16(feld.Tag) - 1].Value);
                            return;
                        }

                    }

                    parameterForm2[Convert.ToInt16(feld.Tag)-1].Value = feld.Value.ToString();



                    //IntegralLimit berechnen wenn Integral geändert wurde
                    if (feld.Tag.ToString() == "2" || feld.Tag.ToString() == "7" || feld.Tag.ToString() == "12")
                    {
                        //integral Limit errechnen

                        int tempValue = Convert.ToInt32(parameterForm2[Convert.ToInt16(feld.Tag) - 1].Value);
                        if (tempValue < 0)
                            tempValue = tempValue * -1;
                        int limitTemp = (int)Math.Floor(127.0 / Convert.ToDouble(tempValue));
                        
                        if (limitTemp > 0 && limitTemp < Convert.ToInt32(parameterForm2[Convert.ToInt16(feld.Tag) + 2].Value))
                        {
                            switch (feld.Tag.ToString())
                            {
                                case "2":
                                    mainForm.rollIntLimit2NumericUpDown.Value = limitTemp;
                                    break;
                                case "7":
                                    mainForm.nickIntLimit2NumericUpDown.Value = limitTemp;
                                    break;
                                case "12":
                                    mainForm.gierIntLimit2NumericUpDown.Value = limitTemp;
                                    break;
                            }
                        }
                    }
                    
                    
                    // wenn es gleich dem PIC wert ist -> wieder grün
                    //Hier die Idee der Farbdarstellung:
                    //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                    //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                    //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                    //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind
                    
                    // wenn pic noch nicht gelesen wurde ist Chapter leer
                    if (parameterPic2[Convert.ToInt16(feld.Tag) - 1].Chapter == "ChannelSet")
                    {
                        // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                        if (parameterForm2[Convert.ToInt16(feld.Tag) - 1].Value ==
                            parameterPic2[Convert.ToInt16(feld.Tag) - 1].Value)
                        {
                            feld.ForeColor = Color.Green;
                        }
                        // wenn wert nicht mit PIC ident
                        else
                        { 
                            // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                            if (mainForm.writeUpdate == true)
                            {
                                feld.ForeColor = Color.Red;
                            }
                            else
                            { 
                                feld.ForeColor = Color.Orange;
                            }
                        }
                    }
                    else
                    // dann wird feld auf black gesetzt
                    {
                        feld.ForeColor = Color.Black;
                    }
                }
            }
            // wenn es ein BIT Wert ist muss der Parameter Wert errechnet werden
            else {
                CheckBox feld = (CheckBox)objekt;

                // welches BIT ist betroffen?
                switch (feld.Name) 
                {
                    case "bit01CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) | 1).ToString();
                            }
                            else
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) & 30).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic1[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm1[15].Value) & 1) ==
                                (Convert.ToInt16(parameterPic1[15].Value) & 1))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }



                        break;
                    case "bit11CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) | 2).ToString();
                            }
                            else
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) & 29).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic1[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm1[15].Value) & 2) ==
                                (Convert.ToInt16(parameterPic1[15].Value) & 2))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit21CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) | 4).ToString();
                            }
                            else
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) & 27).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic1[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm1[15].Value) & 4) ==
                                (Convert.ToInt16(parameterPic1[15].Value) & 4))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit31CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) | 8).ToString();
                            }
                            else
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) & 23).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic1[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm1[15].Value) & 8) ==
                                (Convert.ToInt16(parameterPic1[15].Value) & 8))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit41CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) | 16).ToString();
                            }
                            else
                            {
                                parameterForm1[15].Value = (Convert.ToInt16(parameterForm1[15].Value) & 15).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic1[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm1[15].Value) & 16) ==
                                (Convert.ToInt16(parameterPic1[15].Value) & 16))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit02CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) | 1).ToString();
                            }
                            else
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) & 30).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic2[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm2[15].Value) & 1) ==
                                (Convert.ToInt16(parameterPic2[15].Value) & 1))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit12CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) | 2).ToString();
                            }
                            else
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) & 29).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic2[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm2[15].Value) & 2) ==
                                (Convert.ToInt16(parameterPic2[15].Value) & 2))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit22CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) | 4).ToString();
                            }
                            else
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) & 27).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic2[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm2[15].Value) & 4) ==
                                (Convert.ToInt16(parameterPic2[15].Value) & 4))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit32CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) | 8).ToString();
                            }
                            else
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) & 23).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic2[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm2[15].Value) & 8) ==
                                (Convert.ToInt16(parameterPic2[15].Value) & 8))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                    case "bit42CheckBox":
                        if (mainForm.writeUpdate == false)
                        {
                            if (feld.Checked == true)
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) | 16).ToString();
                            }
                            else
                            {
                                parameterForm2[15].Value = (Convert.ToInt16(parameterForm2[15].Value) & 15).ToString();
                            }
                        }

                        // wenn es gleich dem PIC wert ist -> wieder grün
                        //Hier die Idee der Farbdarstellung:
                        //schwarz: wenn noch keine Daten gelesen wurden (dh nicht so wie jetzt wo die Werte ja rot werden und beim Initialwert grün)
                        //orange: wenn Pic schon gelesen wurde und die Werte ungeleich den Werten des PICs sind
                        //grün: wenn Pic gelesen wurde und die Werte gleich dem PIC sind
                        //rot: wenn Pic geschrieben wurde und die Werte nach nochmaligem lesen nicht gleich sind

                        // wenn pic noch nicht gelesen wurde ist Chapter leer
                        if (parameterPic2[15].Chapter == "ChannelSet")
                        {
                            // wenn pic schon gelesen wurde ist ein Vergleich notwendig
                            if ((Convert.ToInt16(parameterForm2[15].Value) & 16) ==
                                (Convert.ToInt16(parameterPic2[15].Value) & 16))
                            {
                                feld.ForeColor = Color.Green;
                            }
                            // wenn wert nicht mit PIC ident
                            else
                            {
                                // wird das Feld beim schreiben upgedatet dann ist das Feld bei fehler rot
                                if (mainForm.writeUpdate == true)
                                {
                                    feld.ForeColor = Color.Red;
                                }
                                else
                                {
                                    feld.ForeColor = Color.Orange;
                                }
                            }
                        }
                        else
                        // dann wird feld auf black gesetzt
                        {
                            feld.ForeColor = Color.Black;
                        }
                        break;
                }

            }
        }


     


        /// <summary>
        /// Update der Felder in der Form
        /// </summary>
        /// <param name="registers">Structur der Registerwerte</param>
        ///// <param name="mainForm">Mainform</param>
        public void updateForm(ParameterSetsStruc[] registers, FormMain mainForm)
        {

            farbenSetzen(mainForm, Farbe.black);

            //TODO: hier case für neuen Parameter hinzufügen
            // alle Parameter durchgehen und lt. array setzen
            foreach (ParameterSetsStruc register in registers) 
            {
                if (mainForm.tabControlParameter.SelectedIndex == 0) 
                {
                    switch (Convert.ToInt16(register.Command.Substring(8))) 
                    {
                        case 1:
                            parameterForm1[0].Value = register.Value;
                            mainForm.rollProp1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollProp1NumericUpDown, mainForm);
                            break;
                        case 2:
                            parameterForm1[1].Value = register.Value;
                            mainForm.rollInt1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollInt1NumericUpDown, mainForm);
                            break;
                        case 3:
                            parameterForm1[2].Value = register.Value;
                            mainForm.rollDiff1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollDiff1NumericUpDown, mainForm);
                            break;
                        case 4:
                            if (Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.12" ||
                                Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.13")
                            {
                                mainForm.rollLimit1NumericUpDown.Value = 1;
                                mainForm.baroTemp1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            else
                            {
                                mainForm.rollLimit1NumericUpDown.Value = Convert.ToInt16(register.Value);
                                mainForm.baroTemp1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            parameterForm1[3].Value = register.Value;
                            feldUpdaten(mainForm.rollLimit1NumericUpDown, mainForm);
                            feldUpdaten(mainForm.baroTemp1NumericUpDown, mainForm);
                            break;
                        case 5:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm1[4].Value = register.Value;
                                mainForm.rollIntLimit1NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm1[4].Value = register.Value;
                                mainForm.rollIntLimit1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.rollIntLimit1NumericUpDown, mainForm);
                            break;
                        case 6:
                            parameterForm1[5].Value = register.Value;
                            mainForm.nickProp1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickProp1NumericUpDown, mainForm);
                            break;
                        case 7:
                            parameterForm1[6].Value = register.Value;
                            mainForm.nickInt1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickInt1NumericUpDown, mainForm);
                            break;
                        case 8:
                            parameterForm1[7].Value = register.Value;
                            mainForm.nickDiff1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickDiff1NumericUpDown, mainForm);
                            break;
                        case 9:
                            if (Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.12" ||
                                Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.13")
                            {
                                mainForm.nickLimit1NumericUpDown.Value = 1;
                                mainForm.baro1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            else
                            {
                                mainForm.nickLimit1NumericUpDown.Value = Convert.ToInt16(register.Value);
                                mainForm.baro1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            parameterForm1[8].Value = register.Value;
                            feldUpdaten(mainForm.nickLimit1NumericUpDown, mainForm);
                            feldUpdaten(mainForm.baro1NumericUpDown, mainForm);
                            break;
                        case 10:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm1[9].Value = register.Value;
                                mainForm.nickIntLimit1NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm1[9].Value = register.Value;
                                mainForm.nickIntLimit1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.nickIntLimit1NumericUpDown, mainForm);
                            break;
                        case 11:
                            parameterForm1[10].Value = register.Value;
                            mainForm.gierProp1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierProp1NumericUpDown, mainForm);
                            break;
                        case 12:
                            parameterForm1[11].Value = register.Value;
                            mainForm.gierInt1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierInt1NumericUpDown, mainForm);
                            break;
                        case 13:
                            parameterForm1[12].Value = register.Value;
                            mainForm.gierDiff1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierDiff1NumericUpDown, mainForm);
                            break;
                        case 14:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm1[13].Value = register.Value;
                                mainForm.gierLimit1NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm1[13].Value = register.Value;
                                mainForm.gierLimit1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.gierLimit1NumericUpDown, mainForm);
                            break;
                        case 15:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm1[14].Value = register.Value;
                                mainForm.gierIntLimit1NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm1[14].Value = register.Value;
                                mainForm.gierIntLimit1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.gierIntLimit1NumericUpDown, mainForm);
                            break;
                        case 16:
                            int temp = Convert.ToInt16(register.Value);
                            if (temp == -1)
                            {
                                temp = 0;
                            }
                            if ((temp & 1) == 1) 
                            {
                                mainForm.bit01CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit01CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit01CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit01CheckBox, mainForm);
                            }
                            if ((temp & 2) == 2) 
                            {
                                mainForm.bit11CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit11CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit11CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit11CheckBox, mainForm);
                            }
                            if ((temp & 4) == 4) 
                            {
                                mainForm.bit21CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit21CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit21CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit21CheckBox, mainForm);
                            }
                            if ((temp & 8) == 8) 
                            {
                                mainForm.bit31CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit31CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit31CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit31CheckBox, mainForm);
                            }
                            if ((temp & 16) == 16) 
                            {
                                mainForm.bit41CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit41CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit41CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit41CheckBox, mainForm);
                            }
                            //parameterForm1[15].Value = temp.ToString();
                            break;
                        case 17:
                            parameterForm1[16].Value = register.Value;
                            mainForm.impulseAusgabe1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.impulseAusgabe1NumericUpDown, mainForm);
                            break;
                        case 18:
                            if (Convert.ToInt16(register.Value) < 0)
                            {
                                parameterForm1[17].Value = register.Value;
                                mainForm.akku1NumericUpDown.Value = 0;
                            }
                            else
                            {
                                parameterForm1[17].Value = register.Value;
                                mainForm.akku1NumericUpDown.Value = Convert.ToDecimal(Math.Round(Convert.ToDouble(register.Value), 1, MidpointRounding.AwayFromZero));
                            }
                            feldUpdaten(mainForm.akku1NumericUpDown, mainForm);
                            break;
                        case 19:
                            parameterForm1[18].Value = register.Value;
                            mainForm.rollMa1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollMa1NumericUpDown, mainForm);
                            mainForm.kameraRoll1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.kameraRoll1NumericUpDown, mainForm);
                            break;
                        case 20:
                            parameterForm1[19].Value = register.Value;
                            mainForm.nickMa1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickMa1NumericUpDown, mainForm);
                            break;
                        case 21:
                            parameterForm1[20].Value = register.Value;
                            mainForm.gierMa1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierMa1NumericUpDown, mainForm);
                            break;
                        case 22:
                            parameterForm1[21].Value = register.Value;
                            mainForm.gierA1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierA1NumericUpDown, mainForm);
                            break;
                        case 23:
                            if (Convert.ToInt16(register.Value) < 5) 
                            {
                                parameterForm1[22].Value = register.Value;
                                mainForm.leerlaufgas1NumericUpDown.Value = 5;
                            } 
                            else 
                            {
                                parameterForm1[22].Value = register.Value;
                                mainForm.leerlaufgas1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.leerlaufgas1NumericUpDown, mainForm);
                            break;
                        case 24:
                            parameterForm1[23].Value = register.Value;
                            mainForm.rollA1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollA1NumericUpDown, mainForm);
                            break;
                        case 25:
                            parameterForm1[24].Value = register.Value;
                            mainForm.nickA1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickA1NumericUpDown, mainForm);
                            break;
                        case 26:
                            parameterForm1[25].Value = register.Value;
                            mainForm.kamera1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.kamera1NumericUpDown, mainForm);
                            break;
                        case 27:
                            parameterForm1[26].Value = register.Value;
                            mainForm.compass1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.compass1NumericUpDown, mainForm);
                            break;
                        case 28:
                            parameterForm1[27].Value = register.Value;
                            mainForm.baroInt1NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.baroInt1NumericUpDown, mainForm);
                            break;
                    }
                } 
                else 
                {
                    switch (Convert.ToInt16(register.Command.Substring(9))) 
                    {
                        case 1:
                            mainForm.rollProp2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            parameterForm2[0].Value = register.Value;
                            feldUpdaten(mainForm.rollProp2NumericUpDown, mainForm);
                            break;
                        case 2:
                            parameterForm2[1].Value = register.Value;
                            mainForm.rollInt2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollInt2NumericUpDown, mainForm);
                            break;
                        case 3:
                            parameterForm2[2].Value = register.Value;
                            mainForm.rollDiff2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollDiff2NumericUpDown, mainForm);
                            break;
                        case 4:
                            if (Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.12" ||
                                Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.13" )
                            {
                                mainForm.rollLimit2NumericUpDown.Value = 1;
                                mainForm.baroTemp2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            else
                            {
                                mainForm.rollLimit2NumericUpDown.Value = Convert.ToInt16(register.Value);
                                mainForm.baroTemp2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            parameterForm2[3].Value = register.Value;
                            feldUpdaten(mainForm.baroTemp2NumericUpDown, mainForm);
                            feldUpdaten(mainForm.rollLimit2NumericUpDown, mainForm);
                            break;
                        case 5:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm2[4].Value = register.Value;
                                mainForm.rollIntLimit2NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm2[4].Value = register.Value;
                                mainForm.rollIntLimit2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.rollIntLimit2NumericUpDown, mainForm);
                            break;
                        case 6:
                            parameterForm2[5].Value = register.Value;
                            mainForm.nickProp2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickProp2NumericUpDown, mainForm);
                            break;
                        case 7:
                            parameterForm2[6].Value = register.Value;
                            mainForm.nickInt2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickInt2NumericUpDown, mainForm);
                            break;
                        case 8:
                            parameterForm2[7].Value = register.Value;
                            mainForm.nickDiff2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickDiff2NumericUpDown, mainForm);
                            break;
                        case 9:
                            if (Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.12" ||
                                Convert.ToInt16(register.Value) <= 0 && Properties.Settings.Default.version == "3.13" )
                            {
                                mainForm.nickLimit2NumericUpDown.Value = 1;
                                mainForm.baro2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            else
                            {
                                mainForm.nickLimit2NumericUpDown.Value = Convert.ToInt16(register.Value);
                                mainForm.baro2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            parameterForm2[8].Value = register.Value;
                            feldUpdaten(mainForm.baro2NumericUpDown, mainForm);
                            feldUpdaten(mainForm.nickLimit2NumericUpDown, mainForm);
                            break;
                        case 10:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm2[9].Value = register.Value;
                                mainForm.nickIntLimit2NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm2[9].Value = register.Value;
                                mainForm.nickIntLimit2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.nickIntLimit2NumericUpDown, mainForm);
                            break;
                        case 11:
                            parameterForm2[10].Value = register.Value;
                            mainForm.gierProp2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierProp2NumericUpDown, mainForm);
                            break;
                        case 12:
                            parameterForm2[11].Value = register.Value;
                            mainForm.gierInt2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierInt2NumericUpDown, mainForm);
                            break;
                        case 13:
                            parameterForm2[12].Value = register.Value;
                            mainForm.gierDiff2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierDiff2NumericUpDown, mainForm);
                            break;
                        case 14:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                mainForm.gierLimit2NumericUpDown.Value = 1;
                                parameterForm2[13].Value = register.Value;
                            } 
                            else 
                            {
                                mainForm.gierLimit2NumericUpDown.Value = Convert.ToInt16(register.Value);
                                parameterForm2[13].Value = register.Value;
                            }
                            feldUpdaten(mainForm.gierLimit2NumericUpDown, mainForm);
                            break;
                        case 15:
                            if (Convert.ToInt16(register.Value) <= 0) 
                            {
                                parameterForm2[14].Value = register.Value;
                                mainForm.gierIntLimit2NumericUpDown.Value = 1;
                            } 
                            else 
                            {
                                parameterForm2[14].Value = register.Value;
                                mainForm.gierIntLimit2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.gierIntLimit2NumericUpDown, mainForm);
                            break;
                        case 16:
                            int temp = Convert.ToInt16(register.Value);
                            if (temp == -1)
                            {
                                temp = 0;
                            }
                            if ((temp & 1) == 1) 
                            {
                                mainForm.bit02CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit02CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit02CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit02CheckBox, mainForm);
                            }
                            if ((temp & 2) == 2) 
                            {
                                mainForm.bit12CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit12CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit12CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit12CheckBox, mainForm);
                            }
                            if ((temp & 4) == 4) 
                            {
                                mainForm.bit22CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit22CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit22CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit22CheckBox, mainForm);
                            }
                            if ((temp & 8) == 8) 
                            {
                                mainForm.bit32CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit32CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit32CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit32CheckBox, mainForm);
                            }
                            if ((temp & 16) == 16) 
                            {
                                mainForm.bit42CheckBox.Checked = true;
                                feldUpdaten(mainForm.bit42CheckBox, mainForm);
                            } 
                            else 
                            {
                                mainForm.bit42CheckBox.Checked = false;
                                feldUpdaten(mainForm.bit42CheckBox, mainForm);
                            }
                            //parameterForm2[15].Value = temp.ToString();
                            break;
                        case 17:
                            parameterForm2[16].Value = register.Value;
                            mainForm.impulseAusgabe2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.impulseAusgabe2NumericUpDown, mainForm);
                            break;
                        case 18:
                            if (Convert.ToInt16(register.Value) < 0)
                            {
                                parameterForm2[17].Value = register.Value;
                                mainForm.akku2NumericUpDown.Value = 0;
                            }
                            else
                            {
                                parameterForm2[17].Value = register.Value;
                                mainForm.akku2NumericUpDown.Value = Convert.ToDecimal(Math.Round(Convert.ToDouble(register.Value), 1, MidpointRounding.AwayFromZero));
                            }
                            feldUpdaten(mainForm.akku2NumericUpDown, mainForm);
                            break;
                        case 19:
                            parameterForm2[18].Value = register.Value;
                            mainForm.rollMa2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollMa2NumericUpDown, mainForm);
                            mainForm.kameraRoll2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.kameraRoll2NumericUpDown, mainForm);
                            break;
                        case 20:
                            parameterForm2[19].Value = register.Value;
                            mainForm.nickMa2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickMa2NumericUpDown, mainForm);
                            break;
                        case 21:
                            parameterForm2[20].Value = register.Value;
                            mainForm.gierMa2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierMa2NumericUpDown, mainForm);
                            break;
                        case 22:
                            parameterForm2[21].Value = register.Value;
                            mainForm.gierA2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.gierA2NumericUpDown, mainForm);
                            break;
                        case 23:
                            if (Convert.ToInt16(register.Value) < 5) 
                            {
                                parameterForm2[22].Value = register.Value;
                                mainForm.leerlaufgas2NumericUpDown.Value = 5;
                            } 
                            else 
                            {
                                parameterForm2[22].Value = register.Value;
                                mainForm.leerlaufgas2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            }
                            feldUpdaten(mainForm.leerlaufgas2NumericUpDown, mainForm);
                            break;
                        case 24:
                            parameterForm2[23].Value = register.Value;
                            mainForm.rollA2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.rollA2NumericUpDown, mainForm);
                            break;
                        case 25:
                            parameterForm2[24].Value = register.Value;
                            mainForm.nickA2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.nickA2NumericUpDown, mainForm);
                            break;
                        case 26:
                            parameterForm2[25].Value = register.Value;
                            mainForm.kamera2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.kamera2NumericUpDown, mainForm);
                            break;
                        case 27:
                            parameterForm2[26].Value = register.Value;
                            mainForm.compass2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.compass2NumericUpDown, mainForm);
                            break;
                        case 28:
                            parameterForm2[27].Value = register.Value;
                            mainForm.baroInt2NumericUpDown.Value = Convert.ToInt16(register.Value);
                            feldUpdaten(mainForm.baroInt2NumericUpDown, mainForm);
                            break;
                    }
                }
            }
        }


        /// <summary>
        /// fabe bei allen Feldern setzen
        /// </summary>
        /// <param name="mainForm"></param>
        /// <param name="farbe"></param>
        public void farbenSetzen(FormMain mainForm, Farbe farbe)
        { 
            
            Color farbeElement = Color.Black;
            switch (farbe) 
            { 
                case Farbe.red:
                   farbeElement = Color.Red;
                    break;
                case Farbe.green:
                    farbeElement = Color.Green;
                    break;
                case Farbe.orange:
                    farbeElement = Color.Orange;
                    break;
                case Farbe.black:
                    farbeElement = Color.Black;
                    break;
            }

            //TODO: hier Parameter hinzufügen wenn neu
            if (mainForm.tabControlParameter.SelectedIndex == 0) 
            {
                mainForm.rollProp1NumericUpDown.ForeColor = farbeElement;
                mainForm.rollInt1NumericUpDown.ForeColor = farbeElement;
                mainForm.rollDiff1NumericUpDown.ForeColor = farbeElement;
                mainForm.rollLimit1NumericUpDown.ForeColor = farbeElement;
                mainForm.rollIntLimit1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickProp1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickInt1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickDiff1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickLimit1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickIntLimit1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierProp1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierInt1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierDiff1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierLimit1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierIntLimit1NumericUpDown.ForeColor = farbeElement;
                mainForm.bit01CheckBox.ForeColor = farbeElement;
                mainForm.bit11CheckBox.ForeColor = farbeElement;
                mainForm.bit21CheckBox.ForeColor = farbeElement;
                mainForm.bit31CheckBox.ForeColor = farbeElement;
                mainForm.bit41CheckBox.ForeColor = farbeElement;
                mainForm.impulseAusgabe1NumericUpDown.ForeColor = farbeElement;
                mainForm.akku1NumericUpDown.ForeColor = farbeElement;
                mainForm.rollMa1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickMa1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierMa1NumericUpDown.ForeColor = farbeElement;
                mainForm.gierA1NumericUpDown.ForeColor = farbeElement;
                mainForm.leerlaufgas1NumericUpDown.ForeColor = farbeElement;
                mainForm.rollA1NumericUpDown.ForeColor = farbeElement;
                mainForm.nickA1NumericUpDown.ForeColor = farbeElement;
                mainForm.kamera1NumericUpDown.ForeColor = farbeElement;
                mainForm.compass1NumericUpDown.ForeColor = farbeElement;
                mainForm.baroInt1NumericUpDown.ForeColor = farbeElement;
            } 
            else 
            {
                mainForm.rollProp2NumericUpDown.ForeColor = farbeElement;
                mainForm.rollInt2NumericUpDown.ForeColor = farbeElement;
                mainForm.rollDiff2NumericUpDown.ForeColor = farbeElement;
                mainForm.rollLimit2NumericUpDown.ForeColor = farbeElement;
                mainForm.rollIntLimit2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickProp2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickInt2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickDiff2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickLimit2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickIntLimit2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierProp2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierInt2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierDiff2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierLimit2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierIntLimit2NumericUpDown.ForeColor = farbeElement;
                mainForm.bit02CheckBox.ForeColor = farbeElement;
                mainForm.bit12CheckBox.ForeColor = farbeElement;
                mainForm.bit22CheckBox.ForeColor = farbeElement;
                mainForm.bit32CheckBox.ForeColor = farbeElement;
                mainForm.bit42CheckBox.ForeColor = farbeElement;
                mainForm.impulseAusgabe2NumericUpDown.ForeColor = farbeElement;
                mainForm.akku2NumericUpDown.ForeColor = farbeElement;
                mainForm.rollMa2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickMa2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierMa2NumericUpDown.ForeColor = farbeElement;
                mainForm.gierA2NumericUpDown.ForeColor = farbeElement;
                mainForm.leerlaufgas2NumericUpDown.ForeColor = farbeElement;
                mainForm.rollA2NumericUpDown.ForeColor = farbeElement;
                mainForm.nickA2NumericUpDown.ForeColor = farbeElement;
                mainForm.kamera2NumericUpDown.ForeColor = farbeElement;
                mainForm.compass2NumericUpDown.ForeColor = farbeElement;
                mainForm.baroInt2NumericUpDown.ForeColor = farbeElement;
            }
        }

        public void copySet1ToSet2(FormMain mainForm)
        {
            //for (int i = 0; i < parameterForm1.Length; i++)
            //{
            //    parameterForm2[i].Chapter   = parameterForm1[i].Chapter;
            //    parameterForm2[i].Value     = parameterForm1[i].Value;
            //    parameterForm2[i].Command   = parameterForm1[i].Command;
            //    parameterForm2[i].Comment   = parameterForm1[i].Comment;
            //}
            mainForm.tabControlParameter.SelectedIndex = 1;
            updateForm(parameterForm1, mainForm);

        }

        public void copySet2ToSet1(FormMain mainForm)
        {
            //for (int i = 0; i < parameterForm1.Length; i++)
            //{
            //    parameterForm2[i].Chapter   = parameterForm1[i].Chapter;
            //    parameterForm2[i].Value     = parameterForm1[i].Value;
            //    parameterForm2[i].Command   = parameterForm1[i].Command;
            //    parameterForm2[i].Comment   = parameterForm1[i].Comment;
            //}
            mainForm.tabControlParameter.SelectedIndex = 0;
            updateForm(parameterForm2, mainForm);

        }
    }
}
