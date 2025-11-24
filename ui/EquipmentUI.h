#ifndef EQUIPMENTUI_H
#define EQUIPMENTUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

using namespace ftxui;

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
                filler(),
            }),
            filler(),
            hbox({
                filler(),
                close_btn->Render() | size(WIDTH, EQUAL, 15),
                filler(),
            }),
        }) | border | size(WIDTH, EQUAL, 60) | size(HEIGHT, EQUAL, 30) | center;
    });
}

#endif // EQUIPMENTUI_H
