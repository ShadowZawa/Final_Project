# Final Project - 木風之谷

## 專案結構
```
Final_Project/
├── main.cpp              # 主程式入口
├── scenes/               # 遊戲場景
├── ui/                   # UI 介面元件
├── models/               # 資料模型
├── managers/             # 遊戲控制器
├── external/             # 外部函式庫
│   └── ftxui/           # FTXUI 終端 UI 函式庫
│       ├── include/
│       └── lib/
└── build/               # CMake 建置輸出
```

## 環境需求

### 必要環境
1. **MinGW-w64 GCC 編譯器**
   - 需要 C++17 支援
   - 確保 `g++` 在系統 PATH 中

### 設定 PATH (重要!)
將 MinGW 的 bin 目錄加入系統 PATH，例如：
```
C:\winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-13.0.0-r3\mingw64\bin
```

驗證設定：
```powershell
g++ --version
```

## 建置方式

### 方法 1: 使用 VS Code Task (推薦)
1. 開啟專案資料夾
2. 按 `Ctrl+Shift+B` 選擇建置任務
3. 選擇 **C/C++: g++.exe 建置使用中檔案**

### 方法 2: 使用 CMake
```powershell
# 配置
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=./external/ftxui

# 建置
cmake --build build

# 執行
.\build\main.exe
```

### 方法 3: 手動編譯
```powershell
g++ -g main.cpp -o main.exe `
    -I. `
    -I.\external\ftxui\include `
    -L.\external\ftxui\lib `
    -lftxui-component -lftxui-dom -lftxui-screen
```

## 執行程式
```powershell
.\main.exe
```

## 遊戲操作
- **B** - 開啟背包
- **E** - 裝備介面
- **U** - 強化裝備
- **S** - 商店
- **C** - 角色狀態
- **M** - 世界地圖
- **ESC** - 關閉介面

## 專案可移植性

### ✅ 此專案已配置為可移植
- FTXUI 函式庫已內建於 `external/ftxui/`
- 所有路徑使用 `${workspaceFolder}` 變數
- 只需確保 MinGW GCC 在系統 PATH 中

### 📦 移動專案到其他位置
1. 直接複製整個 `Final_Project` 資料夾到任何位置
2. 確保目標電腦有 MinGW GCC 並在 PATH 中
3. 用 VS Code 開啟即可建置

### 🔧 在新電腦上使用
1. 安裝 MinGW-w64 GCC
2. 將 MinGW 的 `bin` 目錄加入 PATH
3. 複製專案資料夾
4. 開啟 VS Code 建置

## 專案依賴
- **FTXUI v6.0+** - 已包含在 `external/ftxui/`
- **C++17 標準** - 編譯器支援

## 疑難排解

### 問題：找不到 g++
```
解決：將 MinGW 的 bin 目錄加入系統 PATH
```

### 問題：找不到 ftxui 標頭檔
```
解決：確認 external/ftxui/include 存在
```

### 問題：連結錯誤
```
解決：確認 external/ftxui/lib 包含 .a 函式庫檔案
```
