using System.Diagnostics;
using System.Net;
using System.Runtime.InteropServices;
using System;
using System.Windows.Forms;
using System.Net.Http;
using System.Threading.Tasks;

namespace TestAssembly{
    public class Program{

        [DllImport("kernel32.dll")]
        static extern IntPtr OpenProcess(
  int dwDesiredAccess,
  bool bInheritHandle,
  int dwProcessId
);

        static void Main(string[] args)
        {
            using (var client = new HttpClient())
            {
                client.BaseAddress = new Uri("http://192.168.45.210");
                var task = Task.Run(() => client.GetAsync("/"));
                task.Wait();
                var response = task.Result;
            }
        }
    }
}
