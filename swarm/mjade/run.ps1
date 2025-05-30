# Script use file type for UTF8-BOM
# get the current directory
$currentDir = Get-Location

$filePath = Read-Host "add file name"
$executionCount = Read-Host "add execution count"
$numSwarms = Read-Host "add number of swarms"

$executable = (Resolve-Path ".\a.exe").Path
$filePath = (Resolve-Path $filePath).Path

$commands = Get-Content -Path $filePath
$jobs = foreach ($command in $commands) {
    Start-Job -ScriptBlock {
        param($executable, $executionCount, $numSwarms, $command, $currentDir)
        
        # set the current directory
        Set-Location $currentDir
        
        # create the full command
        $fullCommand = "& `"$executable`" $executionCount $numSwarms $command"
        Write-Host "running: $fullCommand"
        
        # run the command
        Invoke-Expression $fullCommand
    } -ArgumentList $executable, $executionCount, $numSwarms, $command, $currentDir
}

# Wait for all jobs to complete
$jobs | Wait-Job

# Show the output of all jobs
$jobs | ForEach-Object {
    Receive-Job $_
    Remove-Job $_
}