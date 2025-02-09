# Change to the script directory and then to the parent directory
Set-Location (Split-Path -Path $MyInvocation.MyCommand.Path -Parent)
Set-Location ..

# Check if the first argument is -c
if ($args.Count -gt 0 -and $args[0] -eq "-c") {
    Write-Host "Checking with clang-format..."
    Get-ChildItem -Recurse -Include *.h, *.hpp, *.inl, *.cpp -Path src/, include/, test/ | 
        ForEach-Object { & clang-format --dry-run --Werror $_.FullName }

    Write-Host "Checking with clang-tidy..."
    Get-ChildItem -Recurse -Include *.h, *.hpp, *.inl, *.cpp -Path src/, include/ | 
        ForEach-Object { & clang-tidy -p=build_debug $_.FullName }
} else {
    Write-Host "Formatting files..."
    Get-ChildItem -Recurse -Include *.h, *.hpp, *.inl, *.cpp -Path src/, include/, test/ | 
        ForEach-Object { & clang-format -i $_.FullName }

    Write-Host "Letting clang-tidy try to fix stuff..."
    Get-ChildItem -Recurse -Include *.h, *.hpp, *.inl, *.cpp -Path src/, include/ | 
        ForEach-Object { & clang-tidy -p=build_debug --fix $_.FullName }
}