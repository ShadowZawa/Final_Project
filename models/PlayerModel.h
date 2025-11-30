#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include <string>
#include <vector>
#include "ItemModel.h"

using namespace std;

enum jobType
{
    WARRIOR,
    MAGE,
    ARCHER,
    THIEF
};
const vector<SuitModel> suits = {
    SuitModel{"iron_man", "鐵人套裝", {suitEffect{5, 0, 20, 10, 2}, suitEffect{10, 0, 50, 25, 4}}},
};
class PlayerModel
{
public:
    PlayerModel(int level = 1, jobType job = WARRIOR, int exp = 0, int attr_lvl = 0, int attr_dmg = 0, int attr_def = 0, int attr_hp = 0, int attr_mp = 0, int skill_lvl = 1, int skill1_lvl = 1, int skill2_lvl = 1)
    {
        this->level = level;
        this->job = job;
        this->hp = getMaxHp();
        this->mp = getMaxMp();
        this->gold = 100;
        this->exp = exp;
        this->attr_lvl = attr_lvl;
        this->attr_dmg = attr_dmg;
        this->attr_def = attr_def;
        this->attr_hp = attr_hp;
        this->attr_mp = attr_mp;

        this->skill1_lvl = skill1_lvl;
        this->skill2_lvl = skill2_lvl;
        this->skill_lvl = skill_lvl;
        checkSuitEffects();
        initClassEffect();
    }
    void addDamageBoost(int amount, int duration){
        damageBoostAmount = amount;
        damageBoostEndTime = std::chrono::steady_clock::now() + std::chrono::seconds(duration);
    }
    bool isDamageBoostActive() const {
        return std::chrono::steady_clock::now() < damageBoostEndTime;
    }
    int getDamageBoostAmount() const {
        if (isDamageBoostActive()) {
            return damageBoostAmount;
        }
        return 0;
    }
    jobType getJobType()
    {
        return job;
    }
    string getJob()
    {
        switch (job)
        {
        case WARRIOR:
            return "戰士";
        case MAGE:
            return "法師";
        case ARCHER:
            return "弓箭手";
        case THIEF:
            return "盜賊";
        default:
            return "未知";
        }
    }
    void setJob(jobType newJob)
    {
        this->job = newJob;
    }
    // 取得函式
    int getCalcExpReq()
    {
        return level * level * 3; // 每級所需經驗
    }
    int getLevel() { return level; }
    int getHp() { return hp; }
    int getMaxHp() { return 100 + level * 10 + attr_hp*5+basic_hp+suit_hp; }
    int getMp() { return mp; }
    int getMaxMp() { return 50 + level * 5 + attr_mp*5+basic_mp+suit_mp; }
    int getGold() { return gold; }
    int getExp() { return exp; }
    int getAttrLvl() { return attr_lvl; }
    int getAttrDmg() { return attr_dmg; }
    int getAttrDef() { return attr_def; }
    int getAttrHp() { return attr_hp; }
    int getAttrMp() { return attr_mp; }
    string getSkillName()
    {
        
        switch (job)
        {
        case WARRIOR:
            return "旋風斬";
        case MAGE:
            return "火球術";
        case ARCHER:
            return "多重箭";
        case THIEF:
            return "背刺";
        default:
            return "未知技能";
        }
    }
    bool gainExp(int amount)
    {
        exp += amount;
        // 檢查是否升級
        while (exp >= getCalcExpReq())
        {
            exp -= getCalcExpReq();
            upgrade();
            return true;
        }
        return false;
    }
    // 功能函式
    void upgrade()
    {
        level++;
        attr_lvl += 3;
        skill_lvl++;
        hp = getMaxHp();
        mp = getMaxMp();
    }
    int getSkillLevel() { return skill_lvl; }
    int getSkill1Level() { return skill1_lvl; }
    int getSkill2Level() { return skill2_lvl; }    
    void addSkillLevel(int skillNumber)
    {
        if (skill_lvl >0){
            skill_lvl--;
            if (skillNumber == 1)
                skill1_lvl++;
            else if (skillNumber == 2)
                skill2_lvl++;
        }
    }
    void removeSkillLevel(int skillNumber)
    {
        if (skillNumber == 1 && skill1_lvl > 1)
        {
            skill1_lvl--;
            skill_lvl++;
        }
        else if (skillNumber == 2 && skill2_lvl > 1)
        {
            skill2_lvl--;
            skill_lvl++;
        }
    }
    void takeDamage(int damage)
    {
        hp -= damage;
        if (hp < 0)
        {
            hp = 0;
            // 死亡
        }
    }
    void heal(int amount)
    {
        hp += amount;
        if (hp > getMaxHp())
        {
            hp = getMaxHp();
        }
    }
    void useMp(int amount)
    {
        mp -= amount;
        if (mp < 0)
        {
            mp = 0;
        }
    }
    void recoverMp(int amount)
    {
        mp += amount;
        if (mp > getMaxMp())
        {
            mp = getMaxMp();
        }
    }
    void addGold(int amount) { gold += amount; }
    void deductGold(int amount)
    {
        gold -= amount;
        if (gold < 0)
        {
            gold = 0;
        }
    }
    bool isEquipped(ItemModel::Type type)
    {
        switch (type)
        {
        case ItemModel::SWORD:
        case ItemModel::WAND:
        case ItemModel::BOW:
        case ItemModel::KNIFE:
        case ItemModel::WEAPON:
            return equippedWeapon.name != "";
        case ItemModel::HELMET:
            return equippedHelmet.name != "";
        case ItemModel::CHESTPLATE:
            return equippedChestplate.name != "";
        case ItemModel::LEGGINGS:
            return equippedLeggings.name != "";
        case ItemModel::BOOTS:
            return equippedBoots.name != "";
        default:
            return false;
        }
    }
    void UnEquip(ItemModel::Type type)
    {
        switch (type)
        {
        case ItemModel::SWORD:
        case ItemModel::WAND:
        case ItemModel::BOW:
        case ItemModel::KNIFE:
        case ItemModel::WEAPON:
            equippedWeapon = ItemModel();
            break;
        case ItemModel::HELMET:
            equippedHelmet = ItemModel();
            break;
        case ItemModel::CHESTPLATE:
            equippedChestplate = ItemModel();
            break;
        case ItemModel::LEGGINGS:
            equippedLeggings = ItemModel();
            break;
        case ItemModel::BOOTS:
            equippedBoots = ItemModel();
            break;
        default:
            break;
        }
        
        // 卸下裝備後重新計算套裝效果
        checkSuitEffects();
    }
    ItemModel &getEquippedItem(ItemModel::Type type)
    {
        switch (type)
        {
        case ItemModel::SWORD:
        case ItemModel::WAND:
        case ItemModel::BOW:
        case ItemModel::KNIFE:
        case ItemModel::WEAPON:
            return equippedWeapon;
        case ItemModel::HELMET:
            return equippedHelmet;
        case ItemModel::CHESTPLATE:
            return equippedChestplate;
        case ItemModel::LEGGINGS:
            return equippedLeggings;
        case ItemModel::BOOTS:
            return equippedBoots;
        default:
            throw std::invalid_argument("Invalid equipment type");
        }
    }
    bool Equip(const ItemModel &item)
    {
        bool equipped = false;
        // 武器裝備
        if (item.type == ItemModel::SWORD && this->job == WARRIOR)
        {
            this->equippedWeapon = item;
            equipped = true;
        }
        else if (item.type == ItemModel::WAND && this->job == MAGE)
        {
            this->equippedWeapon = item;
            equipped = true;
        }
        else if (item.type == ItemModel::BOW && this->job == ARCHER)
        {
            this->equippedWeapon = item;
            equipped = true;
        }
        else if (item.type == ItemModel::KNIFE && this->job == THIEF)
        {
            this->equippedWeapon = item;
            equipped = true;
        }
        // 防具裝備
        else if (item.type == ItemModel::HELMET)
        {
            this->equippedHelmet = item;
            equipped = true;
        }
        else if (item.type == ItemModel::CHESTPLATE)
        {
            this->equippedChestplate = item;
            equipped = true;
        }
        else if (item.type == ItemModel::LEGGINGS)
        {
            this->equippedLeggings = item;
            equipped = true;
        }
        else if (item.type == ItemModel::BOOTS)
        {
            this->equippedBoots = item;
            equipped = true;
        }
        
        // 裝備後重新計算套裝效果
        if (equipped) {
            checkSuitEffects();
        }
        
        return equipped;
    }
    void removeStat(int statType)
    {
        if (statType < 0 || statType > 3)
            return;
        attr_lvl++;
        switch (statType)
        {

        case 0:
            if (attr_dmg > 0)
                attr_dmg--;
            break;
        case 1:
            if (attr_def > 0)
                attr_def--;
            break;
        case 2:
            if (attr_hp > 0)
                attr_hp--;
            break;
        case 3:
            if (attr_mp > 0)
                attr_mp--;
            break;
        default:
            break;
        }
    }
    int CalcDef()
    {
        int def = (level) + (attr_def);
        if (equippedHelmet.name != "")
        {
            def += equippedHelmet.effect;
        }
        if (equippedChestplate.name != "")
        {
            def += equippedChestplate.effect;
        }
        if (equippedLeggings.name != "")
        {
            def += equippedLeggings.effect;
        }
        if (equippedBoots.name != "")
        {
            def += equippedBoots.effect;
        }
        def += basic_def;
        def += suit_def;
        return def;
    }
    int CalcDamage()
    {
        int dmg = (level * 2) + (attr_dmg);
        if (equippedWeapon.name != "")
        {
            dmg += equippedWeapon.effect;
        }
        auto now = std::chrono::steady_clock::now();
        if (now < damageBoostEndTime) {
            dmg += damageBoostAmount;
        }else{
            damageBoostAmount = 0;

        }
        dmg += basic_dmg;
        dmg += suit_dmg;
        return dmg;
    }
    
    // 獲取套裝效果資訊
    map<string, int> getSuitCounts() const {
        map<string, int> suitCount;
        if (equippedHelmet.name != "" && equippedHelmet.suit != "")
            suitCount[equippedHelmet.suit]++;
        if (equippedChestplate.name != "" && equippedChestplate.suit != "")
            suitCount[equippedChestplate.suit]++;
        if (equippedLeggings.name != "" && equippedLeggings.suit != "")
            suitCount[equippedLeggings.suit]++;
        if (equippedBoots.name != "" && equippedBoots.suit != "")
            suitCount[equippedBoots.suit]++;
        if (equippedWeapon.name != "" && equippedWeapon.suit != "")
            suitCount[equippedWeapon.suit]++;
        return suitCount;
    }
    
    int getSuitDmg() const { return suit_dmg; }
    int getSuitDef() const { return suit_def; }
    int getSuitHp() const { return suit_hp; }
    int getSuitMp() const { return suit_mp; }
    
    void addStat(int statType)
    {
        if (statType < 0 || statType > 3)
            return;
        if (attr_lvl < 1)
            return;
        attr_lvl--;
        switch (statType)
        {
        case 0:
            attr_dmg++;
            break;
        case 1:
            attr_def++;
            break;
        case 2:
            attr_hp++;
            break;
        case 3:
            attr_mp++;
            break;
        default:
            break;
        }
    }
    void checkSuitEffects(){
        suit_dmg = 0;
        suit_def = 0;
        suit_hp = 0;
        suit_mp = 0;
        //count equipped suit items
        map<string, int> suitCount;
        if (equippedHelmet.name != "" && equippedHelmet.suit != "")
        {
            suitCount[equippedHelmet.suit]++;
        }
        if (equippedChestplate.name != "" && equippedChestplate.suit != "")
        {
            suitCount[equippedChestplate.suit]++;
        }
        if (equippedLeggings.name != "" && equippedLeggings.suit != "")
        {
            suitCount[equippedLeggings.suit]++;
        }
        if (equippedBoots.name != "" && equippedBoots.suit != "")
        {
            suitCount[equippedBoots.suit]++;
        }
        if (equippedWeapon.name != "" && equippedWeapon.suit != "")
        {
            suitCount[equippedWeapon.suit]++;
        }

        //apply suit effects
        for (const auto &pair : suitCount)
        {
            string suitName = pair.first;
            int count = pair.second;
            for (const auto &suit : suits)
            {
                if (suit.id == suitName)
                {
                    for (const suitEffect &effect : suit.effects)
                    {
                        if (count >= effect.count)
                        {
                            suit_dmg += effect.dmg;
                            suit_def += effect.def;
                            suit_hp += effect.hp;
                            suit_mp += effect.mp;
                        }
                    }
                }
            }
        }
    }
    void initClassEffect(){
        switch (job)
        {
        case WARRIOR:
            basic_dmg = 0;
            basic_def = 0;
            basic_hp = 50;
            basic_mp = 0;
            break;
        case MAGE:
            basic_dmg = 0;
            basic_def = 0;
            basic_hp = 0;
            basic_mp = 50;
            break;
        case ARCHER:
            basic_dmg = 5;
            basic_def = 0;
            basic_hp = 0;
            basic_mp = 0;
            break;
        case THIEF:
            basic_dmg = 3;
            basic_def = 0;
            basic_hp = 20;
            basic_mp = 0;
            break;
        default:
            break;
        }
    }
private:

    // potion boost
    int damageBoostAmount = 0;
    std::chrono::steady_clock::time_point damageBoostEndTime;

    jobType job = WARRIOR;
    int level = 1;
    int hp = 100;
    int mp = 50;
    int gold = 100;
    int exp = 0;
    //skill level
    int skill_lvl = 1;
    int skill1_lvl = 1;
    int skill2_lvl = 1;
    //class basic attr
    int basic_dmg = 0;
    int basic_def = 0;
    int basic_hp = 0;
    int basic_mp = 0;
    //suit attr
    int suit_dmg = 0;
    int suit_def = 0;
    int suit_hp = 0;
    int suit_mp = 0;

    // attr
    int attr_lvl = 3;
    int attr_dmg = 0;
    int attr_def = 0;
    int attr_hp = 0;
    int attr_mp = 0;
    // equipment
    ItemModel equippedWeapon;
    ItemModel equippedHelmet;
    ItemModel equippedChestplate;
    ItemModel equippedLeggings;
    ItemModel equippedBoots;
};

#endif // PLAYERMODEL_H
