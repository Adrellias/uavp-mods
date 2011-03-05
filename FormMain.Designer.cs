namespace UAVXGS
{
    partial class FormMain
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMain));
            this.FlyingButton = new System.Windows.Forms.Button();
            this.RollAngle = new System.Windows.Forms.TextBox();
            this.PitchAngle = new System.Windows.Forms.TextBox();
            this.BatteryVolts = new System.Windows.Forms.TextBox();
            this.BatteryCurrent = new System.Windows.Forms.TextBox();
            this.RCGlitches = new System.Windows.Forms.TextBox();
            this.DesiredThrottle = new System.Windows.Forms.TextBox();
            this.DesiredRoll = new System.Windows.Forms.TextBox();
            this.DesiredPitch = new System.Windows.Forms.TextBox();
            this.DesiredYaw = new System.Windows.Forms.TextBox();
            this.YawRate = new System.Windows.Forms.TextBox();
            this.PitchRate = new System.Windows.Forms.TextBox();
            this.RollRate = new System.Windows.Forms.TextBox();
            this.LRAcc = new System.Windows.Forms.TextBox();
            this.YawAngle = new System.Windows.Forms.TextBox();
            this.DUAcc = new System.Windows.Forms.TextBox();
            this.FBAcc = new System.Windows.Forms.TextBox();
            this.VoltsLabel = new System.Windows.Forms.Label();
            this.LRAccLabel = new System.Windows.Forms.Label();
            this.FBAccLabel = new System.Windows.Forms.Label();
            this.DUAccLabel = new System.Windows.Forms.Label();
            this.DesiredYawLabel = new System.Windows.Forms.Label();
            this.DesiredPitchLabel = new System.Windows.Forms.Label();
            this.DesiredRollLabel = new System.Windows.Forms.Label();
            this.DesiredThrottleLabel = new System.Windows.Forms.Label();
            this.RCGlitchesLabel = new System.Windows.Forms.Label();
            this.CurrentLabel = new System.Windows.Forms.Label();
            this.GPSHDilute = new System.Windows.Forms.TextBox();
            this.RangefinderAltitude = new System.Windows.Forms.TextBox();
            this.RelBaroAltitude = new System.Windows.Forms.TextBox();
            this.GPSFix = new System.Windows.Forms.TextBox();
            this.BaroROC = new System.Windows.Forms.TextBox();
            this.GPSNoOfSats = new System.Windows.Forms.TextBox();
            this.FailState = new System.Windows.Forms.TextBox();
            this.GPSRelAltitude = new System.Windows.Forms.TextBox();
            this.RelAltitudeLabel = new System.Windows.Forms.Label();
            this.ROCLabel = new System.Windows.Forms.Label();
            this.HDiluteLabel = new System.Windows.Forms.Label();
            this.RangefinderAltLabel = new System.Windows.Forms.Label();
            this.GPSFixLabel = new System.Windows.Forms.Label();
            this.GPSNoOfSatsLabel = new System.Windows.Forms.Label();
            this.FailStateLabel = new System.Windows.Forms.Label();
            this.GPSAltLabel = new System.Windows.Forms.Label();
            this.BaroAltitudeLabel = new System.Windows.Forms.Label();
            this.YawRateLabel = new System.Windows.Forms.Label();
            this.PitchRateLabel = new System.Windows.Forms.Label();
            this.RollRateLabel = new System.Windows.Forms.Label();
            this.RxTypeErr = new System.Windows.Forms.TextBox();
            this.RxCSumErr = new System.Windows.Forms.TextBox();
            this.RxLenErr = new System.Windows.Forms.TextBox();
            this.RxTypeLabel = new System.Windows.Forms.Label();
            this.RxCSumLabel = new System.Windows.Forms.Label();
            this.RxLenLabel = new System.Windows.Forms.Label();
            this.FlightState = new System.Windows.Forms.TextBox();
            this.FlightStateLabel = new System.Windows.Forms.Label();
            this.GyroGroupBox = new System.Windows.Forms.GroupBox();
            this.ControlsGroupBox = new System.Windows.Forms.GroupBox();
            this.AttitudeGroupBox = new System.Windows.Forms.GroupBox();
            this.YawLabel = new System.Windows.Forms.Label();
            this.PitchLabel = new System.Windows.Forms.Label();
            this.RollLabel = new System.Windows.Forms.Label();
            this.AccelerationsGroupBox = new System.Windows.Forms.GroupBox();
            this.CommsGroupBox = new System.Windows.Forms.GroupBox();
            this.EnvGroupBox = new System.Windows.Forms.GroupBox();
            this.AmbientTemp = new System.Windows.Forms.TextBox();
            this.AmbientTempLabel = new System.Windows.Forms.Label();
            this.MissionTimeSecLabel = new System.Windows.Forms.Label();
            this.MissionTimeSec = new System.Windows.Forms.TextBox();
            this.BatteryGroupBox = new System.Windows.Forms.GroupBox();
            this.BatteryCharge = new System.Windows.Forms.TextBox();
            this.BatteryChargeLabel = new System.Windows.Forms.Label();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.AltHoldBox = new System.Windows.Forms.TextBox();
            this.TurnToWPBox = new System.Windows.Forms.TextBox();
            this.PHLockEnBox = new System.Windows.Forms.TextBox();
            this.UseRTHAutoDescendBox = new System.Windows.Forms.TextBox();
            this.BaroAltValidBox = new System.Windows.Forms.TextBox();
            this.RangefinderAltValidBox = new System.Windows.Forms.TextBox();
            this.GPSValidBox = new System.Windows.Forms.TextBox();
            this.NavValidBox = new System.Windows.Forms.TextBox();
            this.SticksFrozenBox = new System.Windows.Forms.TextBox();
            this.LowBatteryBox = new System.Windows.Forms.TextBox();
            this.ThrottleMovingBox = new System.Windows.Forms.TextBox();
            this.HoldingAltBox = new System.Windows.Forms.TextBox();
            this.AttitudeHoldBox = new System.Windows.Forms.TextBox();
            this.NearLevelBox = new System.Windows.Forms.TextBox();
            this.FocusLockedBox = new System.Windows.Forms.TextBox();
            this.NavigateBox = new System.Windows.Forms.TextBox();
            this.ReturnHomeBox = new System.Windows.Forms.TextBox();
            this.UsingRangefinderBox = new System.Windows.Forms.TextBox();
            this.CloseProximityBox = new System.Windows.Forms.TextBox();
            this.ProximityBox = new System.Windows.Forms.TextBox();
            this.LostModelBox = new System.Windows.Forms.TextBox();
            this.CompassFailBox = new System.Windows.Forms.TextBox();
            this.GyroFailureBox = new System.Windows.Forms.TextBox();
            this.BaroFailBox = new System.Windows.Forms.TextBox();
            this.AccFailBox = new System.Windows.Forms.TextBox();
            this.GPSFailBox = new System.Windows.Forms.TextBox();
            this.FlagsGroupBox = new System.Windows.Forms.GroupBox();
            this.GPSStatBox = new System.Windows.Forms.GroupBox();
            this.AltitudeGroupBox = new System.Windows.Forms.GroupBox();
            this.WayHeadingLabel = new System.Windows.Forms.Label();
            this.GPSLongitudeLabel = new System.Windows.Forms.Label();
            this.GPSLatitudeLabel = new System.Windows.Forms.Label();
            this.NavStateLabel = new System.Windows.Forms.Label();
            this.GPSVelLabel = new System.Windows.Forms.Label();
            this.CurrWPLabel = new System.Windows.Forms.Label();
            this.GPSLatitude = new System.Windows.Forms.TextBox();
            this.GPSLongitude = new System.Windows.Forms.TextBox();
            this.NavState = new System.Windows.Forms.TextBox();
            this.CurrWP = new System.Windows.Forms.TextBox();
            this.WayHeading = new System.Windows.Forms.TextBox();
            this.WPAltitude = new System.Windows.Forms.Label();
            this.GPSVel = new System.Windows.Forms.TextBox();
            this.HeadingLabel = new System.Windows.Forms.Label();
            this.AltitudeError = new System.Windows.Forms.TextBox();
            this.Heading = new System.Windows.Forms.TextBox();
            this.WPDistanceLabel = new System.Windows.Forms.Label();
            this.DistanceToDesired = new System.Windows.Forms.TextBox();
            this.NavGroupBox = new System.Windows.Forms.GroupBox();
            this.GPSHeadingLabel = new System.Windows.Forms.Label();
            this.GPSHeading = new System.Windows.Forms.TextBox();
            this.NavStateTimeoutLabel = new System.Windows.Forms.Label();
            this.NavStateTimeout = new System.Windows.Forms.TextBox();
            this.CurrentAltitude = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.AltitudeSource = new System.Windows.Forms.TextBox();
            this.GoogleButton = new System.Windows.Forms.Button();
            this.MessageTextBox = new System.Windows.Forms.TextBox();
            this.LocationBox = new System.Windows.Forms.GroupBox();
            this.WhereDistance = new System.Windows.Forms.TextBox();
            this.WhereBearing = new System.Windows.Forms.TextBox();
            this.WhereBearingLabel = new System.Windows.Forms.Label();
            this.WhereDistanceLabel = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.LRComp = new System.Windows.Forms.TextBox();
            this.LRCompLabel = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.AltComp = new System.Windows.Forms.TextBox();
            this.FBComp = new System.Windows.Forms.TextBox();
            this.DUComp = new System.Windows.Forms.TextBox();
            this.FBCompLabel = new System.Windows.Forms.Label();
            this.DUCompLabel = new System.Windows.Forms.Label();
            this.AltCompLabel = new System.Windows.Forms.Label();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.SimulationTextBox = new System.Windows.Forms.TextBox();
            this.OutputGroupBox = new System.Windows.Forms.GroupBox();
            this.OutputTotalLabel = new System.Windows.Forms.Label();
            this.OutputTot = new System.Windows.Forms.TextBox();
            this.Output4Label = new System.Windows.Forms.Label();
            this.Output5Label = new System.Windows.Forms.Label();
            this.OutputT5 = new System.Windows.Forms.TextBox();
            this.OutputT4 = new System.Windows.Forms.TextBox();
            this.Output2Label = new System.Windows.Forms.Label();
            this.Output3Label = new System.Windows.Forms.Label();
            this.Output1Label = new System.Windows.Forms.Label();
            this.Output0Label = new System.Windows.Forms.Label();
            this.OutputT3 = new System.Windows.Forms.TextBox();
            this.OutputT2 = new System.Windows.Forms.TextBox();
            this.OutputT1 = new System.Windows.Forms.TextBox();
            this.OutputT0 = new System.Windows.Forms.TextBox();
            this.Output7Label = new System.Windows.Forms.Label();
            this.Output6Label = new System.Windows.Forms.Label();
            this.OutputT7 = new System.Windows.Forms.TextBox();
            this.OutputT6 = new System.Windows.Forms.TextBox();
            this.BaroStatsGroupBox = new System.Windows.Forms.GroupBox();
            this.MaxTempS = new System.Windows.Forms.Label();
            this.TempSLabel = new System.Windows.Forms.Label();
            this.MinTempS = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.BaroMaxROCS = new System.Windows.Forms.Label();
            this.RelBaroAltitudeLabel = new System.Windows.Forms.Label();
            this.BaroRelAltitudeS = new System.Windows.Forms.Label();
            this.BaroMinROCS = new System.Windows.Forms.Label();
            this.ErrorStatsGroupBox = new System.Windows.Forms.GroupBox();
            this.BadS = new System.Windows.Forms.Label();
            this.BadSLabel = new System.Windows.Forms.Label();
            this.FailsafeSLabel = new System.Windows.Forms.Label();
            this.GyroSLabel = new System.Windows.Forms.Label();
            this.RCFailSafeS = new System.Windows.Forms.Label();
            this.I2CESCFailS = new System.Windows.Forms.Label();
            this.GPSInvalidSLabel = new System.Windows.Forms.Label();
            this.GPSFailS = new System.Windows.Forms.Label();
            this.ESCSLabel = new System.Windows.Forms.Label();
            this.GyroFailS = new System.Windows.Forms.Label();
            this.AccFailLabel = new System.Windows.Forms.Label();
            this.AccFailS = new System.Windows.Forms.Label();
            this.I2CFailLabel = new System.Windows.Forms.Label();
            this.CompassFailLabel = new System.Windows.Forms.Label();
            this.I2CFailS = new System.Windows.Forms.Label();
            this.CompassFailS = new System.Windows.Forms.Label();
            this.BaroFailLabel = new System.Windows.Forms.Label();
            this.BaroFailS = new System.Windows.Forms.Label();
            this.GPSStatsGroupBox = new System.Windows.Forms.GroupBox();
            this.label9 = new System.Windows.Forms.Label();
            this.GPSMaxHDiluteS = new System.Windows.Forms.Label();
            this.GPSMinHDiluteS = new System.Windows.Forms.Label();
            this.GPSAltitudeLabel = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.GPSMaxVelS = new System.Windows.Forms.Label();
            this.GPSAltitudeS = new System.Windows.Forms.Label();
            this.SatsLabel = new System.Windows.Forms.Label();
            this.GPSMinSatS = new System.Windows.Forms.Label();
            this.GPSMaxSatS = new System.Windows.Forms.Label();
            this.OpenLogFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.COMSelectComboBox = new System.Windows.Forms.ToolStripComboBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.COMBaudRateComboBox = new System.Windows.Forms.ToolStripComboBox();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.testsoftwareToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ReplayButton = new System.Windows.Forms.Button();
            this.ReplayProgressBar = new System.Windows.Forms.ProgressBar();
            this.Airframe = new System.Windows.Forms.TextBox();
            this.ReplayNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.NavCompBox = new System.Windows.Forms.GroupBox();
            this.NavSensitivity = new System.Windows.Forms.TextBox();
            this.NavSensitivityLabel = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.NavYCorr = new System.Windows.Forms.TextBox();
            this.NavRCorr = new System.Windows.Forms.TextBox();
            this.NavPCorr = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.FrSkycheckBox1 = new System.Windows.Forms.CheckBox();
            this.FrSkyBox = new System.Windows.Forms.GroupBox();
            this.FrSkyErrLabel = new System.Windows.Forms.Label();
            this.RxFrSkyErr = new System.Windows.Forms.TextBox();
            this.FrSkyLQLabel = new System.Windows.Forms.Label();
            this.FrSkyA2Label = new System.Windows.Forms.Label();
            this.FrSkyA1Label = new System.Windows.Forms.Label();
            this.FrSkyLQ = new System.Windows.Forms.TextBox();
            this.FrSkyA2 = new System.Windows.Forms.TextBox();
            this.FrSkyA1 = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.headingIndicatorInstrumentControl1 = new Instruments.HeadingIndicatorInstrumentControl();
            this.attitudeIndicatorInstrumentControl1 = new Instruments.AttitudeIndicatorInstrumentControl();
            this.GyroGroupBox.SuspendLayout();
            this.ControlsGroupBox.SuspendLayout();
            this.AttitudeGroupBox.SuspendLayout();
            this.AccelerationsGroupBox.SuspendLayout();
            this.CommsGroupBox.SuspendLayout();
            this.EnvGroupBox.SuspendLayout();
            this.BatteryGroupBox.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.FlagsGroupBox.SuspendLayout();
            this.GPSStatBox.SuspendLayout();
            this.AltitudeGroupBox.SuspendLayout();
            this.NavGroupBox.SuspendLayout();
            this.LocationBox.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.OutputGroupBox.SuspendLayout();
            this.BaroStatsGroupBox.SuspendLayout();
            this.ErrorStatsGroupBox.SuspendLayout();
            this.GPSStatsGroupBox.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ReplayNumericUpDown)).BeginInit();
            this.NavCompBox.SuspendLayout();
            this.FrSkyBox.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // FlyingButton
            // 
            this.FlyingButton.BackColor = System.Drawing.Color.Red;
            this.FlyingButton.ForeColor = System.Drawing.SystemColors.ControlText;
            this.FlyingButton.Location = new System.Drawing.Point(653, 0);
            this.FlyingButton.Name = "FlyingButton";
            this.FlyingButton.Size = new System.Drawing.Size(91, 23);
            this.FlyingButton.TabIndex = 0;
            this.FlyingButton.Text = "Disconnected";
            this.FlyingButton.UseVisualStyleBackColor = false;
            this.FlyingButton.Click += new System.EventHandler(this.FlyingButton_Click);
            // 
            // RollAngle
            // 
            this.RollAngle.Location = new System.Drawing.Point(41, 45);
            this.RollAngle.Name = "RollAngle";
            this.RollAngle.ReadOnly = true;
            this.RollAngle.Size = new System.Drawing.Size(29, 20);
            this.RollAngle.TabIndex = 5;
            this.RollAngle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // PitchAngle
            // 
            this.PitchAngle.Location = new System.Drawing.Point(41, 71);
            this.PitchAngle.Name = "PitchAngle";
            this.PitchAngle.ReadOnly = true;
            this.PitchAngle.Size = new System.Drawing.Size(29, 20);
            this.PitchAngle.TabIndex = 6;
            this.PitchAngle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // BatteryVolts
            // 
            this.BatteryVolts.Location = new System.Drawing.Point(53, 19);
            this.BatteryVolts.Name = "BatteryVolts";
            this.BatteryVolts.ReadOnly = true;
            this.BatteryVolts.Size = new System.Drawing.Size(32, 20);
            this.BatteryVolts.TabIndex = 8;
            this.BatteryVolts.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // BatteryCurrent
            // 
            this.BatteryCurrent.BackColor = System.Drawing.SystemColors.Control;
            this.BatteryCurrent.Location = new System.Drawing.Point(53, 45);
            this.BatteryCurrent.Name = "BatteryCurrent";
            this.BatteryCurrent.ReadOnly = true;
            this.BatteryCurrent.Size = new System.Drawing.Size(32, 20);
            this.BatteryCurrent.TabIndex = 9;
            this.BatteryCurrent.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RCGlitches
            // 
            this.RCGlitches.Location = new System.Drawing.Point(216, 52);
            this.RCGlitches.Name = "RCGlitches";
            this.RCGlitches.ReadOnly = true;
            this.RCGlitches.Size = new System.Drawing.Size(51, 20);
            this.RCGlitches.TabIndex = 10;
            this.RCGlitches.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // DesiredThrottle
            // 
            this.DesiredThrottle.Location = new System.Drawing.Point(40, 19);
            this.DesiredThrottle.Name = "DesiredThrottle";
            this.DesiredThrottle.ReadOnly = true;
            this.DesiredThrottle.Size = new System.Drawing.Size(32, 20);
            this.DesiredThrottle.TabIndex = 11;
            this.DesiredThrottle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // DesiredRoll
            // 
            this.DesiredRoll.Location = new System.Drawing.Point(40, 45);
            this.DesiredRoll.Name = "DesiredRoll";
            this.DesiredRoll.ReadOnly = true;
            this.DesiredRoll.Size = new System.Drawing.Size(32, 20);
            this.DesiredRoll.TabIndex = 12;
            this.DesiredRoll.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // DesiredPitch
            // 
            this.DesiredPitch.Location = new System.Drawing.Point(40, 71);
            this.DesiredPitch.Name = "DesiredPitch";
            this.DesiredPitch.ReadOnly = true;
            this.DesiredPitch.Size = new System.Drawing.Size(32, 20);
            this.DesiredPitch.TabIndex = 13;
            this.DesiredPitch.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // DesiredYaw
            // 
            this.DesiredYaw.Location = new System.Drawing.Point(40, 97);
            this.DesiredYaw.Name = "DesiredYaw";
            this.DesiredYaw.ReadOnly = true;
            this.DesiredYaw.Size = new System.Drawing.Size(32, 20);
            this.DesiredYaw.TabIndex = 14;
            this.DesiredYaw.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // YawRate
            // 
            this.YawRate.Location = new System.Drawing.Point(40, 71);
            this.YawRate.Name = "YawRate";
            this.YawRate.ReadOnly = true;
            this.YawRate.Size = new System.Drawing.Size(32, 20);
            this.YawRate.TabIndex = 17;
            this.YawRate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // PitchRate
            // 
            this.PitchRate.Location = new System.Drawing.Point(40, 45);
            this.PitchRate.Name = "PitchRate";
            this.PitchRate.ReadOnly = true;
            this.PitchRate.Size = new System.Drawing.Size(32, 20);
            this.PitchRate.TabIndex = 16;
            this.PitchRate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RollRate
            // 
            this.RollRate.Location = new System.Drawing.Point(40, 16);
            this.RollRate.Name = "RollRate";
            this.RollRate.ReadOnly = true;
            this.RollRate.Size = new System.Drawing.Size(32, 20);
            this.RollRate.TabIndex = 15;
            this.RollRate.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // LRAcc
            // 
            this.LRAcc.Location = new System.Drawing.Point(39, 19);
            this.LRAcc.Name = "LRAcc";
            this.LRAcc.ReadOnly = true;
            this.LRAcc.Size = new System.Drawing.Size(32, 20);
            this.LRAcc.TabIndex = 21;
            this.LRAcc.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // YawAngle
            // 
            this.YawAngle.Location = new System.Drawing.Point(41, 97);
            this.YawAngle.Name = "YawAngle";
            this.YawAngle.ReadOnly = true;
            this.YawAngle.Size = new System.Drawing.Size(29, 20);
            this.YawAngle.TabIndex = 20;
            this.YawAngle.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // DUAcc
            // 
            this.DUAcc.Location = new System.Drawing.Point(39, 71);
            this.DUAcc.Name = "DUAcc";
            this.DUAcc.ReadOnly = true;
            this.DUAcc.Size = new System.Drawing.Size(32, 20);
            this.DUAcc.TabIndex = 19;
            this.DUAcc.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FBAcc
            // 
            this.FBAcc.Location = new System.Drawing.Point(39, 45);
            this.FBAcc.Name = "FBAcc";
            this.FBAcc.ReadOnly = true;
            this.FBAcc.Size = new System.Drawing.Size(32, 20);
            this.FBAcc.TabIndex = 18;
            this.FBAcc.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // VoltsLabel
            // 
            this.VoltsLabel.AutoSize = true;
            this.VoltsLabel.Location = new System.Drawing.Point(6, 22);
            this.VoltsLabel.Name = "VoltsLabel";
            this.VoltsLabel.Size = new System.Drawing.Size(30, 13);
            this.VoltsLabel.TabIndex = 22;
            this.VoltsLabel.Text = "Volts";
            // 
            // LRAccLabel
            // 
            this.LRAccLabel.AutoSize = true;
            this.LRAccLabel.Location = new System.Drawing.Point(6, 20);
            this.LRAccLabel.Name = "LRAccLabel";
            this.LRAccLabel.Size = new System.Drawing.Size(21, 13);
            this.LRAccLabel.TabIndex = 26;
            this.LRAccLabel.Text = "LR";
            // 
            // FBAccLabel
            // 
            this.FBAccLabel.AutoSize = true;
            this.FBAccLabel.Location = new System.Drawing.Point(5, 48);
            this.FBAccLabel.Name = "FBAccLabel";
            this.FBAccLabel.Size = new System.Drawing.Size(20, 13);
            this.FBAccLabel.TabIndex = 27;
            this.FBAccLabel.Text = "FB";
            // 
            // DUAccLabel
            // 
            this.DUAccLabel.AutoSize = true;
            this.DUAccLabel.Location = new System.Drawing.Point(6, 74);
            this.DUAccLabel.Name = "DUAccLabel";
            this.DUAccLabel.Size = new System.Drawing.Size(23, 13);
            this.DUAccLabel.TabIndex = 28;
            this.DUAccLabel.Text = "DU";
            // 
            // DesiredYawLabel
            // 
            this.DesiredYawLabel.AutoSize = true;
            this.DesiredYawLabel.Location = new System.Drawing.Point(6, 100);
            this.DesiredYawLabel.Name = "DesiredYawLabel";
            this.DesiredYawLabel.Size = new System.Drawing.Size(28, 13);
            this.DesiredYawLabel.TabIndex = 34;
            this.DesiredYawLabel.Text = "Yaw";
            // 
            // DesiredPitchLabel
            // 
            this.DesiredPitchLabel.AutoSize = true;
            this.DesiredPitchLabel.Location = new System.Drawing.Point(6, 74);
            this.DesiredPitchLabel.Name = "DesiredPitchLabel";
            this.DesiredPitchLabel.Size = new System.Drawing.Size(31, 13);
            this.DesiredPitchLabel.TabIndex = 33;
            this.DesiredPitchLabel.Text = "Pitch";
            // 
            // DesiredRollLabel
            // 
            this.DesiredRollLabel.AutoSize = true;
            this.DesiredRollLabel.Location = new System.Drawing.Point(6, 48);
            this.DesiredRollLabel.Name = "DesiredRollLabel";
            this.DesiredRollLabel.Size = new System.Drawing.Size(25, 13);
            this.DesiredRollLabel.TabIndex = 32;
            this.DesiredRollLabel.Text = "Roll";
            // 
            // DesiredThrottleLabel
            // 
            this.DesiredThrottleLabel.AutoSize = true;
            this.DesiredThrottleLabel.Location = new System.Drawing.Point(6, 22);
            this.DesiredThrottleLabel.Name = "DesiredThrottleLabel";
            this.DesiredThrottleLabel.Size = new System.Drawing.Size(26, 13);
            this.DesiredThrottleLabel.TabIndex = 31;
            this.DesiredThrottleLabel.Text = "Thr.";
            // 
            // RCGlitchesLabel
            // 
            this.RCGlitchesLabel.AutoSize = true;
            this.RCGlitchesLabel.Location = new System.Drawing.Point(153, 56);
            this.RCGlitchesLabel.Name = "RCGlitchesLabel";
            this.RCGlitchesLabel.Size = new System.Drawing.Size(45, 13);
            this.RCGlitchesLabel.TabIndex = 30;
            this.RCGlitchesLabel.Text = "Glitches";
            // 
            // CurrentLabel
            // 
            this.CurrentLabel.AutoSize = true;
            this.CurrentLabel.Location = new System.Drawing.Point(6, 48);
            this.CurrentLabel.Name = "CurrentLabel";
            this.CurrentLabel.Size = new System.Drawing.Size(33, 13);
            this.CurrentLabel.TabIndex = 29;
            this.CurrentLabel.Text = "Amps";
            // 
            // GPSHDilute
            // 
            this.GPSHDilute.Location = new System.Drawing.Point(52, 71);
            this.GPSHDilute.Name = "GPSHDilute";
            this.GPSHDilute.ReadOnly = true;
            this.GPSHDilute.Size = new System.Drawing.Size(32, 20);
            this.GPSHDilute.TabIndex = 44;
            this.GPSHDilute.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RangefinderAltitude
            // 
            this.RangefinderAltitude.Location = new System.Drawing.Point(122, 29);
            this.RangefinderAltitude.Name = "RangefinderAltitude";
            this.RangefinderAltitude.ReadOnly = true;
            this.RangefinderAltitude.Size = new System.Drawing.Size(32, 20);
            this.RangefinderAltitude.TabIndex = 43;
            this.RangefinderAltitude.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RelBaroAltitude
            // 
            this.RelBaroAltitude.Location = new System.Drawing.Point(84, 29);
            this.RelBaroAltitude.Name = "RelBaroAltitude";
            this.RelBaroAltitude.ReadOnly = true;
            this.RelBaroAltitude.Size = new System.Drawing.Size(32, 20);
            this.RelBaroAltitude.TabIndex = 46;
            this.RelBaroAltitude.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // GPSFix
            // 
            this.GPSFix.Location = new System.Drawing.Point(52, 45);
            this.GPSFix.Name = "GPSFix";
            this.GPSFix.ReadOnly = true;
            this.GPSFix.Size = new System.Drawing.Size(32, 20);
            this.GPSFix.TabIndex = 51;
            this.GPSFix.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // BaroROC
            // 
            this.BaroROC.Location = new System.Drawing.Point(84, 55);
            this.BaroROC.Name = "BaroROC";
            this.BaroROC.ReadOnly = true;
            this.BaroROC.Size = new System.Drawing.Size(32, 20);
            this.BaroROC.TabIndex = 49;
            this.BaroROC.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // GPSNoOfSats
            // 
            this.GPSNoOfSats.Location = new System.Drawing.Point(52, 19);
            this.GPSNoOfSats.Name = "GPSNoOfSats";
            this.GPSNoOfSats.ReadOnly = true;
            this.GPSNoOfSats.Size = new System.Drawing.Size(32, 20);
            this.GPSNoOfSats.TabIndex = 54;
            this.GPSNoOfSats.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FailState
            // 
            this.FailState.Location = new System.Drawing.Point(239, 30);
            this.FailState.Name = "FailState";
            this.FailState.ReadOnly = true;
            this.FailState.Size = new System.Drawing.Size(73, 20);
            this.FailState.TabIndex = 53;
            this.FailState.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // GPSRelAltitude
            // 
            this.GPSRelAltitude.Location = new System.Drawing.Point(46, 29);
            this.GPSRelAltitude.Name = "GPSRelAltitude";
            this.GPSRelAltitude.ReadOnly = true;
            this.GPSRelAltitude.Size = new System.Drawing.Size(32, 20);
            this.GPSRelAltitude.TabIndex = 58;
            this.GPSRelAltitude.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RelAltitudeLabel
            // 
            this.RelAltitudeLabel.AutoSize = true;
            this.RelAltitudeLabel.Location = new System.Drawing.Point(9, 29);
            this.RelAltitudeLabel.Name = "RelAltitudeLabel";
            this.RelAltitudeLabel.Size = new System.Drawing.Size(22, 13);
            this.RelAltitudeLabel.TabIndex = 62;
            this.RelAltitudeLabel.Text = "Alt.";
            // 
            // ROCLabel
            // 
            this.ROCLabel.AutoSize = true;
            this.ROCLabel.Location = new System.Drawing.Point(6, 58);
            this.ROCLabel.Name = "ROCLabel";
            this.ROCLabel.Size = new System.Drawing.Size(30, 13);
            this.ROCLabel.TabIndex = 65;
            this.ROCLabel.Text = "ROC";
            // 
            // HDiluteLabel
            // 
            this.HDiluteLabel.AutoSize = true;
            this.HDiluteLabel.Location = new System.Drawing.Point(6, 74);
            this.HDiluteLabel.Name = "HDiluteLabel";
            this.HDiluteLabel.Size = new System.Drawing.Size(42, 13);
            this.HDiluteLabel.TabIndex = 66;
            this.HDiluteLabel.Text = "HDilute";
            // 
            // RangefinderAltLabel
            // 
            this.RangefinderAltLabel.AutoSize = true;
            this.RangefinderAltLabel.Location = new System.Drawing.Point(132, 13);
            this.RangefinderAltLabel.Name = "RangefinderAltLabel";
            this.RangefinderAltLabel.Size = new System.Drawing.Size(21, 13);
            this.RangefinderAltLabel.TabIndex = 67;
            this.RangefinderAltLabel.Text = "RF";
            // 
            // GPSFixLabel
            // 
            this.GPSFixLabel.AutoSize = true;
            this.GPSFixLabel.Location = new System.Drawing.Point(6, 48);
            this.GPSFixLabel.Name = "GPSFixLabel";
            this.GPSFixLabel.Size = new System.Drawing.Size(20, 13);
            this.GPSFixLabel.TabIndex = 69;
            this.GPSFixLabel.Text = "Fix";
            // 
            // GPSNoOfSatsLabel
            // 
            this.GPSNoOfSatsLabel.AutoSize = true;
            this.GPSNoOfSatsLabel.Location = new System.Drawing.Point(6, 22);
            this.GPSNoOfSatsLabel.Name = "GPSNoOfSatsLabel";
            this.GPSNoOfSatsLabel.Size = new System.Drawing.Size(31, 13);
            this.GPSNoOfSatsLabel.TabIndex = 70;
            this.GPSNoOfSatsLabel.Text = "Sats.";
            // 
            // FailStateLabel
            // 
            this.FailStateLabel.AutoSize = true;
            this.FailStateLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FailStateLabel.Location = new System.Drawing.Point(182, 33);
            this.FailStateLabel.Name = "FailStateLabel";
            this.FailStateLabel.Size = new System.Drawing.Size(51, 13);
            this.FailStateLabel.TabIndex = 71;
            this.FailStateLabel.Text = "Fail State";
            // 
            // GPSAltLabel
            // 
            this.GPSAltLabel.AutoSize = true;
            this.GPSAltLabel.Location = new System.Drawing.Point(48, 13);
            this.GPSAltLabel.Name = "GPSAltLabel";
            this.GPSAltLabel.Size = new System.Drawing.Size(29, 13);
            this.GPSAltLabel.TabIndex = 74;
            this.GPSAltLabel.Text = "GPS";
            // 
            // BaroAltitudeLabel
            // 
            this.BaroAltitudeLabel.AutoSize = true;
            this.BaroAltitudeLabel.Location = new System.Drawing.Point(87, 13);
            this.BaroAltitudeLabel.Name = "BaroAltitudeLabel";
            this.BaroAltitudeLabel.Size = new System.Drawing.Size(29, 13);
            this.BaroAltitudeLabel.TabIndex = 75;
            this.BaroAltitudeLabel.Text = "Baro";
            // 
            // YawRateLabel
            // 
            this.YawRateLabel.AutoSize = true;
            this.YawRateLabel.Location = new System.Drawing.Point(6, 74);
            this.YawRateLabel.Name = "YawRateLabel";
            this.YawRateLabel.Size = new System.Drawing.Size(28, 13);
            this.YawRateLabel.TabIndex = 78;
            this.YawRateLabel.Text = "Yaw";
            // 
            // PitchRateLabel
            // 
            this.PitchRateLabel.AutoSize = true;
            this.PitchRateLabel.Location = new System.Drawing.Point(6, 48);
            this.PitchRateLabel.Name = "PitchRateLabel";
            this.PitchRateLabel.Size = new System.Drawing.Size(31, 13);
            this.PitchRateLabel.TabIndex = 77;
            this.PitchRateLabel.Text = "Pitch";
            // 
            // RollRateLabel
            // 
            this.RollRateLabel.AutoSize = true;
            this.RollRateLabel.Location = new System.Drawing.Point(6, 22);
            this.RollRateLabel.Name = "RollRateLabel";
            this.RollRateLabel.Size = new System.Drawing.Size(25, 13);
            this.RollRateLabel.TabIndex = 76;
            this.RollRateLabel.Text = "Roll";
            // 
            // RxTypeErr
            // 
            this.RxTypeErr.Location = new System.Drawing.Point(65, 18);
            this.RxTypeErr.Name = "RxTypeErr";
            this.RxTypeErr.ReadOnly = true;
            this.RxTypeErr.Size = new System.Drawing.Size(38, 20);
            this.RxTypeErr.TabIndex = 82;
            this.RxTypeErr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RxCSumErr
            // 
            this.RxCSumErr.Location = new System.Drawing.Point(65, 70);
            this.RxCSumErr.Name = "RxCSumErr";
            this.RxCSumErr.ReadOnly = true;
            this.RxCSumErr.Size = new System.Drawing.Size(38, 20);
            this.RxCSumErr.TabIndex = 83;
            this.RxCSumErr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RxLenErr
            // 
            this.RxLenErr.Location = new System.Drawing.Point(65, 44);
            this.RxLenErr.Name = "RxLenErr";
            this.RxLenErr.ReadOnly = true;
            this.RxLenErr.Size = new System.Drawing.Size(38, 20);
            this.RxLenErr.TabIndex = 84;
            this.RxLenErr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RxTypeLabel
            // 
            this.RxTypeLabel.AutoSize = true;
            this.RxTypeLabel.Location = new System.Drawing.Point(6, 22);
            this.RxTypeLabel.Name = "RxTypeLabel";
            this.RxTypeLabel.Size = new System.Drawing.Size(31, 13);
            this.RxTypeLabel.TabIndex = 85;
            this.RxTypeLabel.Text = "Type";
            // 
            // RxCSumLabel
            // 
            this.RxCSumLabel.AutoSize = true;
            this.RxCSumLabel.Location = new System.Drawing.Point(6, 73);
            this.RxCSumLabel.Name = "RxCSumLabel";
            this.RxCSumLabel.Size = new System.Drawing.Size(59, 13);
            this.RxCSumLabel.TabIndex = 86;
            this.RxCSumLabel.Text = "CheckSum";
            // 
            // RxLenLabel
            // 
            this.RxLenLabel.AutoSize = true;
            this.RxLenLabel.Location = new System.Drawing.Point(6, 48);
            this.RxLenLabel.Name = "RxLenLabel";
            this.RxLenLabel.Size = new System.Drawing.Size(40, 13);
            this.RxLenLabel.TabIndex = 87;
            this.RxLenLabel.Text = "Length";
            // 
            // FlightState
            // 
            this.FlightState.Location = new System.Drawing.Point(92, 30);
            this.FlightState.Name = "FlightState";
            this.FlightState.ReadOnly = true;
            this.FlightState.Size = new System.Drawing.Size(73, 20);
            this.FlightState.TabIndex = 89;
            this.FlightState.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // FlightStateLabel
            // 
            this.FlightStateLabel.AutoSize = true;
            this.FlightStateLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FlightStateLabel.Location = new System.Drawing.Point(42, 33);
            this.FlightStateLabel.Name = "FlightStateLabel";
            this.FlightStateLabel.Size = new System.Drawing.Size(32, 13);
            this.FlightStateLabel.TabIndex = 90;
            this.FlightStateLabel.Text = "State";
            // 
            // GyroGroupBox
            // 
            this.GyroGroupBox.Controls.Add(this.RollRate);
            this.GyroGroupBox.Controls.Add(this.PitchRate);
            this.GyroGroupBox.Controls.Add(this.YawRate);
            this.GyroGroupBox.Controls.Add(this.RollRateLabel);
            this.GyroGroupBox.Controls.Add(this.PitchRateLabel);
            this.GyroGroupBox.Controls.Add(this.YawRateLabel);
            this.GyroGroupBox.Location = new System.Drawing.Point(12, 187);
            this.GyroGroupBox.Name = "GyroGroupBox";
            this.GyroGroupBox.Size = new System.Drawing.Size(77, 99);
            this.GyroGroupBox.TabIndex = 92;
            this.GyroGroupBox.TabStop = false;
            this.GyroGroupBox.Text = "Gyros";
            // 
            // ControlsGroupBox
            // 
            this.ControlsGroupBox.Controls.Add(this.DesiredYaw);
            this.ControlsGroupBox.Controls.Add(this.DesiredRoll);
            this.ControlsGroupBox.Controls.Add(this.DesiredPitch);
            this.ControlsGroupBox.Controls.Add(this.DesiredRollLabel);
            this.ControlsGroupBox.Controls.Add(this.DesiredPitchLabel);
            this.ControlsGroupBox.Controls.Add(this.DesiredYawLabel);
            this.ControlsGroupBox.Controls.Add(this.DesiredThrottle);
            this.ControlsGroupBox.Controls.Add(this.DesiredThrottleLabel);
            this.ControlsGroupBox.Location = new System.Drawing.Point(12, 57);
            this.ControlsGroupBox.Name = "ControlsGroupBox";
            this.ControlsGroupBox.Size = new System.Drawing.Size(77, 124);
            this.ControlsGroupBox.TabIndex = 93;
            this.ControlsGroupBox.TabStop = false;
            this.ControlsGroupBox.Text = "Controls (%)";
            // 
            // AttitudeGroupBox
            // 
            this.AttitudeGroupBox.Controls.Add(this.YawLabel);
            this.AttitudeGroupBox.Controls.Add(this.PitchLabel);
            this.AttitudeGroupBox.Controls.Add(this.RollLabel);
            this.AttitudeGroupBox.Controls.Add(this.RollAngle);
            this.AttitudeGroupBox.Controls.Add(this.PitchAngle);
            this.AttitudeGroupBox.Controls.Add(this.YawAngle);
            this.AttitudeGroupBox.Location = new System.Drawing.Point(95, 57);
            this.AttitudeGroupBox.Name = "AttitudeGroupBox";
            this.AttitudeGroupBox.Size = new System.Drawing.Size(77, 124);
            this.AttitudeGroupBox.TabIndex = 94;
            this.AttitudeGroupBox.TabStop = false;
            this.AttitudeGroupBox.Text = "Attitude";
            // 
            // YawLabel
            // 
            this.YawLabel.AutoSize = true;
            this.YawLabel.Location = new System.Drawing.Point(4, 100);
            this.YawLabel.Name = "YawLabel";
            this.YawLabel.Size = new System.Drawing.Size(35, 13);
            this.YawLabel.TabIndex = 109;
            this.YawLabel.Text = "YawE";
            // 
            // PitchLabel
            // 
            this.PitchLabel.AutoSize = true;
            this.PitchLabel.Location = new System.Drawing.Point(5, 74);
            this.PitchLabel.Name = "PitchLabel";
            this.PitchLabel.Size = new System.Drawing.Size(31, 13);
            this.PitchLabel.TabIndex = 108;
            this.PitchLabel.Text = "Pitch";
            // 
            // RollLabel
            // 
            this.RollLabel.AutoSize = true;
            this.RollLabel.Location = new System.Drawing.Point(5, 48);
            this.RollLabel.Name = "RollLabel";
            this.RollLabel.Size = new System.Drawing.Size(25, 13);
            this.RollLabel.TabIndex = 107;
            this.RollLabel.Text = "Roll";
            // 
            // AccelerationsGroupBox
            // 
            this.AccelerationsGroupBox.Controls.Add(this.LRAcc);
            this.AccelerationsGroupBox.Controls.Add(this.FBAcc);
            this.AccelerationsGroupBox.Controls.Add(this.DUAcc);
            this.AccelerationsGroupBox.Controls.Add(this.LRAccLabel);
            this.AccelerationsGroupBox.Controls.Add(this.FBAccLabel);
            this.AccelerationsGroupBox.Controls.Add(this.DUAccLabel);
            this.AccelerationsGroupBox.Location = new System.Drawing.Point(95, 187);
            this.AccelerationsGroupBox.Name = "AccelerationsGroupBox";
            this.AccelerationsGroupBox.Size = new System.Drawing.Size(77, 99);
            this.AccelerationsGroupBox.TabIndex = 95;
            this.AccelerationsGroupBox.TabStop = false;
            this.AccelerationsGroupBox.Text = "Accel. (G)";
            // 
            // CommsGroupBox
            // 
            this.CommsGroupBox.Controls.Add(this.RxTypeErr);
            this.CommsGroupBox.Controls.Add(this.RxCSumErr);
            this.CommsGroupBox.Controls.Add(this.RxLenErr);
            this.CommsGroupBox.Controls.Add(this.RxTypeLabel);
            this.CommsGroupBox.Controls.Add(this.RxCSumLabel);
            this.CommsGroupBox.Controls.Add(this.RxLenLabel);
            this.CommsGroupBox.Location = new System.Drawing.Point(12, 444);
            this.CommsGroupBox.Name = "CommsGroupBox";
            this.CommsGroupBox.Size = new System.Drawing.Size(109, 98);
            this.CommsGroupBox.TabIndex = 96;
            this.CommsGroupBox.TabStop = false;
            this.CommsGroupBox.Text = "Telemetry Errors";
            // 
            // EnvGroupBox
            // 
            this.EnvGroupBox.Controls.Add(this.AmbientTemp);
            this.EnvGroupBox.Controls.Add(this.AmbientTempLabel);
            this.EnvGroupBox.Location = new System.Drawing.Point(587, 33);
            this.EnvGroupBox.Name = "EnvGroupBox";
            this.EnvGroupBox.Size = new System.Drawing.Size(125, 43);
            this.EnvGroupBox.TabIndex = 97;
            this.EnvGroupBox.TabStop = false;
            this.EnvGroupBox.Text = "Board";
            // 
            // AmbientTemp
            // 
            this.AmbientTemp.Location = new System.Drawing.Point(71, 14);
            this.AmbientTemp.Name = "AmbientTemp";
            this.AmbientTemp.ReadOnly = true;
            this.AmbientTemp.Size = new System.Drawing.Size(48, 20);
            this.AmbientTemp.TabIndex = 161;
            this.AmbientTemp.Text = "0.0";
            this.AmbientTemp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // AmbientTempLabel
            // 
            this.AmbientTempLabel.AutoSize = true;
            this.AmbientTempLabel.Location = new System.Drawing.Point(6, 17);
            this.AmbientTempLabel.Name = "AmbientTempLabel";
            this.AmbientTempLabel.Size = new System.Drawing.Size(53, 13);
            this.AmbientTempLabel.TabIndex = 162;
            this.AmbientTempLabel.Text = "Temp. (C)";
            // 
            // MissionTimeSecLabel
            // 
            this.MissionTimeSecLabel.AutoSize = true;
            this.MissionTimeSecLabel.Location = new System.Drawing.Point(524, 5);
            this.MissionTimeSecLabel.Name = "MissionTimeSecLabel";
            this.MissionTimeSecLabel.Size = new System.Drawing.Size(46, 13);
            this.MissionTimeSecLabel.TabIndex = 163;
            this.MissionTimeSecLabel.Text = "Time (S)";
            // 
            // MissionTimeSec
            // 
            this.MissionTimeSec.Location = new System.Drawing.Point(576, 2);
            this.MissionTimeSec.Name = "MissionTimeSec";
            this.MissionTimeSec.ReadOnly = true;
            this.MissionTimeSec.Size = new System.Drawing.Size(70, 20);
            this.MissionTimeSec.TabIndex = 160;
            this.MissionTimeSec.Text = "0.0";
            this.MissionTimeSec.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // BatteryGroupBox
            // 
            this.BatteryGroupBox.Controls.Add(this.BatteryCharge);
            this.BatteryGroupBox.Controls.Add(this.BatteryChargeLabel);
            this.BatteryGroupBox.Controls.Add(this.BatteryVolts);
            this.BatteryGroupBox.Controls.Add(this.BatteryCurrent);
            this.BatteryGroupBox.Controls.Add(this.VoltsLabel);
            this.BatteryGroupBox.Controls.Add(this.CurrentLabel);
            this.BatteryGroupBox.Location = new System.Drawing.Point(227, 445);
            this.BatteryGroupBox.Name = "BatteryGroupBox";
            this.BatteryGroupBox.Size = new System.Drawing.Size(90, 97);
            this.BatteryGroupBox.TabIndex = 98;
            this.BatteryGroupBox.TabStop = false;
            this.BatteryGroupBox.Text = "Battery";
            // 
            // BatteryCharge
            // 
            this.BatteryCharge.BackColor = System.Drawing.SystemColors.Control;
            this.BatteryCharge.Location = new System.Drawing.Point(35, 71);
            this.BatteryCharge.Name = "BatteryCharge";
            this.BatteryCharge.ReadOnly = true;
            this.BatteryCharge.Size = new System.Drawing.Size(50, 20);
            this.BatteryCharge.TabIndex = 30;
            this.BatteryCharge.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // BatteryChargeLabel
            // 
            this.BatteryChargeLabel.AutoSize = true;
            this.BatteryChargeLabel.Location = new System.Drawing.Point(6, 74);
            this.BatteryChargeLabel.Name = "BatteryChargeLabel";
            this.BatteryChargeLabel.Size = new System.Drawing.Size(30, 13);
            this.BatteryChargeLabel.TabIndex = 31;
            this.BatteryChargeLabel.Text = "mAH";
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.AltHoldBox);
            this.flowLayoutPanel1.Controls.Add(this.TurnToWPBox);
            this.flowLayoutPanel1.Controls.Add(this.PHLockEnBox);
            this.flowLayoutPanel1.Controls.Add(this.UseRTHAutoDescendBox);
            this.flowLayoutPanel1.Controls.Add(this.BaroAltValidBox);
            this.flowLayoutPanel1.Controls.Add(this.RangefinderAltValidBox);
            this.flowLayoutPanel1.Controls.Add(this.GPSValidBox);
            this.flowLayoutPanel1.Controls.Add(this.NavValidBox);
            this.flowLayoutPanel1.Controls.Add(this.SticksFrozenBox);
            this.flowLayoutPanel1.Controls.Add(this.LowBatteryBox);
            this.flowLayoutPanel1.Controls.Add(this.ThrottleMovingBox);
            this.flowLayoutPanel1.Controls.Add(this.HoldingAltBox);
            this.flowLayoutPanel1.Controls.Add(this.AttitudeHoldBox);
            this.flowLayoutPanel1.Controls.Add(this.NearLevelBox);
            this.flowLayoutPanel1.Controls.Add(this.FocusLockedBox);
            this.flowLayoutPanel1.Controls.Add(this.NavigateBox);
            this.flowLayoutPanel1.Controls.Add(this.ReturnHomeBox);
            this.flowLayoutPanel1.Controls.Add(this.UsingRangefinderBox);
            this.flowLayoutPanel1.Controls.Add(this.CloseProximityBox);
            this.flowLayoutPanel1.Controls.Add(this.ProximityBox);
            this.flowLayoutPanel1.Controls.Add(this.LostModelBox);
            this.flowLayoutPanel1.Controls.Add(this.CompassFailBox);
            this.flowLayoutPanel1.Controls.Add(this.GyroFailureBox);
            this.flowLayoutPanel1.Controls.Add(this.BaroFailBox);
            this.flowLayoutPanel1.Controls.Add(this.AccFailBox);
            this.flowLayoutPanel1.Controls.Add(this.GPSFailBox);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(6, 19);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(237, 336);
            this.flowLayoutPanel1.TabIndex = 99;
            // 
            // AltHoldBox
            // 
            this.AltHoldBox.AcceptsTab = true;
            this.AltHoldBox.BackColor = System.Drawing.SystemColors.Window;
            this.AltHoldBox.Location = new System.Drawing.Point(3, 3);
            this.AltHoldBox.Name = "AltHoldBox";
            this.AltHoldBox.ReadOnly = true;
            this.AltHoldBox.Size = new System.Drawing.Size(109, 20);
            this.AltHoldBox.TabIndex = 100;
            this.AltHoldBox.Text = "Alt Hold Enabled";
            this.AltHoldBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // TurnToWPBox
            // 
            this.TurnToWPBox.BackColor = System.Drawing.SystemColors.Window;
            this.TurnToWPBox.Location = new System.Drawing.Point(118, 3);
            this.TurnToWPBox.Name = "TurnToWPBox";
            this.TurnToWPBox.ReadOnly = true;
            this.TurnToWPBox.Size = new System.Drawing.Size(109, 20);
            this.TurnToWPBox.TabIndex = 101;
            this.TurnToWPBox.Text = "Using Turn to Pos.";
            this.TurnToWPBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // PHLockEnBox
            // 
            this.PHLockEnBox.BackColor = System.Drawing.SystemColors.Window;
            this.PHLockEnBox.Location = new System.Drawing.Point(3, 29);
            this.PHLockEnBox.Name = "PHLockEnBox";
            this.PHLockEnBox.ReadOnly = true;
            this.PHLockEnBox.Size = new System.Drawing.Size(109, 20);
            this.PHLockEnBox.TabIndex = 124;
            this.PHLockEnBox.Text = "Pt. of Focus Enabled";
            this.PHLockEnBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // UseRTHAutoDescendBox
            // 
            this.UseRTHAutoDescendBox.BackColor = System.Drawing.SystemColors.Window;
            this.UseRTHAutoDescendBox.Location = new System.Drawing.Point(118, 29);
            this.UseRTHAutoDescendBox.Name = "UseRTHAutoDescendBox";
            this.UseRTHAutoDescendBox.ReadOnly = true;
            this.UseRTHAutoDescendBox.Size = new System.Drawing.Size(109, 20);
            this.UseRTHAutoDescendBox.TabIndex = 120;
            this.UseRTHAutoDescendBox.Text = "Using Auto Land";
            this.UseRTHAutoDescendBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // BaroAltValidBox
            // 
            this.BaroAltValidBox.BackColor = System.Drawing.SystemColors.Window;
            this.BaroAltValidBox.Location = new System.Drawing.Point(3, 55);
            this.BaroAltValidBox.Name = "BaroAltValidBox";
            this.BaroAltValidBox.ReadOnly = true;
            this.BaroAltValidBox.Size = new System.Drawing.Size(109, 20);
            this.BaroAltValidBox.TabIndex = 121;
            this.BaroAltValidBox.Text = "Baro Alt Valid";
            this.BaroAltValidBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // RangefinderAltValidBox
            // 
            this.RangefinderAltValidBox.BackColor = System.Drawing.SystemColors.Window;
            this.RangefinderAltValidBox.Location = new System.Drawing.Point(118, 55);
            this.RangefinderAltValidBox.Name = "RangefinderAltValidBox";
            this.RangefinderAltValidBox.ReadOnly = true;
            this.RangefinderAltValidBox.Size = new System.Drawing.Size(109, 20);
            this.RangefinderAltValidBox.TabIndex = 122;
            this.RangefinderAltValidBox.Text = "Rangefinder Valid";
            this.RangefinderAltValidBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // GPSValidBox
            // 
            this.GPSValidBox.BackColor = System.Drawing.SystemColors.Window;
            this.GPSValidBox.Location = new System.Drawing.Point(3, 81);
            this.GPSValidBox.Name = "GPSValidBox";
            this.GPSValidBox.ReadOnly = true;
            this.GPSValidBox.Size = new System.Drawing.Size(109, 20);
            this.GPSValidBox.TabIndex = 105;
            this.GPSValidBox.Text = "GPS Valid";
            this.GPSValidBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // NavValidBox
            // 
            this.NavValidBox.BackColor = System.Drawing.SystemColors.Window;
            this.NavValidBox.Location = new System.Drawing.Point(118, 81);
            this.NavValidBox.Name = "NavValidBox";
            this.NavValidBox.ReadOnly = true;
            this.NavValidBox.Size = new System.Drawing.Size(109, 20);
            this.NavValidBox.TabIndex = 107;
            this.NavValidBox.Text = "Origin Recorded";
            this.NavValidBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // SticksFrozenBox
            // 
            this.SticksFrozenBox.AcceptsTab = true;
            this.SticksFrozenBox.BackColor = System.Drawing.SystemColors.Window;
            this.SticksFrozenBox.Location = new System.Drawing.Point(3, 107);
            this.SticksFrozenBox.Name = "SticksFrozenBox";
            this.SticksFrozenBox.ReadOnly = true;
            this.SticksFrozenBox.Size = new System.Drawing.Size(109, 20);
            this.SticksFrozenBox.TabIndex = 126;
            this.SticksFrozenBox.Text = "Rx Active";
            this.SticksFrozenBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // LowBatteryBox
            // 
            this.LowBatteryBox.BackColor = System.Drawing.SystemColors.Window;
            this.LowBatteryBox.Location = new System.Drawing.Point(118, 107);
            this.LowBatteryBox.Name = "LowBatteryBox";
            this.LowBatteryBox.ReadOnly = true;
            this.LowBatteryBox.Size = new System.Drawing.Size(109, 20);
            this.LowBatteryBox.TabIndex = 106;
            this.LowBatteryBox.Text = "Low Battery";
            this.LowBatteryBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // ThrottleMovingBox
            // 
            this.ThrottleMovingBox.BackColor = System.Drawing.SystemColors.Window;
            this.ThrottleMovingBox.Location = new System.Drawing.Point(3, 133);
            this.ThrottleMovingBox.Name = "ThrottleMovingBox";
            this.ThrottleMovingBox.ReadOnly = true;
            this.ThrottleMovingBox.Size = new System.Drawing.Size(109, 20);
            this.ThrottleMovingBox.TabIndex = 113;
            this.ThrottleMovingBox.Text = "Throttle Moving";
            this.ThrottleMovingBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // HoldingAltBox
            // 
            this.HoldingAltBox.BackColor = System.Drawing.SystemColors.Window;
            this.HoldingAltBox.Location = new System.Drawing.Point(118, 133);
            this.HoldingAltBox.Name = "HoldingAltBox";
            this.HoldingAltBox.ReadOnly = true;
            this.HoldingAltBox.Size = new System.Drawing.Size(109, 20);
            this.HoldingAltBox.TabIndex = 114;
            this.HoldingAltBox.Text = "Holding Altitude";
            this.HoldingAltBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // AttitudeHoldBox
            // 
            this.AttitudeHoldBox.BackColor = System.Drawing.SystemColors.Window;
            this.AttitudeHoldBox.Location = new System.Drawing.Point(3, 159);
            this.AttitudeHoldBox.Name = "AttitudeHoldBox";
            this.AttitudeHoldBox.ReadOnly = true;
            this.AttitudeHoldBox.Size = new System.Drawing.Size(109, 20);
            this.AttitudeHoldBox.TabIndex = 112;
            this.AttitudeHoldBox.Text = "Holding Roll&Pitch";
            this.AttitudeHoldBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // NearLevelBox
            // 
            this.NearLevelBox.BackColor = System.Drawing.SystemColors.Window;
            this.NearLevelBox.Location = new System.Drawing.Point(118, 159);
            this.NearLevelBox.Name = "NearLevelBox";
            this.NearLevelBox.ReadOnly = true;
            this.NearLevelBox.Size = new System.Drawing.Size(109, 20);
            this.NearLevelBox.TabIndex = 104;
            this.NearLevelBox.Text = "Near Level";
            this.NearLevelBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // FocusLockedBox
            // 
            this.FocusLockedBox.BackColor = System.Drawing.SystemColors.Window;
            this.FocusLockedBox.Location = new System.Drawing.Point(3, 185);
            this.FocusLockedBox.Name = "FocusLockedBox";
            this.FocusLockedBox.ReadOnly = true;
            this.FocusLockedBox.Size = new System.Drawing.Size(109, 20);
            this.FocusLockedBox.TabIndex = 125;
            this.FocusLockedBox.Text = "Focus Locked";
            this.FocusLockedBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // NavigateBox
            // 
            this.NavigateBox.BackColor = System.Drawing.SystemColors.Window;
            this.NavigateBox.Location = new System.Drawing.Point(118, 185);
            this.NavigateBox.Name = "NavigateBox";
            this.NavigateBox.ReadOnly = true;
            this.NavigateBox.Size = new System.Drawing.Size(109, 20);
            this.NavigateBox.TabIndex = 115;
            this.NavigateBox.Text = "Navigate";
            this.NavigateBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // ReturnHomeBox
            // 
            this.ReturnHomeBox.BackColor = System.Drawing.SystemColors.Window;
            this.ReturnHomeBox.Location = new System.Drawing.Point(3, 211);
            this.ReturnHomeBox.Name = "ReturnHomeBox";
            this.ReturnHomeBox.ReadOnly = true;
            this.ReturnHomeBox.Size = new System.Drawing.Size(109, 20);
            this.ReturnHomeBox.TabIndex = 116;
            this.ReturnHomeBox.Text = "Ch5 Active";
            this.ReturnHomeBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // UsingRangefinderBox
            // 
            this.UsingRangefinderBox.BackColor = System.Drawing.SystemColors.Window;
            this.UsingRangefinderBox.Location = new System.Drawing.Point(118, 211);
            this.UsingRangefinderBox.Name = "UsingRangefinderBox";
            this.UsingRangefinderBox.ReadOnly = true;
            this.UsingRangefinderBox.Size = new System.Drawing.Size(109, 20);
            this.UsingRangefinderBox.TabIndex = 123;
            this.UsingRangefinderBox.Text = "Using Rangefinder";
            this.UsingRangefinderBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // CloseProximityBox
            // 
            this.CloseProximityBox.BackColor = System.Drawing.SystemColors.Window;
            this.CloseProximityBox.Location = new System.Drawing.Point(3, 237);
            this.CloseProximityBox.Name = "CloseProximityBox";
            this.CloseProximityBox.ReadOnly = true;
            this.CloseProximityBox.Size = new System.Drawing.Size(109, 20);
            this.CloseProximityBox.TabIndex = 118;
            this.CloseProximityBox.Text = "Centered";
            this.CloseProximityBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // ProximityBox
            // 
            this.ProximityBox.BackColor = System.Drawing.SystemColors.Window;
            this.ProximityBox.Location = new System.Drawing.Point(118, 237);
            this.ProximityBox.Name = "ProximityBox";
            this.ProximityBox.ReadOnly = true;
            this.ProximityBox.Size = new System.Drawing.Size(109, 20);
            this.ProximityBox.TabIndex = 117;
            this.ProximityBox.Text = "WP Achieved";
            this.ProximityBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // LostModelBox
            // 
            this.LostModelBox.BackColor = System.Drawing.SystemColors.Window;
            this.LostModelBox.Location = new System.Drawing.Point(3, 263);
            this.LostModelBox.Name = "LostModelBox";
            this.LostModelBox.ReadOnly = true;
            this.LostModelBox.Size = new System.Drawing.Size(109, 20);
            this.LostModelBox.TabIndex = 103;
            this.LostModelBox.Text = "Lost Model";
            this.LostModelBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // CompassFailBox
            // 
            this.CompassFailBox.BackColor = System.Drawing.SystemColors.Window;
            this.CompassFailBox.Location = new System.Drawing.Point(118, 263);
            this.CompassFailBox.Name = "CompassFailBox";
            this.CompassFailBox.ReadOnly = true;
            this.CompassFailBox.Size = new System.Drawing.Size(109, 20);
            this.CompassFailBox.TabIndex = 110;
            this.CompassFailBox.Text = "Compass Error";
            this.CompassFailBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // GyroFailureBox
            // 
            this.GyroFailureBox.BackColor = System.Drawing.SystemColors.Window;
            this.GyroFailureBox.Location = new System.Drawing.Point(3, 289);
            this.GyroFailureBox.Name = "GyroFailureBox";
            this.GyroFailureBox.ReadOnly = true;
            this.GyroFailureBox.Size = new System.Drawing.Size(109, 20);
            this.GyroFailureBox.TabIndex = 102;
            this.GyroFailureBox.Text = "Gyro Error";
            this.GyroFailureBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // BaroFailBox
            // 
            this.BaroFailBox.BackColor = System.Drawing.SystemColors.Window;
            this.BaroFailBox.Location = new System.Drawing.Point(118, 289);
            this.BaroFailBox.Name = "BaroFailBox";
            this.BaroFailBox.ReadOnly = true;
            this.BaroFailBox.Size = new System.Drawing.Size(109, 20);
            this.BaroFailBox.TabIndex = 108;
            this.BaroFailBox.Text = "Barometer Error";
            this.BaroFailBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // AccFailBox
            // 
            this.AccFailBox.BackColor = System.Drawing.SystemColors.Window;
            this.AccFailBox.Location = new System.Drawing.Point(3, 315);
            this.AccFailBox.Name = "AccFailBox";
            this.AccFailBox.ReadOnly = true;
            this.AccFailBox.Size = new System.Drawing.Size(109, 20);
            this.AccFailBox.TabIndex = 109;
            this.AccFailBox.Text = "Accelerometer Error";
            this.AccFailBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // GPSFailBox
            // 
            this.GPSFailBox.BackColor = System.Drawing.SystemColors.Window;
            this.GPSFailBox.Location = new System.Drawing.Point(118, 315);
            this.GPSFailBox.Name = "GPSFailBox";
            this.GPSFailBox.ReadOnly = true;
            this.GPSFailBox.Size = new System.Drawing.Size(109, 20);
            this.GPSFailBox.TabIndex = 111;
            this.GPSFailBox.Text = "GPS Error";
            this.GPSFailBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // FlagsGroupBox
            // 
            this.FlagsGroupBox.Controls.Add(this.flowLayoutPanel1);
            this.FlagsGroupBox.Location = new System.Drawing.Point(587, 104);
            this.FlagsGroupBox.Name = "FlagsGroupBox";
            this.FlagsGroupBox.Size = new System.Drawing.Size(249, 361);
            this.FlagsGroupBox.TabIndex = 100;
            this.FlagsGroupBox.TabStop = false;
            this.FlagsGroupBox.Text = "Flags";
            // 
            // GPSStatBox
            // 
            this.GPSStatBox.Controls.Add(this.GPSNoOfSats);
            this.GPSStatBox.Controls.Add(this.GPSFixLabel);
            this.GPSStatBox.Controls.Add(this.GPSNoOfSatsLabel);
            this.GPSStatBox.Controls.Add(this.GPSFix);
            this.GPSStatBox.Controls.Add(this.GPSHDilute);
            this.GPSStatBox.Controls.Add(this.HDiluteLabel);
            this.GPSStatBox.Location = new System.Drawing.Point(128, 444);
            this.GPSStatBox.Name = "GPSStatBox";
            this.GPSStatBox.Size = new System.Drawing.Size(93, 98);
            this.GPSStatBox.TabIndex = 101;
            this.GPSStatBox.TabStop = false;
            this.GPSStatBox.Text = "GPS Status";
            // 
            // AltitudeGroupBox
            // 
            this.AltitudeGroupBox.Controls.Add(this.ROCLabel);
            this.AltitudeGroupBox.Controls.Add(this.RelAltitudeLabel);
            this.AltitudeGroupBox.Controls.Add(this.RangefinderAltitude);
            this.AltitudeGroupBox.Controls.Add(this.GPSRelAltitude);
            this.AltitudeGroupBox.Controls.Add(this.BaroROC);
            this.AltitudeGroupBox.Controls.Add(this.RelBaroAltitude);
            this.AltitudeGroupBox.Controls.Add(this.GPSAltLabel);
            this.AltitudeGroupBox.Controls.Add(this.BaroAltitudeLabel);
            this.AltitudeGroupBox.Controls.Add(this.RangefinderAltLabel);
            this.AltitudeGroupBox.Location = new System.Drawing.Point(12, 292);
            this.AltitudeGroupBox.Name = "AltitudeGroupBox";
            this.AltitudeGroupBox.Size = new System.Drawing.Size(160, 83);
            this.AltitudeGroupBox.TabIndex = 102;
            this.AltitudeGroupBox.TabStop = false;
            this.AltitudeGroupBox.Text = "Altitude";
            // 
            // WayHeadingLabel
            // 
            this.WayHeadingLabel.AutoSize = true;
            this.WayHeadingLabel.Location = new System.Drawing.Point(6, 189);
            this.WayHeadingLabel.Name = "WayHeadingLabel";
            this.WayHeadingLabel.Size = new System.Drawing.Size(72, 13);
            this.WayHeadingLabel.TabIndex = 64;
            this.WayHeadingLabel.Text = "Course (Deg.)";
            // 
            // GPSLongitudeLabel
            // 
            this.GPSLongitudeLabel.AutoSize = true;
            this.GPSLongitudeLabel.Location = new System.Drawing.Point(6, 293);
            this.GPSLongitudeLabel.Name = "GPSLongitudeLabel";
            this.GPSLongitudeLabel.Size = new System.Drawing.Size(54, 13);
            this.GPSLongitudeLabel.TabIndex = 63;
            this.GPSLongitudeLabel.Text = "Longitude";
            // 
            // GPSLatitudeLabel
            // 
            this.GPSLatitudeLabel.AutoSize = true;
            this.GPSLatitudeLabel.Location = new System.Drawing.Point(5, 267);
            this.GPSLatitudeLabel.Name = "GPSLatitudeLabel";
            this.GPSLatitudeLabel.Size = new System.Drawing.Size(45, 13);
            this.GPSLatitudeLabel.TabIndex = 61;
            this.GPSLatitudeLabel.Text = "Latitude";
            // 
            // NavStateLabel
            // 
            this.NavStateLabel.AutoSize = true;
            this.NavStateLabel.Location = new System.Drawing.Point(6, 22);
            this.NavStateLabel.Name = "NavStateLabel";
            this.NavStateLabel.Size = new System.Drawing.Size(55, 13);
            this.NavStateLabel.TabIndex = 72;
            this.NavStateLabel.Text = "Nav State";
            // 
            // GPSVelLabel
            // 
            this.GPSVelLabel.AutoSize = true;
            this.GPSVelLabel.Location = new System.Drawing.Point(6, 163);
            this.GPSVelLabel.Name = "GPSVelLabel";
            this.GPSVelLabel.Size = new System.Drawing.Size(47, 13);
            this.GPSVelLabel.TabIndex = 60;
            this.GPSVelLabel.Text = "GPS Vel";
            // 
            // CurrWPLabel
            // 
            this.CurrWPLabel.AutoSize = true;
            this.CurrWPLabel.Location = new System.Drawing.Point(6, 104);
            this.CurrWPLabel.Name = "CurrWPLabel";
            this.CurrWPLabel.Size = new System.Drawing.Size(32, 13);
            this.CurrWPLabel.TabIndex = 73;
            this.CurrWPLabel.Text = "WP#";
            // 
            // GPSLatitude
            // 
            this.GPSLatitude.Location = new System.Drawing.Point(61, 264);
            this.GPSLatitude.Name = "GPSLatitude";
            this.GPSLatitude.ReadOnly = true;
            this.GPSLatitude.Size = new System.Drawing.Size(73, 20);
            this.GPSLatitude.TabIndex = 57;
            this.GPSLatitude.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // GPSLongitude
            // 
            this.GPSLongitude.Location = new System.Drawing.Point(61, 290);
            this.GPSLongitude.Name = "GPSLongitude";
            this.GPSLongitude.ReadOnly = true;
            this.GPSLongitude.Size = new System.Drawing.Size(73, 20);
            this.GPSLongitude.TabIndex = 56;
            this.GPSLongitude.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // NavState
            // 
            this.NavState.Location = new System.Drawing.Point(61, 19);
            this.NavState.Name = "NavState";
            this.NavState.ReadOnly = true;
            this.NavState.Size = new System.Drawing.Size(73, 20);
            this.NavState.TabIndex = 52;
            this.NavState.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // CurrWP
            // 
            this.CurrWP.Location = new System.Drawing.Point(97, 101);
            this.CurrWP.Name = "CurrWP";
            this.CurrWP.ReadOnly = true;
            this.CurrWP.Size = new System.Drawing.Size(37, 20);
            this.CurrWP.TabIndex = 50;
            this.CurrWP.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // WayHeading
            // 
            this.WayHeading.Location = new System.Drawing.Point(97, 186);
            this.WayHeading.Name = "WayHeading";
            this.WayHeading.ReadOnly = true;
            this.WayHeading.Size = new System.Drawing.Size(37, 20);
            this.WayHeading.TabIndex = 48;
            this.WayHeading.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // WPAltitude
            // 
            this.WPAltitude.AutoSize = true;
            this.WPAltitude.Location = new System.Drawing.Point(6, 241);
            this.WPAltitude.Name = "WPAltitude";
            this.WPAltitude.Size = new System.Drawing.Size(47, 13);
            this.WPAltitude.TabIndex = 77;
            this.WPAltitude.Text = "Alt. Error";
            // 
            // GPSVel
            // 
            this.GPSVel.Location = new System.Drawing.Point(97, 160);
            this.GPSVel.Name = "GPSVel";
            this.GPSVel.ReadOnly = true;
            this.GPSVel.Size = new System.Drawing.Size(37, 20);
            this.GPSVel.TabIndex = 45;
            this.GPSVel.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // HeadingLabel
            // 
            this.HeadingLabel.AutoSize = true;
            this.HeadingLabel.Location = new System.Drawing.Point(6, 78);
            this.HeadingLabel.Name = "HeadingLabel";
            this.HeadingLabel.Size = new System.Drawing.Size(82, 13);
            this.HeadingLabel.TabIndex = 41;
            this.HeadingLabel.Text = "Compass (Deg.)";
            // 
            // AltitudeError
            // 
            this.AltitudeError.Location = new System.Drawing.Point(97, 238);
            this.AltitudeError.Name = "AltitudeError";
            this.AltitudeError.ReadOnly = true;
            this.AltitudeError.Size = new System.Drawing.Size(37, 20);
            this.AltitudeError.TabIndex = 76;
            this.AltitudeError.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // Heading
            // 
            this.Heading.Location = new System.Drawing.Point(97, 75);
            this.Heading.Name = "Heading";
            this.Heading.ReadOnly = true;
            this.Heading.Size = new System.Drawing.Size(37, 20);
            this.Heading.TabIndex = 7;
            this.Heading.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // WPDistanceLabel
            // 
            this.WPDistanceLabel.AutoSize = true;
            this.WPDistanceLabel.Location = new System.Drawing.Point(5, 215);
            this.WPDistanceLabel.Name = "WPDistanceLabel";
            this.WPDistanceLabel.Size = new System.Drawing.Size(49, 13);
            this.WPDistanceLabel.TabIndex = 75;
            this.WPDistanceLabel.Text = "Distance";
            // 
            // DistanceToDesired
            // 
            this.DistanceToDesired.Location = new System.Drawing.Point(97, 212);
            this.DistanceToDesired.Name = "DistanceToDesired";
            this.DistanceToDesired.ReadOnly = true;
            this.DistanceToDesired.Size = new System.Drawing.Size(37, 20);
            this.DistanceToDesired.TabIndex = 74;
            this.DistanceToDesired.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // NavGroupBox
            // 
            this.NavGroupBox.Controls.Add(this.GPSHeadingLabel);
            this.NavGroupBox.Controls.Add(this.GPSHeading);
            this.NavGroupBox.Controls.Add(this.NavStateTimeoutLabel);
            this.NavGroupBox.Controls.Add(this.NavStateTimeout);
            this.NavGroupBox.Controls.Add(this.DistanceToDesired);
            this.NavGroupBox.Controls.Add(this.NavState);
            this.NavGroupBox.Controls.Add(this.WayHeadingLabel);
            this.NavGroupBox.Controls.Add(this.WPDistanceLabel);
            this.NavGroupBox.Controls.Add(this.GPSLongitudeLabel);
            this.NavGroupBox.Controls.Add(this.GPSLatitudeLabel);
            this.NavGroupBox.Controls.Add(this.Heading);
            this.NavGroupBox.Controls.Add(this.NavStateLabel);
            this.NavGroupBox.Controls.Add(this.GPSVelLabel);
            this.NavGroupBox.Controls.Add(this.AltitudeError);
            this.NavGroupBox.Controls.Add(this.CurrWPLabel);
            this.NavGroupBox.Controls.Add(this.GPSLatitude);
            this.NavGroupBox.Controls.Add(this.GPSLongitude);
            this.NavGroupBox.Controls.Add(this.CurrWP);
            this.NavGroupBox.Controls.Add(this.HeadingLabel);
            this.NavGroupBox.Controls.Add(this.GPSVel);
            this.NavGroupBox.Controls.Add(this.WayHeading);
            this.NavGroupBox.Controls.Add(this.WPAltitude);
            this.NavGroupBox.Location = new System.Drawing.Point(178, 57);
            this.NavGroupBox.Name = "NavGroupBox";
            this.NavGroupBox.Size = new System.Drawing.Size(139, 318);
            this.NavGroupBox.TabIndex = 91;
            this.NavGroupBox.TabStop = false;
            this.NavGroupBox.Text = "Nav";
            // 
            // GPSHeadingLabel
            // 
            this.GPSHeadingLabel.AutoSize = true;
            this.GPSHeadingLabel.Location = new System.Drawing.Point(6, 130);
            this.GPSHeadingLabel.Name = "GPSHeadingLabel";
            this.GPSHeadingLabel.Size = new System.Drawing.Size(61, 13);
            this.GPSHeadingLabel.TabIndex = 155;
            this.GPSHeadingLabel.Text = "GPS (Deg.)";
            // 
            // GPSHeading
            // 
            this.GPSHeading.Location = new System.Drawing.Point(97, 127);
            this.GPSHeading.Name = "GPSHeading";
            this.GPSHeading.ReadOnly = true;
            this.GPSHeading.Size = new System.Drawing.Size(37, 20);
            this.GPSHeading.TabIndex = 154;
            this.GPSHeading.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // NavStateTimeoutLabel
            // 
            this.NavStateTimeoutLabel.AutoSize = true;
            this.NavStateTimeoutLabel.Location = new System.Drawing.Point(6, 51);
            this.NavStateTimeoutLabel.Name = "NavStateTimeoutLabel";
            this.NavStateTimeoutLabel.Size = new System.Drawing.Size(60, 13);
            this.NavStateTimeoutLabel.TabIndex = 153;
            this.NavStateTimeoutLabel.Text = "Delay Time";
            // 
            // NavStateTimeout
            // 
            this.NavStateTimeout.Location = new System.Drawing.Point(97, 48);
            this.NavStateTimeout.Name = "NavStateTimeout";
            this.NavStateTimeout.ReadOnly = true;
            this.NavStateTimeout.Size = new System.Drawing.Size(37, 20);
            this.NavStateTimeout.TabIndex = 153;
            this.NavStateTimeout.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // CurrentAltitude
            // 
            this.CurrentAltitude.BackColor = System.Drawing.SystemColors.Control;
            this.CurrentAltitude.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.CurrentAltitude.Font = new System.Drawing.Font("Microsoft Sans Serif", 30F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CurrentAltitude.Location = new System.Drawing.Point(369, 56);
            this.CurrentAltitude.Name = "CurrentAltitude";
            this.CurrentAltitude.Size = new System.Drawing.Size(159, 46);
            this.CurrentAltitude.TabIndex = 103;
            this.CurrentAltitude.Text = "-1";
            this.CurrentAltitude.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(349, 533);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(200, 13);
            this.label1.TabIndex = 104;
            this.label1.Text = "UAVX Groundstation (C) G.K. Egan 2010";
            // 
            // AltitudeSource
            // 
            this.AltitudeSource.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.AltitudeSource.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.AltitudeSource.Location = new System.Drawing.Point(389, 47);
            this.AltitudeSource.Name = "AltitudeSource";
            this.AltitudeSource.ReadOnly = true;
            this.AltitudeSource.Size = new System.Drawing.Size(121, 13);
            this.AltitudeSource.TabIndex = 107;
            this.AltitudeSource.Text = "Barometer";
            this.AltitudeSource.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // GoogleButton
            // 
            this.GoogleButton.BackColor = System.Drawing.Color.Silver;
            this.GoogleButton.ForeColor = System.Drawing.SystemColors.ControlText;
            this.GoogleButton.Location = new System.Drawing.Point(745, 0);
            this.GoogleButton.Name = "GoogleButton";
            this.GoogleButton.Size = new System.Drawing.Size(91, 23);
            this.GoogleButton.TabIndex = 108;
            this.GoogleButton.Text = "Google";
            this.GoogleButton.UseVisualStyleBackColor = false;
            // 
            // MessageTextBox
            // 
            this.MessageTextBox.Location = new System.Drawing.Point(587, 595);
            this.MessageTextBox.Multiline = true;
            this.MessageTextBox.Name = "MessageTextBox";
            this.MessageTextBox.Size = new System.Drawing.Size(249, 31);
            this.MessageTextBox.TabIndex = 110;
            // 
            // LocationBox
            // 
            this.LocationBox.Controls.Add(this.WhereDistance);
            this.LocationBox.Controls.Add(this.WhereBearing);
            this.LocationBox.Controls.Add(this.WhereBearingLabel);
            this.LocationBox.Controls.Add(this.WhereDistanceLabel);
            this.LocationBox.Location = new System.Drawing.Point(718, 33);
            this.LocationBox.Name = "LocationBox";
            this.LocationBox.Size = new System.Drawing.Size(118, 69);
            this.LocationBox.TabIndex = 152;
            this.LocationBox.TabStop = false;
            this.LocationBox.Text = "Where?";
            // 
            // WhereDistance
            // 
            this.WhereDistance.Location = new System.Drawing.Point(73, 40);
            this.WhereDistance.Name = "WhereDistance";
            this.WhereDistance.ReadOnly = true;
            this.WhereDistance.Size = new System.Drawing.Size(39, 20);
            this.WhereDistance.TabIndex = 165;
            this.WhereDistance.Text = "0";
            this.WhereDistance.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // WhereBearing
            // 
            this.WhereBearing.Location = new System.Drawing.Point(73, 14);
            this.WhereBearing.Name = "WhereBearing";
            this.WhereBearing.ReadOnly = true;
            this.WhereBearing.Size = new System.Drawing.Size(39, 20);
            this.WhereBearing.TabIndex = 164;
            this.WhereBearing.Text = "0";
            this.WhereBearing.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // WhereBearingLabel
            // 
            this.WhereBearingLabel.AutoSize = true;
            this.WhereBearingLabel.Location = new System.Drawing.Point(6, 17);
            this.WhereBearingLabel.Name = "WhereBearingLabel";
            this.WhereBearingLabel.Size = new System.Drawing.Size(43, 13);
            this.WhereBearingLabel.TabIndex = 54;
            this.WhereBearingLabel.Text = "Bearing";
            // 
            // WhereDistanceLabel
            // 
            this.WhereDistanceLabel.AutoSize = true;
            this.WhereDistanceLabel.Location = new System.Drawing.Point(6, 43);
            this.WhereDistanceLabel.Name = "WhereDistanceLabel";
            this.WhereDistanceLabel.Size = new System.Drawing.Size(49, 13);
            this.WhereDistanceLabel.TabIndex = 53;
            this.WhereDistanceLabel.Text = "Distance";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.LRComp);
            this.groupBox1.Controls.Add(this.LRCompLabel);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.AltComp);
            this.groupBox1.Controls.Add(this.FBComp);
            this.groupBox1.Controls.Add(this.DUComp);
            this.groupBox1.Controls.Add(this.FBCompLabel);
            this.groupBox1.Controls.Add(this.DUCompLabel);
            this.groupBox1.Controls.Add(this.AltCompLabel);
            this.groupBox1.Location = new System.Drawing.Point(178, 381);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(139, 57);
            this.groupBox1.TabIndex = 103;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Compensation (%)";
            // 
            // LRComp
            // 
            this.LRComp.Location = new System.Drawing.Point(8, 29);
            this.LRComp.Name = "LRComp";
            this.LRComp.ReadOnly = true;
            this.LRComp.Size = new System.Drawing.Size(26, 20);
            this.LRComp.TabIndex = 59;
            this.LRComp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // LRCompLabel
            // 
            this.LRCompLabel.AutoSize = true;
            this.LRCompLabel.Location = new System.Drawing.Point(9, 13);
            this.LRCompLabel.Name = "LRCompLabel";
            this.LRCompLabel.Size = new System.Drawing.Size(21, 13);
            this.LRCompLabel.TabIndex = 65;
            this.LRCompLabel.Text = "LR";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 29);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(0, 13);
            this.label3.TabIndex = 62;
            // 
            // AltComp
            // 
            this.AltComp.Location = new System.Drawing.Point(105, 29);
            this.AltComp.Name = "AltComp";
            this.AltComp.ReadOnly = true;
            this.AltComp.Size = new System.Drawing.Size(26, 20);
            this.AltComp.TabIndex = 43;
            this.AltComp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FBComp
            // 
            this.FBComp.Location = new System.Drawing.Point(40, 29);
            this.FBComp.Name = "FBComp";
            this.FBComp.ReadOnly = true;
            this.FBComp.Size = new System.Drawing.Size(26, 20);
            this.FBComp.TabIndex = 58;
            this.FBComp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // DUComp
            // 
            this.DUComp.Location = new System.Drawing.Point(72, 29);
            this.DUComp.Name = "DUComp";
            this.DUComp.ReadOnly = true;
            this.DUComp.Size = new System.Drawing.Size(27, 20);
            this.DUComp.TabIndex = 46;
            this.DUComp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FBCompLabel
            // 
            this.FBCompLabel.AutoSize = true;
            this.FBCompLabel.Location = new System.Drawing.Point(41, 13);
            this.FBCompLabel.Name = "FBCompLabel";
            this.FBCompLabel.Size = new System.Drawing.Size(20, 13);
            this.FBCompLabel.TabIndex = 74;
            this.FBCompLabel.Text = "FB";
            // 
            // DUCompLabel
            // 
            this.DUCompLabel.AutoSize = true;
            this.DUCompLabel.Location = new System.Drawing.Point(76, 13);
            this.DUCompLabel.Name = "DUCompLabel";
            this.DUCompLabel.Size = new System.Drawing.Size(23, 13);
            this.DUCompLabel.TabIndex = 75;
            this.DUCompLabel.Text = "DU";
            // 
            // AltCompLabel
            // 
            this.AltCompLabel.AutoSize = true;
            this.AltCompLabel.Location = new System.Drawing.Point(109, 13);
            this.AltCompLabel.Name = "AltCompLabel";
            this.AltCompLabel.Size = new System.Drawing.Size(19, 13);
            this.AltCompLabel.TabIndex = 67;
            this.AltCompLabel.Text = "Alt";
            // 
            // serialPort1
            // 
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // SimulationTextBox
            // 
            this.SimulationTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.SimulationTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SimulationTextBox.Location = new System.Drawing.Point(389, 28);
            this.SimulationTextBox.Name = "SimulationTextBox";
            this.SimulationTextBox.ReadOnly = true;
            this.SimulationTextBox.Size = new System.Drawing.Size(121, 13);
            this.SimulationTextBox.TabIndex = 154;
            this.SimulationTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // OutputGroupBox
            // 
            this.OutputGroupBox.Controls.Add(this.OutputTotalLabel);
            this.OutputGroupBox.Controls.Add(this.OutputTot);
            this.OutputGroupBox.Controls.Add(this.Output4Label);
            this.OutputGroupBox.Controls.Add(this.Output5Label);
            this.OutputGroupBox.Controls.Add(this.OutputT5);
            this.OutputGroupBox.Controls.Add(this.OutputT4);
            this.OutputGroupBox.Controls.Add(this.Output2Label);
            this.OutputGroupBox.Controls.Add(this.Output3Label);
            this.OutputGroupBox.Controls.Add(this.Output1Label);
            this.OutputGroupBox.Controls.Add(this.Output0Label);
            this.OutputGroupBox.Controls.Add(this.OutputT3);
            this.OutputGroupBox.Controls.Add(this.OutputT2);
            this.OutputGroupBox.Controls.Add(this.OutputT1);
            this.OutputGroupBox.Controls.Add(this.OutputT0);
            this.OutputGroupBox.Location = new System.Drawing.Point(587, 471);
            this.OutputGroupBox.Name = "OutputGroupBox";
            this.OutputGroupBox.Size = new System.Drawing.Size(249, 56);
            this.OutputGroupBox.TabIndex = 156;
            this.OutputGroupBox.TabStop = false;
            this.OutputGroupBox.Text = "Outputs (%)";
            // 
            // OutputTotalLabel
            // 
            this.OutputTotalLabel.AutoSize = true;
            this.OutputTotalLabel.Location = new System.Drawing.Point(208, 13);
            this.OutputTotalLabel.Name = "OutputTotalLabel";
            this.OutputTotalLabel.Size = new System.Drawing.Size(26, 13);
            this.OutputTotalLabel.TabIndex = 79;
            this.OutputTotalLabel.Text = "Tot.";
            // 
            // OutputTot
            // 
            this.OutputTot.Location = new System.Drawing.Point(207, 29);
            this.OutputTot.Name = "OutputTot";
            this.OutputTot.ReadOnly = true;
            this.OutputTot.Size = new System.Drawing.Size(27, 20);
            this.OutputTot.TabIndex = 78;
            this.OutputTot.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // Output4Label
            // 
            this.Output4Label.AutoSize = true;
            this.Output4Label.Location = new System.Drawing.Point(148, 14);
            this.Output4Label.Name = "Output4Label";
            this.Output4Label.Size = new System.Drawing.Size(13, 13);
            this.Output4Label.TabIndex = 77;
            this.Output4Label.Text = "4";
            // 
            // Output5Label
            // 
            this.Output5Label.AutoSize = true;
            this.Output5Label.Location = new System.Drawing.Point(181, 14);
            this.Output5Label.Name = "Output5Label";
            this.Output5Label.Size = new System.Drawing.Size(13, 13);
            this.Output5Label.TabIndex = 76;
            this.Output5Label.Text = "5";
            // 
            // OutputT5
            // 
            this.OutputT5.Location = new System.Drawing.Point(174, 29);
            this.OutputT5.Name = "OutputT5";
            this.OutputT5.ReadOnly = true;
            this.OutputT5.Size = new System.Drawing.Size(27, 20);
            this.OutputT5.TabIndex = 75;
            this.OutputT5.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // OutputT4
            // 
            this.OutputT4.Location = new System.Drawing.Point(141, 29);
            this.OutputT4.Name = "OutputT4";
            this.OutputT4.ReadOnly = true;
            this.OutputT4.Size = new System.Drawing.Size(27, 20);
            this.OutputT4.TabIndex = 74;
            this.OutputT4.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // Output2Label
            // 
            this.Output2Label.AutoSize = true;
            this.Output2Label.Location = new System.Drawing.Point(83, 13);
            this.Output2Label.Name = "Output2Label";
            this.Output2Label.Size = new System.Drawing.Size(13, 13);
            this.Output2Label.TabIndex = 73;
            this.Output2Label.Text = "2";
            // 
            // Output3Label
            // 
            this.Output3Label.AutoSize = true;
            this.Output3Label.Location = new System.Drawing.Point(114, 13);
            this.Output3Label.Name = "Output3Label";
            this.Output3Label.Size = new System.Drawing.Size(13, 13);
            this.Output3Label.TabIndex = 72;
            this.Output3Label.Text = "3";
            // 
            // Output1Label
            // 
            this.Output1Label.AutoSize = true;
            this.Output1Label.Location = new System.Drawing.Point(49, 14);
            this.Output1Label.Name = "Output1Label";
            this.Output1Label.Size = new System.Drawing.Size(13, 13);
            this.Output1Label.TabIndex = 71;
            this.Output1Label.Text = "1";
            // 
            // Output0Label
            // 
            this.Output0Label.AutoSize = true;
            this.Output0Label.Location = new System.Drawing.Point(17, 14);
            this.Output0Label.Name = "Output0Label";
            this.Output0Label.Size = new System.Drawing.Size(13, 13);
            this.Output0Label.TabIndex = 68;
            this.Output0Label.Text = "0";
            // 
            // OutputT3
            // 
            this.OutputT3.Location = new System.Drawing.Point(108, 29);
            this.OutputT3.Name = "OutputT3";
            this.OutputT3.ReadOnly = true;
            this.OutputT3.Size = new System.Drawing.Size(27, 20);
            this.OutputT3.TabIndex = 47;
            this.OutputT3.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // OutputT2
            // 
            this.OutputT2.Location = new System.Drawing.Point(75, 29);
            this.OutputT2.Name = "OutputT2";
            this.OutputT2.ReadOnly = true;
            this.OutputT2.Size = new System.Drawing.Size(27, 20);
            this.OutputT2.TabIndex = 46;
            this.OutputT2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // OutputT1
            // 
            this.OutputT1.Location = new System.Drawing.Point(42, 30);
            this.OutputT1.Name = "OutputT1";
            this.OutputT1.ReadOnly = true;
            this.OutputT1.Size = new System.Drawing.Size(27, 20);
            this.OutputT1.TabIndex = 45;
            this.OutputT1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // OutputT0
            // 
            this.OutputT0.Location = new System.Drawing.Point(9, 30);
            this.OutputT0.Name = "OutputT0";
            this.OutputT0.ReadOnly = true;
            this.OutputT0.Size = new System.Drawing.Size(27, 20);
            this.OutputT0.TabIndex = 44;
            this.OutputT0.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // Output7Label
            // 
            this.Output7Label.AutoSize = true;
            this.Output7Label.Location = new System.Drawing.Point(33, 12);
            this.Output7Label.Name = "Output7Label";
            this.Output7Label.Size = new System.Drawing.Size(21, 13);
            this.Output7Label.TabIndex = 70;
            this.Output7Label.Text = "Tilt";
            // 
            // Output6Label
            // 
            this.Output6Label.AutoSize = true;
            this.Output6Label.Location = new System.Drawing.Point(6, 12);
            this.Output6Label.Name = "Output6Label";
            this.Output6Label.Size = new System.Drawing.Size(25, 13);
            this.Output6Label.TabIndex = 69;
            this.Output6Label.Text = "Roll";
            // 
            // OutputT7
            // 
            this.OutputT7.Location = new System.Drawing.Point(34, 28);
            this.OutputT7.Name = "OutputT7";
            this.OutputT7.ReadOnly = true;
            this.OutputT7.Size = new System.Drawing.Size(23, 20);
            this.OutputT7.TabIndex = 49;
            this.OutputT7.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // OutputT6
            // 
            this.OutputT6.Location = new System.Drawing.Point(6, 28);
            this.OutputT6.Name = "OutputT6";
            this.OutputT6.ReadOnly = true;
            this.OutputT6.Size = new System.Drawing.Size(22, 20);
            this.OutputT6.TabIndex = 48;
            this.OutputT6.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // BaroStatsGroupBox
            // 
            this.BaroStatsGroupBox.Controls.Add(this.MaxTempS);
            this.BaroStatsGroupBox.Controls.Add(this.TempSLabel);
            this.BaroStatsGroupBox.Controls.Add(this.MinTempS);
            this.BaroStatsGroupBox.Controls.Add(this.label2);
            this.BaroStatsGroupBox.Controls.Add(this.BaroMaxROCS);
            this.BaroStatsGroupBox.Controls.Add(this.RelBaroAltitudeLabel);
            this.BaroStatsGroupBox.Controls.Add(this.BaroRelAltitudeS);
            this.BaroStatsGroupBox.Controls.Add(this.BaroMinROCS);
            this.BaroStatsGroupBox.Location = new System.Drawing.Point(147, 548);
            this.BaroStatsGroupBox.Name = "BaroStatsGroupBox";
            this.BaroStatsGroupBox.Size = new System.Drawing.Size(121, 78);
            this.BaroStatsGroupBox.TabIndex = 159;
            this.BaroStatsGroupBox.TabStop = false;
            this.BaroStatsGroupBox.Text = "Baro Stats";
            // 
            // MaxTempS
            // 
            this.MaxTempS.Location = new System.Drawing.Point(83, 56);
            this.MaxTempS.Name = "MaxTempS";
            this.MaxTempS.Size = new System.Drawing.Size(31, 13);
            this.MaxTempS.TabIndex = 70;
            this.MaxTempS.Text = "0";
            this.MaxTempS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // TempSLabel
            // 
            this.TempSLabel.AutoSize = true;
            this.TempSLabel.Location = new System.Drawing.Point(7, 56);
            this.TempSLabel.Name = "TempSLabel";
            this.TempSLabel.Size = new System.Drawing.Size(37, 13);
            this.TempSLabel.TabIndex = 69;
            this.TempSLabel.Text = "Temp:";
            // 
            // MinTempS
            // 
            this.MinTempS.Location = new System.Drawing.Point(49, 56);
            this.MinTempS.Name = "MinTempS";
            this.MinTempS.Size = new System.Drawing.Size(31, 13);
            this.MinTempS.TabIndex = 68;
            this.MinTempS.Text = "0";
            this.MinTempS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 36);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(33, 13);
            this.label2.TabIndex = 53;
            this.label2.Text = "ROC:";
            // 
            // BaroMaxROCS
            // 
            this.BaroMaxROCS.Location = new System.Drawing.Point(86, 36);
            this.BaroMaxROCS.Name = "BaroMaxROCS";
            this.BaroMaxROCS.Size = new System.Drawing.Size(29, 13);
            this.BaroMaxROCS.TabIndex = 52;
            this.BaroMaxROCS.Text = "0";
            this.BaroMaxROCS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // RelBaroAltitudeLabel
            // 
            this.RelBaroAltitudeLabel.AutoSize = true;
            this.RelBaroAltitudeLabel.Location = new System.Drawing.Point(7, 16);
            this.RelBaroAltitudeLabel.Name = "RelBaroAltitudeLabel";
            this.RelBaroAltitudeLabel.Size = new System.Drawing.Size(22, 13);
            this.RelBaroAltitudeLabel.TabIndex = 49;
            this.RelBaroAltitudeLabel.Text = "Alt:";
            // 
            // BaroRelAltitudeS
            // 
            this.BaroRelAltitudeS.Location = new System.Drawing.Point(46, 16);
            this.BaroRelAltitudeS.Name = "BaroRelAltitudeS";
            this.BaroRelAltitudeS.Size = new System.Drawing.Size(34, 13);
            this.BaroRelAltitudeS.TabIndex = 48;
            this.BaroRelAltitudeS.Text = "0";
            this.BaroRelAltitudeS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // BaroMinROCS
            // 
            this.BaroMinROCS.Location = new System.Drawing.Point(38, 36);
            this.BaroMinROCS.Name = "BaroMinROCS";
            this.BaroMinROCS.Size = new System.Drawing.Size(42, 13);
            this.BaroMinROCS.TabIndex = 51;
            this.BaroMinROCS.Text = "0";
            this.BaroMinROCS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // ErrorStatsGroupBox
            // 
            this.ErrorStatsGroupBox.Controls.Add(this.RCGlitches);
            this.ErrorStatsGroupBox.Controls.Add(this.BadS);
            this.ErrorStatsGroupBox.Controls.Add(this.RCGlitchesLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.BadSLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.FailsafeSLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.GyroSLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.RCFailSafeS);
            this.ErrorStatsGroupBox.Controls.Add(this.I2CESCFailS);
            this.ErrorStatsGroupBox.Controls.Add(this.GPSInvalidSLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.GPSFailS);
            this.ErrorStatsGroupBox.Controls.Add(this.ESCSLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.GyroFailS);
            this.ErrorStatsGroupBox.Controls.Add(this.AccFailLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.AccFailS);
            this.ErrorStatsGroupBox.Controls.Add(this.I2CFailLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.CompassFailLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.I2CFailS);
            this.ErrorStatsGroupBox.Controls.Add(this.CompassFailS);
            this.ErrorStatsGroupBox.Controls.Add(this.BaroFailLabel);
            this.ErrorStatsGroupBox.Controls.Add(this.BaroFailS);
            this.ErrorStatsGroupBox.Location = new System.Drawing.Point(274, 549);
            this.ErrorStatsGroupBox.Name = "ErrorStatsGroupBox";
            this.ErrorStatsGroupBox.Size = new System.Drawing.Size(307, 78);
            this.ErrorStatsGroupBox.TabIndex = 158;
            this.ErrorStatsGroupBox.TabStop = false;
            this.ErrorStatsGroupBox.Text = "Error Stats";
            // 
            // BadS
            // 
            this.BadS.Location = new System.Drawing.Point(269, 15);
            this.BadS.Name = "BadS";
            this.BadS.Size = new System.Drawing.Size(32, 13);
            this.BadS.TabIndex = 77;
            this.BadS.Text = "0";
            this.BadS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // BadSLabel
            // 
            this.BadSLabel.AutoSize = true;
            this.BadSLabel.Location = new System.Drawing.Point(235, 15);
            this.BadSLabel.Name = "BadSLabel";
            this.BadSLabel.Size = new System.Drawing.Size(32, 13);
            this.BadSLabel.TabIndex = 76;
            this.BadSLabel.Text = "GKE:";
            // 
            // FailsafeSLabel
            // 
            this.FailsafeSLabel.AutoSize = true;
            this.FailsafeSLabel.Location = new System.Drawing.Point(153, 35);
            this.FailsafeSLabel.Name = "FailsafeSLabel";
            this.FailsafeSLabel.Size = new System.Drawing.Size(38, 13);
            this.FailsafeSLabel.TabIndex = 75;
            this.FailsafeSLabel.Text = "FSafe:";
            // 
            // GyroSLabel
            // 
            this.GyroSLabel.AutoSize = true;
            this.GyroSLabel.Location = new System.Drawing.Point(75, 15);
            this.GyroSLabel.Name = "GyroSLabel";
            this.GyroSLabel.Size = new System.Drawing.Size(35, 13);
            this.GyroSLabel.TabIndex = 69;
            this.GyroSLabel.Text = "Gyro.:";
            // 
            // RCFailSafeS
            // 
            this.RCFailSafeS.Location = new System.Drawing.Point(194, 35);
            this.RCFailSafeS.Name = "RCFailSafeS";
            this.RCFailSafeS.Size = new System.Drawing.Size(35, 13);
            this.RCFailSafeS.TabIndex = 74;
            this.RCFailSafeS.Text = "0";
            this.RCFailSafeS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // I2CESCFailS
            // 
            this.I2CESCFailS.Location = new System.Drawing.Point(187, 14);
            this.I2CESCFailS.Name = "I2CESCFailS";
            this.I2CESCFailS.Size = new System.Drawing.Size(42, 14);
            this.I2CESCFailS.TabIndex = 65;
            this.I2CESCFailS.Text = "0";
            this.I2CESCFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // GPSInvalidSLabel
            // 
            this.GPSInvalidSLabel.AutoSize = true;
            this.GPSInvalidSLabel.Location = new System.Drawing.Point(6, 35);
            this.GPSInvalidSLabel.Name = "GPSInvalidSLabel";
            this.GPSInvalidSLabel.Size = new System.Drawing.Size(32, 13);
            this.GPSInvalidSLabel.TabIndex = 67;
            this.GPSInvalidSLabel.Text = "GPS:";
            // 
            // GPSFailS
            // 
            this.GPSFailS.Location = new System.Drawing.Point(37, 35);
            this.GPSFailS.Name = "GPSFailS";
            this.GPSFailS.Size = new System.Drawing.Size(32, 13);
            this.GPSFailS.TabIndex = 66;
            this.GPSFailS.Text = "0";
            this.GPSFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // ESCSLabel
            // 
            this.ESCSLabel.AutoSize = true;
            this.ESCSLabel.Location = new System.Drawing.Point(153, 15);
            this.ESCSLabel.Name = "ESCSLabel";
            this.ESCSLabel.Size = new System.Drawing.Size(31, 13);
            this.ESCSLabel.TabIndex = 52;
            this.ESCSLabel.Text = "ESC:";
            // 
            // GyroFailS
            // 
            this.GyroFailS.Location = new System.Drawing.Point(112, 16);
            this.GyroFailS.Name = "GyroFailS";
            this.GyroFailS.Size = new System.Drawing.Size(35, 13);
            this.GyroFailS.TabIndex = 68;
            this.GyroFailS.Text = "0";
            this.GyroFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // AccFailLabel
            // 
            this.AccFailLabel.AutoSize = true;
            this.AccFailLabel.Location = new System.Drawing.Point(6, 55);
            this.AccFailLabel.Name = "AccFailLabel";
            this.AccFailLabel.Size = new System.Drawing.Size(32, 13);
            this.AccFailLabel.TabIndex = 60;
            this.AccFailLabel.Text = "Acc.:";
            // 
            // AccFailS
            // 
            this.AccFailS.Location = new System.Drawing.Point(37, 55);
            this.AccFailS.Name = "AccFailS";
            this.AccFailS.Size = new System.Drawing.Size(32, 13);
            this.AccFailS.TabIndex = 51;
            this.AccFailS.Text = "0";
            this.AccFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // I2CFailLabel
            // 
            this.I2CFailLabel.AutoSize = true;
            this.I2CFailLabel.Location = new System.Drawing.Point(6, 15);
            this.I2CFailLabel.Name = "I2CFailLabel";
            this.I2CFailLabel.Size = new System.Drawing.Size(26, 13);
            this.I2CFailLabel.TabIndex = 71;
            this.I2CFailLabel.Text = "I2C:";
            // 
            // CompassFailLabel
            // 
            this.CompassFailLabel.AutoSize = true;
            this.CompassFailLabel.Location = new System.Drawing.Point(75, 36);
            this.CompassFailLabel.Name = "CompassFailLabel";
            this.CompassFailLabel.Size = new System.Drawing.Size(40, 13);
            this.CompassFailLabel.TabIndex = 60;
            this.CompassFailLabel.Text = "Comp.:";
            // 
            // I2CFailS
            // 
            this.I2CFailS.Location = new System.Drawing.Point(40, 15);
            this.I2CFailS.Name = "I2CFailS";
            this.I2CFailS.Size = new System.Drawing.Size(29, 13);
            this.I2CFailS.TabIndex = 70;
            this.I2CFailS.Text = "0";
            this.I2CFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // CompassFailS
            // 
            this.CompassFailS.BackColor = System.Drawing.SystemColors.Control;
            this.CompassFailS.Location = new System.Drawing.Point(121, 36);
            this.CompassFailS.Name = "CompassFailS";
            this.CompassFailS.Size = new System.Drawing.Size(26, 13);
            this.CompassFailS.TabIndex = 59;
            this.CompassFailS.Text = "0";
            this.CompassFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // BaroFailLabel
            // 
            this.BaroFailLabel.AutoSize = true;
            this.BaroFailLabel.Location = new System.Drawing.Point(75, 55);
            this.BaroFailLabel.Name = "BaroFailLabel";
            this.BaroFailLabel.Size = new System.Drawing.Size(35, 13);
            this.BaroFailLabel.TabIndex = 58;
            this.BaroFailLabel.Text = "Baro.:";
            // 
            // BaroFailS
            // 
            this.BaroFailS.Location = new System.Drawing.Point(118, 56);
            this.BaroFailS.Name = "BaroFailS";
            this.BaroFailS.Size = new System.Drawing.Size(29, 13);
            this.BaroFailS.TabIndex = 55;
            this.BaroFailS.Text = "0";
            this.BaroFailS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // GPSStatsGroupBox
            // 
            this.GPSStatsGroupBox.Controls.Add(this.label9);
            this.GPSStatsGroupBox.Controls.Add(this.GPSMaxHDiluteS);
            this.GPSStatsGroupBox.Controls.Add(this.GPSMinHDiluteS);
            this.GPSStatsGroupBox.Controls.Add(this.GPSAltitudeLabel);
            this.GPSStatsGroupBox.Controls.Add(this.label10);
            this.GPSStatsGroupBox.Controls.Add(this.GPSMaxVelS);
            this.GPSStatsGroupBox.Controls.Add(this.GPSAltitudeS);
            this.GPSStatsGroupBox.Controls.Add(this.SatsLabel);
            this.GPSStatsGroupBox.Controls.Add(this.GPSMinSatS);
            this.GPSStatsGroupBox.Controls.Add(this.GPSMaxSatS);
            this.GPSStatsGroupBox.Location = new System.Drawing.Point(12, 548);
            this.GPSStatsGroupBox.Name = "GPSStatsGroupBox";
            this.GPSStatsGroupBox.Size = new System.Drawing.Size(129, 79);
            this.GPSStatsGroupBox.TabIndex = 157;
            this.GPSStatsGroupBox.TabStop = false;
            this.GPSStatsGroupBox.Text = "GPS Stats";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 57);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(45, 13);
            this.label9.TabIndex = 63;
            this.label9.Text = "HDilute:";
            // 
            // GPSMaxHDiluteS
            // 
            this.GPSMaxHDiluteS.Location = new System.Drawing.Point(86, 57);
            this.GPSMaxHDiluteS.Name = "GPSMaxHDiluteS";
            this.GPSMaxHDiluteS.Size = new System.Drawing.Size(37, 15);
            this.GPSMaxHDiluteS.TabIndex = 61;
            this.GPSMaxHDiluteS.Text = "0";
            this.GPSMaxHDiluteS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // GPSMinHDiluteS
            // 
            this.GPSMinHDiluteS.Location = new System.Drawing.Point(51, 57);
            this.GPSMinHDiluteS.Name = "GPSMinHDiluteS";
            this.GPSMinHDiluteS.Size = new System.Drawing.Size(33, 14);
            this.GPSMinHDiluteS.TabIndex = 62;
            this.GPSMinHDiluteS.Text = "0";
            this.GPSMinHDiluteS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // GPSAltitudeLabel
            // 
            this.GPSAltitudeLabel.AutoSize = true;
            this.GPSAltitudeLabel.Location = new System.Drawing.Point(6, 16);
            this.GPSAltitudeLabel.Name = "GPSAltitudeLabel";
            this.GPSAltitudeLabel.Size = new System.Drawing.Size(22, 13);
            this.GPSAltitudeLabel.TabIndex = 50;
            this.GPSAltitudeLabel.Text = "Alt:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(65, 16);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(28, 13);
            this.label10.TabIndex = 49;
            this.label10.Text = "Vel.:";
            // 
            // GPSMaxVelS
            // 
            this.GPSMaxVelS.Location = new System.Drawing.Point(89, 16);
            this.GPSMaxVelS.Name = "GPSMaxVelS";
            this.GPSMaxVelS.Size = new System.Drawing.Size(35, 13);
            this.GPSMaxVelS.TabIndex = 48;
            this.GPSMaxVelS.Text = "0";
            this.GPSMaxVelS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // GPSAltitudeS
            // 
            this.GPSAltitudeS.Location = new System.Drawing.Point(30, 16);
            this.GPSAltitudeS.Name = "GPSAltitudeS";
            this.GPSAltitudeS.Size = new System.Drawing.Size(29, 13);
            this.GPSAltitudeS.TabIndex = 47;
            this.GPSAltitudeS.Text = "0";
            this.GPSAltitudeS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // SatsLabel
            // 
            this.SatsLabel.AutoSize = true;
            this.SatsLabel.Location = new System.Drawing.Point(6, 37);
            this.SatsLabel.Name = "SatsLabel";
            this.SatsLabel.Size = new System.Drawing.Size(34, 13);
            this.SatsLabel.TabIndex = 57;
            this.SatsLabel.Text = "Sats.:";
            // 
            // GPSMinSatS
            // 
            this.GPSMinSatS.Location = new System.Drawing.Point(51, 37);
            this.GPSMinSatS.Name = "GPSMinSatS";
            this.GPSMinSatS.Size = new System.Drawing.Size(33, 13);
            this.GPSMinSatS.TabIndex = 56;
            this.GPSMinSatS.Text = "0";
            this.GPSMinSatS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // GPSMaxSatS
            // 
            this.GPSMaxSatS.Location = new System.Drawing.Point(86, 36);
            this.GPSMaxSatS.Name = "GPSMaxSatS";
            this.GPSMaxSatS.Size = new System.Drawing.Size(38, 14);
            this.GPSMaxSatS.TabIndex = 55;
            this.GPSMaxSatS.Text = "0";
            this.GPSMaxSatS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // OpenLogFileDialog
            // 
            this.OpenLogFileDialog.FileName = "UAVX.log";
            // 
            // COMSelectComboBox
            // 
            this.COMSelectComboBox.Name = "COMSelectComboBox";
            this.COMSelectComboBox.Size = new System.Drawing.Size(75, 21);
            this.COMSelectComboBox.SelectedIndexChanged += new System.EventHandler(this.COMSelectComboBox_SelectedIndexChanged);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.COMSelectComboBox,
            this.COMBaudRateComboBox,
            this.toolsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(848, 25);
            this.menuStrip1.TabIndex = 153;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // COMBaudRateComboBox
            // 
            this.COMBaudRateComboBox.Items.AddRange(new object[] {
            "9600",
            "19200",
            "38400",
            "57600",
            "115200",
            "128000"});
            this.COMBaudRateComboBox.Name = "COMBaudRateComboBox";
            this.COMBaudRateComboBox.Size = new System.Drawing.Size(75, 21);
            this.COMBaudRateComboBox.Text = "115200";
            this.COMBaudRateComboBox.SelectedIndexChanged += new System.EventHandler(this.COMBaudRateComboBox_SelectedIndexChanged);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.testsoftwareToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(44, 21);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // testsoftwareToolStripMenuItem
            // 
            this.testsoftwareToolStripMenuItem.Name = "testsoftwareToolStripMenuItem";
            this.testsoftwareToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.testsoftwareToolStripMenuItem.Text = "Parameters";
            // 
            // ReplayButton
            // 
            this.ReplayButton.Location = new System.Drawing.Point(288, 0);
            this.ReplayButton.Name = "ReplayButton";
            this.ReplayButton.Size = new System.Drawing.Size(75, 23);
            this.ReplayButton.TabIndex = 164;
            this.ReplayButton.Text = "Replay";
            this.ReplayButton.UseVisualStyleBackColor = true;
            this.ReplayButton.Click += new System.EventHandler(this.ReplayButton_Click);
            // 
            // ReplayProgressBar
            // 
            this.ReplayProgressBar.Location = new System.Drawing.Point(381, 0);
            this.ReplayProgressBar.Name = "ReplayProgressBar";
            this.ReplayProgressBar.Size = new System.Drawing.Size(82, 23);
            this.ReplayProgressBar.TabIndex = 165;
            // 
            // Airframe
            // 
            this.Airframe.BackColor = System.Drawing.SystemColors.Control;
            this.Airframe.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.Airframe.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Airframe.Location = new System.Drawing.Point(596, 82);
            this.Airframe.Name = "Airframe";
            this.Airframe.Size = new System.Drawing.Size(100, 13);
            this.Airframe.TabIndex = 166;
            this.Airframe.Text = "Quadrocopter";
            // 
            // ReplayNumericUpDown
            // 
            this.ReplayNumericUpDown.Location = new System.Drawing.Point(477, 3);
            this.ReplayNumericUpDown.Maximum = new decimal(new int[] {
            20,
            0,
            0,
            0});
            this.ReplayNumericUpDown.Name = "ReplayNumericUpDown";
            this.ReplayNumericUpDown.Size = new System.Drawing.Size(41, 20);
            this.ReplayNumericUpDown.TabIndex = 167;
            this.ReplayNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ReplayNumericUpDown.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.ReplayNumericUpDown.ValueChanged += new System.EventHandler(this.ReplayNumericUpDown_Changed);
            // 
            // NavCompBox
            // 
            this.NavCompBox.Controls.Add(this.NavSensitivity);
            this.NavCompBox.Controls.Add(this.NavSensitivityLabel);
            this.NavCompBox.Controls.Add(this.label5);
            this.NavCompBox.Controls.Add(this.NavYCorr);
            this.NavCompBox.Controls.Add(this.NavRCorr);
            this.NavCompBox.Controls.Add(this.NavPCorr);
            this.NavCompBox.Controls.Add(this.label6);
            this.NavCompBox.Controls.Add(this.label7);
            this.NavCompBox.Controls.Add(this.label8);
            this.NavCompBox.Location = new System.Drawing.Point(12, 381);
            this.NavCompBox.Name = "NavCompBox";
            this.NavCompBox.Size = new System.Drawing.Size(160, 57);
            this.NavCompBox.TabIndex = 104;
            this.NavCompBox.TabStop = false;
            this.NavCompBox.Text = "Nav (%)";
            // 
            // NavSensitivity
            // 
            this.NavSensitivity.Location = new System.Drawing.Point(6, 29);
            this.NavSensitivity.Name = "NavSensitivity";
            this.NavSensitivity.ReadOnly = true;
            this.NavSensitivity.Size = new System.Drawing.Size(34, 20);
            this.NavSensitivity.TabIndex = 59;
            this.NavSensitivity.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // NavSensitivityLabel
            // 
            this.NavSensitivityLabel.AutoSize = true;
            this.NavSensitivityLabel.Location = new System.Drawing.Point(9, 13);
            this.NavSensitivityLabel.Name = "NavSensitivityLabel";
            this.NavSensitivityLabel.Size = new System.Drawing.Size(34, 13);
            this.NavSensitivityLabel.TabIndex = 65;
            this.NavSensitivityLabel.Text = "Sens.";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(9, 29);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(0, 13);
            this.label5.TabIndex = 62;
            // 
            // NavYCorr
            // 
            this.NavYCorr.Location = new System.Drawing.Point(124, 29);
            this.NavYCorr.Name = "NavYCorr";
            this.NavYCorr.ReadOnly = true;
            this.NavYCorr.Size = new System.Drawing.Size(30, 20);
            this.NavYCorr.TabIndex = 43;
            this.NavYCorr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // NavRCorr
            // 
            this.NavRCorr.Location = new System.Drawing.Point(47, 29);
            this.NavRCorr.Name = "NavRCorr";
            this.NavRCorr.ReadOnly = true;
            this.NavRCorr.Size = new System.Drawing.Size(34, 20);
            this.NavRCorr.TabIndex = 58;
            this.NavRCorr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // NavPCorr
            // 
            this.NavPCorr.Location = new System.Drawing.Point(87, 29);
            this.NavPCorr.Name = "NavPCorr";
            this.NavPCorr.ReadOnly = true;
            this.NavPCorr.Size = new System.Drawing.Size(31, 20);
            this.NavPCorr.TabIndex = 46;
            this.NavPCorr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(51, 13);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(25, 13);
            this.label6.TabIndex = 74;
            this.label6.Text = "Roll";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(87, 13);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(31, 13);
            this.label7.TabIndex = 75;
            this.label7.Text = "Pitch";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(125, 13);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(28, 13);
            this.label8.TabIndex = 67;
            this.label8.Text = "Yaw";
            // 
            // FrSkycheckBox1
            // 
            this.FrSkycheckBox1.AutoSize = true;
            this.FrSkycheckBox1.Location = new System.Drawing.Point(6, 31);
            this.FrSkycheckBox1.Name = "FrSkycheckBox1";
            this.FrSkycheckBox1.Size = new System.Drawing.Size(15, 14);
            this.FrSkycheckBox1.TabIndex = 168;
            this.FrSkycheckBox1.UseVisualStyleBackColor = true;
            // 
            // FrSkyBox
            // 
            this.FrSkyBox.Controls.Add(this.FrSkyErrLabel);
            this.FrSkyBox.Controls.Add(this.RxFrSkyErr);
            this.FrSkyBox.Controls.Add(this.FrSkycheckBox1);
            this.FrSkyBox.Controls.Add(this.FrSkyLQLabel);
            this.FrSkyBox.Controls.Add(this.FrSkyA2Label);
            this.FrSkyBox.Controls.Add(this.FrSkyA1Label);
            this.FrSkyBox.Controls.Add(this.FrSkyLQ);
            this.FrSkyBox.Controls.Add(this.FrSkyA2);
            this.FrSkyBox.Controls.Add(this.FrSkyA1);
            this.FrSkyBox.Location = new System.Drawing.Point(587, 533);
            this.FrSkyBox.Name = "FrSkyBox";
            this.FrSkyBox.Size = new System.Drawing.Size(174, 56);
            this.FrSkyBox.TabIndex = 169;
            this.FrSkyBox.TabStop = false;
            this.FrSkyBox.Text = "FrSky";
            // 
            // FrSkyErrLabel
            // 
            this.FrSkyErrLabel.AutoSize = true;
            this.FrSkyErrLabel.Location = new System.Drawing.Point(140, 12);
            this.FrSkyErrLabel.Name = "FrSkyErrLabel";
            this.FrSkyErrLabel.Size = new System.Drawing.Size(23, 13);
            this.FrSkyErrLabel.TabIndex = 75;
            this.FrSkyErrLabel.Text = "Err.";
            // 
            // RxFrSkyErr
            // 
            this.RxFrSkyErr.Location = new System.Drawing.Point(140, 28);
            this.RxFrSkyErr.Name = "RxFrSkyErr";
            this.RxFrSkyErr.ReadOnly = true;
            this.RxFrSkyErr.Size = new System.Drawing.Size(28, 20);
            this.RxFrSkyErr.TabIndex = 74;
            this.RxFrSkyErr.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FrSkyLQLabel
            // 
            this.FrSkyLQLabel.AutoSize = true;
            this.FrSkyLQLabel.Location = new System.Drawing.Point(23, 12);
            this.FrSkyLQLabel.Name = "FrSkyLQLabel";
            this.FrSkyLQLabel.Size = new System.Drawing.Size(39, 13);
            this.FrSkyLQLabel.TabIndex = 73;
            this.FrSkyLQLabel.Text = "LQ(db)";
            // 
            // FrSkyA2Label
            // 
            this.FrSkyA2Label.AutoSize = true;
            this.FrSkyA2Label.Location = new System.Drawing.Point(102, 12);
            this.FrSkyA2Label.Name = "FrSkyA2Label";
            this.FrSkyA2Label.Size = new System.Drawing.Size(33, 13);
            this.FrSkyA2Label.TabIndex = 71;
            this.FrSkyA2Label.Text = "A2(V)";
            // 
            // FrSkyA1Label
            // 
            this.FrSkyA1Label.AutoSize = true;
            this.FrSkyA1Label.Location = new System.Drawing.Point(63, 12);
            this.FrSkyA1Label.Name = "FrSkyA1Label";
            this.FrSkyA1Label.Size = new System.Drawing.Size(33, 13);
            this.FrSkyA1Label.TabIndex = 68;
            this.FrSkyA1Label.Text = "A1(V)";
            // 
            // FrSkyLQ
            // 
            this.FrSkyLQ.Location = new System.Drawing.Point(26, 28);
            this.FrSkyLQ.Name = "FrSkyLQ";
            this.FrSkyLQ.ReadOnly = true;
            this.FrSkyLQ.Size = new System.Drawing.Size(32, 20);
            this.FrSkyLQ.TabIndex = 46;
            this.FrSkyLQ.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FrSkyA2
            // 
            this.FrSkyA2.Location = new System.Drawing.Point(102, 28);
            this.FrSkyA2.Name = "FrSkyA2";
            this.FrSkyA2.ReadOnly = true;
            this.FrSkyA2.Size = new System.Drawing.Size(32, 20);
            this.FrSkyA2.TabIndex = 45;
            this.FrSkyA2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // FrSkyA1
            // 
            this.FrSkyA1.Location = new System.Drawing.Point(64, 28);
            this.FrSkyA1.Name = "FrSkyA1";
            this.FrSkyA1.ReadOnly = true;
            this.FrSkyA1.Size = new System.Drawing.Size(32, 20);
            this.FrSkyA1.TabIndex = 44;
            this.FrSkyA1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Output7Label);
            this.groupBox3.Controls.Add(this.OutputT6);
            this.groupBox3.Controls.Add(this.OutputT7);
            this.groupBox3.Controls.Add(this.Output6Label);
            this.groupBox3.Location = new System.Drawing.Point(767, 533);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(69, 56);
            this.groupBox3.TabIndex = 170;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Camera";
            // 
            // headingIndicatorInstrumentControl1
            // 
            this.headingIndicatorInstrumentControl1.Location = new System.Drawing.Point(369, 368);
            this.headingIndicatorInstrumentControl1.Name = "headingIndicatorInstrumentControl1";
            this.headingIndicatorInstrumentControl1.Size = new System.Drawing.Size(162, 162);
            this.headingIndicatorInstrumentControl1.TabIndex = 4;
            this.headingIndicatorInstrumentControl1.Text = "headingIndicatorInstrumentControl1";
            // 
            // attitudeIndicatorInstrumentControl1
            // 
            this.attitudeIndicatorInstrumentControl1.Location = new System.Drawing.Point(323, 108);
            this.attitudeIndicatorInstrumentControl1.Name = "attitudeIndicatorInstrumentControl1";
            this.attitudeIndicatorInstrumentControl1.Size = new System.Drawing.Size(258, 259);
            this.attitudeIndicatorInstrumentControl1.TabIndex = 3;
            this.attitudeIndicatorInstrumentControl1.Text = "attitudeIndicatorInstrumentControl1";
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(848, 633);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.FrSkyBox);
            this.Controls.Add(this.NavCompBox);
            this.Controls.Add(this.ReplayNumericUpDown);
            this.Controls.Add(this.Airframe);
            this.Controls.Add(this.ReplayProgressBar);
            this.Controls.Add(this.ReplayButton);
            this.Controls.Add(this.MissionTimeSec);
            this.Controls.Add(this.MissionTimeSecLabel);
            this.Controls.Add(this.FlightState);
            this.Controls.Add(this.FlightStateLabel);
            this.Controls.Add(this.FailStateLabel);
            this.Controls.Add(this.BaroStatsGroupBox);
            this.Controls.Add(this.ErrorStatsGroupBox);
            this.Controls.Add(this.LocationBox);
            this.Controls.Add(this.GPSStatsGroupBox);
            this.Controls.Add(this.OutputGroupBox);
            this.Controls.Add(this.SimulationTextBox);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.MessageTextBox);
            this.Controls.Add(this.GoogleButton);
            this.Controls.Add(this.AltitudeSource);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.CurrentAltitude);
            this.Controls.Add(this.AltitudeGroupBox);
            this.Controls.Add(this.GPSStatBox);
            this.Controls.Add(this.FlagsGroupBox);
            this.Controls.Add(this.FailState);
            this.Controls.Add(this.BatteryGroupBox);
            this.Controls.Add(this.EnvGroupBox);
            this.Controls.Add(this.AccelerationsGroupBox);
            this.Controls.Add(this.AttitudeGroupBox);
            this.Controls.Add(this.ControlsGroupBox);
            this.Controls.Add(this.GyroGroupBox);
            this.Controls.Add(this.CommsGroupBox);
            this.Controls.Add(this.NavGroupBox);
            this.Controls.Add(this.headingIndicatorInstrumentControl1);
            this.Controls.Add(this.attitudeIndicatorInstrumentControl1);
            this.Controls.Add(this.FlyingButton);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "FormMain";
            this.Text = "UAVXGS";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.GyroGroupBox.ResumeLayout(false);
            this.GyroGroupBox.PerformLayout();
            this.ControlsGroupBox.ResumeLayout(false);
            this.ControlsGroupBox.PerformLayout();
            this.AttitudeGroupBox.ResumeLayout(false);
            this.AttitudeGroupBox.PerformLayout();
            this.AccelerationsGroupBox.ResumeLayout(false);
            this.AccelerationsGroupBox.PerformLayout();
            this.CommsGroupBox.ResumeLayout(false);
            this.CommsGroupBox.PerformLayout();
            this.EnvGroupBox.ResumeLayout(false);
            this.EnvGroupBox.PerformLayout();
            this.BatteryGroupBox.ResumeLayout(false);
            this.BatteryGroupBox.PerformLayout();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.FlagsGroupBox.ResumeLayout(false);
            this.GPSStatBox.ResumeLayout(false);
            this.GPSStatBox.PerformLayout();
            this.AltitudeGroupBox.ResumeLayout(false);
            this.AltitudeGroupBox.PerformLayout();
            this.NavGroupBox.ResumeLayout(false);
            this.NavGroupBox.PerformLayout();
            this.LocationBox.ResumeLayout(false);
            this.LocationBox.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.OutputGroupBox.ResumeLayout(false);
            this.OutputGroupBox.PerformLayout();
            this.BaroStatsGroupBox.ResumeLayout(false);
            this.BaroStatsGroupBox.PerformLayout();
            this.ErrorStatsGroupBox.ResumeLayout(false);
            this.ErrorStatsGroupBox.PerformLayout();
            this.GPSStatsGroupBox.ResumeLayout(false);
            this.GPSStatsGroupBox.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ReplayNumericUpDown)).EndInit();
            this.NavCompBox.ResumeLayout(false);
            this.NavCompBox.PerformLayout();
            this.FrSkyBox.ResumeLayout(false);
            this.FrSkyBox.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button FlyingButton;
        private Instruments.AttitudeIndicatorInstrumentControl attitudeIndicatorInstrumentControl1;
        private Instruments.HeadingIndicatorInstrumentControl headingIndicatorInstrumentControl1;
        private System.Windows.Forms.TextBox RollAngle;
        private System.Windows.Forms.TextBox PitchAngle;
        private System.Windows.Forms.TextBox BatteryVolts;
        private System.Windows.Forms.TextBox BatteryCurrent;
        private System.Windows.Forms.TextBox RCGlitches;
        private System.Windows.Forms.TextBox DesiredThrottle;
        private System.Windows.Forms.TextBox DesiredRoll;
        private System.Windows.Forms.TextBox DesiredPitch;
        private System.Windows.Forms.TextBox DesiredYaw;
        private System.Windows.Forms.TextBox YawRate;
        private System.Windows.Forms.TextBox PitchRate;
        private System.Windows.Forms.TextBox RollRate;
        private System.Windows.Forms.TextBox LRAcc;
        private System.Windows.Forms.TextBox YawAngle;
        private System.Windows.Forms.TextBox DUAcc;
        private System.Windows.Forms.TextBox FBAcc;
        private System.Windows.Forms.Label VoltsLabel;
        private System.Windows.Forms.Label LRAccLabel;
        private System.Windows.Forms.Label FBAccLabel;
        private System.Windows.Forms.Label DUAccLabel;
        private System.Windows.Forms.Label DesiredYawLabel;
        private System.Windows.Forms.Label DesiredPitchLabel;
        private System.Windows.Forms.Label DesiredRollLabel;
        private System.Windows.Forms.Label DesiredThrottleLabel;
        private System.Windows.Forms.Label RCGlitchesLabel;
        private System.Windows.Forms.Label CurrentLabel;
        private System.Windows.Forms.TextBox GPSHDilute;
        private System.Windows.Forms.TextBox RangefinderAltitude;
        private System.Windows.Forms.TextBox RelBaroAltitude;
        private System.Windows.Forms.TextBox GPSFix;
        private System.Windows.Forms.TextBox BaroROC;
        private System.Windows.Forms.TextBox GPSNoOfSats;
        private System.Windows.Forms.TextBox FailState;
        private System.Windows.Forms.TextBox GPSRelAltitude;
        private System.Windows.Forms.Label RelAltitudeLabel;
        private System.Windows.Forms.Label ROCLabel;
        private System.Windows.Forms.Label HDiluteLabel;
        private System.Windows.Forms.Label RangefinderAltLabel;
        private System.Windows.Forms.Label GPSFixLabel;
        private System.Windows.Forms.Label GPSNoOfSatsLabel;
        private System.Windows.Forms.Label FailStateLabel;
        private System.Windows.Forms.Label GPSAltLabel;
        private System.Windows.Forms.Label BaroAltitudeLabel;
        private System.Windows.Forms.Label YawRateLabel;
        private System.Windows.Forms.Label PitchRateLabel;
        private System.Windows.Forms.Label RollRateLabel;
        private System.Windows.Forms.TextBox RxTypeErr;
        private System.Windows.Forms.TextBox RxCSumErr;
        private System.Windows.Forms.TextBox RxLenErr;
        private System.Windows.Forms.Label RxTypeLabel;
        private System.Windows.Forms.Label RxCSumLabel;
        private System.Windows.Forms.Label RxLenLabel;
        private System.Windows.Forms.TextBox FlightState;
        private System.Windows.Forms.Label FlightStateLabel;
        private System.Windows.Forms.GroupBox GyroGroupBox;
        private System.Windows.Forms.GroupBox ControlsGroupBox;
        private System.Windows.Forms.GroupBox AttitudeGroupBox;
        private System.Windows.Forms.GroupBox AccelerationsGroupBox;
        private System.Windows.Forms.GroupBox CommsGroupBox;
        private System.Windows.Forms.GroupBox EnvGroupBox;
        private System.Windows.Forms.GroupBox BatteryGroupBox;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.TextBox AltHoldBox;
        private System.Windows.Forms.TextBox TurnToWPBox;
        private System.Windows.Forms.TextBox GyroFailureBox;
        private System.Windows.Forms.TextBox LostModelBox;
        private System.Windows.Forms.TextBox NearLevelBox;
        private System.Windows.Forms.TextBox LowBatteryBox;
        private System.Windows.Forms.TextBox GPSValidBox;
        private System.Windows.Forms.TextBox NavValidBox;
        private System.Windows.Forms.TextBox BaroFailBox;
        private System.Windows.Forms.TextBox AccFailBox;
        private System.Windows.Forms.TextBox CompassFailBox;
        private System.Windows.Forms.TextBox GPSFailBox;
        private System.Windows.Forms.TextBox AttitudeHoldBox;
        private System.Windows.Forms.TextBox ThrottleMovingBox;
        private System.Windows.Forms.TextBox HoldingAltBox;
        private System.Windows.Forms.TextBox NavigateBox;
        private System.Windows.Forms.TextBox ReturnHomeBox;
        private System.Windows.Forms.TextBox ProximityBox;
        private System.Windows.Forms.TextBox CloseProximityBox;
        private System.Windows.Forms.TextBox UseRTHAutoDescendBox;
        private System.Windows.Forms.TextBox BaroAltValidBox;
        private System.Windows.Forms.TextBox RangefinderAltValidBox;
        private System.Windows.Forms.TextBox UsingRangefinderBox;
        private System.Windows.Forms.GroupBox FlagsGroupBox;
        private System.Windows.Forms.GroupBox GPSStatBox;
        private System.Windows.Forms.GroupBox AltitudeGroupBox;
        private System.Windows.Forms.Label WayHeadingLabel;
        private System.Windows.Forms.Label GPSLongitudeLabel;
        private System.Windows.Forms.Label GPSLatitudeLabel;
        private System.Windows.Forms.Label NavStateLabel;
        private System.Windows.Forms.Label GPSVelLabel;
        private System.Windows.Forms.Label CurrWPLabel;
        private System.Windows.Forms.TextBox GPSLatitude;
        private System.Windows.Forms.TextBox GPSLongitude;
        private System.Windows.Forms.TextBox NavState;
        private System.Windows.Forms.TextBox CurrWP;
        private System.Windows.Forms.TextBox WayHeading;
        private System.Windows.Forms.Label WPAltitude;
        private System.Windows.Forms.TextBox GPSVel;
        private System.Windows.Forms.Label HeadingLabel;
        private System.Windows.Forms.TextBox AltitudeError;
        private System.Windows.Forms.TextBox Heading;
        private System.Windows.Forms.Label WPDistanceLabel;
        private System.Windows.Forms.TextBox DistanceToDesired;
        private System.Windows.Forms.GroupBox NavGroupBox;
        private System.Windows.Forms.TextBox CurrentAltitude;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox AltitudeSource;
        private System.Windows.Forms.Button GoogleButton;
        private System.Windows.Forms.TextBox MessageTextBox;
        private System.Windows.Forms.GroupBox LocationBox;
        private System.Windows.Forms.Label WhereBearingLabel;
        private System.Windows.Forms.Label WhereDistanceLabel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox LRComp;
        private System.Windows.Forms.Label LRCompLabel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox AltComp;
        private System.Windows.Forms.TextBox FBComp;
        private System.Windows.Forms.TextBox DUComp;
        private System.Windows.Forms.Label FBCompLabel;
        private System.Windows.Forms.Label DUCompLabel;
        private System.Windows.Forms.Label AltCompLabel;
        private System.Windows.Forms.Label NavStateTimeoutLabel;
        private System.Windows.Forms.TextBox NavStateTimeout;
        private System.Windows.Forms.TextBox BatteryCharge;
        private System.Windows.Forms.Label BatteryChargeLabel;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.TextBox SimulationTextBox;
        private System.Windows.Forms.GroupBox OutputGroupBox;
        private System.Windows.Forms.TextBox OutputT7;
        private System.Windows.Forms.TextBox OutputT6;
        private System.Windows.Forms.TextBox OutputT3;
        private System.Windows.Forms.TextBox OutputT2;
        private System.Windows.Forms.TextBox OutputT1;
        private System.Windows.Forms.TextBox OutputT0;
        private System.Windows.Forms.GroupBox BaroStatsGroupBox;
        private System.Windows.Forms.Label TempSLabel;
        private System.Windows.Forms.Label MinTempS;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label BaroMaxROCS;
        private System.Windows.Forms.Label RelBaroAltitudeLabel;
        private System.Windows.Forms.Label BaroRelAltitudeS;
        private System.Windows.Forms.Label BaroMinROCS;
        private System.Windows.Forms.GroupBox ErrorStatsGroupBox;
        private System.Windows.Forms.Label GyroSLabel;
        private System.Windows.Forms.Label GyroFailS;
        private System.Windows.Forms.Label GPSInvalidSLabel;
        private System.Windows.Forms.Label GPSFailS;
        private System.Windows.Forms.Label I2CESCFailS;
        private System.Windows.Forms.Label CompassFailLabel;
        private System.Windows.Forms.Label AccFailLabel;
        private System.Windows.Forms.Label CompassFailS;
        private System.Windows.Forms.Label BaroFailLabel;
        private System.Windows.Forms.Label BaroFailS;
        private System.Windows.Forms.Label ESCSLabel;
        private System.Windows.Forms.Label AccFailS;
        private System.Windows.Forms.GroupBox GPSStatsGroupBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label GPSMaxHDiluteS;
        private System.Windows.Forms.Label GPSMinHDiluteS;
        private System.Windows.Forms.Label GPSAltitudeLabel;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label GPSMaxVelS;
        private System.Windows.Forms.Label GPSAltitudeS;
        private System.Windows.Forms.Label SatsLabel;
        private System.Windows.Forms.Label GPSMinSatS;
        private System.Windows.Forms.Label GPSMaxSatS;
        private System.Windows.Forms.Label FailsafeSLabel;
        private System.Windows.Forms.Label RCFailSafeS;
        private System.Windows.Forms.Label I2CFailLabel;
        private System.Windows.Forms.Label I2CFailS;
        private System.Windows.Forms.Label MaxTempS;
        private System.Windows.Forms.Label BadS;
        private System.Windows.Forms.Label BadSLabel;
        private System.Windows.Forms.OpenFileDialog OpenLogFileDialog;
        private System.Windows.Forms.ToolStripComboBox COMSelectComboBox;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripComboBox COMBaudRateComboBox;
        private System.Windows.Forms.TextBox MissionTimeSec;
        private System.Windows.Forms.TextBox AmbientTemp;
        private System.Windows.Forms.Label AmbientTempLabel;
        private System.Windows.Forms.Label MissionTimeSecLabel;
        private System.Windows.Forms.TextBox WhereDistance;
        private System.Windows.Forms.TextBox WhereBearing;
        private System.Windows.Forms.Button ReplayButton;
        public System.Windows.Forms.ProgressBar ReplayProgressBar;
        private System.Windows.Forms.Label YawLabel;
        private System.Windows.Forms.Label PitchLabel;
        private System.Windows.Forms.Label RollLabel;
        private System.Windows.Forms.TextBox PHLockEnBox;
        private System.Windows.Forms.TextBox FocusLockedBox;
        private System.Windows.Forms.TextBox Airframe;
        private System.Windows.Forms.NumericUpDown ReplayNumericUpDown;
        private System.Windows.Forms.TextBox SticksFrozenBox;
        private System.Windows.Forms.GroupBox NavCompBox;
        private System.Windows.Forms.TextBox NavSensitivity;
        private System.Windows.Forms.Label NavSensitivityLabel;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox NavYCorr;
        private System.Windows.Forms.TextBox NavRCorr;
        private System.Windows.Forms.TextBox NavPCorr;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label Output2Label;
        private System.Windows.Forms.Label Output3Label;
        private System.Windows.Forms.Label Output1Label;
        private System.Windows.Forms.Label Output7Label;
        private System.Windows.Forms.Label Output6Label;
        private System.Windows.Forms.Label Output0Label;
        private System.Windows.Forms.Label GPSHeadingLabel;
        private System.Windows.Forms.TextBox GPSHeading;
        private System.Windows.Forms.CheckBox FrSkycheckBox1;
        private System.Windows.Forms.GroupBox FrSkyBox;
        private System.Windows.Forms.Label FrSkyLQLabel;
        private System.Windows.Forms.Label FrSkyA2Label;
        private System.Windows.Forms.Label FrSkyA1Label;
        private System.Windows.Forms.TextBox FrSkyLQ;
        private System.Windows.Forms.TextBox FrSkyA2;
        private System.Windows.Forms.TextBox FrSkyA1;
        private System.Windows.Forms.Label Output4Label;
        private System.Windows.Forms.Label Output5Label;
        private System.Windows.Forms.TextBox OutputT5;
        private System.Windows.Forms.TextBox OutputT4;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label FrSkyErrLabel;
        private System.Windows.Forms.TextBox RxFrSkyErr;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem testsoftwareToolStripMenuItem;
        private System.Windows.Forms.TextBox OutputTot;
        private System.Windows.Forms.Label OutputTotalLabel;
    }
}

