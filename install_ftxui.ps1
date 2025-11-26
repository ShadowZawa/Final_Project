# FTXUI Installation Script for vcpkg
# Run this script in PowerShell as Administrator

Write-Host "=== FTXUI Installation Script ===" -ForegroundColor Green
Write-Host ""

# Check if vcpkg is installed
$vcpkgPath = "C:\vcpkg"
$vcpkgExists = Test-Path $vcpkgPath

if (-not $vcpkgExists) {
    Write-Host "vcpkg not found, installing..." -ForegroundColor Yellow
    
    # Clone vcpkg
    Set-Location C:\
    git clone https://github.com/Microsoft/vcpkg.git
    Set-Location vcpkg
    
    # Bootstrap vcpkg
    .\bootstrap-vcpkg.bat
    
    # Integrate with Visual Studio
    .\vcpkg integrate install
    
    Write-Host "vcpkg installation completed!" -ForegroundColor Green
}
else {
    Write-Host "vcpkg already installed at: $vcpkgPath" -ForegroundColor Green
    Set-Location $vcpkgPath
}

Write-Host ""
Write-Host "Installing FTXUI..." -ForegroundColor Yellow

# Install FTXUI
.\vcpkg install ftxui:x64-windows

Write-Host ""
Write-Host "=== Installation Complete ===" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. Open the project in Visual Studio" 
Write-Host "2. Project -> Properties -> VC++ Directories"
Write-Host "3. Include Directories: Add $vcpkgPath\installed\x64-windows\include"
Write-Host "4. Library Directories: Add $vcpkgPath\installed\x64-windows\lib"
Write-Host "5. Linker -> Input -> Additional Dependencies: Add ftxui-screen.lib;ftxui-dom.lib;ftxui-component.lib"
