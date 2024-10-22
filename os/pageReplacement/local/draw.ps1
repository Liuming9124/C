# 獲取當前資料夾及所有子資料夾內的所有 .plt 檔案
$pltFiles = Get-ChildItem -Recurse -Filter *.plt

# 逐一執行所有找到的 .plt 檔案
foreach ($file in $pltFiles) {
    Write-Host "Executing $($file.FullName)"
    & $file.FullName
}