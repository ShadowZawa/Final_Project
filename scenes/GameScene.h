#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include "../models/WorldModel.h"
#include "../models/PlayerModel.h"
#include "../managers/GameController.h"

using namespace ftxui;

// 主遊戲介面
class GameScene
{
public:
    static Component Create(
        GameController &controller,
        std::function<void()> onBackpack,
        std::function<void()> onEquipment,
        std::function<void()> onEnhancement,
        std::function<void()> onShop,
        std::function<void()> onStats,
        std::function<void()> onMap);
}; 

// 實作
inline Component GameScene::Create(
    GameController &controller,
    std::function<void()> onBackpack,
    std::function<void()> onEquipment,
    std::function<void()> onEnhancement,
    std::function<void()> onShop,
    std::function<void()> onStats,
    std::function<void()> onMap)
{
    auto backpack_btn = Button("[B]背包", onBackpack);
    auto equipment_btn = Button("[E]裝備", onEquipment);
    auto enhancement_btn = Button("[U]強化", onEnhancement);
    auto shop_btn = Button("[S]商店", onShop);
    auto stats_btn = Button("[C]狀態", onStats);
    auto map_btn = Button("[M]地圖", onMap);

    // 操作選單
    static std::vector<std::string> action_items = {
        "向北移動", // 執行controller.Move(moveDirection::NORTH);
        "向南移動", // 同上
        "向東移動",
        "向西移動",
        "戰鬥",
    };
    static std::vector<std::string> combat_items = {
        "普通攻擊",
        "技能攻擊",
        "使用道具",
        "逃跑",
    };
    auto action_selected = std::make_shared<int>(0);

    // 創建選單組件
    MenuOption menu_option;
    menu_option.on_enter = [&controller, action_selected, &onBackpack]()
    {
        int selected = *action_selected;
        if (controller.isCombat())
        {
            // 戰鬥選單處理
            switch (selected)
            {
            case 0:
                controller.Attack();
                controller.LogEvent("你對敵人進行了普通攻擊！");
                break;
            case 1:
                //controller.SkillAttack();
                controller.LogEvent("你對敵人使用了技能攻擊！");
                break;
            case 2:
                onBackpack();  // 打開背包
                break;
            case 3:
                controller.Flee();
                break;
            }
            return;
        }
        else
        {

            switch (selected)
            {
            case 0:
                controller.Move(moveDirection::NORTH);
                controller.LogEvent("向北移動到 " + controller.getCurrentLocationName());
                break;
            case 1:
                controller.Move(moveDirection::SOUTH);
                controller.LogEvent("向南移動到 " + controller.getCurrentLocationName());
                break;
            case 2:
                controller.Move(moveDirection::EAST);
                controller.LogEvent("向東移動到 " + controller.getCurrentLocationName());
                break;
            case 3:
                controller.Move(moveDirection::WEST);
                controller.LogEvent("向西移動到 " + controller.getCurrentLocationName());
                break;
            case 4:
                controller.Combat(); // 預設與第一個敵人戰鬥
                // controller.LogEvent("與敵人展開戰鬥！");
                break;
            }
        }
    };
    auto combat_menu = Menu(&combat_items, action_selected.get(), menu_option);
    auto action_menu = Menu(&action_items, action_selected.get(), menu_option);

    // 改為橫向排列
    auto container = Container::Horizontal({
        backpack_btn,
        equipment_btn,
        enhancement_btn,
        shop_btn,
        stats_btn,
        map_btn,
        action_menu,
    });

    return Renderer(container, [=, &controller]() -> Element
                    {
        // HP 百分比
        float hp_ratio = static_cast<float>(controller.getPlayer().getHp()) / controller.getPlayer().getMaxHp();
        
        // MP 百分比
        float mp_ratio = static_cast<float>(controller.getPlayer().getMp()) / controller.getPlayer().getMaxMp();
        // 取得最新優先的 10 筆紀錄並動態生成行
        auto latest_logs = controller.getLog(10); // 最新在前
        // 準備要放入 UI 的 Elements
        Elements log_lines;
        for (size_t i = 0; i < latest_logs.size(); ++i) {
            log_lines.push_back(text(latest_logs[i]));
        }
        for (size_t i = latest_logs.size(); i < 10; ++i) {
            log_lines.push_back(text("") | dim);
        }
        auto final_menu = controller.isCombat() ? combat_menu : action_menu;
        Elements enemy_lines;
        const auto& enemies = controller.getCurrentEnemies();
        for (size_t i = 0; i < enemies.size(); ++i) {
            enemy_lines.push_back(text(std::to_string(i + 1) + ". " + enemies[i].name + " LV" + std::to_string(enemies[i].level) + " (HP: " + std::to_string(enemies[i].hp) + ")"));
        }
        if (enemies.empty()) {
            enemy_lines.push_back(text("沒有敵人。") | dim);
        }

        return vbox({
                   hbox({
                       vbox({
                           text("職業: " + std::string(controller.getPlayer().getJob())) | bold,
                           text("等級: Lv." + std::to_string(controller.getPlayer().getLevel()) + " (" + std::to_string(controller.getPlayer().getExp()) + "/" + std::to_string(controller.getPlayer().getCalcExpReq()) + ")"),
                           text("金幣: " + std::to_string(controller.getPlayer().getGold()) + "G"),
                       }) | border |
                           size(WIDTH, EQUAL, 25),
                       vbox({
                           text("【角色詳細資訊】") | center | bold,
                           separator(),
                           hbox({text("經驗: " + std::to_string(controller.getPlayer().getExp()) + "/" + std::to_string(controller.getPlayer().getCalcExpReq())), text("攻擊: " + std::to_string(controller.getPlayer().CalcDamage())), text("防禦: " + std::to_string(controller.getPlayer().CalcDef()))}) | center,
                       }) | border |
                           flex,
                       vbox({
                           text("【小地圖】") | center | bold,
                           separator(),
                           text("當前位置:") | center,
                           text(controller.getCurrentLocationName()) | center | color(Color::Yellow),
                       }) | border |
                           size(WIDTH, EQUAL, 25),
                   }),
                   hbox({
                       // 遊戲區域`
                       vbox({
                           text("戰鬥紀錄"),
                           separator(),
                           // 顯示最新優先的最後 10 筆紀錄
                           vbox(log_lines) | flex,
                       }) | flex |
                           border,

                       vbox({
                           text("敵人列表"),
                           separator(),
                           vbox(enemy_lines) | flex,
                            }) | flex | border, 

                   }) | flex,

               hbox({
                   vbox({
                       hbox({text("HP "), gauge(hp_ratio) | color(Color::Red) | flex, text(" " + std::to_string(controller.getPlayer().getHp()) + "/" + std::to_string(controller.getPlayer().getMaxHp()))}),
                       hbox({text("MP "), gauge(mp_ratio) | color(Color::Blue) | flex, text(" " + std::to_string(controller.getPlayer().getMp()) + "/" + std::to_string(controller.getPlayer().getMaxMp()))}),
                   }) | border |
                       size(HEIGHT, EQUAL, 5) | flex,
                   vbox({
                       text("【操作選項】") | center | bold,
                       separator(),
                       final_menu->Render(),
                   }) | border |
                       size(HEIGHT, EQUAL, 10) | flex,
                   hbox({
                       backpack_btn->Render(),
                       separator(),
                       equipment_btn->Render(),
                       separator(),
                       enhancement_btn->Render(),
                       separator(),
                       shop_btn->Render(),
                       separator(),
                       stats_btn->Render(),
                       separator(),
                       map_btn->Render(),
                   }) | border |
                       size(HEIGHT, EQUAL, 5) | flex,
               }),
        }); });
}

#endif // GAMESCENE_H
