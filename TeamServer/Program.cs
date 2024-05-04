using System.Net;
using System.Text;
using TeamServer.Menu;

namespace TeamServer
{
    class Program
    {
        public static List<IListener> listeners;
        public static Beacon communicatingBeacon = null;

        public static void Main(string[] args)
        {
            listeners = new List<IListener>();

            var testMenu = new MainMenu();
            testMenu.Display();
        }
    }
}