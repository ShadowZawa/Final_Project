#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "../models/PlayerModel.h"
#include "../models/ItemModel.h"
#include "InventoryController.h"

using namespace std;

class DataManager {
private:
    // 輔助函數：從字串轉換為 jobType
    jobType stringToJobType(const string& jobStr) {
        if (jobStr == "戰士") return WARRIOR;
        if (jobStr == "法師") return MAGE;
        if (jobStr == "弓箭手") return ARCHER;
        if (jobStr == "盜賊") return THIEF;
        return WARRIOR;
    }

public:
    // 儲存遊戲資料 (username_characterName)
    bool save(const string& username, const string& characterName, PlayerModel& playerRef, InventoryController& inventoryRef, const string& currentMap) {
        try {
            // 建立存檔目錄
            string saveDir = "saves/";
            system(("mkdir " + saveDir + " 2>nul").c_str());
            
            // 檔案名稱格式: username_characterName.txt
            string filename = saveDir + username + "_" + characterName + ".txt";
            ofstream file(filename);
            
            if (!file.is_open()) {
                return false;
            }

            // 儲存角色名稱
            file << "CHARACTER_NAME" << endl;
            file << characterName << endl;

            // 儲存玩家基本資料
            file << "PLAYER_DATA" << endl;
            file << playerRef.getLevel() << endl;
            file << playerRef.getJob() << endl;  // 儲存為字串
            file << playerRef.getHp() << endl;
            file << playerRef.getMp() << endl;
            file << playerRef.getGold() << endl;
            file << playerRef.getExp() << endl;
            file << playerRef.getAttrLvl() << endl;
            file << playerRef.getAttrDmg() << endl;
            file << playerRef.getAttrDef() << endl;
            file << playerRef.getAttrHp() << endl;
            file << playerRef.getAttrMp() << endl;
            file << playerRef.getSkillLevel() << endl;
            file << playerRef.getSkill1Level() << endl;
            file << playerRef.getSkill2Level() << endl;
            
            // 儲存當前地圖
            file << "CURRENT_MAP" << endl;
            file << currentMap << endl;

            // 儲存裝備資料
            file << "EQUIPMENT_DATA" << endl;
            
            // 儲存武器
            ItemModel weapon = playerRef.getEquippedItem(ItemModel::WEAPON);
            if (weapon.name != "") {
                file << weapon.name << "|"
                     << weapon.description << "|"
                     << weapon.price << "|"
                     << weapon.effect << "|"
                     << weapon.star << "|"
                     << static_cast<int>(weapon.type) << "|"
                     << weapon.suit << endl;
            } else {
                file << "NONE" << endl;
            }
            
            // 儲存頭盔
            ItemModel helmet = playerRef.getEquippedItem(ItemModel::HELMET);
            if (helmet.name != "") {
                file << helmet.name << "|"
                     << helmet.description << "|"
                     << helmet.price << "|"
                     << helmet.effect << "|"
                     << helmet.star << "|"
                     << static_cast<int>(helmet.type) << "|"
                     << helmet.suit << endl;
            } else {
                file << "NONE" << endl;
            }
            
            // 儲存胸甲
            ItemModel chestplate = playerRef.getEquippedItem(ItemModel::CHESTPLATE);
            if (chestplate.name != "") {
                file << chestplate.name << "|"
                     << chestplate.description << "|"
                     << chestplate.price << "|"
                     << chestplate.effect << "|"
                     << chestplate.star << "|"
                     << static_cast<int>(chestplate.type) << "|"
                     << chestplate.suit << endl;
            } else {
                file << "NONE" << endl;
            }
            
            // 儲存護腿
            ItemModel leggings = playerRef.getEquippedItem(ItemModel::LEGGINGS);
            if (leggings.name != "") {
                file << leggings.name << "|"
                     << leggings.description << "|"
                     << leggings.price << "|"
                     << leggings.effect << "|"
                     << leggings.star << "|"
                     << static_cast<int>(leggings.type) << "|"
                     << leggings.suit << endl;
            } else {
                file << "NONE" << endl;
            }
            
            // 儲存靴子
            ItemModel boots = playerRef.getEquippedItem(ItemModel::BOOTS);
            if (boots.name != "") {
                file << boots.name << "|"
                     << boots.description << "|"
                     << boots.price << "|"
                     << boots.effect << "|"
                     << boots.star << "|"
                     << static_cast<int>(boots.type) << "|"
                     << boots.suit << endl;
            } else {
                file << "NONE" << endl;
            }

            // 儲存背包資料
            file << "INVENTORY_DATA" << endl;
            vector<ItemModel> items = inventoryRef.getItems();
            file << items.size() << endl;
            
            for (const auto& item : items) {
                file << item.name << "|"
                     << item.description << "|"
                     << item.price << "|"
                     << item.effect << "|"
                     << item.star << "|"
                     << static_cast<int>(item.type) << "|"
                     << item.suit << endl;
            }

            file.close();
            return true;
        }
        catch (...) {
            return false;
        }
    }

    // 讀取遊戲資料 (username_characterName)
    bool load(const string& username, const string& characterName, PlayerModel& player, InventoryController& inventory, string& currentMap) {
        try {
            string filename = "saves/" + username + "_" + characterName + ".txt";
            ifstream file(filename);
            
            if (!file.is_open()) {
                return false;
            }

            string line;
            
            // 讀取角色名稱
            getline(file, line); // "CHARACTER_NAME"
            if (line != "CHARACTER_NAME") {
                return false;
            }
            string loadedCharacterName;
            getline(file, loadedCharacterName);
            
            // 讀取玩家資料
            getline(file, line); // "PLAYER_DATA"
            if (line != "PLAYER_DATA") {
                return false;
            }

            int level, hp, mp, gold, exp, attr_lvl, attr_dmg, attr_def, attr_hp, attr_mp, skill_lvl, skill1_lvl, skill2_lvl;
            string jobStr;
            
            file >> level;
            getline(file, line); // 讀取換行符
            getline(file, jobStr); // 讀取職業字串
            file >> hp >> mp >> gold >> exp >> attr_lvl >> attr_dmg >> attr_def >> attr_hp >> attr_mp >> skill_lvl>> skill1_lvl >> skill2_lvl;
            
            // 重建玩家物件
            jobType job = stringToJobType(jobStr);
            player = PlayerModel(level, job, exp, attr_lvl, attr_dmg, attr_def, attr_hp, attr_mp, skill_lvl, skill1_lvl, skill2_lvl);
            // 這裡需要在PlayerModel中添加setter方法，或使用friend class
            // 暫時透過damage和heal來調整
            int hpDiff = hp - player.getHp();
            if (hpDiff > 0) player.heal(hpDiff);
            else if (hpDiff < 0) player.takeDamage(-hpDiff);
            
            int mpDiff = mp - player.getMp();
            if (mpDiff > 0) player.recoverMp(mpDiff);
            else if (mpDiff < 0) player.useMp(-mpDiff);
            
            int goldDiff = gold - player.getGold();
            if (goldDiff > 0) player.addGold(goldDiff);
            else if (goldDiff < 0) player.deductGold(-goldDiff);
            
            // 經驗值和屬性點需要PlayerModel提供setter
            // 這裡假設有相應的方法或使用friend class

            getline(file, line); // 讀取換行符
            
            // 讀取當前地圖
            getline(file, line); // "CURRENT_MAP"
            if (line != "CURRENT_MAP") {
                return false;
            }
            getline(file, currentMap);

            // 讀取裝備資料（向後兼容：如果沒有 EQUIPMENT_DATA 則跳過）
            getline(file, line);
            if (line == "EQUIPMENT_DATA") {
                // 新格式：讀取裝備資料
                
                // 讀取武器
                getline(file, line);
                if (line != "NONE") {
                    stringstream ss(line);
                    string token;
                    ItemModel weapon;
                    
                    getline(ss, weapon.name, '|');
                    getline(ss, weapon.description, '|');
                    getline(ss, token, '|');
                    weapon.price = stoi(token);
                    getline(ss, token, '|');
                    weapon.effect = stoi(token);
                    getline(ss, token, '|');
                    weapon.star = stoi(token);
                    getline(ss, token, '|');
                    weapon.type = static_cast<ItemModel::Type>(stoi(token));
                    getline(ss, weapon.suit, '|');
                    
                    player.Equip(weapon);
                }
                
                // 讀取頭盔
                getline(file, line);
                if (line != "NONE") {
                    stringstream ss(line);
                    string token;
                    ItemModel helmet;
                    
                    getline(ss, helmet.name, '|');
                    getline(ss, helmet.description, '|');
                    getline(ss, token, '|');
                    helmet.price = stoi(token);
                    getline(ss, token, '|');
                    helmet.effect = stoi(token);
                    getline(ss, token, '|');
                    helmet.star = stoi(token);
                    getline(ss, token, '|');
                    helmet.type = static_cast<ItemModel::Type>(stoi(token));
                    getline(ss, helmet.suit, '|');
                    
                    player.Equip(helmet);
                }
                
                // 讀取胸甲
                getline(file, line);
                if (line != "NONE") {
                    stringstream ss(line);
                    string token;
                    ItemModel chestplate;
                    
                    getline(ss, chestplate.name, '|');
                    getline(ss, chestplate.description, '|');
                    getline(ss, token, '|');
                    chestplate.price = stoi(token);
                    getline(ss, token, '|');
                    chestplate.effect = stoi(token);
                    getline(ss, token, '|');
                    chestplate.star = stoi(token);
                    getline(ss, token, '|');
                    chestplate.type = static_cast<ItemModel::Type>(stoi(token));
                    getline(ss, chestplate.suit, '|');
                    
                    player.Equip(chestplate);
                }
                
                // 讀取護腿
                getline(file, line);
                if (line != "NONE") {
                    stringstream ss(line);
                    string token;
                    ItemModel leggings;
                    
                    getline(ss, leggings.name, '|');
                    getline(ss, leggings.description, '|');
                    getline(ss, token, '|');
                    leggings.price = stoi(token);
                    getline(ss, token, '|');
                    leggings.effect = stoi(token);
                    getline(ss, token, '|');
                    leggings.star = stoi(token);
                    getline(ss, token, '|');
                    leggings.type = static_cast<ItemModel::Type>(stoi(token));
                    getline(ss, leggings.suit, '|');
                    
                    player.Equip(leggings);
                }
                
                // 讀取靴子
                getline(file, line);
                if (line != "NONE") {
                    stringstream ss(line);
                    string token;
                    ItemModel boots;
                    
                    getline(ss, boots.name, '|');
                    getline(ss, boots.description, '|');
                    getline(ss, token, '|');
                    boots.price = stoi(token);
                    getline(ss, token, '|');
                    boots.effect = stoi(token);
                    getline(ss, token, '|');
                    boots.star = stoi(token);
                    getline(ss, token, '|');
                    boots.type = static_cast<ItemModel::Type>(stoi(token));
                    getline(ss, boots.suit, '|');
                    
                    player.Equip(boots);
                }

                // 讀取背包資料標記
                getline(file, line);
            }
            // 如果 line 不是 "EQUIPMENT_DATA"，代表是舊格式，line 已經是 "INVENTORY_DATA"
            
            // 讀取背包資料
            // line 此時應該是 "INVENTORY_DATA"（新格式剛讀取，舊格式從上面帶下來）
            if (line != "INVENTORY_DATA") {
                return false;
            }

            int itemCount;
            file >> itemCount;
            getline(file, line); // 讀取換行符

            // 清空現有背包
            while (inventory.getSize() > 0) {
                inventory.removeItem(0);
            }

            // 讀取每個物品
            for (int i = 0; i < itemCount; ++i) {
                getline(file, line);
                stringstream ss(line);
                string token;
                ItemModel item;

                getline(ss, item.name, '|');
                getline(ss, item.description, '|');
                
                getline(ss, token, '|');
                item.price = stoi(token);
                
                getline(ss, token, '|');
                item.effect = stoi(token);
                
                getline(ss, token, '|');
                item.star = stoi(token);
                
                getline(ss, token, '|');
                item.type = static_cast<ItemModel::Type>(stoi(token));
                
                getline(ss, item.suit, '|');

                inventory.addItem(item);
            }

            file.close();
            return true;
        }
        catch (...) {
            return false;
        }
    }

    // 檢查存檔是否存在
    bool saveExists(const string& username, const string& characterName) {
        string filename = "saves/" + username + "_" + characterName + ".txt";
        ifstream file(filename);
        bool exists = file.good();
        file.close();
        return exists;
    }

    // 刪除存檔
    bool deleteSave(const string& username, const string& characterName) {
        string filename = "saves/" + username + "_" + characterName + ".txt";
        return remove(filename.c_str()) == 0;
    }
    
    // 刪除角色（與deleteSave相同，提供統一接口）
    bool deleteCharacter(const string& username, const string& characterName) {
        return deleteSave(username, characterName);
    }
    
    // 創建新角色存檔（初始化）
    bool createNewCharacter(const string& username, const string& characterName, int job = 0) {
        // 創建一個新的玩家和空背包
        PlayerModel newPlayer(1, static_cast<jobType>(job), 0,3);
        newPlayer.initClassEffect();
        newPlayer.addGold(100); // 初始金幣100
        InventoryController newInventory;
        newInventory.InitBasicItem();
        string startingMap = "維多利亞港";  // 預設起始地圖
        
        return save(username, characterName, newPlayer, newInventory, startingMap);
    }
    
    // ===== 倉庫系統 (全帳號共用) =====
    
    // 儲存倉庫資料
    bool saveStorage(InventoryController& storage) {
        try {
            string saveDir = "saves/";
            system(("mkdir " + saveDir + " 2>nul").c_str());
            
            string filename = saveDir + "global_storage.txt";
            ofstream file(filename);
            
            if (!file.is_open()) {
                return false;
            }

            file << "STORAGE_DATA" << endl;
            vector<ItemModel> items = storage.getItems();
            file << items.size() << endl;
            
            for (const auto& item : items) {
                file << item.name << "|"
                     << item.description << "|"
                     << item.price << "|"
                     << item.effect << "|"
                     << item.star << "|"
                     << static_cast<int>(item.type) << "|"
                     << item.suit << endl;
            }

            file.close();
            return true;
        }
        catch (...) {
            return false;
        }
    }
    
    // 讀取倉庫資料
    bool loadStorage(InventoryController& storage) {
        try {
            string filename = "saves/global_storage.txt";
            ifstream file(filename);
            
            if (!file.is_open()) {
                // 如果檔案不存在，創建空倉庫
                return saveStorage(storage);
            }

            string line;
            
            getline(file, line); // "STORAGE_DATA"
            if (line != "STORAGE_DATA") {
                file.close();
                return false;
            }

            int itemCount;
            file >> itemCount;
            getline(file, line); // 讀取換行符

            // 清空現有倉庫
            while (storage.getSize() > 0) {
                storage.removeItem(0);
            }

            // 讀取每個物品
            for (int i = 0; i < itemCount; ++i) {
                getline(file, line);
                stringstream ss(line);
                string token;
                ItemModel item;

                getline(ss, item.name, '|');
                getline(ss, item.description, '|');
                
                getline(ss, token, '|');
                item.price = stoi(token);
                
                getline(ss, token, '|');
                item.effect = stoi(token);
                
                getline(ss, token, '|');
                item.star = stoi(token);
                
                getline(ss, token, '|');
                item.type = static_cast<ItemModel::Type>(stoi(token));
                
                getline(ss, item.suit, '|');

                storage.addItem(item);
            }

            file.close();
            return true;
        }
        catch (...) {
            return false;
        }
    }
};

#endif // DATAMANAGER_H
