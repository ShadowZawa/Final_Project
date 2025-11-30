#ifndef BACKPACKUI_H
#define BACKPACKUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include "../managers/GameController.h"
#include <vector>
#include <string>
using namespace ftxui;

// 背包介面 (B鍵)
class BackpackUI {
public:
    static Component Create(GameController &controller, function<void()> onClose);
};

inline Component BackpackUI::Create(GameController &controller, std::function<void()> onClose) {
    auto selected_index = std::make_shared<int>(0);
    auto scroll_offset = std::make_shared<int>(0);  // 新增滾動偏移量
    
    auto use_btn = Button("使用/裝備", [&controller, selected_index] {
        auto items = controller.getInventory().getItems();
        if (*selected_index >= 0 && *selected_index < (int)items.size()) {
            controller.UseInventoryItem(*selected_index);
        }
    });
    
    
    auto drop_btn = Button("丟棄", [&controller, selected_index] {
        auto items = controller.getInventory().getItems();
        if (*selected_index >= 0 && *selected_index < (int)items.size()) {
            controller.DropItem(*selected_index);
        }
    });
    
    auto close_btn = Button("關閉 [ESC]", onClose);
    
    auto button_container = Container::Vertical({use_btn, drop_btn, close_btn});
    
    // 整個容器捕獲方向鍵事件
    auto main_container = Container::Horizontal({
        button_container
    });
    
    auto with_events = CatchEvent(main_container, [&controller, selected_index, scroll_offset](Event event) {
        auto items = controller.getInventory().getItems();
        if (items.empty()) return false;
        
        const int visible_items = 12;  // 可見物品數量
        
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            *selected_index = (*selected_index + 1) % (int)items.size();
            
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
            *selected_index = (*selected_index - 1 + (int)items.size()) % (int)items.size();
            
            // 自動向上滾動
            if (*selected_index < *scroll_offset) {
                *scroll_offset = *selected_index;
            }
            // 循環回到底部時調整滾動
            if (*selected_index == (int)items.size() - 1) {
                *scroll_offset = std::max(0, (int)items.size() - visible_items);
            }
            return true;
        }
        return false;
    });
    
    return Renderer(with_events, [&controller, selected_index, scroll_offset, use_btn, drop_btn, close_btn]() -> Element {
        auto items = controller.getInventory().getItems();
        bool is_empty = items.empty();
        int idx = *selected_index;
        
        // 修正索引範圍
        if (!is_empty && idx >= (int)items.size()) {
            idx = 0;
            *selected_index = 0;
            *scroll_offset = 0;
        }
        
        std::string name = "無";
        std::string desc = "";
        std::string type_str = "";
        std::string suit;
        int effect = 0;
        bool is_potion = false;
        bool is_equip = false;
        
        if (!is_empty && idx >= 0 && idx < (int)items.size()) {
            const auto& item = items[idx];
            name = item.name;
            if (item.star > 0) {
                name += " +" + std::to_string(item.star);
            }
            desc = item.description;
            effect = item.effect;
            suit = (item.suit == "") ? "無套裝" : item.suit;
            switch(item.type) {
                case ItemModel::BOW: type_str = "弓箭"; is_equip = true; break;
                case ItemModel::SWORD: type_str = "長劍"; is_equip = true; break;
                case ItemModel::KNIFE: type_str = "匕首"; is_equip = true; break;
                case ItemModel::WAND: type_str = "魔杖"; is_equip = true; break;
                case ItemModel::HELMET: type_str = "頭盔"; is_equip = true; break;
                case ItemModel::CHESTPLATE: type_str = "胸甲"; is_equip = true; break;
                case ItemModel::LEGGINGS: type_str = "護腿"; is_equip = true; break;
                case ItemModel::BOOTS: type_str = "靴子"; is_equip = true; break;
                case ItemModel::SCROLL_TELEPORT: type_str = "傳送卷軸"; break;
                case ItemModel::SCROLL_SKILL: type_str = "技能卷軸"; break;
                case ItemModel::SCROLL_UPGRADE: type_str = "強化卷軸"; break;
                case ItemModel::MISC: type_str = "雜物"; break;
                case ItemModel::POTION_HEALHP: type_str = "回血藥水"; is_potion = true; break;
                case ItemModel::POTION_HEALMP: type_str = "回魔藥水"; is_potion = true; break;
                case ItemModel::POTION_DAMAGEBOOST: type_str = "傷害提升藥水"; is_potion = true; break;
                default: type_str = "未知類型"; break;
            }
        }
        
        // 生成物品列表（只顯示可見範圍）
        Elements item_elements;
        const int visible_items = 12;
        int start_idx = *scroll_offset;
        int end_idx = std::min(start_idx + visible_items, (int)items.size());
        
        for (int i = start_idx; i < end_idx; ++i) {
            auto item_text = text("  " + items[i].name);
            if (i == idx) {
                item_text = text("> " + items[i].name) | bold | color(Color::Yellow);
            }
            item_elements.push_back(item_text);
        }
        
        if (item_elements.empty()) {
            item_elements.push_back(text("  背包是空的") | dim);
        }
        
        // 顯示滾動指示器
        std::string scroll_indicator = "";
        if (!items.empty()) {
            if (*scroll_offset > 0) {
                scroll_indicator += "↑ ";
            }
            scroll_indicator += std::to_string(idx + 1) + "/" + std::to_string(items.size());
            if (end_idx < (int)items.size()) {
                scroll_indicator += " ↓";
            }
        }
        
        return window(text("背包"), hbox({
            vbox({
                hbox({
                    text("物品列表 (↑↓ 選擇)") | center | flex,
                    text(scroll_indicator) | dim
                }),
                separator(),
                vbox(item_elements) | flex
            }) | size(WIDTH, EQUAL, 25) | border,
            
            vbox({
                text("詳細資料") | center,
                separator(),
                text("名稱: " + name),
                text("類型: " + type_str),
                text("效果: " + std::to_string(effect)),
                separator(),
                text(desc) | flex,
                text("套裝: " + suit),
                separator(),
                hbox({
                    (!is_empty ? use_btn->Render() : text("")),
                    (!is_empty ? drop_btn->Render() : text("")),
                    filler(),
                    close_btn->Render()
                }) | center
            }) | flex | border
        })) | center | size(WIDTH, EQUAL, 80) | size(HEIGHT, EQUAL, 20);
    });
}

#endif // BACKPACKUI_H
