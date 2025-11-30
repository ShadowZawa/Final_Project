#ifndef STATSUI_H
#define STATSUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include "../managers/GameController.h"

using namespace ftxui;

// 角色狀態介面 (C鍵)
class StatsUI {
public:
    static Component Create(GameController& controller, std::function<void()> onClose);
};

inline Component StatsUI::Create(GameController& controller, std::function<void()> onClose) {
    // 0=攻擊, 1=防禦, 2=生命, 3=魔力
    auto add_dmg_btn = Button("+", [&controller]() { controller.getMutablePlayer().addStat(0); });
    auto remove_dmg_btn = Button("-", [&controller]() { controller.getMutablePlayer().removeStat(0); });
    
    auto add_def_btn = Button("+", [&controller]() { controller.getMutablePlayer().addStat(1); });
    auto remove_def_btn = Button("-", [&controller]() { controller.getMutablePlayer().removeStat(1); });
    
    auto add_hp_btn = Button("+", [&controller]() { controller.getMutablePlayer().addStat(2); });
    auto remove_hp_btn = Button("-", [&controller]() { controller.getMutablePlayer().removeStat(2); });
    
    auto add_mp_btn = Button("+", [&controller]() { controller.getMutablePlayer().addStat(3); });
    auto remove_mp_btn = Button("-", [&controller]() { controller.getMutablePlayer().removeStat(3); });
    
    auto add_skill1_btn = Button("+", [&controller]() { controller.getMutablePlayer().addSkillLevel(1); });
    auto remove_skill1_btn = Button("-", [&controller]() { controller.getMutablePlayer().removeSkillLevel(1); });
    auto add_skill2_btn = Button("+", [&controller]() { controller.getMutablePlayer().addSkillLevel(2); });
    auto remove_skill2_btn = Button("-", [&controller]() { controller.getMutablePlayer().removeSkillLevel(2); });
    auto close_btn = Button("關閉 [ESC]", onClose);
    
    auto button_container = Container::Vertical({
        Container::Horizontal({add_dmg_btn, remove_dmg_btn}),
        Container::Horizontal({add_def_btn, remove_def_btn}),
        Container::Horizontal({add_hp_btn, remove_hp_btn}),
        Container::Horizontal({add_mp_btn, remove_mp_btn}),
        Container::Horizontal({add_skill1_btn, remove_skill1_btn}),
        Container::Horizontal({add_skill2_btn, remove_skill2_btn}),
        close_btn,
    });
    
    return Renderer(button_container, [&controller, 
                                        add_dmg_btn, remove_dmg_btn,
                                        add_def_btn, remove_def_btn,
                                        add_hp_btn, remove_hp_btn,
                                        add_mp_btn, remove_mp_btn,
                                        add_skill1_btn, remove_skill1_btn,
                                        add_skill2_btn, remove_skill2_btn,
                                        close_btn] {
        auto player = controller.getPlayer();
        int available_points = player.getAttrLvl();

        return vbox({
                   text("【角色狀態】") | bold | center,
                   separator(),
                   hbox({
                       // 左側：基本資訊
                       vbox({
                           text("基本資訊") | bold | center,
                           separator(),
                           text("職業: " + player.getJob()),
                           text("等級: " + std::to_string(player.getLevel())),
                           text("經驗: " + std::to_string(player.getExp()) + "/" + std::to_string(player.getCalcExpReq())),
                           separator(),
                           text("生命: " + std::to_string(player.getHp()) + "/" + std::to_string(player.getMaxHp())),
                           text("魔力: " + std::to_string(player.getMp()) + "/" + std::to_string(player.getMaxMp())),
                           text("金幣: " + std::to_string(player.getGold()) + " G"),
                           separator(),
                           text("可用技能點數: " + std::to_string(player.getSkillLevel())) |
                               (player.getSkillLevel() > 0 ? color(Color::Green) : color(Color::White)) | bold,
                           hbox({
                               text("普攻等級: " + std::to_string(player.getSkill1Level())) | size(WIDTH, EQUAL, 20),
                               add_skill1_btn->Render() | size(WIDTH, EQUAL, 5),
                               text(" "),
                               remove_skill1_btn->Render() | size(WIDTH, EQUAL, 5),
                           }),
                           hbox({
                               text("技能等級: " + std::to_string(player.getSkill2Level())) | size(WIDTH, EQUAL, 20),
                               add_skill2_btn->Render() | size(WIDTH, EQUAL, 5),
                               text(" "),
                               remove_skill2_btn->Render() | size(WIDTH, EQUAL, 5),
                           }),
                       }) | size(WIDTH, EQUAL, 30) |
                           border,

                       // 右側：屬性點分配
                       vbox({
                           text("點數分配") | bold | center,
                           text("可用屬性點數: " + std::to_string(available_points)) |
                               (available_points > 0 ? color(Color::Green) : color(Color::White)) | bold,
                           separator(),
                           hbox({
                               text("攻擊屬性: " + std::to_string(player.getAttrDmg())) | size(WIDTH, EQUAL, 20),
                               add_dmg_btn->Render() | size(WIDTH, EQUAL, 5),
                               text(" "),
                               remove_dmg_btn->Render() | size(WIDTH, EQUAL, 5),
                           }),
                           hbox({
                               text("防禦屬性: " + std::to_string(player.getAttrDef())) | size(WIDTH, EQUAL, 20),
                               add_def_btn->Render() | size(WIDTH, EQUAL, 5),
                               text(" "),
                               remove_def_btn->Render() | size(WIDTH, EQUAL, 5),
                           }),
                           hbox({
                               text("生命屬性: " + std::to_string(player.getAttrHp())) | size(WIDTH, EQUAL, 20),
                               add_hp_btn->Render() | size(WIDTH, EQUAL, 5),
                               text(" "),
                               remove_hp_btn->Render() | size(WIDTH, EQUAL, 5),
                           }),
                           hbox({
                               text("魔力屬性: " + std::to_string(player.getAttrMp())) | size(WIDTH, EQUAL, 20),
                               add_mp_btn->Render() | size(WIDTH, EQUAL, 5),
                               text(" "),
                               remove_mp_btn->Render() | size(WIDTH, EQUAL, 5),
                           }),
                           
                       }) | flex |
                           border,
                   }),
                   separator(),
                   hbox({
                       filler(),
                       close_btn->Render() | size(WIDTH, EQUAL, 15),
                       filler(),
                   }),
               }) |
               border | size(WIDTH, EQUAL, 80) | size(HEIGHT, EQUAL, 28) | center;
    });
}

#endif // STATSUI_H
