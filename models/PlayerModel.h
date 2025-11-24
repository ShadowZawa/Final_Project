#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include <string>
#include "ItemModel.h"

enum jobType {
    WARRIOR,
    MAGE,
    ARCHER,
    THIEF
};


class PlayerModel {
    public:
        PlayerModel(int level = 1, jobType job = WARRIOR){
            this->level = level;
            this->job = job;
            this->hp = getMaxHp();
            this->mp = getMaxMp();
            this->gold = 100;
            this->exp = 0;
        }
        string getJob(){
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
        //取得函式
        int getCalcExpReq()
        {
            return level * level * 3; // 每級所需經驗
        }
        int getLevel() {return level;}
        int getHp() {return hp;}
        int getMaxHp() {return 100+level*10;}
        int getMp() {return mp;}
        int getMaxMp() {return 50+level*5;}
        int getGold() {return gold;}
        int getExp() {return exp;}
        int getAttrLvl() {return attr_lvl;}
        int getAttrDmg() {return attr_dmg;}
        int getAttrDef() {return attr_def;}
        int getAttrHp() {return attr_hp;}
        int getAttrMp() {return attr_mp;}
        bool gainExp(int amount) {
            exp += amount;
            // 檢查是否升級
            while (exp >= getCalcExpReq()) {
                exp -= getCalcExpReq();
                upgrade();
                return true;
            }
            return false;
        }
        //功能函式
        void upgrade() {
            level++;
            attr_lvl+=3;
        }
        void takeDamage(int damage) {
            hp -= damage; 
            if(hp < 0) {
                hp = 0;
                //死亡
            }
        }
        void heal(int amount) {
            hp += amount; 
            if(hp > getMaxHp()) {
                hp = getMaxHp();
            }
        }
        void useMp(int amount) {
            mp -= amount; 
            if(mp < 0) {
                mp = 0;
            }
        }
        void recoverMp(int amount) {
            mp += amount; 
            if(mp > getMaxMp()) {
                mp = getMaxMp();
            }
        }
        void addGold(int amount) {gold += amount;}
        void deductGold(int amount) {
            gold -= amount;
            if(gold < 0) {
                gold = 0;
            }
        }
        bool isEquipped(ItemModel::Type type) {
            switch(type) {
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
        void UnEquip(ItemModel::Type type) {
            switch(type) {
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
        }
        ItemModel& getEquippedItem(ItemModel::Type type) {
            switch(type) {
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
        bool Equip(const ItemModel& item) {
            // 武器裝備
            if (item.type == ItemModel::SWORD && this->job == WARRIOR){
                this->equippedWeapon = item;
                return true;
            }else if (item.type == ItemModel::WAND && this->job == MAGE){
                this->equippedWeapon = item;
                return true;
            }else if (item.type == ItemModel::BOW && this->job == ARCHER){
                this->equippedWeapon = item;
                return true;
            }else if (item.type == ItemModel::KNIFE && this->job == THIEF){
                this->equippedWeapon = item;
                return true;
            }
            // 防具裝備
            else if (item.type == ItemModel::HELMET){
                this->equippedHelmet = item;
                return true;
            }else if (item.type == ItemModel::CHESTPLATE){
                this->equippedChestplate = item;
                return true;
            }else if (item.type == ItemModel::LEGGINGS){
                this->equippedLeggings = item;
                return true;
            }else if (item.type == ItemModel::BOOTS){
                this->equippedBoots = item;
                return true;
            }
            return false;
        }
    void removeStat(int statType){
        if (statType < 0 || statType > 3) return;
        attr_lvl++;
        switch(statType){
            
            case 0:
                if(attr_dmg>0)
                    attr_dmg--;
                break;
            case 1:
                if(attr_def>0)
                    attr_def--;
                break;
            case 2:
                if(attr_hp>0)
                    attr_hp--;
                break;
            case 3:
                if(attr_mp>0)
                    attr_mp--;
                break;
            default:
                break;
        }
    }
    void addStat(int statType){
        if (statType < 0 || statType > 3) return;
        if (attr_lvl < 1) return;
        attr_lvl--;
        switch(statType){
            
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

    private:
    jobType job = WARRIOR;
    int level = 1;
    int hp = 100;
    int mp = 50;
    int gold = 100;
    int exp = 0;
    //attr

    int attr_lvl = 3;
    int attr_dmg = 0;
    int attr_def = 0;
    int attr_hp=0;
    int attr_mp=0;
    //equipment
    ItemModel equippedWeapon;
    ItemModel equippedHelmet;
    ItemModel equippedChestplate;
    ItemModel equippedLeggings;
    ItemModel equippedBoots;
    
};

#endif // PLAYERMODEL_H
