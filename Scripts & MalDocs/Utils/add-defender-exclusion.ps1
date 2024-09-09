# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Host "This script must be run as Administrator. Exiting."
    return
}

# Define folders to exclude, adjust if needed
$foldersToExclude = @(
    "C:\Users\$env:USERNAME\AppData\Local\JetBrains",
    "C:\Program Files\Docker",
    "C:\Program Files\JetBrains",
    "C:\Users\tradoi\Documents\Code\Mine\Custom-C2-Framework"
)

# Add folder exclusions
Write-Host "Adding folder exclusions..."
foreach ($folder in $foldersToExclude) {
    Add-MpPreference -ExclusionPath $folder
}


Write-Host "Script execution completed."