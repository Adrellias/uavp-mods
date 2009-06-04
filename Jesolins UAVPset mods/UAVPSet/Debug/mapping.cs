using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace UAVP.UAVPSet.Debug
{
    public partial class mapping : Form
    {
        public mapping()
        {
            InitializeComponent();

            textBox1.Text = Properties.Settings.Default.map1.ToString();
            textBox2.Text = Properties.Settings.Default.map2.ToString();
            textBox3.Text = Properties.Settings.Default.map3.ToString();
            textBox4.Text = Properties.Settings.Default.map4.ToString();
            textBox5.Text = Properties.Settings.Default.map5.ToString();
            textBox6.Text = Properties.Settings.Default.map6.ToString();
            textBox7.Text = Properties.Settings.Default.map7.ToString();
            textBox8.Text = Properties.Settings.Default.map8.ToString();
            textBox9.Text = Properties.Settings.Default.map9.ToString();

            textBox10.Text = Properties.Settings.Default.div1.ToString();
            textBox11.Text = Properties.Settings.Default.div2.ToString();
            textBox12.Text = Properties.Settings.Default.div3.ToString();
            textBox13.Text = Properties.Settings.Default.div4.ToString();
            textBox14.Text = Properties.Settings.Default.div5.ToString();
            textBox15.Text = Properties.Settings.Default.div6.ToString();
            textBox16.Text = Properties.Settings.Default.div7.ToString();
            textBox17.Text = Properties.Settings.Default.div8.ToString();
            textBox18.Text = Properties.Settings.Default.div9.ToString();

            textBox19.Text = Properties.Settings.Default.off1.ToString();
            textBox20.Text = Properties.Settings.Default.off2.ToString();
            textBox21.Text = Properties.Settings.Default.off3.ToString();
            textBox22.Text = Properties.Settings.Default.off4.ToString();
            textBox23.Text = Properties.Settings.Default.off5.ToString();
            textBox24.Text = Properties.Settings.Default.off6.ToString();
            textBox25.Text = Properties.Settings.Default.off7.ToString();
            textBox26.Text = Properties.Settings.Default.off8.ToString();
            textBox27.Text = Properties.Settings.Default.off9.ToString();

            textBox28.Text = Properties.Settings.Default.bas1.ToString();
            textBox29.Text = Properties.Settings.Default.bas2.ToString();
            textBox30.Text = Properties.Settings.Default.bas3.ToString();
            textBox31.Text = Properties.Settings.Default.bas4.ToString();
            textBox32.Text = Properties.Settings.Default.bas5.ToString();
            textBox33.Text = Properties.Settings.Default.bas6.ToString();
            textBox34.Text = Properties.Settings.Default.bas7.ToString();
            textBox35.Text = Properties.Settings.Default.bas8.ToString();
            textBox36.Text = Properties.Settings.Default.bas9.ToString();
        }


        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (textBox1.Text != "")
            {
                Properties.Settings.Default.map1 = Convert.ToInt32(textBox1.Text);
            }
            else
            {
                Properties.Settings.Default.map1 = 0;
            }
            
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            if (textBox2.Text != "")
            {
                Properties.Settings.Default.map2 = Convert.ToInt32(textBox2.Text);
            }
            else
            {
                Properties.Settings.Default.map2 = 0;
            }
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {
            if (textBox3.Text != "")
            {
                Properties.Settings.Default.map3 = Convert.ToInt32(textBox3.Text);
            }
            else
            {
                Properties.Settings.Default.map3 = 0;
            }
        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            if (textBox4.Text != "")
            {
                Properties.Settings.Default.map4 = Convert.ToInt32(textBox4.Text);
            }
            else
            {
                Properties.Settings.Default.map4 = 0;
            }
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            if (textBox5.Text != "")
            {
                Properties.Settings.Default.map5 = Convert.ToInt32(textBox5.Text);
            }
            else
            {
                Properties.Settings.Default.map5 = 0;
            }
        }

        private void textBox6_TextChanged(object sender, EventArgs e)
        {
            if (textBox6.Text != "")
            {
                Properties.Settings.Default.map6 = Convert.ToInt32(textBox6.Text);
            }
            else
            {
                Properties.Settings.Default.map6 = 0;
            }
        }

        private void textBox7_TextChanged(object sender, EventArgs e)
        {
            if (textBox7.Text != "")
            {
                Properties.Settings.Default.map7 = Convert.ToInt32(textBox7.Text);
            }
            else
            {
                Properties.Settings.Default.map7 = 0;
            }
        }

        private void textBox8_TextChanged(object sender, EventArgs e)
        {
            if (textBox8.Text != "")
            {
                Properties.Settings.Default.map8 = Convert.ToInt32(textBox8.Text);
            }
            else
            {
                Properties.Settings.Default.map8 = 0;
            }
        }

        private void textBox9_TextChanged(object sender, EventArgs e)
        {
            if (textBox9.Text != "")
            {
                Properties.Settings.Default.map9 = Convert.ToInt32(textBox9.Text);
            }
            else
            {
                Properties.Settings.Default.map9 = 0;
            }
        }

        private void label11_Click(object sender, EventArgs e)
        {

        }

        //###################################

        private void textBox10_TextChanged(object sender, EventArgs e)
        {
            if (textBox10.Text != "")
            {
                Properties.Settings.Default.div1 = Convert.ToInt32(textBox10.Text);
            }
            else 
            {
                Properties.Settings.Default.div1 = 0;
            }
        }

        private void textBox11_TextChanged(object sender, EventArgs e)
        {
            if (textBox11.Text != "")
            {
                Properties.Settings.Default.div2 = Convert.ToInt32(textBox11.Text);
            }
            else
            {
                Properties.Settings.Default.div2 = 0;
            }
        }

        private void textBox12_TextChanged(object sender, EventArgs e)
        {
            if (textBox12.Text != "")
            {
                Properties.Settings.Default.div3 = Convert.ToInt32(textBox12.Text);
            }
            else
            {
                Properties.Settings.Default.div3 = 0;
            }
        }

        private void textBox13_TextChanged(object sender, EventArgs e)
        {
            if (textBox13.Text != "")
            {
                Properties.Settings.Default.div4 = Convert.ToInt32(textBox13.Text);
            }
            else
            {
                Properties.Settings.Default.div4 = 0;
            }
        }

        private void textBox14_TextChanged(object sender, EventArgs e)
        {

            if (textBox14.Text != "")
            {
                Properties.Settings.Default.div5 = Convert.ToInt32(textBox14.Text);
            }
            else
            {
                Properties.Settings.Default.div5 = 0;
            }
        }

        private void textBox15_TextChanged(object sender, EventArgs e)
        {

            if (textBox15.Text != "")
            {
                Properties.Settings.Default.div6 = Convert.ToInt32(textBox15.Text);
            }
            else
            {
                Properties.Settings.Default.div6 = 0;
            }
        }

        private void textBox16_TextChanged(object sender, EventArgs e)
        {

            if (textBox16.Text != "")
            {
                Properties.Settings.Default.div7 = Convert.ToInt32(textBox16.Text);
            }
            else
            {
                Properties.Settings.Default.div7 = 0;
            }
        }

        private void textBox17_TextChanged(object sender, EventArgs e)
        {

            if (textBox17.Text != "")
            {
                Properties.Settings.Default.div8 = Convert.ToInt32(textBox17.Text);
            }
            else
            {
                Properties.Settings.Default.div8 = 0;
            }
        }

        private void textBox18_TextChanged(object sender, EventArgs e)
        {

            if (textBox18.Text != "")
            {
                Properties.Settings.Default.div9 = Convert.ToInt32(textBox18.Text);
            }
            else
            {
                Properties.Settings.Default.div9 = 0;
            }
        }

        // ##################################
        // offset
        // ####################################

        private void textBox19_TextChanged(object sender, EventArgs e)
        {
            if (textBox19.Text == "-")
                return;
            if (textBox19.Text != "")
            {
                Properties.Settings.Default.off1 = Convert.ToInt32(textBox19.Text);
            }
            else
            {
                Properties.Settings.Default.off1 = 0;
            }
        }

        private void textBox20_TextChanged(object sender, EventArgs e)
        {
            if (textBox20.Text == "-")
                return;
            if (textBox20.Text != "")
            {
                Properties.Settings.Default.off2 = Convert.ToInt32(textBox20.Text);
            }
            else
            {
                Properties.Settings.Default.off2 = 0;
            }
        }

        private void textBox21_TextChanged(object sender, EventArgs e)
        {
            if (textBox21.Text == "-")
                return;
            if (textBox21.Text != "")
            {
                Properties.Settings.Default.off3 = Convert.ToInt32(textBox21.Text);
            }
            else
            {
                Properties.Settings.Default.off3 = 0;
            }
        }

        private void textBox22_TextChanged(object sender, EventArgs e)
        {
            if (textBox22.Text == "-")
                return;
            if (textBox22.Text != "")
            {
                Properties.Settings.Default.off4 = Convert.ToInt32(textBox22.Text);
            }
            else
            {
                Properties.Settings.Default.off4 = 0;
            }
        }

        private void textBox23_TextChanged(object sender, EventArgs e)
        {
            if (textBox23.Text == "-")
                return;
            if (textBox23.Text != "")
            {
                Properties.Settings.Default.off5 = Convert.ToInt32(textBox23.Text);
            }
            else
            {
                Properties.Settings.Default.off5 = 0;
            }
        }

        private void textBox24_TextChanged(object sender, EventArgs e)
        {
            if (textBox24.Text == "-")
                return;
            if (textBox24.Text != "")
            {
                Properties.Settings.Default.off6 = Convert.ToInt32(textBox24.Text);
            }
            else
            {
                Properties.Settings.Default.off6 = 0;
            }
        }

        private void textBox25_TextChanged(object sender, EventArgs e)
        {
            if (textBox25.Text == "-")
                return;
            if (textBox25.Text != "")
            {
                Properties.Settings.Default.off7 = Convert.ToInt32(textBox25.Text);
            }
            else
            {
                Properties.Settings.Default.off7 = 0;
            }
        }

        private void textBox26_TextChanged(object sender, EventArgs e)
        {
            if (textBox26.Text == "-")
                return;
            if (textBox26.Text != "")
            {
                Properties.Settings.Default.off8 = Convert.ToInt32(textBox26.Text);
            }
            else
            {
                Properties.Settings.Default.off8 = 0;
            }
        }

        private void textBox27_TextChanged(object sender, EventArgs e)
        {
            if (textBox27.Text == "-")
                return;
            if (textBox27.Text != "")
            {
                Properties.Settings.Default.off9 = Convert.ToInt32(textBox27.Text);
            }
            else
            {
                Properties.Settings.Default.off9 = 0;
            }
        }

        //#####################################
        // BIT
        //#####################################

        private void textBox28_TextChanged(object sender, EventArgs e)
        {
            if (textBox28.Text != "")
            {
                Properties.Settings.Default.bas1 = Convert.ToInt32(textBox28.Text);
            }
            else
            {
                Properties.Settings.Default.bas1 = 0;
            }
        }

        private void textBox29_TextChanged(object sender, EventArgs e)
        {
            if (textBox29.Text != "")
            {
                Properties.Settings.Default.bas2 = Convert.ToInt32(textBox29.Text);
            }
            else
            {
                Properties.Settings.Default.bas2 = 0;
            }
        }

        private void textBox30_TextChanged(object sender, EventArgs e)
        {

            if (textBox30.Text != "")
            {
                Properties.Settings.Default.bas3 = Convert.ToInt32(textBox30.Text);
            }
            else
            {
                Properties.Settings.Default.bas3 = 0;
            }
        }

        private void textBox31_TextChanged(object sender, EventArgs e)
        {

            if (textBox31.Text != "")
            {
                Properties.Settings.Default.bas4 = Convert.ToInt32(textBox31.Text);
            }
            else
            {
                Properties.Settings.Default.bas4 = 0;
            }
        }

        private void textBox32_TextChanged(object sender, EventArgs e)
        {

            if (textBox32.Text != "")
            {
                Properties.Settings.Default.bas5 = Convert.ToInt32(textBox32.Text);
            }
            else
            {
                Properties.Settings.Default.bas5 = 0;
            }
        }

        private void textBox33_TextChanged(object sender, EventArgs e)
        {

            if (textBox33.Text != "")
            {
                Properties.Settings.Default.bas6 = Convert.ToInt32(textBox33.Text);
            }
            else
            {
                Properties.Settings.Default.bas6 = 0;
            }
        }

        private void textBox34_TextChanged(object sender, EventArgs e)
        {

            if (textBox34.Text != "")
            {
                Properties.Settings.Default.bas7 = Convert.ToInt32(textBox34.Text);
            }
            else
            {
                Properties.Settings.Default.bas7 = 0;
            }
        }

        private void textBox35_TextChanged(object sender, EventArgs e)
        {

            if (textBox35.Text != "")
            {
                Properties.Settings.Default.bas8 = Convert.ToInt32(textBox35.Text);
            }
            else
            {
                Properties.Settings.Default.bas8 = 0;
            }
        }

        private void textBox36_TextChanged(object sender, EventArgs e)
        {

            if (textBox36.Text != "")
            {
                Properties.Settings.Default.bas9 = Convert.ToInt32(textBox36.Text);
            }
            else
            {
                Properties.Settings.Default.bas9 = 0;
            }
        }
    }
}