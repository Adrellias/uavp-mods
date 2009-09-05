namespace UAVP.UAVPSet
{
    partial class Receiver
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Receiver));
            this.gasTrackBar = new System.Windows.Forms.TrackBar();
            this.RollTrackBar = new System.Windows.Forms.TrackBar();
            this.PitchTrackBar = new System.Windows.Forms.TrackBar();
            this.YawTrackBar = new System.Windows.Forms.TrackBar();
            this.ch5TrackBar = new System.Windows.Forms.TrackBar();
            this.gasLabel = new System.Windows.Forms.Label();
            this.RollLabel = new System.Windows.Forms.Label();
            this.PitchLabel = new System.Windows.Forms.Label();
            this.YawLabel = new System.Windows.Forms.Label();
            this.ch5Label = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.okButton = new System.Windows.Forms.Button();
            this.ch6TrackBar = new System.Windows.Forms.TrackBar();
            this.ch7TrackBar = new System.Windows.Forms.TrackBar();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.ch6Label = new System.Windows.Forms.Label();
            this.ch7Label = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.gasTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RollTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PitchTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.YawTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ch5TrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ch6TrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ch7TrackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // gasTrackBar
            // 
            resources.ApplyResources(this.gasTrackBar, "gasTrackBar");
            this.gasTrackBar.LargeChange = 10;
            this.gasTrackBar.Maximum = 125;
            this.gasTrackBar.Minimum = -25;
            this.gasTrackBar.Name = "gasTrackBar";
            this.gasTrackBar.TickFrequency = 10;
            // 
            // RollTrackBar
            // 
            resources.ApplyResources(this.RollTrackBar, "RollTrackBar");
            this.RollTrackBar.LargeChange = 10;
            this.RollTrackBar.Maximum = 125;
            this.RollTrackBar.Minimum = -125;
            this.RollTrackBar.Name = "RollTrackBar";
            this.RollTrackBar.TickFrequency = 10;
            // 
            // PitchTrackBar
            // 
            resources.ApplyResources(this.PitchTrackBar, "PitchTrackBar");
            this.PitchTrackBar.LargeChange = 10;
            this.PitchTrackBar.Maximum = 125;
            this.PitchTrackBar.Minimum = -125;
            this.PitchTrackBar.Name = "PitchTrackBar";
            this.PitchTrackBar.TickFrequency = 10;
            // 
            // YawTrackBar
            // 
            resources.ApplyResources(this.YawTrackBar, "YawTrackBar");
            this.YawTrackBar.LargeChange = 10;
            this.YawTrackBar.Maximum = 125;
            this.YawTrackBar.Minimum = -125;
            this.YawTrackBar.Name = "YawTrackBar";
            this.YawTrackBar.TickFrequency = 10;
            // 
            // ch5TrackBar
            // 
            this.ch5TrackBar.LargeChange = 10;
            resources.ApplyResources(this.ch5TrackBar, "ch5TrackBar");
            this.ch5TrackBar.Maximum = 125;
            this.ch5TrackBar.Minimum = -25;
            this.ch5TrackBar.Name = "ch5TrackBar";
            this.ch5TrackBar.TickFrequency = 10;
            // 
            // gasLabel
            // 
            resources.ApplyResources(this.gasLabel, "gasLabel");
            this.gasLabel.Name = "gasLabel";
            this.gasLabel.TextChanged += new System.EventHandler(this.gasLabel_TextChanged);
            // 
            // RollLabel
            // 
            resources.ApplyResources(this.RollLabel, "RollLabel");
            this.RollLabel.Name = "RollLabel";
            this.RollLabel.TextChanged += new System.EventHandler(this.RollLabel_TextChanged);
            // 
            // PitchLabel
            // 
            resources.ApplyResources(this.PitchLabel, "PitchLabel");
            this.PitchLabel.Name = "PitchLabel";
            this.PitchLabel.TextChanged += new System.EventHandler(this.PitchLabel_TextChanged);
            // 
            // YawLabel
            // 
            resources.ApplyResources(this.YawLabel, "YawLabel");
            this.YawLabel.Name = "YawLabel";
            this.YawLabel.TextChanged += new System.EventHandler(this.YawLabel_TextChanged);
            // 
            // ch5Label
            // 
            resources.ApplyResources(this.ch5Label, "ch5Label");
            this.ch5Label.Name = "ch5Label";
            this.ch5Label.TextChanged += new System.EventHandler(this.ch5Label_TextChanged);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // timer1
            // 
            this.timer1.Interval = 300;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // okButton
            // 
            resources.ApplyResources(this.okButton, "okButton");
            this.okButton.Name = "okButton";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // ch6TrackBar
            // 
            resources.ApplyResources(this.ch6TrackBar, "ch6TrackBar");
            this.ch6TrackBar.LargeChange = 10;
            this.ch6TrackBar.Maximum = 125;
            this.ch6TrackBar.Minimum = -25;
            this.ch6TrackBar.Name = "ch6TrackBar";
            this.ch6TrackBar.TickFrequency = 10;
            // 
            // ch7TrackBar
            // 
            resources.ApplyResources(this.ch7TrackBar, "ch7TrackBar");
            this.ch7TrackBar.LargeChange = 10;
            this.ch7TrackBar.Maximum = 125;
            this.ch7TrackBar.Minimum = -25;
            this.ch7TrackBar.Name = "ch7TrackBar";
            this.ch7TrackBar.TickFrequency = 10;
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.Name = "label7";
            // 
            // ch6Label
            // 
            resources.ApplyResources(this.ch6Label, "ch6Label");
            this.ch6Label.Name = "ch6Label";
            this.ch6Label.TextChanged += new System.EventHandler(this.ch6Label_TextChanged);
            // 
            // ch7Label
            // 
            resources.ApplyResources(this.ch7Label, "ch7Label");
            this.ch7Label.Name = "ch7Label";
            this.ch7Label.TextChanged += new System.EventHandler(this.ch7Label_TextChanged);
            // 
            // Receiver
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.ch7Label);
            this.Controls.Add(this.ch6Label);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.ch7TrackBar);
            this.Controls.Add(this.ch6TrackBar);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ch5Label);
            this.Controls.Add(this.YawLabel);
            this.Controls.Add(this.PitchLabel);
            this.Controls.Add(this.RollLabel);
            this.Controls.Add(this.gasLabel);
            this.Controls.Add(this.ch5TrackBar);
            this.Controls.Add(this.YawTrackBar);
            this.Controls.Add(this.PitchTrackBar);
            this.Controls.Add(this.RollTrackBar);
            this.Controls.Add(this.gasTrackBar);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "Receiver";
            this.ShowInTaskbar = false;
            this.VisibleChanged += new System.EventHandler(this.Receiver_VisibleChanged);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Receiver_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.gasTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RollTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PitchTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.YawTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ch5TrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ch6TrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ch7TrackBar)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TrackBar gasTrackBar;
        private System.Windows.Forms.TrackBar RollTrackBar;
        private System.Windows.Forms.TrackBar PitchTrackBar;
        private System.Windows.Forms.TrackBar YawTrackBar;
        private System.Windows.Forms.TrackBar ch5TrackBar;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        public System.Windows.Forms.Label gasLabel;
        public System.Windows.Forms.Label RollLabel;
        public System.Windows.Forms.Label PitchLabel;
        public System.Windows.Forms.Label YawLabel;
        public System.Windows.Forms.Label ch5Label;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.TrackBar ch6TrackBar;
        private System.Windows.Forms.TrackBar ch7TrackBar;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        public System.Windows.Forms.Label ch6Label;
        public System.Windows.Forms.Label ch7Label;

    }
}