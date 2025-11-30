#ifndef STORAGEUI_H
#define STORAGEUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <vector>
#include "../managers/InventoryController.h"
#include "../managers/DataManager.h"
#include "../models/ItemModel.h"

using namespace ftxui;

class StorageUI {
public:
    static Component Create(
        InventoryController& playerInventory,
        DataManager& dataManager,
        std::function<void()> onClose
    );
};

// 實作
inline Component StorageUI::Create(
    InventoryController& playerInventory,
    DataManager& dataManager,
    std::function<void()> onClose
) {
    // 共用倉庫
    auto storage = std::make_shared<InventoryController>();
    
    // 載入倉庫資料
    dataManager.loadStorage(*storage);
    
    // 當前選擇的標籤頁 (0: 背包, 1: 倉庫)
    auto selected_tab = std::make_shared<int>(0);
    
    // 選擇的物品索引
    auto selected_inventory_item = std::make_shared<int>(0);
    auto selected_storage_item = std::make_shared<int>(0);
    
    // 滾動偏移量
    auto inventory_scroll_offset = std::make_shared<int>(0);
    auto storage_scroll_offset = std::make_shared<int>(0);
    
    // 訊息顯示
    auto message = std::make_shared<std::string>("");
    
    // 關閉按鈕
    auto close_button = Button("關閉 (ESC)", [onClose, &dataManager, storage] {
        // 儲存倉庫資料
        dataManager.saveStorage(*storage);
        onClose();
    });
    
    // 存入倉庫按鈕 (從背包到倉庫)
    auto deposit_button = Button("存入倉庫 (D)", [&playerInventory, storage, selected_inventory_item, message, &dataManager] {
        if (playerInventory.getSize() == 0) {
            *message = "背包是空的！";
            return;
        }
        
        if (*selected_inventory_item < 0 || *selected_inventory_item >= playerInventory.getSize()) {
            *message = "請選擇有效的物品！";
            return;
        }
        
        // 從背包取出物品
        ItemModel item = playerInventory.getItem(*selected_inventory_item);
        
        // 加入倉庫
        storage->addItem(item);
        
        // 從背包移除
        playerInventory.removeItem(*selected_inventory_item);
        
        // 調整選擇索引
        if (*selected_inventory_item >= playerInventory.getSize() && playerInventory.getSize() > 0) {
            *selected_inventory_item = playerInventory.getSize() - 1;
        } else if (playerInventory.getSize() == 0) {
            *selected_inventory_item = 0;
        }
        
        // 儲存倉庫
        dataManager.saveStorage(*storage);
        
        *message = "已存入：" + item.name;
    });
    
    // 取出倉庫按鈕 (從倉庫到背包)
    auto withdraw_button = Button("取出物品 (W)", [&playerInventory, storage, selected_storage_item, message, &dataManager] {
        if (storage->getSize() == 0) {
            *message = "倉庫是空的！";
            return;
        }
        
        if (*selected_storage_item < 0 || *selected_storage_item >= storage->getSize()) {
            *message = "請選擇有效的物品！";
            return;
        }
        
        // 從倉庫取出物品
        ItemModel item = storage->getItem(*selected_storage_item);
        
        // 加入背包
        playerInventory.addItem(item);
        
        // 從倉庫移除
        storage->removeItem(*selected_storage_item);
        
        // 調整選擇索引
        if (*selected_storage_item >= storage->getSize() && storage->getSize() > 0) {
            *selected_storage_item = storage->getSize() - 1;
        } else if (storage->getSize() == 0) {
            *selected_storage_item = 0;
        }
        
        // 儲存倉庫
        dataManager.saveStorage(*storage);
        
        *message = "已取出：" + item.name;
    });
    
    // 容器
    auto button_container = Container::Horizontal({
        deposit_button,
        withdraw_button,
        close_button
    });
    
    // 鍵盤事件處理
    auto with_events = CatchEvent(button_container, [selected_tab, selected_inventory_item, selected_storage_item, inventory_scroll_offset, storage_scroll_offset, &playerInventory, storage, onClose, deposit_button, withdraw_button, &dataManager](Event event) {
        // Tab 切換
        if (event == Event::Tab) {
            *selected_tab = (*selected_tab + 1) % 2;
            return true;
        }
        
        // ESC 關閉
        if (event == Event::Escape) {
            dataManager.saveStorage(*storage);
            onClose();
            return true;
        }
        
        // 在背包頁面
        if (*selected_tab == 0) {
            int max_idx = playerInventory.getSize() > 0 ? playerInventory.getSize() : 1;
            const int visible_items = 12;
            
            if (event == Event::ArrowDown || event == Event::Character('j')) {
                if (playerInventory.getSize() > 0) {
                    *selected_inventory_item = (*selected_inventory_item + 1) % max_idx;
                    
                    // 自動向下滾動
                    if (*selected_inventory_item >= *inventory_scroll_offset + visible_items) {
                        *inventory_scroll_offset = *selected_inventory_item - visible_items + 1;
                    }
                    // 循環回到頂部時重置滾動
                    if (*selected_inventory_item == 0) {
                        *inventory_scroll_offset = 0;
                    }
                }
                return true;
            }
            if (event == Event::ArrowUp || event == Event::Character('k')) {
                if (playerInventory.getSize() > 0) {
                    *selected_inventory_item = (*selected_inventory_item - 1 + max_idx) % max_idx;
                    
                    // 自動向上滾動
                    if (*selected_inventory_item < *inventory_scroll_offset) {
                        *inventory_scroll_offset = *selected_inventory_item;
                    }
                    // 循環回到底部時調整滾動
                    if (*selected_inventory_item == max_idx - 1) {
                        *inventory_scroll_offset = std::max(0, max_idx - visible_items);
                    }
                }
                return true;
            }
            
            // D 鍵存入
            if (event == Event::Character('d') || event == Event::Character('D')) {
                deposit_button->OnEvent(Event::Return);
                return true;
            }
        }
        
        // 在倉庫頁面
        if (*selected_tab == 1) {
            int max_idx = storage->getSize() > 0 ? storage->getSize() : 1;
            const int visible_items = 12;
            
            if (event == Event::ArrowDown || event == Event::Character('j')) {
                if (storage->getSize() > 0) {
                    *selected_storage_item = (*selected_storage_item + 1) % max_idx;
                    
                    // 自動向下滾動
                    if (*selected_storage_item >= *storage_scroll_offset + visible_items) {
                        *storage_scroll_offset = *selected_storage_item - visible_items + 1;
                    }
                    // 循環回到頂部時重置滾動
                    if (*selected_storage_item == 0) {
                        *storage_scroll_offset = 0;
                    }
                }
                return true;
            }
            if (event == Event::ArrowUp || event == Event::Character('k')) {
                if (storage->getSize() > 0) {
                    *selected_storage_item = (*selected_storage_item - 1 + max_idx) % max_idx;
                    
                    // 自動向上滾動
                    if (*selected_storage_item < *storage_scroll_offset) {
                        *storage_scroll_offset = *selected_storage_item;
                    }
                    // 循環回到底部時調整滾動
                    if (*selected_storage_item == max_idx - 1) {
                        *storage_scroll_offset = std::max(0, max_idx - visible_items);
                    }
                }
                return true;
            }
            
            // W 鍵取出
            if (event == Event::Character('w') || event == Event::Character('W')) {
                withdraw_button->OnEvent(Event::Return);
                return true;
            }
        }
        
        return false;
    });
    
    return Renderer(with_events, [=, &playerInventory, &dataManager] {
        const int visible_items = 12;
        
        // 背包物品列表（只顯示可見範圍）
        Elements inventory_elements;
        if (playerInventory.getSize() == 0) {
            inventory_elements.push_back(text("  (空)") | dim);
        } else {
            int start_idx = *inventory_scroll_offset;
            int end_idx = std::min(start_idx + visible_items, playerInventory.getSize());
            
            for (int i = start_idx; i < end_idx; ++i) {
                const auto& item = playerInventory.getItem(i);
                std::string item_text = "  " + item.name;
                if (item.star > 0) {
                    item_text += " +" + std::to_string(item.star);
                }
                item_text += " (" + std::to_string(item.effect) + ")";
                
                auto elem = text(item_text);
                if (*selected_tab == 0 && i == *selected_inventory_item) {
                    elem = text("> " + item.name + 
                               (item.star > 0 ? " +" + std::to_string(item.star) : "") + 
                               " (" + std::to_string(item.effect) + ")") | bold | color(Color::Yellow);
                }
                inventory_elements.push_back(elem);
            }
        }
        
        // 倉庫物品列表（只顯示可見範圍）
        Elements storage_elements;
        if (storage->getSize() == 0) {
            storage_elements.push_back(text("  (空)") | dim);
        } else {
            int start_idx = *storage_scroll_offset;
            int end_idx = std::min(start_idx + visible_items, storage->getSize());
            
            for (int i = start_idx; i < end_idx; ++i) {
                const auto& item = storage->getItem(i);
                std::string item_text = "  " + item.name;
                if (item.star > 0) {
                    item_text += " +" + std::to_string(item.star);
                }
                item_text += " (" + std::to_string(item.effect) + ")";
                
                auto elem = text(item_text);
                if (*selected_tab == 1 && i == *selected_storage_item) {
                    elem = text("> " + item.name + 
                               (item.star > 0 ? " +" + std::to_string(item.star) : "") + 
                               " (" + std::to_string(item.effect) + ")") | bold | color(Color::Yellow);
                }
                storage_elements.push_back(elem);
            }
        }
        
        // 顯示選中物品的詳細資訊
        std::string detail_info = "";
        if (*selected_tab == 0 && playerInventory.getSize() > 0 && *selected_inventory_item < playerInventory.getSize()) {
            const auto& item = playerInventory.getItem(*selected_inventory_item);
            detail_info = item.description + "\n價值: " + std::to_string(item.price) + "G";
        } else if (*selected_tab == 1 && storage->getSize() > 0 && *selected_storage_item < storage->getSize()) {
            const auto& item = storage->getItem(*selected_storage_item);
            detail_info = item.description + "\n價值: " + std::to_string(item.price) + "G";
        }
        
        // 背包滾動指示器
        std::string inv_scroll_indicator = "";
        if (playerInventory.getSize() > 0) {
            if (*inventory_scroll_offset > 0) {
                inv_scroll_indicator += "↑ ";
            }
            inv_scroll_indicator += std::to_string(*selected_inventory_item + 1) + "/" + std::to_string(playerInventory.getSize());
            if (*inventory_scroll_offset + visible_items < playerInventory.getSize()) {
                inv_scroll_indicator += " ↓";
            }
        }
        
        auto inventory_panel = vbox({
            hbox({
                text("我的背包 (" + std::to_string(playerInventory.getSize()) + " 個物品)") | 
                    (*selected_tab == 0 ? bold | color(Color::Cyan) : bold) | flex,
                text(inv_scroll_indicator) | dim
            }),
            separator(),
            vbox(inventory_elements) | size(HEIGHT, EQUAL, 15)
        }) | border;
        
        // 倉庫滾動指示器
        std::string stor_scroll_indicator = "";
        if (storage->getSize() > 0) {
            if (*storage_scroll_offset > 0) {
                stor_scroll_indicator += "↑ ";
            }
            stor_scroll_indicator += std::to_string(*selected_storage_item + 1) + "/" + std::to_string(storage->getSize());
            if (*storage_scroll_offset + visible_items < storage->getSize()) {
                stor_scroll_indicator += " ↓";
            }
        }
        
        auto storage_panel = vbox({
            hbox({
                text("共用倉庫 (" + std::to_string(storage->getSize()) + " 個物品)") | 
                    (*selected_tab == 1 ? bold | color(Color::Cyan) : bold) | flex,
                text(stor_scroll_indicator) | dim
            }),
            separator(),
            vbox(storage_elements) | size(HEIGHT, EQUAL, 15)
        }) | border;
        
        auto main_content = hbox({
            inventory_panel | flex,
            storage_panel | flex
        });
        
        return vbox({
            text("倉庫系統") | bold | center,
            text("(全帳號共用)") | dim | center,
            separator(),
            main_content,
            separator(),
            text(detail_info) | dim | center | size(HEIGHT, EQUAL, 2),
            separator(),
            (*message != "" ? text(*message) | color(Color::Green) | center : text("") | center),
            hbox({
                deposit_button->Render() | size(WIDTH, EQUAL, 18),
                text("  "),
                withdraw_button->Render() | size(WIDTH, EQUAL, 18),
                text("  "),
                close_button->Render() | size(WIDTH, EQUAL, 18)
            }) | center,
            separator(),
            text("提示: Tab切換 ↑↓選擇 D存入 W取出 ESC關閉") | dim | center
        }) | border | size(WIDTH, GREATER_THAN, 80);
    });
}

#endif // STORAGEUI_H
