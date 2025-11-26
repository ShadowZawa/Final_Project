# FTXUI 函式庫設置指南 (Visual Studio 2022)

## 問題說明

Visual Studio 2022 找不到 `ftxui-component.lib` 等函式庫文件。

## 解決方案

### 選項 1: 使用 vcpkg 安裝 FTXUI (推薦)

1. 安裝 vcpkg (如果尚未安裝):
```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

2. 安裝 FTXUI:
```powershell
.\vcpkg install ftxui:x64-windows
```

3. 在專案中使用 vcpkg:
   - 專案屬性 → VC++ 目錄 → 包含目錄 → 添加 `$(VCPKG_ROOT)\installed\x64-windows\include`
   - 專案屬性 → VC++ 目錄 → 程式庫目錄 → 添加 `$(VCPKG_ROOT)\installed\x64-windows\lib`
   - 專案屬性 → 連結器 → 輸入 → 其他相依性 → 添加 `ftxui-screen.lib;ftxui-dom.lib;ftxui-component.lib`

### 選項 2: 從源碼編譯 FTXUI

1. 克隆 FTXUI 源碼:
```powershell
cd external
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
```

2. 使用 CMake 編譯:
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

3. 複製編譯結果:
```powershell
# 複製 lib 文件到 external/ftxui/lib/
Copy-Item .\Release\*.lib ..\..\ftxui\lib\

# include 文件已經在 external/ftxui/include/ 中
```

### 選項 3: 使用當前配置 (Header-Only 模式)

當前專案已配置為不需要預編譯的 .lib 文件。FTXUI 的某些版本支持 header-only 模式。

**如果遇到連結錯誤**，需要選擇選項 1 或 2。

## 當前專案配置

- 已移除對 `ftxui-component.lib`、`ftxui-dom.lib`、`ftxui-screen.lib` 的依賴
- 使用標準 Windows 函式庫
- Include 路徑指向 `external/ftxui/include`

## 驗證設置

編譯專案，如果出現以下錯誤：
- `LNK2019: unresolved external symbol` → 需要安裝 FTXUI 函式庫 (使用選項 1 或 2)
- `C1083: Cannot open include file` → 檢查 FTXUI include 目錄是否正確

## 推薦步驟

1. 使用 vcpkg 安裝 FTXUI (最簡單)
2. 更新專案文件中的連結器設定，添加 FTXUI 函式庫
3. 重新編譯專案

## 需要協助？

如果遇到問題，請確認：
- [ ] Visual Studio 2022 已安裝
- [ ] Windows 10 SDK 已安裝
- [ ] FTXUI 函式庫已正確安裝
- [ ] 專案配置中的路徑正確
