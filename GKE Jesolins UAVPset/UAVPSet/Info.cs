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
using System.Reflection;

namespace UAVP.UAVPSet
{
    /// <summary>
    /// info Form
    /// </summary>
    public partial class Info : Form
    {
        public Info()
        {
            InitializeComponent();
            versionLabel.Parent = pictureBox1;
            // Version aus Assambly auslesen
            versionLabel.Text = "Version: " + Assembly.GetExecutingAssembly().GetName().Version.ToString();
            richTextBox1.Text = "UAVPSet \n" +
                    "http://www.uavp.de \n" +
                    "Programmer: Thorsten Raab \n" +
                    "Email: thorsten.raab@gmx.at \n\n" +
                    "English Text and Graphics modified April 2009 by Jim Solinski. \n\n" +
                    "Adapted for UAVX, a rewritten version of UAVP, June 2009 by Greg Egan. \n\n" +
                    //"Programmer: Michael Sachs \n" +
                    //"Email: michael.sachs@online.de \n\n" +

                    "This program is free software; you can redistribute it and/or " +
                    "modify it under the terms of the GNU General Public License " +
                    "as published by the Free Software Foundation; either version 2 " +
                    "of the License, or (at your option) any later version. \n\n" +

                    "This program is distributed in the hope that it will be useful, " +
                    "but WITHOUT ANY WARRANTY; without even the implied warranty of " +
                    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the " +
                    "GNU General Public License for more details. \n\n" +

                    "You should have received a copy of the GNU General Public License " +
                    "along with this program; if not, write to the Free Software " +
                    "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. ";
        }
    }
}
