#pragma once
// 物品資料


#include <string>
using namespace std;

struct ItemModel
{
    string name;
    string description;
    int price;
    int effect; // 效果值（如回復量）
    int star=0; // 強化等級
    enum Type
    {
        POTION, // 藥水
        SCROLL, // 卷軸
        SCROLL_TELEPORT, // 傳送卷軸
        SCROLL_SKILL,    // 技能卷軸
        SCROLL_UPGRADE,  // 強化卷軸
        MISC,   // 雜物

        WEAPON, // 武器
        HELMET,  // 頭盔
        CHESTPLATE, // 胸甲
        LEGGINGS,   // 護腿
        BOOTS,       // 靴子
        BOW,    // 弓
        SWORD,  // 劍
        KNIFE,  // 匕首
        WAND,   // 魔杖
    } type;
    string suit; // 套裝名稱

};

class SuitModel{
public:
    string name;
    string description;
    int effect; // 套裝效果(全屬性加成)
};