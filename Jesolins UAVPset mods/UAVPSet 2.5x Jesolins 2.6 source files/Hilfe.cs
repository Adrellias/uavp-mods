//// UAVPSet
// Copyright (C) 2007  Thorsten Raab
// Email: thorsten.raab@gmx.at
// Michael Sachs
// Email: michael.sachs@online.de
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
using System.Resources;

namespace UAVP.UAVPSet
{
    /// <summary>
    /// hilfeklasse für die Infos der Steuerelemente
    /// </summary>
    static class Hilfe
    {
        /// <summary>
        /// allgemeine Funktion die von jedem Steuerelement aufgerufen werden kann
        /// </summary>
        /// <param name="mainForm"></param>
        static public void info(FormMain mainForm)
        {
// RollAchse Hilfe
            // Setting 1       
                if (mainForm.rollProp1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.rollDiff1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differenzial");
                if (mainForm.rollInt1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.rollLimit1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.rollIntLimit1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.rollMa1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("EbenenAusgleich");
                if (mainForm.rollA1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");
                //Setting 2
                if (mainForm.rollProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.rollDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differenzial");
                if (mainForm.rollInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.rollLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.rollIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.rollMa2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("EbenenAusgleich");
                if (mainForm.rollA2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");     
// NickAchse Hilfe
                // Setting 1
                if (mainForm.nickProp1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.nickDiff1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differenzial");
                if (mainForm.nickInt1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.nickLimit1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.nickIntLimit1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.nickMa1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("EbenenAusgleich");
                if (mainForm.nickA1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");
                // Setting 2
                if (mainForm.nickProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.nickDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differenzial");
                if (mainForm.nickInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.nickLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.nickIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.nickMa2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("EbenenAusgleich");
                if (mainForm.nickA2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");                
// GierAchse Hilfe
                // Setting 1
                if (mainForm.gierProp1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                 if (mainForm.gierDiff1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differenzial");
                if (mainForm.gierInt1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.gierLimit1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.gierIntLimit1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.gierMa1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("EbenenAusgleich");
                if (mainForm.gierA1NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");
                // Setting 2
                if (mainForm.gierProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.gierDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differenzial");
                if (mainForm.gierInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.gierLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.gierIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.gierMa2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("EbenenAusgleich");
                if (mainForm.gierA2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");
// Sonstiges
            // Setting 1
            if (mainForm.bit01CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("XModus");
            if (mainForm.bit11CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("GasKanal");
            if (mainForm.bit21CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Integrierzustand");
            if (mainForm.bit31CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("HalbRollNick");
            if (mainForm.bit41CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("EmpfaengerImpulse");
            if (mainForm.impulseAusgabe1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Impuls");
            if (mainForm.leerlaufgas1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Leerlaufgas");
            if (mainForm.kamera1NumericUpDown.Focused || mainForm.kameraRoll1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Kameraausgleich");
            if (mainForm.compass1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Kompass");
            if (mainForm.akku1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Unterspannung");
            if (mainForm.baroTemp1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("baroTemp");
            if (mainForm.baro1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("baro");
            if (mainForm.baroInt1NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("baroDif");
            //GPS
            if (mainForm.numericUpDownGPSP1.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("GPSProportional");
            if (mainForm.numericUpDownGPSD1.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("GPSDifferential");
            if (mainForm.numericUpDownGPSSmooth1.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("GPSSmoothing");
            // Setting 2
            if (mainForm.bit02CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("XModus");
            if (mainForm.bit12CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("GasKanal");
            if (mainForm.bit22CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Integrierzustand");
            if (mainForm.bit32CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("HalbRollNick");
            if (mainForm.bit42CheckBox.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("EmpfaengerImpulse");
            if (mainForm.impulseAusgabe2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Impuls");
            if (mainForm.leerlaufgas2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Leerlaufgas");
            if (mainForm.kamera2NumericUpDown.Focused || mainForm.kameraRoll2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Kameraausgleich");
            if (mainForm.compass2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Kompass");
            if (mainForm.akku2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("Unterspannung");
            if (mainForm.baro2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("baro");
            if (mainForm.baroInt2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("baroDif");
            if (mainForm.baroTemp2NumericUpDown.Focused)
                mainForm.infoTextBox.Text = mainForm.help.GetString("baroTemp");
        }
    }
}
