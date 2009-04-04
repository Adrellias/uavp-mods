namespace UAVP.UAVPSet
{
    partial class Configuration
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Configuration));
            this.configurationTabControl = new System.Windows.Forms.TabControl();
            this.tabPageCOM = new System.Windows.Forms.TabPage();
            this.timeOutMaskedTextBox = new System.Windows.Forms.MaskedTextBox();
            this.timeLabel = new System.Windows.Forms.Label();
            this.flussLabel = new System.Windows.Forms.Label();
            this.flussComboBox = new System.Windows.Forms.ComboBox();
            this.stopBitsLabel = new System.Windows.Forms.Label();
            this.stopBitsComboBox = new System.Windows.Forms.ComboBox();
            this.paritätLabel = new System.Windows.Forms.Label();
            this.paritaetComboBox = new System.Windows.Forms.ComboBox();
            this.bitsSLabel = new System.Windows.Forms.Label();
            this.bitsSComboBox = new System.Windows.Forms.ComboBox();
            this.comPortLabel = new System.Windows.Forms.Label();
            this.comPortComboBox = new System.Windows.Forms.ComboBox();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.loglevelComboBox = new System.Windows.Forms.ComboBox();
            this.loglevelLabel = new System.Windows.Forms.Label();
            this.divTabPage = new System.Windows.Forms.TabPage();
            this.autoDisCheckBox = new System.Windows.Forms.CheckBox();
            this.readSleepMaskedText = new System.Windows.Forms.MaskedTextBox();
            this.ReadSleepLabel = new System.Windows.Forms.Label();
            this.versionComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.languageComboBox = new System.Windows.Forms.ComboBox();
            this.languageLabel = new System.Windows.Forms.Label();
            this.askBurnPicCheckBox = new System.Windows.Forms.CheckBox();
            this.writeSleepMaskedTextBox = new System.Windows.Forms.MaskedTextBox();
            this.writeSleepLabel = new System.Windows.Forms.Label();
            this.splashCheckBox = new System.Windows.Forms.CheckBox();
            this.okButton = new System.Windows.Forms.Button();
            this.configurationTabControl.SuspendLayout();
            this.tabPageCOM.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.divTabPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // configurationTabControl
            // 
            this.configurationTabControl.AccessibleDescription = null;
            this.configurationTabControl.AccessibleName = null;
            resources.ApplyResources(this.configurationTabControl, "configurationTabControl");
            this.configurationTabControl.BackgroundImage = null;
            this.configurationTabControl.Controls.Add(this.tabPageCOM);
            this.configurationTabControl.Controls.Add(this.tabPage1);
            this.configurationTabControl.Controls.Add(this.divTabPage);
            this.configurationTabControl.Font = null;
            this.configurationTabControl.Name = "configurationTabControl";
            this.configurationTabControl.SelectedIndex = 0;
            // 
            // tabPageCOM
            // 
            this.tabPageCOM.AccessibleDescription = null;
            this.tabPageCOM.AccessibleName = null;
            resources.ApplyResources(this.tabPageCOM, "tabPageCOM");
            this.tabPageCOM.BackgroundImage = null;
            this.tabPageCOM.Controls.Add(this.timeOutMaskedTextBox);
            this.tabPageCOM.Controls.Add(this.timeLabel);
            this.tabPageCOM.Controls.Add(this.flussLabel);
            this.tabPageCOM.Controls.Add(this.flussComboBox);
            this.tabPageCOM.Controls.Add(this.stopBitsLabel);
            this.tabPageCOM.Controls.Add(this.stopBitsComboBox);
            this.tabPageCOM.Controls.Add(this.paritätLabel);
            this.tabPageCOM.Controls.Add(this.paritaetComboBox);
            this.tabPageCOM.Controls.Add(this.bitsSLabel);
            this.tabPageCOM.Controls.Add(this.bitsSComboBox);
            this.tabPageCOM.Controls.Add(this.comPortLabel);
            this.tabPageCOM.Controls.Add(this.comPortComboBox);
            this.tabPageCOM.Font = null;
            this.tabPageCOM.Name = "tabPageCOM";
            this.tabPageCOM.UseVisualStyleBackColor = true;
            // 
            // timeOutMaskedTextBox
            // 
            this.timeOutMaskedTextBox.AccessibleDescription = null;
            this.timeOutMaskedTextBox.AccessibleName = null;
            resources.ApplyResources(this.timeOutMaskedTextBox, "timeOutMaskedTextBox");
            this.timeOutMaskedTextBox.BackgroundImage = null;
            this.timeOutMaskedTextBox.Font = null;
            this.timeOutMaskedTextBox.Name = "timeOutMaskedTextBox";
            this.timeOutMaskedTextBox.TextChanged += new System.EventHandler(this.timeOutMaskedTextBox_TextChanged);
            // 
            // timeLabel
            // 
            this.timeLabel.AccessibleDescription = null;
            this.timeLabel.AccessibleName = null;
            resources.ApplyResources(this.timeLabel, "timeLabel");
            this.timeLabel.Font = null;
            this.timeLabel.Name = "timeLabel";
            // 
            // flussLabel
            // 
            this.flussLabel.AccessibleDescription = null;
            this.flussLabel.AccessibleName = null;
            resources.ApplyResources(this.flussLabel, "flussLabel");
            this.flussLabel.Font = null;
            this.flussLabel.Name = "flussLabel";
            // 
            // flussComboBox
            // 
            this.flussComboBox.AccessibleDescription = null;
            this.flussComboBox.AccessibleName = null;
            resources.ApplyResources(this.flussComboBox, "flussComboBox");
            this.flussComboBox.BackgroundImage = null;
            this.flussComboBox.Font = null;
            this.flussComboBox.FormattingEnabled = true;
            this.flussComboBox.Items.AddRange(new object[] {
            resources.GetString("flussComboBox.Items"),
            resources.GetString("flussComboBox.Items1"),
            resources.GetString("flussComboBox.Items2"),
            resources.GetString("flussComboBox.Items3")});
            this.flussComboBox.Name = "flussComboBox";
            this.flussComboBox.SelectedIndexChanged += new System.EventHandler(this.flussComboBox_SelectedIndexChanged);
            // 
            // stopBitsLabel
            // 
            this.stopBitsLabel.AccessibleDescription = null;
            this.stopBitsLabel.AccessibleName = null;
            resources.ApplyResources(this.stopBitsLabel, "stopBitsLabel");
            this.stopBitsLabel.Font = null;
            this.stopBitsLabel.Name = "stopBitsLabel";
            // 
            // stopBitsComboBox
            // 
            this.stopBitsComboBox.AccessibleDescription = null;
            this.stopBitsComboBox.AccessibleName = null;
            resources.ApplyResources(this.stopBitsComboBox, "stopBitsComboBox");
            this.stopBitsComboBox.BackgroundImage = null;
            this.stopBitsComboBox.Font = null;
            this.stopBitsComboBox.FormattingEnabled = true;
            this.stopBitsComboBox.Items.AddRange(new object[] {
            resources.GetString("stopBitsComboBox.Items"),
            resources.GetString("stopBitsComboBox.Items1"),
            resources.GetString("stopBitsComboBox.Items2"),
            resources.GetString("stopBitsComboBox.Items3")});
            this.stopBitsComboBox.Name = "stopBitsComboBox";
            this.stopBitsComboBox.SelectedIndexChanged += new System.EventHandler(this.stopBitsComboBox_SelectedIndexChanged);
            // 
            // paritätLabel
            // 
            this.paritätLabel.AccessibleDescription = null;
            this.paritätLabel.AccessibleName = null;
            resources.ApplyResources(this.paritätLabel, "paritätLabel");
            this.paritätLabel.Font = null;
            this.paritätLabel.Name = "paritätLabel";
            // 
            // paritaetComboBox
            // 
            this.paritaetComboBox.AccessibleDescription = null;
            this.paritaetComboBox.AccessibleName = null;
            resources.ApplyResources(this.paritaetComboBox, "paritaetComboBox");
            this.paritaetComboBox.BackgroundImage = null;
            this.paritaetComboBox.Font = null;
            this.paritaetComboBox.FormattingEnabled = true;
            this.paritaetComboBox.Items.AddRange(new object[] {
            resources.GetString("paritaetComboBox.Items"),
            resources.GetString("paritaetComboBox.Items1"),
            resources.GetString("paritaetComboBox.Items2"),
            resources.GetString("paritaetComboBox.Items3"),
            resources.GetString("paritaetComboBox.Items4")});
            this.paritaetComboBox.Name = "paritaetComboBox";
            this.paritaetComboBox.SelectedIndexChanged += new System.EventHandler(this.paritaetComboBox_SelectedIndexChanged);
            // 
            // bitsSLabel
            // 
            this.bitsSLabel.AccessibleDescription = null;
            this.bitsSLabel.AccessibleName = null;
            resources.ApplyResources(this.bitsSLabel, "bitsSLabel");
            this.bitsSLabel.Font = null;
            this.bitsSLabel.Name = "bitsSLabel";
            // 
            // bitsSComboBox
            // 
            this.bitsSComboBox.AccessibleDescription = null;
            this.bitsSComboBox.AccessibleName = null;
            resources.ApplyResources(this.bitsSComboBox, "bitsSComboBox");
            this.bitsSComboBox.BackgroundImage = null;
            this.bitsSComboBox.Font = null;
            this.bitsSComboBox.FormattingEnabled = true;
            this.bitsSComboBox.Items.AddRange(new object[] {
            resources.GetString("bitsSComboBox.Items"),
            resources.GetString("bitsSComboBox.Items1"),
            resources.GetString("bitsSComboBox.Items2"),
            resources.GetString("bitsSComboBox.Items3"),
            resources.GetString("bitsSComboBox.Items4"),
            resources.GetString("bitsSComboBox.Items5"),
            resources.GetString("bitsSComboBox.Items6"),
            resources.GetString("bitsSComboBox.Items7"),
            resources.GetString("bitsSComboBox.Items8"),
            resources.GetString("bitsSComboBox.Items9"),
            resources.GetString("bitsSComboBox.Items10"),
            resources.GetString("bitsSComboBox.Items11"),
            resources.GetString("bitsSComboBox.Items12")});
            this.bitsSComboBox.Name = "bitsSComboBox";
            this.bitsSComboBox.SelectedIndexChanged += new System.EventHandler(this.bitsSComboBox_SelectedIndexChanged);
            // 
            // comPortLabel
            // 
            this.comPortLabel.AccessibleDescription = null;
            this.comPortLabel.AccessibleName = null;
            resources.ApplyResources(this.comPortLabel, "comPortLabel");
            this.comPortLabel.Font = null;
            this.comPortLabel.Name = "comPortLabel";
            // 
            // comPortComboBox
            // 
            this.comPortComboBox.AccessibleDescription = null;
            this.comPortComboBox.AccessibleName = null;
            resources.ApplyResources(this.comPortComboBox, "comPortComboBox");
            this.comPortComboBox.BackgroundImage = null;
            this.comPortComboBox.Font = null;
            this.comPortComboBox.FormattingEnabled = true;
            this.comPortComboBox.Name = "comPortComboBox";
            this.comPortComboBox.SelectedIndexChanged += new System.EventHandler(this.comPortComboBox_SelectedIndexChanged_1);
            // 
            // tabPage1
            // 
            this.tabPage1.AccessibleDescription = null;
            this.tabPage1.AccessibleName = null;
            resources.ApplyResources(this.tabPage1, "tabPage1");
            this.tabPage1.BackgroundImage = null;
            this.tabPage1.Controls.Add(this.loglevelComboBox);
            this.tabPage1.Controls.Add(this.loglevelLabel);
            this.tabPage1.Font = null;
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // loglevelComboBox
            // 
            this.loglevelComboBox.AccessibleDescription = null;
            this.loglevelComboBox.AccessibleName = null;
            resources.ApplyResources(this.loglevelComboBox, "loglevelComboBox");
            this.loglevelComboBox.BackgroundImage = null;
            this.loglevelComboBox.Font = null;
            this.loglevelComboBox.FormattingEnabled = true;
            this.loglevelComboBox.Items.AddRange(new object[] {
            resources.GetString("loglevelComboBox.Items"),
            resources.GetString("loglevelComboBox.Items1")});
            this.loglevelComboBox.Name = "loglevelComboBox";
            this.loglevelComboBox.SelectedIndexChanged += new System.EventHandler(this.loglevelComboBox_SelectedIndexChanged);
            // 
            // loglevelLabel
            // 
            this.loglevelLabel.AccessibleDescription = null;
            this.loglevelLabel.AccessibleName = null;
            resources.ApplyResources(this.loglevelLabel, "loglevelLabel");
            this.loglevelLabel.Font = null;
            this.loglevelLabel.Name = "loglevelLabel";
            // 
            // divTabPage
            // 
            this.divTabPage.AccessibleDescription = null;
            this.divTabPage.AccessibleName = null;
            resources.ApplyResources(this.divTabPage, "divTabPage");
            this.divTabPage.BackgroundImage = null;
            this.divTabPage.Controls.Add(this.autoDisCheckBox);
            this.divTabPage.Controls.Add(this.readSleepMaskedText);
            this.divTabPage.Controls.Add(this.ReadSleepLabel);
            this.divTabPage.Controls.Add(this.versionComboBox);
            this.divTabPage.Controls.Add(this.label1);
            this.divTabPage.Controls.Add(this.languageComboBox);
            this.divTabPage.Controls.Add(this.languageLabel);
            this.divTabPage.Controls.Add(this.askBurnPicCheckBox);
            this.divTabPage.Controls.Add(this.writeSleepMaskedTextBox);
            this.divTabPage.Controls.Add(this.writeSleepLabel);
            this.divTabPage.Controls.Add(this.splashCheckBox);
            this.divTabPage.Font = null;
            this.divTabPage.Name = "divTabPage";
            this.divTabPage.UseVisualStyleBackColor = true;
            // 
            // autoDisCheckBox
            // 
            this.autoDisCheckBox.AccessibleDescription = null;
            this.autoDisCheckBox.AccessibleName = null;
            resources.ApplyResources(this.autoDisCheckBox, "autoDisCheckBox");
            this.autoDisCheckBox.BackgroundImage = null;
            this.autoDisCheckBox.Font = null;
            this.autoDisCheckBox.Name = "autoDisCheckBox";
            this.autoDisCheckBox.UseVisualStyleBackColor = true;
            this.autoDisCheckBox.CheckedChanged += new System.EventHandler(this.autoDisCheckBox_CheckedChanged);
            // 
            // readSleepMaskedText
            // 
            this.readSleepMaskedText.AccessibleDescription = null;
            this.readSleepMaskedText.AccessibleName = null;
            resources.ApplyResources(this.readSleepMaskedText, "readSleepMaskedText");
            this.readSleepMaskedText.BackgroundImage = null;
            this.readSleepMaskedText.Culture = new System.Globalization.CultureInfo("");
            this.readSleepMaskedText.Font = null;
            this.readSleepMaskedText.Name = "readSleepMaskedText";
            this.readSleepMaskedText.ValidatingType = typeof(int);
            this.readSleepMaskedText.TextChanged += new System.EventHandler(this.readSleepMaskedText_TextChanged);
            // 
            // ReadSleepLabel
            // 
            this.ReadSleepLabel.AccessibleDescription = null;
            this.ReadSleepLabel.AccessibleName = null;
            resources.ApplyResources(this.ReadSleepLabel, "ReadSleepLabel");
            this.ReadSleepLabel.Font = null;
            this.ReadSleepLabel.Name = "ReadSleepLabel";
            // 
            // versionComboBox
            // 
            this.versionComboBox.AccessibleDescription = null;
            this.versionComboBox.AccessibleName = null;
            resources.ApplyResources(this.versionComboBox, "versionComboBox");
            this.versionComboBox.BackgroundImage = null;
            this.versionComboBox.Font = null;
            this.versionComboBox.FormattingEnabled = true;
            this.versionComboBox.Items.AddRange(new object[] {
            resources.GetString("versionComboBox.Items"),
            resources.GetString("versionComboBox.Items1"),
            resources.GetString("versionComboBox.Items2"),
            resources.GetString("versionComboBox.Items3")});
            this.versionComboBox.Name = "versionComboBox";
            this.versionComboBox.SelectedIndexChanged += new System.EventHandler(this.versionComboBox_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AccessibleDescription = null;
            this.label1.AccessibleName = null;
            resources.ApplyResources(this.label1, "label1");
            this.label1.Font = null;
            this.label1.Name = "label1";
            // 
            // languageComboBox
            // 
            this.languageComboBox.AccessibleDescription = null;
            this.languageComboBox.AccessibleName = null;
            resources.ApplyResources(this.languageComboBox, "languageComboBox");
            this.languageComboBox.BackgroundImage = null;
            this.languageComboBox.Font = null;
            this.languageComboBox.FormattingEnabled = true;
            this.languageComboBox.Items.AddRange(new object[] {
            resources.GetString("languageComboBox.Items"),
            resources.GetString("languageComboBox.Items1"),
            resources.GetString("languageComboBox.Items2"),
            resources.GetString("languageComboBox.Items3")});
            this.languageComboBox.Name = "languageComboBox";
            this.languageComboBox.SelectedIndexChanged += new System.EventHandler(this.languageComboBox_SelectedIndexChanged);
            // 
            // languageLabel
            // 
            this.languageLabel.AccessibleDescription = null;
            this.languageLabel.AccessibleName = null;
            resources.ApplyResources(this.languageLabel, "languageLabel");
            this.languageLabel.Font = null;
            this.languageLabel.Name = "languageLabel";
            // 
            // askBurnPicCheckBox
            // 
            this.askBurnPicCheckBox.AccessibleDescription = null;
            this.askBurnPicCheckBox.AccessibleName = null;
            resources.ApplyResources(this.askBurnPicCheckBox, "askBurnPicCheckBox");
            this.askBurnPicCheckBox.BackgroundImage = null;
            this.askBurnPicCheckBox.Checked = true;
            this.askBurnPicCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.askBurnPicCheckBox.Font = null;
            this.askBurnPicCheckBox.Name = "askBurnPicCheckBox";
            this.askBurnPicCheckBox.UseVisualStyleBackColor = true;
            this.askBurnPicCheckBox.CheckedChanged += new System.EventHandler(this.askBurnPicCheckBox_CheckedChanged);
            // 
            // writeSleepMaskedTextBox
            // 
            this.writeSleepMaskedTextBox.AccessibleDescription = null;
            this.writeSleepMaskedTextBox.AccessibleName = null;
            resources.ApplyResources(this.writeSleepMaskedTextBox, "writeSleepMaskedTextBox");
            this.writeSleepMaskedTextBox.BackgroundImage = null;
            this.writeSleepMaskedTextBox.Culture = new System.Globalization.CultureInfo("");
            this.writeSleepMaskedTextBox.Font = null;
            this.writeSleepMaskedTextBox.Name = "writeSleepMaskedTextBox";
            this.writeSleepMaskedTextBox.ValidatingType = typeof(int);
            this.writeSleepMaskedTextBox.TextChanged += new System.EventHandler(this.writeSleepMaskedTextBox_TextChanged);
            // 
            // writeSleepLabel
            // 
            this.writeSleepLabel.AccessibleDescription = null;
            this.writeSleepLabel.AccessibleName = null;
            resources.ApplyResources(this.writeSleepLabel, "writeSleepLabel");
            this.writeSleepLabel.Font = null;
            this.writeSleepLabel.Name = "writeSleepLabel";
            // 
            // splashCheckBox
            // 
            this.splashCheckBox.AccessibleDescription = null;
            this.splashCheckBox.AccessibleName = null;
            resources.ApplyResources(this.splashCheckBox, "splashCheckBox");
            this.splashCheckBox.BackgroundImage = null;
            this.splashCheckBox.Checked = true;
            this.splashCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.splashCheckBox.Font = null;
            this.splashCheckBox.Name = "splashCheckBox";
            this.splashCheckBox.UseVisualStyleBackColor = true;
            this.splashCheckBox.CheckedChanged += new System.EventHandler(this.splashCheckBox_CheckedChanged);
            // 
            // okButton
            // 
            this.okButton.AccessibleDescription = null;
            this.okButton.AccessibleName = null;
            resources.ApplyResources(this.okButton, "okButton");
            this.okButton.BackgroundImage = null;
            this.okButton.Font = null;
            this.okButton.Name = "okButton";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // Configuration
            // 
            this.AccessibleDescription = null;
            this.AccessibleName = null;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = null;
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.configurationTabControl);
            this.Font = null;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = null;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Configuration";
            this.ShowInTaskbar = false;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Configuration_FormClosing);
            this.configurationTabControl.ResumeLayout(false);
            this.tabPageCOM.ResumeLayout(false);
            this.tabPageCOM.PerformLayout();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.divTabPage.ResumeLayout(false);
            this.divTabPage.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabPage tabPageCOM;
        private System.Windows.Forms.ComboBox comPortComboBox;
        private System.Windows.Forms.Label comPortLabel;
        private System.Windows.Forms.Label bitsSLabel;
        private System.Windows.Forms.ComboBox bitsSComboBox;
        private System.Windows.Forms.Label paritätLabel;
        private System.Windows.Forms.ComboBox paritaetComboBox;
        private System.Windows.Forms.Label flussLabel;
        private System.Windows.Forms.ComboBox flussComboBox;
        private System.Windows.Forms.Label stopBitsLabel;
        private System.Windows.Forms.ComboBox stopBitsComboBox;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.ComboBox loglevelComboBox;
        private System.Windows.Forms.Label loglevelLabel;
        public System.Windows.Forms.TabControl configurationTabControl;
        private System.Windows.Forms.TabPage divTabPage;
        private System.Windows.Forms.CheckBox splashCheckBox;
        private System.Windows.Forms.Label timeLabel;
        private System.Windows.Forms.MaskedTextBox writeSleepMaskedTextBox;
        private System.Windows.Forms.Label writeSleepLabel;
        private System.Windows.Forms.MaskedTextBox timeOutMaskedTextBox;
        private System.Windows.Forms.CheckBox askBurnPicCheckBox;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.ComboBox languageComboBox;
        private System.Windows.Forms.Label languageLabel;
        private System.Windows.Forms.ComboBox versionComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox autoDisCheckBox;
        private System.Windows.Forms.Label ReadSleepLabel;
        private System.Windows.Forms.MaskedTextBox readSleepMaskedText;
    }
}