#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../models/WorldModel.h"
#include "../models/PlayerModel.h"
#include "WorldController.h"
#include "InventoryController.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
using namespace std;

class GameController
{
public:
    GameController()
    {
        this->player = PlayerModel(1, WARRIOR);
        this->worldController = WorldController();
        this->inventoryController = InventoryController();
        this->current_loc = 0;
        this->is_combat = false;
        
    }
    PlayerModel getPlayer() const
    {
        return this->player;
    }
    PlayerModel& getMutablePlayer()
    {
        return this->player;
    }
    string getCurrentLocationName() const
    {
        return this->worldController.getWorld(this->current_loc).getName();
    }
    void Move(moveDirection direction)
    {
        // 根據方向更新當前地點
        switch (direction)
        {
        case moveDirection::NORTH:

            this->current_loc = this->worldController.MoveTo(this->current_loc, moveDirection::NORTH);
            // 更新current_loc為北方地點
            break;
        case moveDirection::SOUTH:
            this->current_loc = this->worldController.MoveTo(this->current_loc, moveDirection::SOUTH);
            // 更新current_loc為南方地點
            break;
        case moveDirection::EAST:
            this->current_loc = this->worldController.MoveTo(this->current_loc, moveDirection::EAST);
            // 更新current_loc為東方地點
            break;
        case moveDirection::WEST:
            this->current_loc = this->worldController.MoveTo(this->current_loc, moveDirection::WEST);
            // 更新current_loc為西方地點
            break;
        }
    }
    void BuyItem(const ItemModel& item) {
        if (getCurrentLocationName() != "維多利亞港" && getCurrentLocationName() != "弓箭手村") {
            LogEvent("只能在村莊購買物品。");
            return;
        }
        if (player.getGold() >= item.price) {
            player.deductGold(item.price);
            inventoryController.addItem(item);
            LogEvent("購買了 " + item.name + "，花費 " + to_string(item.price) + " 金幣。");
        } else {
            LogEvent("金幣不足，無法購買 " + item.name + "。");
        }
    }
    bool isCombat() const
    {
        return this->is_combat;
    }
    void Combat()
    {
        if (is_combat)
        {
            return;
        }
        const auto &enemies = worldController.getWorld(current_loc).getCurrentEnemies();
        if (enemies.size() > 0)
        {
            int random_index = std::rand() % enemies.size();
            this->is_combat = true;
            enemy_index = random_index;
            Enemy current_enemy = enemies[enemy_index];
            LogEvent("與" + current_enemy.name + " (LV" + std::to_string(current_enemy.level) + ") 展開戰鬥!");
        }
        else
        {
            // 錯誤處理
            LogEvent("沒有可進行戰鬥的敵人");
        }
    }
    const vector<Enemy> &getCurrentEnemies() const
    {
        return this->worldController.getWorld(this->current_loc).getCurrentEnemies();
    }
    vector<string> getLog(int size) const
    {
        // 回傳最新優先的最後 size 筆紀錄（index 最後面代表最新）
        vector<string> result;
        if (size <= 0 || data_log.empty())
        {
            return result;
        }
        // 僅回傳已達顯示時間的訊息
        auto now = std::chrono::steady_clock::now();
        for (int i = (int)data_log.size() - 1; i >= 0 && (int)result.size() < size; --i)
        {
            const auto &entry = data_log[i];
            if (entry.show_at <= now) {
                result.push_back(entry.msg);
            }
        }
        return result; // result[0] 為最新紀錄
    }
    void LogEvent(const string &event, float delay_s=0)
    {
        // 將訊息加入佇列，設定延遲顯示時間，不阻塞其他邏輯。
        auto show_time = std::chrono::steady_clock::now() + std::chrono::microseconds((int)(delay_s * 1000));
        data_log.push_back({event, show_time});
    }
    // 戰鬥行為
    void Attack(int type=0)
    {
        if (!is_combat)
            return;
        auto &enemies = worldController.getWorld(current_loc).getCurrentEnemies();
        if (enemy_index < 0 || enemy_index >= (int)enemies.size())
            return;
        int dmg = player.CalcDamage();
        if (type == 0){
            dmg += player.getSkill1Level(); // 普攻加成
        } else if (type == 1){
            dmg += player.getSkill2Level() * 5; // 職業技能加成
        }
        enemies[enemy_index].hp -= dmg;
        LogEvent("你對" + enemies[enemy_index].name + "造成 " + to_string(dmg) + " 點傷害", 0.1f);
        if (enemies[enemy_index].hp <= 0)
        {
            LogEvent(enemies[enemy_index].name + " 被擊敗！", 0.5f);
            // 移除已死亡的敵人
            if (player.gainExp(enemies[enemy_index].calcDropExp()))
            {
                LogEvent("升級！目前等級：" + to_string(player.getLevel()), 1.0f);
            }
            else
            {
                LogEvent("獲得 " + to_string(enemies[enemy_index].calcDropExp()) + " 點經驗值", 1.0f);
            }
            // 掉落物品
            for (const auto& it : enemies[enemy_index].dropItems) {
                inventoryController.addItem(it);
                LogEvent("獲得物品：" + it.name, 1.5f);
            }
            enemies.erase(enemies.begin() + enemy_index);
            if (enemies.size() < 2){
                LogEvent("刷新了新的敵人", 1.5f);
                worldController.getWorld(current_loc).SpawnRandomEnemies();
            }
            is_combat = false;
            enemy_index = -1;
            return;
        }
        // 怪物反擊
        int edmg = enemies[enemy_index].attack;
        player.takeDamage(edmg);
        LogEvent(enemies[enemy_index].name + " 反擊你造成 " + to_string(edmg) + " 點傷害", 2.0f);
        if (player.getHp() <= 0)
        {
            is_combat = false;
            player.heal(player.getMaxHp()); // 死亡後恢復滿血
            player.recoverMp(player.getMaxMp()); // 死亡後恢復滿魔
            current_loc = 0; // 傳送回維多利亞港
            LogEvent("你已死亡。", 2.5f);
            LogEvent("頭腦一震暈眩，你發現似乎回到了維多利亞港...", 2.5f);
        }
    }

    void Flee()
    {
        if (!is_combat)
            return;
        is_combat = false;
        enemy_index = -1;
        LogEvent("你成功逃跑，離開戰鬥。");
    }

    InventoryController& getInventory() {
        return inventoryController;
    }
    
    void UseInventoryItem(int index) {
        if (index < 0 || index >= inventoryController.getSize()) return;
        
        ItemModel& item = inventoryController.getItem(index);
        
        switch (item.type) {
            case ItemModel::POTION_DAMAGEBOOST:
                // 增加傷害提升效果，持續30秒
                player.addDamageBoost(item.effect, item.star);
                LogEvent("使用了 " + item.name + "，傷害提升 " + to_string(item.effect) + " 點，持續" + to_string(item.star) + "秒。");
                inventoryController.removeItem(index);
                break;
            case ItemModel::POTION_HEALHP:
                player.heal(item.effect);
                LogEvent("使用了 " + item.name + "，恢復了 " + to_string(item.effect) + " 點生命。");
                inventoryController.removeItem(index);
                break;
                
            case ItemModel::POTION_HEALMP:
                player.recoverMp(item.effect);
                LogEvent("使用了 " + item.name + "，恢復了 " + to_string(item.effect) + " 點魔力。");
                inventoryController.removeItem(index);
                break;
                
            case ItemModel::SCROLL_TELEPORT:
                if (is_combat) {
                    LogEvent("戰鬥中無法使用回家卷軸！");
                    return;
                }
                this->current_loc = 0;
                LogEvent("使用了 " + item.name + "，傳送回維多利亞港。");
                inventoryController.removeItem(index);
                break;
                
            case ItemModel::SCROLL_SKILL: {
                if (!is_combat) {
                    LogEvent("沒有選定的敵人。");
                    return;
                }
                auto &enemies = worldController.getWorld(current_loc).getCurrentEnemies();
                LogEvent("對" + enemies[enemy_index].name + "使用了 " + item.name + "卷軸");
                enemies[enemy_index].hp -= item.effect;
                
                if (enemies[enemy_index].hp <= 0) {
                    LogEvent(enemies[enemy_index].name + " 被擊敗！");
                    
                    if (player.gainExp(enemies[enemy_index].calcDropExp())) {
                        LogEvent("升級！目前等級：" + to_string(player.getLevel()));
                    } else {
                        LogEvent("獲得 " + to_string(enemies[enemy_index].calcDropExp()) + " 點經驗值");
                    }
                    
                    for (const auto &it : enemies[enemy_index].dropItems) {
                        inventoryController.addItem(it);
                        LogEvent("獲得物品：" + it.name);
                    }
                    
                    enemies.erase(enemies.begin() + enemy_index);
                    if (enemies.size() < 2) {
                        LogEvent("刷新了新的敵人", 1);
                        worldController.getWorld(current_loc).SpawnRandomEnemies();
                    }
                    is_combat = false;
                    enemy_index = -1;
                    return;
                }
                inventoryController.removeItem(index);
                break;
            }
            
            case ItemModel::WAND:
            case ItemModel::BOW:
            case ItemModel::SWORD:
            case ItemModel::KNIFE:
            case ItemModel::HELMET:
            case ItemModel::CHESTPLATE:
            case ItemModel::LEGGINGS:
            case ItemModel::BOOTS:
                EquipItem(index);
                break;
                
            default:
                LogEvent("此物品無法直接使用。");
                break;
        }
    }

    void EquipItem(int index) {
        if (index < 0 || index >= inventoryController.getSize()) return;
        ItemModel& item = inventoryController.getItem(index);
        if (item.type == ItemModel::WAND || item.type == ItemModel::BOW || item.type == ItemModel::SWORD || item.type == ItemModel::KNIFE || item.type == ItemModel::HELMET || item.type == ItemModel::CHESTPLATE || item.type == ItemModel::LEGGINGS || item.type == ItemModel::BOOTS) {
            if (jobType::WARRIOR != player.getJobType() && (item.type == ItemModel::SWORD)) {
                LogEvent("只有戰士職業可以裝備劍類武器！");
                return;
            }
            if (jobType::MAGE != player.getJobType() && (item.type == ItemModel::WAND)) {
                LogEvent("只有法師職業可以裝備魔杖類武器！");
                return;
            }
            if (jobType::ARCHER != player.getJobType() && (item.type == ItemModel::BOW)) {
                LogEvent("只有弓箭手職業可以裝備弓類武器！");
                return;
            }
            if (jobType::THIEF != player.getJobType() && (item.type == ItemModel::KNIFE)) {
                LogEvent("只有盜賊職業可以裝備匕首類武器！");
                return;
            }
            LogEvent("裝備了 " + item.name);
            if (player.isEquipped(item.type)) {
                inventoryController.addItem(player.getEquippedItem(item.type));
                player.UnEquip(item.type);
            }
             player.Equip(item);
             inventoryController.removeItem(index);
        } else {
            LogEvent("此物品無法裝備。");
        }
    }

    void DropItem(int index) {
        if (index < 0 || index >= inventoryController.getSize()) return;
        ItemModel item = inventoryController.getItem(index);
        int p = item.price;
        inventoryController.removeItem(index);
        LogEvent("丟棄了 " + item.name);
        if (p>0){

            player.addGold(p);
            LogEvent("獲得 " + to_string(p) + " G");
        }
    }
    
    // 強化裝備（按照類型）
    bool EnhanceEquipment(ItemModel::Type equipType) {
        if (!player.isEquipped(equipType)) {
            LogEvent("該裝備欄位無裝備！");
            return false;
        }
        
        ItemModel& item = player.getEquippedItem(equipType);
        
        // 計算成功率：100->100->70->50->10->10->10
        int success_rate;
        if (item.star == 0) success_rate = 100;
        else if (item.star == 1) success_rate = 100;
        else if (item.star == 2) success_rate = 70;
        else if (item.star == 3) success_rate = 50;
        else success_rate = 10;
        
        // 計算所需金幣：10 * (star + 1)
        int cost = 10 * (item.star + 1);
        
        if (player.getGold() < cost) {
            LogEvent("金幣不足！需要 " + to_string(cost) + " G");
            return false;
        }
        
        // 扣除金幣
        player.deductGold(cost);
        
        // 判斷是否成功
        int random = std::rand() % 100;
        if (random < success_rate) {
            item.star++;
            item.effect += 5; // 每次強化增加 5 點效果
            LogEvent("強化成功！" + item.name + " +" + to_string(item.star));
            return true;
        } else {
            LogEvent("強化失敗！");
            return false;
        }
    }

private:
    bool is_combat;
    int enemy_index;
    struct LogEntry {
        string msg;
        std::chrono::steady_clock::time_point show_at;
    };
    vector<LogEntry> data_log;
    int current_loc;
    mutable WorldController worldController;
    PlayerModel player;
    mutable InventoryController inventoryController;
};

#endif // GAMECONTROLLER_H