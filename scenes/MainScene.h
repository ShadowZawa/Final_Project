#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

using namespace ftxui;

// 主選單介面
class MainScene {
public:
    static Component Create(std::function<void()> onStartGame);
};

// 實作
inline Component MainScene::Create(std::function<void()> onStartGame) {
    auto start_button = Button("開始遊戲", onStartGame);
    
    auto container = Container::Vertical({
        start_button,
    });
    
    return Renderer(container, [=] {
        return vbox({
            filler(),
            text("木風之谷") | bold | center | size(HEIGHT, EQUAL, 3),
            filler(),
            hbox({
                filler(),
                start_button->Render() | size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 5),
                filler(),
            }),
            filler(),
        }) | border;
    });
}

#endif // MAINSCENE_H
