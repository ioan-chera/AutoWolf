using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Media;
using System.Diagnostics;

namespace AutoWolfStarter
{

    public partial class Form1 : Form
    {
        private ApplicationSettings applicationSettings;
        private string currentPath;

        public Form1()
        {
            InitializeComponent();
            currentPath = ".";
            applicationSettings = new ApplicationSettings();
        }

        private void updateListArray()
        {
            
            applicationSettings.WolfensteinLocations = locationListBox.Items.Cast<string>().ToArray();

        }

        
        private void addButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();

            folderBrowserDialog.Description = "Select Wolfenstein game folder:";
            folderBrowserDialog.SelectedPath = currentPath;
            folderBrowserDialog.ShowNewFolderButton = false;

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                currentPath = folderBrowserDialog.SelectedPath;
                locationListBox.Items.Add(currentPath);
                updateListArray();
                applicationSettings.FolderDialogPath = currentPath;
            }

        }

        private void removeButton_Click(object sender, EventArgs e)
        {
            int selind = locationListBox.SelectedIndex;
            if (selind != -1)
            {
                locationListBox.Items.RemoveAt(selind);
                updateListArray();
            }
            else
                SystemSounds.Beep.Play();
        }

        private void locationListBox_DragDrop(object sender, DragEventArgs e)
        {
            //addListEntry(e.Data.ToString());
        }

        
        private void Form1_Load(object sender, EventArgs e)
        {
            applicationSettings.FormLocation = this.Location;
            applicationSettings.FormSize = this.Size;
            
            ApplicationSettings aps = this.applicationSettings;
            
            if (aps.LoadAppSettings())
            {
                this.Location = aps.FormLocation;
                this.Size = aps.FormSize;
                commandLineArgumentsTextBox.Text = aps.CommandLineArguments;
                if (aps.WolfensteinLocations != null && aps.WolfensteinLocations.Length > 0)
                    locationListBox.Items.AddRange(aps.WolfensteinLocations);
                this.currentPath = aps.FolderDialogPath;
            }
            
        }

        private void commandLineArgumentsTextBox_TextChanged(object sender, EventArgs e)
        {
            applicationSettings.CommandLineArguments = commandLineArgumentsTextBox.Text;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            applicationSettings.SaveAppSettings();
        }

        private void Form1_FormDeactivating(object sender, EventArgs e)
        {
            applicationSettings.SaveAppSettings();
        }

        private void Form1_LocationChanged(object sender, EventArgs e)
        {
            applicationSettings.FormLocation = this.Location;
        }

        private void Form1_SizeChanged(object sender, EventArgs e)
        {
            applicationSettings.FormSize = this.Size;
        }

        private void quitButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            if(locationListBox.SelectedIndex == -1)
            {
                MessageBox.Show("You must select a file path in the list before starting AutoWolf. " +
                                "If the list is empty, you have to add a path.");
                return;
            }
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.EnvironmentVariables["AUTOWOLFDIR"] = 
                locationListBox.Items[locationListBox.SelectedIndex].ToString();
            startInfo.Arguments = commandLineArgumentsTextBox.Text;
            startInfo.FileName = "AutoWolf.exe";
            startInfo.UseShellExecute = false;
            Process.Start(startInfo);
        }
    }
}
