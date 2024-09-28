using System.IO;
using System.Text.Json;
using TeamServer.Menu;

namespace TeamServer
{
    class Program
    {
        public static Dictionary<string, string> config = new Dictionary<string, string>();
        public static List<IListener> listeners;
        public static Beacon communicatingBeacon = null;

        public static void Main(string[] args)
        {
            // INIT CONFIG
            String configRaw = File.ReadAllText("config.json");
            config = JsonSerializer.Deserialize<Dictionary<string, string>>(configRaw);

            // INIT LISTENERS
            listeners = new List<IListener>();

            // INIT MENU - TEMPORARY (we will have GUI)
            var testMenu = new MainMenu();
            testMenu.Display();
        }
    }
}