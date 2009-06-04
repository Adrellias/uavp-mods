namespace UAVP.UAVPSet
{
    partial class Neutral
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Neutral));
            this.neutralLabel = new System.Windows.Forms.Label();
            this.ueberButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // neutralLabel
            // 
            resources.ApplyResources(this.neutralLabel, "neutralLabel");
            this.neutralLabel.Name = "neutralLabel";
            this.neutralLabel.Click += new System.EventHandler(this.neutralLabel_Click);
            // 
            // ueberButton
            // 
            resources.ApplyResources(this.ueberButton, "ueberButton");
            this.ueberButton.Name = "ueberButton";
            this.ueberButton.UseVisualStyleBackColor = true;
            this.ueberButton.Click += new System.EventHandler(this.ueberButton_Click);
            // 
            // cancelButton
            // 
            resources.ApplyResources(this.cancelButton, "cancelButton");
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // Neutral
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.ueberButton);
            this.Controls.Add(this.neutralLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "Neutral";
            this.ShowInTaskbar = false;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label neutralLabel;
        private System.Windows.Forms.Button ueberButton;
        private System.Windows.Forms.Button cancelButton;

    }
}