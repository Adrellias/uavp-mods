using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace UAVP.UAVPSet
{
    /// <summary>
    /// forumular für die Neutralwerte
    /// </summary>
    public partial class Neutral : Form
    {
        FormMain mainForm;

        public Neutral(FormMain mainForm)
        {
            InitializeComponent();
            this.mainForm = mainForm;
        }

        /// <summary>
        /// übernehmen der Werte ins formular
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ueberButton_Click(object sender, EventArgs e)
        {
            // je nachdem welcher TAB aktiv ist werden die Werte eingetragen
            if (mainForm.tabControlParameter.SelectedIndex == 0) 
            {
                Log.write(mainForm, "TAB 1 selected", 1);
                // update RollAccchse
                mainForm.RollAcc1NumericUpDown.Value = Convert.ToInt16(neutralLabel.Text.Substring(13,4));
                Log.write(mainForm, "Roll :" + Convert.ToInt16(neutralLabel.Text.Substring(13, 4)), 1);
                // update Pitch achse
                mainForm.PitchAcc1NumericUpDown.Value = Convert.ToInt16(neutralLabel.Text.Substring(23, 4));
                Log.write(mainForm, "Pitch :" + Convert.ToInt16(neutralLabel.Text.Substring(23, 4)), 1);
                // update Yaw achse
                mainForm.VertAcc1NumericUpDown.Value = Convert.ToInt16(neutralLabel.Text.Substring(32, 4));
                Log.write(mainForm, "Ver :" + Convert.ToInt16(neutralLabel.Text.Substring(32, 4)), 1);
            } 
            else 
            {
                Log.write(mainForm, "TAB 2 selected", 1);
                // update RollAccchse
               
                Log.write(mainForm, "Ver :" + Convert.ToInt16(neutralLabel.Text.Substring(32, 4)), 1);
            }
            this.Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void neutralLabel_Click(object sender, EventArgs e)
        {

        }
    }
}