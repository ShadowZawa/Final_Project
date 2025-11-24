
#ifndef WORLDMODEL_H
#define WORLDMODEL_H

#include <string>
#include <vector>
#include "../models/ItemModel.h"
using namespace std;

// 敵人資料
class Enemy {
    public:
    std::string name;
    int level;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int exp;      // 擊敗後獲得的經驗值
    int gold;     // 擊敗後獲得的金幣
    
    std::vector<struct ItemModel> dropItems; // 擊敗後可能掉落的物品

    int calcDropExp(){
        return level * 2;
    }
};

enum class moveDirection
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};

// 世界地圖
class WorldModel {
    public:
        WorldModel(string name, string description, int minLevel, vector<Enemy> enemies, string south_connection, string north_connection, string east_connection, string west_connection){
            this->name = name;
            this->description = description;
            this->minLevel = minLevel;
            this->enemies = enemies;
            this->south_connection = south_connection;
            this->north_connection = north_connection;
            this->east_connection = east_connection;
            this->west_connection = west_connection;

            // 初始化當前敵人列表為空
            this->current_enemies.clear();
            SpawnRandomEnemies();
        }
        //取得函式
        string getName(){return this->name;}
        string getDescription(){return this->description;}
        int getMinLevel(){return this->minLevel;}
        // 回傳參考以避免不必要的複製並允許外部修改當前敵人清單
        const vector<Enemy>& getEnemies() const { return this->enemies; }
        vector<Enemy>& getEnemies() { return this->enemies; }
        const vector<Enemy>& getCurrentEnemies() const { return this->current_enemies; }
        vector<Enemy>& getCurrentEnemies() { return this->current_enemies; }
        string getConnection(moveDirection direction){
            switch(direction){
                case moveDirection::NORTH:
                    return this->north_connection;
                case moveDirection::SOUTH:
                    return this->south_connection;
                case moveDirection::EAST:
                    return this->east_connection;
                case moveDirection::WEST:
                    return this->west_connection;
                default:
                    return "";
            }
        }

        //功能函式
        void ClearMap(){
            this->current_enemies.clear();
        }
        void SpawnRandomEnemies(){
            // 隨機生成當前敵人
            if (enemies.empty()) {
                return; // 如果沒有敵人，直接返回
            }
            int num_enemies = rand() % 3 + 1; // 隨機
            for (int i = 0; i < num_enemies; ++i)
            {
                int enemy_index = rand() % enemies.size();
                current_enemies.push_back(enemies[enemy_index]);
            }
        }

    private:
        string name;
        string description;
        int minLevel;               // 建議最低等級
        vector<Enemy> enemies; // 該地點可能出現的敵人
        vector<Enemy> current_enemies; // 當前出現的敵人
        string south_connection;
        string north_connection;
        string east_connection;
        string west_connection;
};

#endif // WORLDMODEL_H  