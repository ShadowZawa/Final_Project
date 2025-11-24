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
    
    auto with_events = CatchEvent(main_container, [&controller, selected_index](Event event) {
        auto items = controller.getInventory().getItems();
        if (items.empty()) return false;
        
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            *selected_index = (*selected_index + 1) % (int)items.size();
            return true;
        }
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            *selected_index = (*selected_index - 1 + (int)items.size()) % (int)items.size();
            return true;
        }
        return false;
    });
    
    return Renderer(with_events, [&controller, selected_index, use_btn, drop_btn, close_btn]() -> Element {
        auto items = controller.getInventory().getItems();
        bool is_empty = items.empty();
        int idx = *selected_index;
        
        // 修正索引範圍
        if (!is_empty && idx >= (int)items.size()) {
            idx = 0;
            *selected_index = 0;
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
                case ItemModel::POTION: type_str = "藥水"; is_potion = true; break;
                case ItemModel::BOW: type_str = "弓箭"; is_equip = true; break;
                case ItemModel::SWORD: type_str = "長劍"; is_equip = true; break;
                case ItemModel::KNIFE: type_str = "匕首"; is_equip = true; break;
                case ItemModel::WAND: type_str = "魔杖"; is_equip = true; break;
                case ItemModel::HELMET: type_str = "頭盔"; is_equip = true; break;
                case ItemModel::CHESTPLATE: type_str = "胸甲"; is_equip = true; break;
                case ItemModel::LEGGINGS: type_str = "護腿"; is_equip = true; break;
                case ItemModel::BOOTS: type_str = "靴子"; is_equip = true; break;
                case ItemModel::SCROLL_TELEPORT: type_str = "傳送卷軸"; break;
                case ItemModel::MISC: type_str = "雜物"; break;
            }
        }
        
        // 生成物品列表
        Elements item_elements;
        for (size_t i = 0; i < items.size(); ++i) {
            auto item_text = text("  " + items[i].name);
            if ((int)i == idx) {
                item_text = text("> " + items[i].name) | bold | color(Color::Yellow);
            }
            item_elements.push_back(item_text);
        }
        
        if (item_elements.empty()) {
            item_elements.push_back(text("  背包是空的") | dim);
        }
        
        return window(text("背包"), hbox({
            vbox({
                text("物品列表 (↑↓ 選擇)") | center,
                separator(),
                vbox(item_elements) | vscroll_indicator | frame | flex
            }) | size(WIDTH, EQUAL, 25) | border,
            
            vbox({
                text("詳細資料") | center,
                separator(),
                text("名稱: " + name),
                text("類型: " + type_str),
                text("效果: " + std::to_string(effect) + " G"),
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
