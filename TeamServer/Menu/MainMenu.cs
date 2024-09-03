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

                if (Program.listeners == null || Program.listeners.Count == 0)
                {
                    Console.WriteLine("No beacons captured");
                    Display();
                    return;
                }

                // collect all beacons
                int idx = 0;
                List<Beacon> localBeacons = new List<Beacon>();
                foreach (IListener listener in Program.listeners)
                {
                    if (listener.Beacons != null && listener.Beacons.Count > 0)
                    {
                        foreach (Beacon beacon in listener.Beacons)
                        {
                            Console.WriteLine(idx.ToString() + ". " + beacon.Id);
                            localBeacons.Add(beacon);
                            idx++;
                        }
                    }
                }

                Console.Write("Interact with (write beacon ID or \"Back\" to return): ");
                string chosenIdxStr = Console.ReadLine();

                bool couldConvert = true;
                int chosenIdx = 0;

                try
                {
                    chosenIdx = Convert.ToInt32(chosenIdxStr);
                }
                catch(Exception e)
                {
                    couldConvert = false;
                }

                if(chosenIdxStr != "Exit" && couldConvert)
                {
                    try
                    {
                        Console.Clear();
                        Beacon targetBeacon = localBeacons[chosenIdx];
                        Program.communicatingBeacon = targetBeacon;
                        targetBeacon.GetOutput(true);
                        do
                        {
                            Console.Write("beacon> ");
                            string command = Console.ReadLine();

                            if(!BeaconOperations.HandleOperation(targetBeacon, command))
                            {
                                break;
                            }

                        } while (true);
                    }catch(Exception e)
                    {
                        Console.WriteLine("Beacon with the specified index does not exist");
                    }
                }

                Program.communicatingBeacon = null;
                Display();
                return;
            }, true));

            AddMenuItem(new MenuItem(2, "Remove a Beacon", () => {
                int selection = 0;
                try
                {
                    bool found = false;

                    foreach (IListener listener in Program.listeners)
                    {
                        if (listener.Beacons != null && listener.Beacons.Count > 0)
                            found = true;
                    }

                    if (!found)
                    {
                        Console.WriteLine("No beacon found.");
                        return;
                    }

                    Console.Write("Write beacon IDX: ");
                    selection = Convert.ToInt32(Console.ReadLine().Trim());
                    Console.WriteLine();



                    int idx = 0;
                    List<Beacon> localBeacons = new List<Beacon>();
                    foreach (IListener listener in Program.listeners)
                    {
                        if (listener.Beacons != null && listener.Beacons.Count > 0)
                        {
                            foreach (Beacon beacon in listener.Beacons)
                            {
                                if (idx == selection)
                                {
                                    localBeacons.Remove(beacon);
                                    Console.WriteLine("Removed beacon");
                                    break;
                                }
                                localBeacons.Add(beacon);
                                idx++;
                            }
                        }
                    }
                    Console.WriteLine("Beacon not found");
                    return;

                }
                catch (Exception ex)
                {
                    Console.WriteLine("Beacon not found");
                    return;
                }

            }));

            AddMenuItem(new MenuItem(3, "Exit menu").SetAsExitOption());

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
                string ip = "127.0.0.1";
                int port = 8888;
                try
                {
                    Console.WriteLine();
                    Console.Write("Write the listening IP: ");
                    ip = Console.ReadLine().Trim();


                    Console.Write("Write the listening port: ");
                    port = Convert.ToInt32(Console.ReadLine().Trim());
                    Console.WriteLine();

                }
                catch (Exception ex)
                {
                    Console.WriteLine("Input a real port");
                    return;
                }

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
