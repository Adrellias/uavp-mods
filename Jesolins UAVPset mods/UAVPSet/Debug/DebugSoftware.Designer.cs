namespace UAVP.UAVPSet.Debug
{
    partial class DebugSoftware
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DebugSoftware));
            this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sTOPToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.dia2Panel = new System.Windows.Forms.Panel();
            this.dia3Panel = new System.Windows.Forms.Panel();
            this.dia1Panel = new System.Windows.Forms.Panel();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // saveFileDialog
            // 
            this.saveFileDialog.Title = "Save Debug Data";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem,
            this.sTOPToolStripMenuItem,
            this.fileToolStripMenuItem,
            this.configurationToolStripMenuItem,
            this.saveToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(925, 24);
            this.menuStrip1.TabIndex = 7;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // connectToolStripMenuItem
            // 
            this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
            this.connectToolStripMenuItem.Size = new System.Drawing.Size(93, 20);
            this.connectToolStripMenuItem.Text = "Connect / Start";
            this.connectToolStripMenuItem.Click += new System.EventHandler(this.connectToolStripMenuItem_Click);
            // 
            // sTOPToolStripMenuItem
            // 
            this.sTOPToolStripMenuItem.Enabled = false;
            this.sTOPToolStripMenuItem.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold);
            this.sTOPToolStripMenuItem.ForeColor = System.Drawing.SystemColors.ControlText;
            this.sTOPToolStripMenuItem.Name = "sTOPToolStripMenuItem";
            this.sTOPToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.sTOPToolStripMenuItem.Text = "STOP";
            this.sTOPToolStripMenuItem.Click += new System.EventHandler(this.sTOPToolStripMenuItem_Click);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // loadToolStripMenuItem
            // 
            this.loadToolStripMenuItem.Name = "loadToolStripMenuItem";
            this.loadToolStripMenuItem.Size = new System.Drawing.Size(108, 22);
            this.loadToolStripMenuItem.Text = "Load";
            this.loadToolStripMenuItem.Click += new System.EventHandler(this.loadToolStripMenuItem_Click);
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(84, 20);
            this.configurationToolStripMenuItem.Text = "Configuration";
            this.configurationToolStripMenuItem.Click += new System.EventHandler(this.configurationToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(43, 20);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // dia2Panel
            // 
            this.dia2Panel.BackColor = System.Drawing.Color.Black;
            this.dia2Panel.BackgroundImage = global::UAVP.UAVPSet.Properties.Resources.diagram;
            this.dia2Panel.Location = new System.Drawing.Point(13, 253);
            this.dia2Panel.Name = "dia2Panel";
            this.dia2Panel.Size = new System.Drawing.Size(900, 220);
            this.dia2Panel.TabIndex = 9;
            // 
            // dia3Panel
            // 
            this.dia3Panel.BackColor = System.Drawing.Color.Black;
            this.dia3Panel.BackgroundImage = global::UAVP.UAVPSet.Properties.Resources.diagram;
            this.dia3Panel.Location = new System.Drawing.Point(13, 479);
            this.dia3Panel.Name = "dia3Panel";
            this.dia3Panel.Size = new System.Drawing.Size(900, 220);
            this.dia3Panel.TabIndex = 9;
            // 
            // dia1Panel
            // 
            this.dia1Panel.BackColor = System.Drawing.Color.Black;
            this.dia1Panel.BackgroundImage = global::UAVP.UAVPSet.Properties.Resources.diagram;
            this.dia1Panel.Location = new System.Drawing.Point(13, 27);
            this.dia1Panel.Name = "dia1Panel";
            this.dia1Panel.Size = new System.Drawing.Size(900, 220);
            this.dia1Panel.TabIndex = 8;
            // 
            // DebugSoftware
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(925, 713);
            this.Controls.Add(this.dia3Panel);
            this.Controls.Add(this.dia2Panel);
            this.Controls.Add(this.dia1Panel);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "DebugSoftware";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Debug Software";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.SaveFileDialog saveFileDialog;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Panel dia1Panel;
        private System.Windows.Forms.Panel dia2Panel;
        private System.Windows.Forms.Panel dia3Panel;
        private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sTOPToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;

    }
}