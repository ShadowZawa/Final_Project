#ifndef MAPUI_H
#define MAPUI_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include "../managers/GameController.h"

using namespace ftxui;

// 世界地圖介面 (M鍵)
class MapUI
{
public:
    static Component Create(GameController &controller, std::function<void()> onClose);
};

inline Component MapUI::Create(GameController &controller, std::function<void()> onClose)
{
    auto close_btn = Button("關閉 [ESC]", onClose);

    auto container = Container::Vertical({
        close_btn,
    });

    return Renderer(container, [=, &controller]
                    {
        // 獲取當前位置名稱
        std::string currentLocation = controller.getCurrentLocationName();
        
        // 節點樣式函式 - 如果是當前位置，使用黃色顯示
        auto node = [&currentLocation](const std::string& name) {
            if (name == currentLocation) {
                return text("[" + name + "]") | bold | color(Color::Yellow);
            } else {
                return text("[" + name + "]") | bold;
            }
        };

        // 地圖佈局 (參考註解)
        // 勇士之村
        // 維多莉亞港 -> 六條岔道 -> 魔法森林
        // 弓箭手村
        // 菇菇城堡

        auto line1 = hbox({ filler(), node("勇士之村"), filler() });
        auto line_conn1 = hbox({ filler(), text("  │"), filler() });
        auto line2 = hbox({
                         text(""),
                         node("維多利亞港"),
                         text("─>"),
                         node("六條岔道"),
                         text("─>"),
                         node("魔法森林"),
                     }) |
                     center;
        auto line_conn2 = hbox({ filler(), text("  │"), filler() });
        auto line3 = hbox({ filler(), node("弓箭手村"), filler() });
        auto line_conn3 = hbox({ filler(), text("  │"), filler() });
        auto line4 = hbox({ filler(), node("菇菇城堡"), filler() });


        return vbox({
            text("【世界地圖】") | bold | center,
            separator(),
            
                line1,
                line_conn1,
                line2,
                line_conn2,
                line3,
                line_conn3,
                line4,
            
            separator(),
            hbox({
                filler(),
                close_btn->Render() | size(WIDTH, EQUAL, 15),
                filler(),
            }),
        }) | border | size(WIDTH, EQUAL, 70) | size(HEIGHT, EQUAL, 30) | center; });
}

#endif // MAPUI_H
