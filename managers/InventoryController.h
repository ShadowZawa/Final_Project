#include<vector>
#include "../models/ItemModel.h"
using namespace std;

class InventoryController
{
public:
    InventoryController(){
        this->inventory = vector<ItemModel>();
        ItemModel item;
        item.name = "小補血藥";
        item.description = "恢復5點生命值的藥水。";
        item.price = 50;
        item.effect = 5;
        item.type = ItemModel::POTION;
        inventory.push_back(item);
        ItemModel item2;
        item2.name = "小回家卷軸";
        item2.description = "傳送回維多利亞港";
        item2.price = 10;
        item2.effect = 0;
        item2.type = ItemModel::SCROLL_TELEPORT;
        inventory.push_back(item2);
        ItemModel item3;
        item3.name = "鐵劍";
        item3.description = "基礎的近戰武器，攻擊力+5。";
        item3.price = 200;
        item3.effect = 5;
        item3.type = ItemModel::SWORD;
        inventory.push_back(item3);
        ItemModel item4;
        item4.name = "強化卷軸";
        item4.description = "可用於強化裝備。";
        item4.price = 10;
        item4.effect = 1;
        item4.type = ItemModel::SCROLL_UPGRADE;
        inventory.push_back(item4);
    }
    vector<ItemModel> getItems() {return this->inventory;}
    
    void addItem(const ItemModel& item) {
        inventory.push_back(item);
    }

    void removeItem(int index) {
        if (index >= 0 && index < inventory.size()) {
            inventory.erase(inventory.begin() + index);
        }
    }

    ItemModel& getItem(int index) {
        return inventory[index];
    }

    int getSize() const {
        return inventory.size();
    }
private:
    vector<ItemModel> inventory;
};