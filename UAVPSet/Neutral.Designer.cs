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
            this.neutralLabel.AccessibleDescription = null;
            this.neutralLabel.AccessibleName = null;
            resources.ApplyResources(this.neutralLabel, "neutralLabel");
            this.neutralLabel.Font = null;
            this.neutralLabel.Name = "neutralLabel";
            // 
            // ueberButton
            // 
            this.ueberButton.AccessibleDescription = null;
            this.ueberButton.AccessibleName = null;
            resources.ApplyResources(this.ueberButton, "ueberButton");
            this.ueberButton.BackgroundImage = null;
            this.ueberButton.Font = null;
            this.ueberButton.Name = "ueberButton";
            this.ueberButton.UseVisualStyleBackColor = true;
            this.ueberButton.Click += new System.EventHandler(this.ueberButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.AccessibleDescription = null;
            this.cancelButton.AccessibleName = null;
            resources.ApplyResources(this.cancelButton, "cancelButton");
            this.cancelButton.BackgroundImage = null;
            this.cancelButton.Font = null;
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // Neutral
            // 
            this.AccessibleDescription = null;
            this.AccessibleName = null;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = null;
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.ueberButton);
            this.Controls.Add(this.neutralLabel);
            this.Font = null;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = null;
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