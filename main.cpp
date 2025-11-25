#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <chrono>
#include <thread>

#include "scenes/MainScene.h"
#include "scenes/GameScene.h"
#include "ui/BackpackUI.h"
#include "ui/EquipmentUI.h"
#include "ui/EnhancementUI.h"
#include "ui/ShopUI.h"
#include "ui/StatsUI.h"
#include "ui/MapUI.h"
#include "models/PlayerModel.h"
#include "managers/AuthManager.h"
#include "managers/DataManager.h"
using namespace std;
using namespace ftxui;

enum class GameState {
    MENU,
    PLAYING
};

enum class PopupType {
    NONE,
    BACKPACK,
    EQUIPMENT,
    ENHANCEMENT,
    SHOP,
    STATS,
    MAP
};

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    
    // 確保必要的目錄存在
    system("if not exist data mkdir data 2>nul");
    system("if not exist saves mkdir saves 2>nul");
    
    // 初始化管理器
    AuthManager authManager;
    DataManager dataManager;
    
    // 遊戲狀態
    GameState current_state = GameState::MENU;
    PopupType current_popup = PopupType::NONE;

    std::string current_username;
    std::string current_character_name;
    
    // 初始化 GameController
    GameController controller;
    
    // 主選單
    auto main_scene = MainScene::Create(
        [&](const std::string& username, const std::string& character_name) {
            current_username = username;
            current_character_name = character_name;
            
            // 載入角色資料
            PlayerModel player;
            InventoryController inventory;
            std::string currentMap;
            
            if (dataManager.load(username, character_name, player, inventory, currentMap)) {
                // 載入存檔（包括新建角色和現有角色）
                controller.getMutablePlayer() = player;
                controller.getInventory() = inventory;
                // TODO: 設定當前地圖
                
                // 判斷是否為新角色（等級為1且經驗為0）
                if (player.getLevel() == 1 && player.getExp() == 0) {
                    controller.LogEvent("歡迎來到木風之谷，" + character_name + "!");
                    controller.LogEvent("這是你的冒險起點！");
                } else {
                    controller.LogEvent("歡迎回來，" + character_name + "!");
                }
                controller.LogEvent("當前位置: " + controller.getCurrentLocationName());
            } else {
                // 載入失敗（不應該發生，因為 createNewCharacter 已經創建了存檔）
                controller.LogEvent("載入角色資料失敗！");
            }
            current_state = GameState::PLAYING;
        },
        authManager,
        dataManager
    );
    // 遊戲場景
    auto game_scene = GameScene::Create(
        controller,
        [&] { current_popup = PopupType::BACKPACK; },    // B - 背包
        [&] { current_popup = PopupType::EQUIPMENT; },  // E - 裝備
        [&] { current_popup = PopupType::ENHANCEMENT; }, // U - 強化
        [&] { current_popup = PopupType::SHOP; },       // S - 商店
        [&] { current_popup = PopupType::STATS; },       // C - 狀態
        [&] { current_popup = PopupType::MAP; }         // M - 地圖
    );
    
    // 彈出介面
    auto backpack_ui = BackpackUI::Create(controller, [&] { current_popup = PopupType::NONE; });
    auto equipment_ui = EquipmentUI::Create(controller, [&] { current_popup = PopupType::NONE; });
    auto enhancement_ui = EnhancementUI::Create(controller, [&] { current_popup = PopupType::NONE; });
    auto shop_ui = ShopUI::Create(controller, [&] { current_popup = PopupType::NONE; });
    auto stats_ui = StatsUI::Create(controller, [&] { current_popup = PopupType::NONE; });
    auto map_ui = MapUI::Create([&] { current_popup = PopupType::NONE; });
    
    
    // 主容器
    auto main_container = Container::Tab(
        {main_scene, game_scene},
        (int*)&current_state
    );
    
    // 使用 Maybe 包裝，當有彈窗時禁用主容器的事件接收
    auto main_maybe = Maybe(main_container, [&] { return current_popup == PopupType::NONE; });
    
    // 使用 Maybe 包裝彈窗組件，只有在激活時才接收事件
    auto backpack_maybe = Maybe(backpack_ui, [&] { return current_popup == PopupType::BACKPACK; });
    auto equipment_maybe = Maybe(equipment_ui, [&] { return current_popup == PopupType::EQUIPMENT; });
    auto enhancement_maybe = Maybe(enhancement_ui, [&] { return current_popup == PopupType::ENHANCEMENT; });
    auto shop_maybe = Maybe(shop_ui, [&] { return current_popup == PopupType::SHOP; });
    auto stats_maybe = Maybe(stats_ui, [&] { return current_popup == PopupType::STATS; });
    auto map_maybe = Maybe(map_ui, [&] { return current_popup == PopupType::MAP; });
    
    
    // 主容器 - 包含遊戲場景和彈窗
    auto all_components = Container::Stacked({
        main_maybe,
        backpack_maybe,
        equipment_maybe,
        enhancement_maybe,
        shop_maybe,
        stats_maybe,
        map_maybe
    });
    
    // 渲染器 - 處理彈出視窗
    auto renderer = Renderer(all_components, [&] {
        Element base_view;
        
        // 根據當前狀態顯示對應場景
        if (current_state == GameState::MENU) {
            base_view = main_scene->Render();
        } else {
            base_view = game_scene->Render();
        }
        
        // 如果有彈出視窗，疊加在上面
        if (current_popup != PopupType::NONE) {
            Element popup_view;
            
            switch (current_popup) {
                case PopupType::BACKPACK:
                    popup_view = backpack_ui->Render();
                    break;
                case PopupType::EQUIPMENT:
                    popup_view = equipment_ui->Render();
                    break;
                case PopupType::ENHANCEMENT:
                    popup_view = enhancement_ui->Render();
                    break;
                case PopupType::SHOP:
                    popup_view = shop_ui->Render();
                    break;
                case PopupType::STATS:
                    popup_view = stats_ui->Render();
                    break;
                case PopupType::MAP:
                    popup_view = map_ui->Render();
                    break;
                default:
                    break;
            }
            
            return dbox({
                base_view | dim,
                popup_view | clear_under | center
            });
        }
        
        return base_view;
    });
    
    // 快捷鍵處理
    auto component_with_keys = CatchEvent(renderer, [&](Event event) {
        // 當有彈窗打開時，只處理 ESC 關閉，其他事件讓彈窗處理
        if (current_popup != PopupType::NONE) {
            if (event == Event::Escape) {
                current_popup = PopupType::NONE;
                return true;
            }
            // 返回 false 讓事件傳遞給當前激活的彈窗組件
            return false;
        }
        
        // 只在遊戲場景中處理快捷鍵
        if (current_state == GameState::PLAYING) {
            if (event == Event::Character('b') || event == Event::Character('B')) {
                current_popup = PopupType::BACKPACK;
                return true;
            }
            if (event == Event::Character('e') || event == Event::Character('E')) {
                current_popup = PopupType::EQUIPMENT;
                return true;
            }
            if (event == Event::Character('u') || event == Event::Character('U')) {
                current_popup = PopupType::ENHANCEMENT;
                return true;
            }
            if (event == Event::Character('s') || event == Event::Character('S')) {
                current_popup = PopupType::SHOP;
                return true;
            }
            if (event == Event::Character('c') || event == Event::Character('C')) {
                current_popup = PopupType::STATS;
                return true;
            }
            if (event == Event::Character('m') || event == Event::Character('M')) {
                current_popup = PopupType::MAP;
                return true;
            }
        }
        
        return false;
    });
    
    // 使用 Loop 強制持續更新畫面（用於遊戲動畫）
    Loop loop(&screen, component_with_keys);
    
    // 設定更新頻率（每 50ms）
    while (!loop.HasQuitted()) {
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        screen.Post(Event::Custom);  // 強制重新渲染
    }
    
    // 遊戲結束時自動存檔
    if (current_state == GameState::PLAYING && !current_username.empty() && !current_character_name.empty()) {
        dataManager.save(
            current_username, 
            current_character_name, 
            controller.getMutablePlayer(), 
            controller.getInventory(), 
            controller.getCurrentLocationName()
        );
    }
    
    return 0;
}