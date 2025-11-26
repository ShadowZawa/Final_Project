# Visual Studio 2022 專案說明

## ⚠️ 重要：FTXUI 函式庫設置

在編譯此專案之前，你需要先安裝 FTXUI 函式庫。

### 快速安裝（推薦）

執行提供的 PowerShell 腳本：
```powershell
.\install_ftxui.ps1
```

### 手動安裝

詳細步驟請參考 [FTXUI_SETUP.md](FTXUI_SETUP.md)

## 專案配置

此專案已配置為使用 Visual Studio 2022 (v143 工具集)。

### 編譯配置

- **Debug|x64**: 除錯版本
- **Release|x64**: 發布版本
- **編碼**: UTF-8 (使用 /utf-8 編譯選項)

### 包含目錄

- `$(ProjectDir)` - 專案根目錄
- `$(ProjectDir)external\ftxui\include` - FTXUI 標頭檔目錄

### 函式庫目錄

- `$(ProjectDir)external\ftxui\lib` - FTXUI 函式庫目錄

### 連結的函式庫

- `ftxui-component.lib`
- `ftxui-dom.lib`
- `ftxui-screen.lib`

## 如何使用

1. 使用 Visual Studio 2022 開啟 `Final_Project.sln`
2. 確保 FTXUI 函式庫已放置在 `external/ftxui/` 目錄下
3. 選擇建置配置 (Debug 或 Release)
4. 按 F5 開始偵錯，或 Ctrl+F5 開始執行 (不偵錯)

## 輸出目錄

- 編譯產物: `bin\Debug\` 或 `bin\Release\`
- 中間檔案: `obj\Debug\` 或 `obj\Release\`

## 必要目錄

專案執行時需要以下目錄 (會自動創建):
- `data\` - 儲存使用者帳號資料
- `saves\` - 儲存遊戲存檔

## 注意事項

- 使用 C++17 標準
- 字元集設定為 Unicode
- 需要 Windows 10 SDK
- **所有源文件使用 UTF-8 編碼** (已設置 /utf-8 編譯選項)
- 如果遇到「不支援的新字元」錯誤：
  1. 確保所有 .cpp 和 .h 文件以 UTF-8 編碼保存
  2. 在 Visual Studio 中：檔案 → 進階儲存選項 → 選擇「Unicode (UTF-8 沒有簽章) - 字碼頁 65001」
  3. 專案已配置 /utf-8 編譯選項來處理中文字符
