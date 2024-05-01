using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TeamServer.Listeners;

namespace TeamServer.Menu
{
    class MainMenu: AbstractMenu
    {
        private bool _showHiddenMenu;

        public MainMenu() : base("Welcome to Best C2 Framework.") { }

        protected override void Init()
        {
            AddMenuItem(new MenuItem(0, "Start HTTP Listener", new HttpListenerMenu()));

            AddMenuItem(new MenuItem(1, "Beacons Captured", () => {
                int idx = 0;
                foreach (IListener listener in Program.listeners)
                {
                    foreach (Beacon beacon in listener.Beacons)
                    {
                        Console.WriteLine(idx.ToString() + ". " + beacon.Id);
                        idx++;
                    }
                }
            }));

            AddMenuItem(new MenuItem(2, "Exit menu").SetAsExitOption());


            //AddHiddenMenuItem(new MenuItem(3, "Hidden menu item", () => Console.WriteLine("I was a hidden menu item")));
        }

        protected override void UpdateMenuItems()
        {
            if (_showHiddenMenu) ShowMenuItem(3);
        }
    }

    public class HttpListenerMenu : AbstractMenu
    {
        public HttpListenerMenu() : base("HTTP Listener Menu.") { }

        protected override void Init()
        {
            AddMenuItem(new MenuItem(0, "Input IP & Port", () => {
                Console.WriteLine("Write the listening IP:");
                string ip = Console.ReadLine().Trim();

                Console.WriteLine();

                Console.WriteLine("Write the listening port:");
                int port = Convert.ToInt32(Console.ReadLine().Trim());

                try
                {
                    CustomHttpListener listener = new CustomHttpListener(ip, port);
                    listener.StartListener();

                    Program.listeners.Add(listener);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
            }));
            AddMenuItem(new MenuItem(1, "Exit current menu").SetAsExitOption());

        }
    }


    public class BeaconsMenu : AbstractMenu
    {
        public BeaconsMenu() : base("HTTP Listener Menu.") { }

        protected override void Init()
        {
            AddMenuItem(new MenuItem(0, "Interact with a Beacon", () => {
                
            }));
            AddMenuItem(new MenuItem(1, "Exit current menu").SetAsExitOption());

        }
    }
}
