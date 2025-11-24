#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

// 使用者資料結構（只儲存帳號密碼和角色名稱列表）
struct UserData {
    string username;
    string password;
    vector<string> characterNames;  // 只儲存角色名稱，實際資料在獨立檔案
};

class AuthManager {
private:
    map<string, UserData> users;  // username -> UserData
    string authFilePath;
    const int MAX_CHARACTERS = 3;  // 每個使用者最多3個角色

    // 載入所有使用者資料
    void loadUsers() {
        users.clear();
        ifstream file(authFilePath);
        
        if (!file.is_open()) {
            return;  // 檔案不存在，初次使用
        }

        string line;
        while (getline(file, line)) {
            if (line == "USER") {
                UserData user;
                
                // 讀取使用者名稱
                getline(file, user.username);
                
                // 讀取密碼
                getline(file, user.password);
                
                // 讀取角色數量
                getline(file, line);
                int charCount = stoi(line);
                
                // 讀取角色名稱列表
                for (int i = 0; i < charCount; ++i) {
                    string characterName;
                    getline(file, characterName);
                    user.characterNames.push_back(characterName);
                }
                
                users[user.username] = user;
            }
        }
        
        file.close();
    }

    // 儲存所有使用者資料
    void saveUsers() {
        ofstream file(authFilePath);
        
        if (!file.is_open()) {
            return;
        }

        for (const auto& pair : users) {
            const UserData& user = pair.second;
            
            file << "USER" << endl;
            file << user.username << endl;
            file << user.password << endl;
            file << user.characterNames.size() << endl;
            
            // 只儲存角色名稱
            for (const auto& characterName : user.characterNames) {
                file << characterName << endl;
            }
        }
        
        file.close();
    }

public:
    AuthManager(const string& filePath = "data/users.dat") : authFilePath(filePath) {
        // 確保資料目錄存在
        system("mkdir data 2>nul");
        loadUsers();
    }

    // 登入功能
    bool login(const string& username, const string& password) {
        loadUsers();  // 重新載入最新資料
        
        auto it = users.find(username);
        
        // 使用者不存在
        if (it == users.end()) {
            return false;
        }
        
        // 密碼錯誤
        if (it->second.password != password) {
            return false;
        }
        
        return true;
    }

    // 註冊新使用者
    bool registerUser(const string& username, const string& password) {
        loadUsers();
        
        // 使用者名稱已存在
        if (users.find(username) != users.end()) {
            return false;
        }
        
        // 創建新使用者
        UserData newUser;
        newUser.username = username;
        newUser.password = password;
        
        users[username] = newUser;
        saveUsers();
        
        return true;
    }

    // 忘記密碼（直接修改為新密碼）
    bool forget(const string& username, const string& newPassword) {
        loadUsers();
        
        auto it = users.find(username);
        
        // 使用者不存在
        if (it == users.end()) {
            return false;
        }
        
        // 修改密碼
        it->second.password = newPassword;
        saveUsers();
        
        return true;
    }

    // 創建角色
    bool createCharacter(const string& username, const string& characterName, int jobType = 0) {
        loadUsers();
        
        auto it = users.find(username);
        
        // 使用者不存在
        if (it == users.end()) {
            return false;
        }
        
        // 檢查角色數量限制
        if (it->second.characterNames.size() >= MAX_CHARACTERS) {
            return false;
        }
        
        // 檢查角色名稱是否重複（同一使用者下）
        for (const auto& name : it->second.characterNames) {
            if (name == characterName) {
                return false;
            }
        }
        
        // 將角色名稱加入列表
        it->second.characterNames.push_back(characterName);
        saveUsers();
        
        // 創建角色的初始存檔（使用 DataManager）
        // 這部分將由 DataManager 處理初始化
        
        return true;
    }

    // 刪除角色
    bool deleteCharacter(const string& username, const string& characterName) {
        loadUsers();
        
        auto it = users.find(username);
        
        // 使用者不存在
        if (it == users.end()) {
            return false;
        }
        
        // 尋找並刪除角色名稱
        auto& characterNames = it->second.characterNames;
        auto charIt = find(characterNames.begin(), characterNames.end(), characterName);
        
        if (charIt != characterNames.end()) {
            characterNames.erase(charIt);
            saveUsers();
            
            // 同時刪除角色的存檔檔案
            string saveFile = "saves/" + username + "_" + characterName + ".txt";
            remove(saveFile.c_str());
            
            return true;
        }
        
        return false;  // 角色不存在
    }

    // 取得使用者的所有角色名稱
    vector<string> getCharacterNames(const string& username) {
        loadUsers();
        
        auto it = users.find(username);
        
        if (it == users.end()) {
            return vector<string>();
        }
        
        return it->second.characterNames;
    }

    // 檢查使用者是否存在
    bool userExists(const string& username) {
        loadUsers();
        return users.find(username) != users.end();
    }

    // 取得角色數量
    int getCharacterCount(const string& username) {
        loadUsers();
        
        auto it = users.find(username);
        
        if (it == users.end()) {
            return 0;
        }
        
        return it->second.characterNames.size();
    }
    
    // 檢查角色是否存在
    bool characterExists(const string& username, const string& characterName) {
        loadUsers();
        
        auto it = users.find(username);
        if (it == users.end()) {
            return false;
        }
        
        auto& names = it->second.characterNames;
        return find(names.begin(), names.end(), characterName) != names.end();
    }

    // 檢查是否可以創建更多角色
    bool canCreateMoreCharacters(const string& username) {
        return getCharacterCount(username) < MAX_CHARACTERS;
    }
};

#endif // AUTHMANAGER_H
