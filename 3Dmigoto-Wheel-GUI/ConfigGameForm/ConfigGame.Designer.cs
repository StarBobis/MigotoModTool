namespace NMBT_GUI
{
    partial class ConfigGame
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
            this.checkBoxDeleteOutputFolder = new System.Windows.Forms.CheckBox();
            this.checkBoxBackUp = new System.Windows.Forms.CheckBox();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.checkBoxAutoCleanLog = new System.Windows.Forms.CheckBox();
            this.textBoxLogReserveNumber = new System.Windows.Forms.TextBox();
            this.checkBoxAutoCleanFrameAnalysisFolder = new System.Windows.Forms.CheckBox();
            this.textBoxFrameAnalysisFolderReserveNumber = new System.Windows.Forms.TextBox();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.SuspendLayout();
            // 
            // checkBoxDeleteOutputFolder
            // 
            this.checkBoxDeleteOutputFolder.AutoSize = true;
            this.checkBoxDeleteOutputFolder.Location = new System.Drawing.Point(12, 56);
            this.checkBoxDeleteOutputFolder.Name = "checkBoxDeleteOutputFolder";
            this.checkBoxDeleteOutputFolder.Size = new System.Drawing.Size(294, 16);
            this.checkBoxDeleteOutputFolder.TabIndex = 56;
            this.checkBoxDeleteOutputFolder.Text = "Auto delete OutputFolder before extract model";
            this.checkBoxDeleteOutputFolder.UseVisualStyleBackColor = true;
            // 
            // checkBoxBackUp
            // 
            this.checkBoxBackUp.AutoSize = true;
            this.checkBoxBackUp.Location = new System.Drawing.Point(12, 78);
            this.checkBoxBackUp.Name = "checkBoxBackUp";
            this.checkBoxBackUp.Size = new System.Drawing.Size(372, 16);
            this.checkBoxBackUp.TabIndex = 58;
            this.checkBoxBackUp.Text = "Back up OutputFolder to Backups folder before generate mod";
            this.checkBoxBackUp.UseVisualStyleBackColor = true;
            // 
            // checkBoxAutoCleanLog
            // 
            this.checkBoxAutoCleanLog.AutoSize = true;
            this.checkBoxAutoCleanLog.Location = new System.Drawing.Point(12, 12);
            this.checkBoxAutoCleanLog.Name = "checkBoxAutoCleanLog";
            this.checkBoxAutoCleanLog.Size = new System.Drawing.Size(414, 16);
            this.checkBoxAutoCleanLog.TabIndex = 78;
            this.checkBoxAutoCleanLog.Text = "Clean log file under Logs folder before exit MMT, reserve number:";
            this.checkBoxAutoCleanLog.UseVisualStyleBackColor = true;
            this.checkBoxAutoCleanLog.CheckedChanged += new System.EventHandler(this.checkBoxAutoCleanLog_CheckedChanged);
            // 
            // textBoxLogReserveNumber
            // 
            this.textBoxLogReserveNumber.Location = new System.Drawing.Point(432, 7);
            this.textBoxLogReserveNumber.Name = "textBoxLogReserveNumber";
            this.textBoxLogReserveNumber.Size = new System.Drawing.Size(43, 21);
            this.textBoxLogReserveNumber.TabIndex = 79;
            // 
            // checkBoxAutoCleanFrameAnalysisFolder
            // 
            this.checkBoxAutoCleanFrameAnalysisFolder.AutoSize = true;
            this.checkBoxAutoCleanFrameAnalysisFolder.Location = new System.Drawing.Point(12, 34);
            this.checkBoxAutoCleanFrameAnalysisFolder.Name = "checkBoxAutoCleanFrameAnalysisFolder";
            this.checkBoxAutoCleanFrameAnalysisFolder.Size = new System.Drawing.Size(498, 16);
            this.checkBoxAutoCleanFrameAnalysisFolder.TabIndex = 80;
            this.checkBoxAutoCleanFrameAnalysisFolder.Text = "Clean FrameAnalysis Folder under Loader Folder before exit MMT, reserve number:";
            this.checkBoxAutoCleanFrameAnalysisFolder.UseVisualStyleBackColor = true;
            this.checkBoxAutoCleanFrameAnalysisFolder.CheckedChanged += new System.EventHandler(this.checkBoxAutoCleanFrameAnalysisFolder_CheckedChanged);
            // 
            // textBoxFrameAnalysisFolderReserveNumber
            // 
            this.textBoxFrameAnalysisFolderReserveNumber.Location = new System.Drawing.Point(516, 29);
            this.textBoxFrameAnalysisFolderReserveNumber.Name = "textBoxFrameAnalysisFolderReserveNumber";
            this.textBoxFrameAnalysisFolderReserveNumber.Size = new System.Drawing.Size(43, 21);
            this.textBoxFrameAnalysisFolderReserveNumber.TabIndex = 81;
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // ConfigGame
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(571, 104);
            this.Controls.Add(this.textBoxFrameAnalysisFolderReserveNumber);
            this.Controls.Add(this.checkBoxAutoCleanFrameAnalysisFolder);
            this.Controls.Add(this.checkBoxAutoCleanLog);
            this.Controls.Add(this.textBoxLogReserveNumber);
            this.Controls.Add(this.checkBoxBackUp);
            this.Controls.Add(this.checkBoxDeleteOutputFolder);
            this.Name = "ConfigGame";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Preferences";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.ConfigGame_FormClosed);
            this.Load += new System.EventHandler(this.ConfigBasic_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox checkBoxDeleteOutputFolder;
        private System.Windows.Forms.CheckBox checkBoxBackUp;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.CheckBox checkBoxAutoCleanLog;
        private System.Windows.Forms.TextBox textBoxLogReserveNumber;
        private System.Windows.Forms.CheckBox checkBoxAutoCleanFrameAnalysisFolder;
        private System.Windows.Forms.TextBox textBoxFrameAnalysisFolderReserveNumber;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
    }
}