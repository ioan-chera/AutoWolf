using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml.Serialization;
using System.Windows.Forms;
using System.Drawing;

namespace AutoWolfStarter
{
    public class ApplicationSettings
    {
        private  const string configName = @"\AutoWolfStarter.config";

        private bool appSettingsChanged;

        private string commandLineArguments;
        private string[] wolfensteinLocations;
        private Point formLocation;
        private Size formSize;
        private string folderDialogPath;

        public string CommandLineArguments
        {
            get { return commandLineArguments; }
            set
            {
                if (value != commandLineArguments)
                {
                    commandLineArguments = value;
                    appSettingsChanged = true;
                }
            }
        }
        public string FolderDialogPath
        {
            get { return folderDialogPath; }
            set
            {
                if (value != folderDialogPath)
                {
                    folderDialogPath = value;
                    appSettingsChanged = true;
                }
            }
        }
        public string[] WolfensteinLocations
        {
            get { return wolfensteinLocations; }
            set
            {
                if (value != wolfensteinLocations)
                {
                    wolfensteinLocations = value;
                    appSettingsChanged = true;
                }
            }
        }
        public Point FormLocation
        {
            get { return formLocation; }
            set
            {
                if (value != formLocation)
                {
                    formLocation = value;
                    appSettingsChanged = true;
                }
            }
        }
        public Size FormSize
        {
            get { return formSize; }
            set
            {
                if (value != formSize)
                {
                    formSize = value;
                    appSettingsChanged = true;
                }
            }
        }

        public ApplicationSettings()
        {
            commandLineArguments = "";
        }

        public bool SaveAppSettings()
        {
            if (appSettingsChanged)
            {
                StreamWriter streamWriter = null;
                XmlSerializer xmlSerializer = null;

                try
                {
                    xmlSerializer = new XmlSerializer(typeof(ApplicationSettings));
                    streamWriter = new StreamWriter(Application.LocalUserAppDataPath + configName, false);
                    
                    xmlSerializer.Serialize(streamWriter, this);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
                finally
                {
                    if (streamWriter != null)
                        streamWriter.Close();
                }
            }
            return appSettingsChanged;
        }

        public bool LoadAppSettings()
        {
            XmlSerializer xmlSerializer = null;
            FileStream fileStream = null;
            bool fileExists = false;

            try
            {
                xmlSerializer = new XmlSerializer(typeof(ApplicationSettings));
                FileInfo fileInfo = new FileInfo(Application.LocalUserAppDataPath + configName);

                if (fileInfo.Exists)
                {
                    fileStream = fileInfo.OpenRead();
                    ApplicationSettings applicationSettings =
                        (ApplicationSettings)xmlSerializer.Deserialize(fileStream);

                    this.commandLineArguments = applicationSettings.CommandLineArguments;
                    this.wolfensteinLocations = applicationSettings.WolfensteinLocations;
                    this.formLocation = applicationSettings.FormLocation;
                    this.formSize = applicationSettings.FormSize;
                    this.folderDialogPath = applicationSettings.FolderDialogPath;
                    fileExists = true;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                if (fileStream != null)
                    fileStream.Close();
            }
            return fileExists;
        }
    }
}
