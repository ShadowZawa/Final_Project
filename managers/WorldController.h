

#include "../models/WorldModel.h"
#include <string>
#include <vector>
using namespace std;
class WorldController
{
public:
    /*
    初始化世界地圖
    參數:
    WorldModel{
        string name;
        string description;
        int minLevel;
        vector<Enemy> enemies;
        string south_connection;
        string north_connection;
        string east_connection;
        string west_connection;
    }
    */
    WorldController()
    {
        this->worlds = vector<WorldModel>();
        worlds.push_back({"維多利亞港",
                          "所有冒險者的起點，充滿活力的港口城市。",
                          1,
                          vector<Enemy>{},
                          "",
                          "",
                          "六條岔道",
                          ""});
        worlds.push_back({"六條岔道",
                          "通往各大地點的交匯處，四通八達。",
                          1,
                          vector<Enemy>{
                              {"史萊姆", 1, 30, 30, 5, 2, 10, 5, vector<ItemModel>{{"史萊姆凝膠", "一塊黏糊糊的史萊姆凝膠。", 5, 0, 0, ItemModel::MISC, ""}, {"小補血藥", "恢復5點生命值的藥水。", 50, 5, 0, ItemModel::POTION, ""}}},
                              {"哥布林", 2, 50, 50, 8, 4, 20, 10, vector<ItemModel>{{"哥布林之牙", "一顆尖銳的哥布林牙齒。", 10, 0, 0, ItemModel::MISC, ""}, {"鐵製弓", "基礎的弓箭武器，攻擊力+3。", 150, 3, 0, ItemModel::BOW, ""}, {"鐵製匕首", "基礎的近戰武器，攻擊力+4。", 120, 4, 0, ItemModel::KNIFE, ""}}}},

                          "弓箭手村",
                          "勇士之村",
                          "魔法森林",
                          "維多利亞港"});
        worlds.push_back({"勇士之村",
                          "勇士們的聚集地，可以看出這裡的環境十分惡劣。",
                          1,
                          vector<Enemy>{
                              {"哥布林", 2, 50, 50, 8, 4, 20, 10},
                              {"地精戰士", 3, 70, 70, 12, 6, 30, 15}},
                          "六條岔道",
                          "",
                          "",
                          ""});
        worlds.push_back({"魔法森林",
                          "充滿神秘魔法和精靈的氣息，適合法師修行。",
                          1,
                          vector<Enemy>{
                              {"魔法小妖", 2, 40, 40, 10, 3, 25, 12},
                              {"樹精", 3, 80, 80, 15, 5, 35, 18}},
                          "",
                          "",
                          "",
                          "六條岔道"});
        worlds.push_back({"弓箭手村",
                          "村民們的家園，四周環繞著茂密的森林。",
                          1,
                          vector<Enemy>{},
                          "菇菇城堡",
                          "六條岔道",
                          "",
                          ""});
        worlds.push_back({"菇菇城堡",
                          "蘑菇人統治的城堡，充滿危險的敵人。",
                          3,
                          vector<Enemy>{
                              {"蘑菇戰士", 3, 60, 60, 12, 4, 30, 15},
                              {"蘑菇法師", 4, 80, 80, 15, 5, 40, 20},
                              {"蘑菇王", 5, 150, 150, 25, 10, 100, 50}},
                          "",
                          "弓箭手村",
                          "",
                          ""});
    }

    int
    getWorldIndex(string name)
    {
        for (int i = 0; i < worlds.size(); i++)
        {
            if (worlds[i].getName() == name)
            {
                return i;
            }
        }
        return -1; // 未找到
    }
    int MoveTo(int world_index, moveDirection direction)
    {
        if (world_index < 0 || world_index >= worlds.size())
        {
            // 跳出錯誤 方法待定
            return world_index; // 無效索引
        }
        string next_location = worlds[world_index].getConnection(direction);
        // 如果沒有連接，保持在原地
        if (next_location.empty())
        {
            return world_index;
        }
        int idx = getWorldIndex(next_location);
        // 如果找不到目的地名稱，保持在原地
        if (idx == -1)
        {
            return world_index;
        }
        return idx;
    }
    WorldModel &getWorld(int index)
    {
        if (index < 0 || index >= (int)worlds.size())
        {
            // 無效索引，回傳第 0 個 (guard)
            return worlds[0];
        }
        return worlds[index];
    }
    const WorldModel &getWorld(int index) const
    {
        if (index < 0 || index >= (int)worlds.size())
        {
            return worlds[0];
        }
        return worlds[index];
    }

private:
    vector<WorldModel> worlds;
};