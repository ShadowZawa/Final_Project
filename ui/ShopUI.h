#ifndef SHOPUI_H
#define SHOPUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <vector>
#include "../models/ItemModel.h"
#include "../managers/GameController.h"
using namespace ftxui;

// 商店介面 (S鍵)
class ShopUI {
public:
    static Component Create(GameController& controller, std::function<void()> onClose);
};

inline Component ShopUI::Create(GameController& controller, std::function<void()> onClose) {
    auto selected_index = std::make_shared<int>(0);
    auto scroll_offset = std::make_shared<int>(0);
    
    // 商店商品列表
    static const std::vector<ItemModel> shop_items = {
        // 藥水
        {"小型生命藥水", "恢復50點生命值的藥水。", 10, 50, 0, ItemModel::POTION_HEALHP, ""},
        {"中型生命藥水", "恢復150點生命值的藥水。", 25, 150, 0, ItemModel::POTION_HEALHP, ""},
        {"大型生命藥水", "恢復300點生命值的藥水。", 50, 300, 0, ItemModel::POTION_HEALHP, ""},
        {"小型魔力藥水", "恢復50點魔力值的藥水。", 10, 50, 0, ItemModel::POTION_HEALMP, ""},
        {"中型魔力藥水", "恢復150點魔力值的藥水。", 25, 150, 0, ItemModel::POTION_HEALMP, ""},
        {"大型魔力藥水", "恢復300點魔力值的藥水。", 50, 300, 0, ItemModel::POTION_HEALMP, ""},
        {"傷害提升藥水", "提升20點傷害持續30秒。", 40, 20, 30, ItemModel::POTION_DAMAGEBOOST, ""},
        
        // 卷軸
        {"回家卷軸(維多利亞港)", "使用後可傳送回維多利亞港。", 40, 0, 0, ItemModel::SCROLL_TELEPORT, ""},
        {"回家卷軸(弓箭手村)", "使用後可傳送回弓箭手村。", 40, 4, 0, ItemModel::SCROLL_TELEPORT, ""},
        {"強化卷軸", "使用後有機率提升一件裝備的強化等級。", 10, 1, 0, ItemModel::SCROLL_UPGRADE, ""},
        {"技能卷軸(天怒)", "對怪物造成999點傷害", 100, 999, 0, ItemModel::SCROLL_SKILL, ""},
        // 每個職業的武器
        {"鋼鐵劍", "一把堅固的鋼鐵劍，攻擊力+15。", 200, 15, 0, ItemModel::SWORD, ""},
        {"獵人弓", "適合遠程攻擊的獵人弓，攻擊力+12。", 180, 12, 0, ItemModel::BOW, ""},
        {"精鋼匕首", "敏捷刺客的首選，攻擊力+10。", 150, 10, 0, ItemModel::KNIFE, ""},
        {"魔法杖", "蘊含魔力的魔法杖，攻擊力+8。", 170, 8, 0, ItemModel::WAND, ""},
        // 防具
        {"鐵靴", "基本防護的鐵製靴子，防禦+4。", 120, 4, 0, ItemModel::BOOTS, "iron_man"},
        {"鐵製護腿", "基本防護的鐵製護腿，防禦+6。", 140, 6, 0, ItemModel::LEGGINGS, "iron_man"},
        {"鐵甲", "基本防護的鐵製胸甲，防禦+8。", 160, 8, 0, ItemModel::CHESTPLATE, "iron_man"},
        {"鐵盔", "基礎防護的鐵製頭盔，防禦+5。", 100, 5, 0, ItemModel::HELMET, "iron_man"},

    };

    auto buy_btn = Button("購買 [Enter]", [&controller, selected_index]() {
        int idx = *selected_index;
        if (idx < 0 || idx >= (int)shop_items.size()) return;
        
        const ItemModel& item = shop_items[idx];
        controller.BuyItem(item);
    });
    
    auto close_btn = Button("關閉 [ESC]", onClose);
    
    auto button_container = Container::Vertical({buy_btn, close_btn});
    
    // 方向鍵事件處理
    auto with_events = CatchEvent(button_container, [selected_index, scroll_offset](Event event) {
        const int visible_items = 12;
        
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            *selected_index = (*selected_index + 1) % (int)shop_items.size();
            
            // 自動向下滾動
            if (*selected_index >= *scroll_offset + visible_items) {
                *scroll_offset = *selected_index - visible_items + 1;
            }
            // 循環回到頂部時重置滾動
            if (*selected_index == 0) {
                *scroll_offset = 0;
            }
            return true;
        }
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            *selected_index = (*selected_index - 1 + (int)shop_items.size()) % (int)shop_items.size();
            
            // 自動向上滾動
            if (*selected_index < *scroll_offset) {
                *scroll_offset = *selected_index;
            }
            // 循環回到底部時調整滾動
            if (*selected_index == (int)shop_items.size() - 1) {
                *scroll_offset = std::max(0, (int)shop_items.size() - visible_items);
            }
            return true;
        }
        return false;
    });
    
    return Renderer(with_events, [&controller, selected_index, scroll_offset, buy_btn, close_btn] {
        int idx = *selected_index;
        int player_gold = controller.getPlayer().getGold();
        
        // 生成商品列表（只顯示可見範圍）
        Elements item_elements;
        const int visible_items = 12;
        int start_idx = *scroll_offset;
        int end_idx = std::min(start_idx + visible_items, (int)shop_items.size());
        
        for (int i = start_idx; i < end_idx; ++i) {
            const auto& item = shop_items[i];
            std::string display = item.name + " - " + std::to_string(item.price) + " G";
            
            auto item_text = text("  " + display);
            if (i == idx) {
                item_text = text("> " + display) | bold | color(Color::Yellow);
            }
            
            // 標示買不起的商品
            if (player_gold < item.price) {
                item_text = item_text | dim;
            }
            
            item_elements.push_back(item_text);
        }
        
        // 顯示滾動指示器
        std::string scroll_indicator = "";
        if (*scroll_offset > 0) {
            scroll_indicator += "↑ ";
        }
        scroll_indicator += std::to_string(idx + 1) + "/" + std::to_string(shop_items.size());
        if (end_idx < (int)shop_items.size()) {
            scroll_indicator += " ↓";
        }
        
        // 當前選中商品的詳細資訊
        const ItemModel& current_item = shop_items[idx];
        std::string type_str = "";
        switch(current_item.type) {
            case ItemModel::POTION: type_str = "藥水"; break;
            case ItemModel::SCROLL_TELEPORT: type_str = "傳送卷軸"; break;
            case ItemModel::SWORD: type_str = "長劍"; break;
            case ItemModel::BOW: type_str = "弓箭"; break;
            case ItemModel::KNIFE: type_str = "匕首"; break;
            case ItemModel::WAND: type_str = "魔杖"; break;
            case ItemModel::HELMET: type_str = "頭盔"; break;
            case ItemModel::CHESTPLATE: type_str = "胸甲"; break;
            case ItemModel::LEGGINGS: type_str = "護腿"; break;
            case ItemModel::BOOTS: type_str = "靴子"; break;
            case ItemModel::MISC: type_str = "雜物"; break;
            default: type_str = "未知"; break;
        }
        
        bool can_afford = player_gold >= current_item.price;
        
        return vbox({
            text("【商店介面】") | bold | center,
            separator(),
            hbox({
                vbox({
                    hbox({
                        text("商品列表 (↑↓ 選擇)") | center | flex,
                        text(scroll_indicator) | dim
                    }),
                    separator(),
                    vbox(item_elements) | flex
                }) | size(WIDTH, EQUAL, 35) | border,
                
                vbox({
                    text("商品詳情") | center,
                    separator(),
                    text("名稱: " + current_item.name),
                    text("類型: " + type_str),
                    text("價格: " + std::to_string(current_item.price) + " G") | 
                        (can_afford ? color(Color::Green) : color(Color::Red)),
                    text("效果: " + std::to_string(current_item.effect)),
                    separator(),
                    text("描述:") | bold,
                    text(current_item.description) | dim,
                    separator(),
                    text("持有金幣: " + std::to_string(player_gold) + " G") | bold,
                    separator(),
                    hbox({
                        (can_afford ? buy_btn->Render() : text("金幣不足") | dim) | size(WIDTH, EQUAL, 15),
                        text(" "),
                        close_btn->Render() | size(WIDTH, EQUAL, 15)
                    }) | center
                }) | flex | border
            }),
        }) | border | size(WIDTH, EQUAL, 80) | size(HEIGHT, EQUAL, 25) | center;
    });
}

#endif // SHOPUI_H
