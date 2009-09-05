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
                // Roll     
                if (mainForm.RollProp1NumericUpDown.Focused || mainForm.RollProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.RollDiff1NumericUpDown.Focused || mainForm.RollDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differential");
                if (mainForm.RollInt1NumericUpDown.Focused || mainForm.RollInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.RollIntLimit1NumericUpDown.Focused || mainForm.RollIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.RollAcc1NumericUpDown.Focused || mainForm.RollAcc2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");
                
                // Pitch
                if (mainForm.PitchProp1NumericUpDown.Focused || mainForm.PitchProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.PitchDiff1NumericUpDown.Focused || mainForm.PitchDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differential");
                if (mainForm.PitchInt1NumericUpDown.Focused || mainForm.PitchInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.PitchIntLimit1NumericUpDown.Focused || mainForm.PitchIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.PitchAcc1NumericUpDown.Focused || mainForm.PitchAcc2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");
                            
                // Yaw
                if (mainForm.YawProp1NumericUpDown.Focused || mainForm.YawProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Proportional");
                if (mainForm.YawDiff1NumericUpDown.Focused || mainForm.YawDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Differential");
                if (mainForm.YawInt1NumericUpDown.Focused || mainForm.YawInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Integral");
                if (mainForm.YawLimit1NumericUpDown.Focused || mainForm.YawLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Limiter");
                if (mainForm.YawIntLimit1NumericUpDown.Focused || mainForm.YawIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("IntegralLimiter");
                if (mainForm.VertDamping1NumericUpDown.Focused || mainForm.VertDamping2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("VerticalDamping");
                if (mainForm.VertAcc1NumericUpDown.Focused || mainForm.VertAcc2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Neutral");

            // General
                if (mainForm.bit01CheckBox.Focused || mainForm.bit02CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("XModus");
                if (mainForm.bit11CheckBox.Focused || mainForm.bit12CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("ThrottleChannel");
                if (mainForm.bit21CheckBox.Focused || mainForm.bit22CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("TxMode");
                if (mainForm.bit31CheckBox.Focused || mainForm.bit32CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("RxFormat");
                if (mainForm.bit41CheckBox.Focused || mainForm.bit42CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("RxPulsePolarity");
                if (mainForm.bit51CheckBox.Focused || mainForm.bit52CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("GPSAltitude");
                if (mainForm.bit61CheckBox.Focused || mainForm.bit62CheckBox.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("RTHDescend");

                if (mainForm.HoverThrottleNumericUpDown1.Focused || mainForm.HoverThrottleNumericUpDown2.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("HoverThrottle");

                if (mainForm.ImpulseTime1NumericUpDown.Focused || mainForm.ImpulseTime2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Impuls");
                if (mainForm.LowMotorRun1NumericUpDown.Focused || mainForm.LowMotorRun2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("LowMotorRun");
                if (mainForm.Camera1NumericUpDown.Focused || mainForm.CameraRoll2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("CameraGain");
                if (mainForm.Compass1NumericUpDown.Focused || mainForm.Compass2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Compass");
                if (mainForm.Battery1NumericUpDown.Focused || mainForm.Battery2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Unterspannung");
                if (mainForm.BaroTemp1NumericUpDown.Focused || mainForm.BaroTemp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("BaroTemp");
                if (mainForm.BaroProp1NumericUpDown.Focused || mainForm.BaroProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("Baro");
                if (mainForm.BaroDiff1NumericUpDown.Focused || mainForm.BaroDiff2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("BaroDif");

                if (mainForm.GyroComboBox1.Focused || mainForm.GyroComboBox2.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("GyroType");
                if (mainForm.ESCComboBox1.Focused || mainForm.ESCComboBox2.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("ESCType");
                if (mainForm.TxRxComboBox1.Focused || mainForm.TxRxComboBox2.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("TxRxType");

                //GPS
                if (mainForm.NavRadius1NumericUpDown.Focused || mainForm.NavRadius2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavRadius");
                if (mainForm.NavNeutralRadius1NumericUpDown.Focused || mainForm.NavNeutralRadius2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavNeutralRadius");
                if (mainForm.NavIntLimit1NumericUpDown.Focused || mainForm.NavIntLimit2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavIntLimit");
                if (mainForm.NavAltProp1NumericUpDown.Focused || mainForm.NavAltProp2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavAltProportional");
                if (mainForm.NavAltInt1NumericUpDown.Focused || mainForm.NavAltInt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavAltIntegral");
                if (mainForm.NavRTHAlt1NumericUpDown.Focused || mainForm.NavRTHAlt2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavRTHAltitude");
                if (mainForm.NavMagVar1NumericUpDown.Focused || mainForm.NavMagVar2NumericUpDown.Focused)
                    mainForm.infoTextBox.Text = mainForm.help.GetString("NavMagVar");
        }
    }
}
