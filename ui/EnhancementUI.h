#ifndef ENHANCEMENTUI_H
#define ENHANCEMENTUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include "../managers/GameController.h"
#include <vector>
#include <chrono>

using namespace ftxui;

// 強化介面 (U鍵)
class EnhancementUI {
public:
    static Component Create(GameController &controller, std::function<void()> onClose);
};

inline Component EnhancementUI::Create(GameController &controller, std::function<void()> onClose)
{
    auto selected_index = std::make_shared<int>(0);
    auto is_capturing = std::make_shared<bool>(false);  // 是否在星力捕捉模式
    auto capture_progress = std::make_shared<float>(0.0f);  // 捕捉進度 (0-100)
    auto capture_speed = std::make_shared<float>(2.0f);  // 捕捉速度
    auto last_update = std::make_shared<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());
    
    auto enhance_btn = Button("強化", [&controller, selected_index, is_capturing, capture_progress, capture_speed, last_update] {
        if (*is_capturing) {
            // 在星力捕捉模式下按下 = 確認強化
            ItemModel::Type types[] = {
                ItemModel::WEAPON,
                ItemModel::HELMET,
                ItemModel::CHESTPLATE,
                ItemModel::LEGGINGS,
                ItemModel::BOOTS
            };
            
            if (*selected_index >= 0 && *selected_index < 5) {
                // 檢查是否在指定範圍內 (40-60)
                bool in_range = (*capture_progress >= 40.0f && *capture_progress <= 60.0f);
                
                // 獲取當前裝備（使用可修改的引用）
                ItemModel& item = controller.getMutablePlayer().getEquippedItem(types[*selected_index]);
                if (item.name == "") {
                    controller.LogEvent("該裝備欄位無裝備！");
                    *is_capturing = false;
                    return;
                }
                
                // 計算成功率
                int base_success_rate;
                if (item.star == 0) base_success_rate = 100;
                else if (item.star == 1) base_success_rate = 100;
                else if (item.star == 2) base_success_rate = 70;
                else if (item.star == 3) base_success_rate = 50;
                else base_success_rate = 10;
                
                // 如果不在範圍內，成功率 -10%
                int final_success_rate = in_range ? base_success_rate : (base_success_rate - 10);
                if (final_success_rate < 0) final_success_rate = 0;
                
                // 檢查是否擁有強化卷軸
                int scroll_index = -1;
                auto& inventory = controller.getInventory();
                for (int i = 0; i < inventory.getSize(); ++i) {
                    if (inventory.getItem(i).type == ItemModel::SCROLL_UPGRADE) {
                        scroll_index = i;
                        break;
                    }
                }
                
                if (scroll_index == -1) {
                    controller.LogEvent("需要強化卷軸才能強化裝備！");
                    *is_capturing = false;
                    return;
                }
                
                // 計算所需金幣
                //int cost = 10 * (item.star + 1);
                
                //if (controller.getMutablePlayer().getGold() < cost) {
                //    controller.LogEvent("金幣不足！需要 " + std::to_string(cost) + " G");
                //    *is_capturing = false;
                //    return;
                //}
                
                // 消耗強化卷軸
                controller.getInventory().removeItem(scroll_index);
                
                // 扣除金幣
                //controller.getMutablePlayer().deductGold(cost);
                
                // 判斷是否成功
                int random = std::rand() % 100;
                if (random < final_success_rate) {
                    item.star++;
                    item.effect += 5;
                    string timing = in_range ? " [完美時機!]" : " [時機不佳]";
                    controller.LogEvent("★ 強化成功！" + item.name + " +" + std::to_string(item.star) + timing);
                } else {
                    string fail_msg = in_range ? "" : " [時機不佳導致失敗率增加]";
                    controller.LogEvent("✗ 強化失敗！" + fail_msg);
                }
                
                *is_capturing = false;
            }
        } else {
            // 開始星力捕捉
            ItemModel::Type types[] = {
                ItemModel::WEAPON,
                ItemModel::HELMET,
                ItemModel::CHESTPLATE,
                ItemModel::LEGGINGS,
                ItemModel::BOOTS
            };
            
            if (*selected_index >= 0 && *selected_index < 5) {
                ItemModel& item = controller.getMutablePlayer().getEquippedItem(types[*selected_index]);
                if (item.name != "") {
                    // 檢查是否擁有強化卷軸
                    bool has_scroll = false;
                    auto& inventory = controller.getInventory();
                    for (int i = 0; i < inventory.getSize(); ++i) {
                        if (inventory.getItem(i).type == ItemModel::SCROLL_UPGRADE) {
                            has_scroll = true;
                            break;
                        }
                    }
                    
                    if (!has_scroll) {
                        controller.LogEvent("✗ 需要強化卷軸才能強化裝備！");
                        return;
                    }
                    
                    // 檢查金幣是否足夠
                    //int cost = 10 * (item.star + 1);
                    //if (controller.getMutablePlayer().getGold() < cost) {
                    //    controller.LogEvent("金幣不足！需要 " + std::to_string(cost) + " G");
                    //    return;
                    //}
                    *is_capturing = true;
                    *capture_progress = 0.0f;
                    *last_update = std::chrono::steady_clock::now();
                }
            }
        }
    });
    
    auto close_btn = Button("關閉 [ESC]", [&onClose, is_capturing] {
        *is_capturing = false;  // 確保退出時重置捕捉狀態
        onClose();
    });
    
    auto button_container = Container::Vertical({enhance_btn, close_btn});
    
    // 事件處理 - 在正常模式下處理方向鍵選擇裝備
    auto with_events = CatchEvent(button_container, [selected_index, is_capturing](Event event) {
        // 在星力捕捉模式下，不攔截任何事件，讓按鈕容器正常處理
        if (*is_capturing) {
            // ESC 鍵：先取消捕捉，然後讓事件繼續傳遞（這樣 main.cpp 也能處理）
            if (event == Event::Escape) {
                *is_capturing = false;
                // 不返回 true，讓事件繼續傳遞
            }
            return false;  // 讓所有事件通過給按鈕
        }
        
        // 正常模式下的方向鍵選擇裝備
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            *selected_index = (*selected_index + 1) % 5;
            return true;
        }
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            *selected_index = (*selected_index - 1 + 5) % 5;
            return true;
        }
        
        return false;
    });
    
    // 動畫更新組件
    auto animated = Renderer(with_events, [&controller, selected_index, enhance_btn, close_btn, 
                                           is_capturing, capture_progress, capture_speed, last_update] {
        // 更新星力捕捉動畫
        if (*is_capturing) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - *last_update).count();
            
            if (elapsed > 0) {
                *last_update = now;
                
                // 更新進度條（來回移動）- 調整速度使其更明顯
                *capture_progress += *capture_speed * (elapsed / 10.0f);
                
                if (*capture_progress >= 100.0f) {
                    *capture_progress = 100.0f;
                    *capture_speed = -*capture_speed;  // 反轉方向
                } else if (*capture_progress <= 0.0f) {
                    *capture_progress = 0.0f;
                    *capture_speed = -*capture_speed;  // 反轉方向
                }
            }
        }
        
        int idx = *selected_index;
        
        // 獲取所有裝備
        auto& weapon = controller.getPlayer().getEquippedItem(ItemModel::WEAPON);
        auto& helmet = controller.getPlayer().getEquippedItem(ItemModel::HELMET);
        auto& chestplate = controller.getPlayer().getEquippedItem(ItemModel::CHESTPLATE);
        auto& leggings = controller.getPlayer().getEquippedItem(ItemModel::LEGGINGS);
        auto& boots = controller.getPlayer().getEquippedItem(ItemModel::BOOTS);
        
        // 建立裝備列表
        struct EquipSlot {
            string name;
            int star;
            bool equipped;
        };
        
        EquipSlot slots[] = {
            {weapon.name == "" ? "無" : weapon.name, weapon.star, weapon.name != ""},
            {helmet.name == "" ? "無" : helmet.name, helmet.star, helmet.name != ""},
            {chestplate.name == "" ? "無" : chestplate.name, chestplate.star, chestplate.name != ""},
            {leggings.name == "" ? "無" : leggings.name, leggings.star, leggings.name != ""},
            {boots.name == "" ? "無" : boots.name, boots.star, boots.name != ""}
        };
        
        string slot_names[] = {"武器", "頭盔", "上衣", "褲子", "鞋子"};
        
        // 生成裝備列表顯示
        Elements equipment_elements;
        for (int i = 0; i < 5; ++i) {
            string display = slot_names[i] + ": " + slots[i].name;
            if (slots[i].equipped && slots[i].star > 0) {
                display += " +" + std::to_string(slots[i].star);
            }
            
            auto item_text = text("  " + display);
            if (i == idx) {
                item_text = text("> " + display) | bold | color(Color::Yellow);
            }
            equipment_elements.push_back(item_text);
        }
        
        // 計算當前選擇裝備的強化資訊
        int current_star = slots[idx].star;
        bool is_equipped = slots[idx].equipped;
        
        int success_rate;
        if (current_star == 0) success_rate = 100;
        else if (current_star == 1) success_rate = 100;
        else if (current_star == 2) success_rate = 70;
        else if (current_star == 3) success_rate = 50;
        else success_rate = 10;
        
        int cost = 10 * (current_star + 1);
        
        // 檢查是否擁有強化卷軸
        bool has_scroll = false;
        int scroll_count = 0;
        auto& inv = controller.getInventory();
        for (int i = 0; i < inv.getSize(); ++i) {
            if (inv.getItem(i).type == ItemModel::SCROLL_UPGRADE) {
                has_scroll = true;
                scroll_count++;
            }
        }
        
        // 建立星力捕捉動畫
        Element capture_animation = text("");
        if (*is_capturing) {
            // 繪製進度條
            int bar_width = 50;
            int current_pos = (int)(*capture_progress * bar_width / 100.0f);
            
            // 成功範圍: 45-55 (對應位置約 22-27)
            int success_start = (int)(40.0f * bar_width / 100.0f);
            int success_end = (int)(60.0f * bar_width / 100.0f);
            
            Elements bar_elements;
            bar_elements.push_back(text("["));
            for (int i = 0; i < bar_width; ++i) {
                if (i >= success_start && i <= success_end) {
                    // 成功範圍 - 綠色
                    if (i == current_pos) {
                        bar_elements.push_back(text("●") | bold | color(Color::Yellow));
                    } else {
                        bar_elements.push_back(text("█") | color(Color::Green));
                    }
                } else {
                    // 失敗範圍 - 紅色
                    if (i == current_pos) {
                        bar_elements.push_back(text("●") | bold | color(Color::Yellow));
                    } else {
                        bar_elements.push_back(text("─") | color(Color::Red) | dim);
                    }
                }
            }
            bar_elements.push_back(text("]"));
            
            capture_animation = vbox({
                text("★ 星力捕捉中 ★") | bold | center | color(Color::Cyan),
                separator(),
                hbox(bar_elements) | center,
                text("在綠色區域內按下「強化」獲得正常成功率！") | center | color(Color::Green),
                text("在紅色區域按下將 -10% 成功率") | center | color(Color::Red) | dim,
                separator(),
                text("按 [Enter] 確認強化，[Tab] 切換到關閉，[ESC] 取消") | center | dim,
            }) | border | color(Color::Cyan);
        }
        
        return vbox({
            text("【強化介面】") | bold | center,
            separator(),
            (*is_capturing ? 
                // 星力捕捉模式
                vbox({
                    capture_animation | flex,
                    separator(),
                    hbox({
                        enhance_btn->Render() | size(WIDTH, EQUAL, 15),
                        text(" "),
                        close_btn->Render() | size(WIDTH, EQUAL, 15)
                    }) | center
                }) | flex
            :
                // 正常選擇模式
                hbox({
                    vbox({
                        text("選擇要強化的裝備 (↑↓ 選擇)") | center,
                        separator(),
                        vbox(equipment_elements) | frame | flex
                    }) | size(WIDTH, EQUAL, 28) | border,
                    
                    vbox({
                        text("強化詳情") | center,
                        separator(),
                        text("當前裝備: " + slots[idx].name),
                        text("當前等級: +" + std::to_string(current_star)),
                        separator(),
                        text("基礎成功率: " + std::to_string(success_rate) + "%") | (success_rate >= 70 ? color(Color::Green) : (success_rate >= 50 ? color(Color::Yellow) : color(Color::Red))),
                        //text("所需金幣: " + std::to_string(cost) + " G") | (controller.getPlayer().getGold() >= cost ? color(Color::White) : color(Color::Red)),
                        separator(),
                        //text("持有金幣: " + std::to_string(controller.getPlayer().getGold()) + " G"),
                        text("強化卷軸: " + std::to_string(scroll_count) + " 個") | (has_scroll ? color(Color::Green) : color(Color::Red)),
                        separator(),
                        text("強化規則:") | bold,
                        text("+0→+1: 100%") | dim,
                        text("+1→+2: 100%") | dim,
                        text("+2→+3: 70%") | dim,
                        text("+3→+4: 50%") | dim,
                        text("+4以上: 10%") | dim,
                        separator(),
                        text("★ 星力捕捉 ★") | bold | color(Color::Cyan),
                        text("在指定範圍內按下強化") | color(Color::Green),
                        text("否則成功率 -10%") | color(Color::Red),
                        separator(),
                        text("💎 需要消耗 1 個強化卷軸") | (has_scroll ? color(Color::Yellow) : color(Color::Red)),
                        separator(),
                        hbox({
                            (is_equipped && has_scroll ? enhance_btn->Render() : text(is_equipped ? "缺少卷軸" : "未裝備") | dim) | size(WIDTH, EQUAL, 15),
                            text(" "),
                            close_btn->Render() | size(WIDTH, EQUAL, 15)
                        }) | center
                    }) | flex | border
                })
            ),
        }) | border | size(WIDTH, EQUAL, 80) | size(HEIGHT, EQUAL, 30) | center;
    });
    
    return animated;
}

#endif // ENHANCEMENTUI_H
