#ifndef EQUIPMENTUI_H
#define EQUIPMENTUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../managers/GameController.h"
#include "../models/ItemModel.h"

using namespace ftxui;
using namespace std;

// 裝備介面 (E鍵)
class EquipmentUI {
public:
    static Component Create(GameController &controller, std::function<void()> onClose);
};

inline Component EquipmentUI::Create(GameController &controller, std::function<void()> onClose) {
    auto close_btn = Button("關閉 [ESC]", onClose);
    
    auto container = Container::Vertical({
        close_btn,
    });
    
    return Renderer(container, [&controller, close_btn] {
        // 動態讀取當前裝備資訊
        string weapon = (controller.getPlayer().getEquippedItem(ItemModel::WEAPON).name == "") ? "無" : controller.getPlayer().getEquippedItem(ItemModel::WEAPON).name;
        string helmet = (controller.getPlayer().getEquippedItem(ItemModel::HELMET).name == "") ? "無" : controller.getPlayer().getEquippedItem(ItemModel::HELMET).name;
        string chestplate = (controller.getPlayer().getEquippedItem(ItemModel::CHESTPLATE).name == "") ? "無" : controller.getPlayer().getEquippedItem(ItemModel::CHESTPLATE).name;
        string leggings = (controller.getPlayer().getEquippedItem(ItemModel::LEGGINGS).name == "") ? "無" : controller.getPlayer().getEquippedItem(ItemModel::LEGGINGS).name;
        string boots = (controller.getPlayer().getEquippedItem(ItemModel::BOOTS).name == "") ? "無" : controller.getPlayer().getEquippedItem(ItemModel::BOOTS).name;
        if (weapon != "無") weapon += " (+" + to_string(controller.getPlayer().getEquippedItem(ItemModel::WEAPON).star) + ")";
        if (helmet != "無") helmet += " (+" + to_string(controller.getPlayer().getEquippedItem(ItemModel::HELMET).star) + ")";
        if (chestplate != "無") chestplate += " (+" + to_string(controller.getPlayer().getEquippedItem(ItemModel::CHESTPLATE).star) + ")";
        if (leggings != "無") leggings += " (+" + to_string(controller.getPlayer().getEquippedItem(ItemModel::LEGGINGS).star) + ")";
        if (boots != "無") boots += " (+" + to_string(controller.getPlayer().getEquippedItem(ItemModel::BOOTS).star) + ")";
        // 獲取套裝效果資訊
        auto suitCounts = controller.getPlayer().getSuitCounts();
        Elements suitElements;
        
        if (suitCounts.empty()) {
            suitElements.push_back(text("未裝備套裝") | dim | center);
        } else {
            suitElements.push_back(text("【套裝效果】") | bold | color(Color::Cyan));
            for (const auto& pair : suitCounts) {
                string suitName = pair.first;
                int count = pair.second;
                
                // 查找套裝資訊
                for (const auto& suit : suits) {
                    if (suit.id == suitName) {
                        suitElements.push_back(text(suit.name + " (" + std::to_string(count) + "件)") | bold);
                        
                        // 顯示已激活的效果
                        for (const auto& effect : suit.effects) {
                            if (count >= effect.count) {
                                string effectText = "  [" + std::to_string(effect.count) + "件] ";
                                vector<string> bonuses;
                                if (effect.dmg > 0) bonuses.push_back("攻擊+" + std::to_string(effect.dmg));
                                if (effect.def > 0) bonuses.push_back("防禦+" + std::to_string(effect.def));
                                if (effect.hp > 0) bonuses.push_back("生命+" + std::to_string(effect.hp));
                                if (effect.mp > 0) bonuses.push_back("魔力+" + std::to_string(effect.mp));
                                
                                for (size_t i = 0; i < bonuses.size(); ++i) {
                                    effectText += bonuses[i];
                                    if (i < bonuses.size() - 1) effectText += " ";
                                }
                                suitElements.push_back(text(effectText) | color(Color::Green));
                            } else {
                                // 未激活的效果顯示為灰色
                                string effectText = "  [" + std::to_string(effect.count) + "件] ";
                                vector<string> bonuses;
                                if (effect.dmg > 0) bonuses.push_back("攻擊+" + std::to_string(effect.dmg));
                                if (effect.def > 0) bonuses.push_back("防禦+" + std::to_string(effect.def));
                                if (effect.hp > 0) bonuses.push_back("生命+" + std::to_string(effect.hp));
                                if (effect.mp > 0) bonuses.push_back("魔力+" + std::to_string(effect.mp));
                                
                                for (size_t i = 0; i < bonuses.size(); ++i) {
                                    effectText += bonuses[i];
                                    if (i < bonuses.size() - 1) effectText += " ";
                                }
                                suitElements.push_back(text(effectText) | dim);
                            }
                        }
                        break;
                    }
                }
            }
            
            // 總套裝加成
            int totalDmg = controller.getPlayer().getSuitDmg();
            int totalDef = controller.getPlayer().getSuitDef();
            int totalHp = controller.getPlayer().getSuitHp();
            int totalMp = controller.getPlayer().getSuitMp();
            
            if (totalDmg > 0 || totalDef > 0 || totalHp > 0 || totalMp > 0) {
                suitElements.push_back(separator());
                suitElements.push_back(text("總加成:") | bold);
                if (totalDmg > 0) suitElements.push_back(text("  攻擊力 +" + std::to_string(totalDmg)) | color(Color::Yellow));
                if (totalDef > 0) suitElements.push_back(text("  防禦力 +" + std::to_string(totalDef)) | color(Color::Yellow));
                if (totalHp > 0) suitElements.push_back(text("  生命值 +" + std::to_string(totalHp)) | color(Color::Yellow));
                if (totalMp > 0) suitElements.push_back(text("  魔力值 +" + std::to_string(totalMp)) | color(Color::Yellow));
            }
        }
        
        return vbox({
            text("【裝備介面】") | bold | center,
            separator(),
            hbox({
                vbox({
                    text("武器: " + weapon) | border | size(WIDTH, EQUAL, 25),
                    text("頭盔: " + helmet) | border | size(WIDTH, EQUAL, 25),
                    text("上衣: " + chestplate) | border | size(WIDTH, EQUAL, 25),
                    text("褲子: " + leggings) | border | size(WIDTH, EQUAL, 25),
                    text("鞋子: " + boots) | border | size(WIDTH, EQUAL, 25),
                }),
                separator(),
                vbox(suitElements) | border | flex,
            }) | flex,
            separator(),
            hbox({
                filler(),
                close_btn->Render() | size(WIDTH, EQUAL, 15),
                filler(),
            }),
        }) | border | size(WIDTH, EQUAL, 80) | size(HEIGHT, EQUAL, 30) | center;
    });
}

#endif // EQUIPMENTUI_H
