#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include "../managers/AuthManager.h"
#include "../managers/DataManager.h"
#include "../managers/InventoryController.h"
#include "../models/PlayerModel.h"

using namespace ftxui;

enum class MainSceneState {
    LOGIN,
    REGISTER,
    FORGOT_PASSWORD,
    SELECT_CHARACTER,
    CREATE_CHARACTER
};

class MainScene {
public:
    static Component Create(
        std::function<void(const std::string&, const std::string&)> onStartGame,
        AuthManager& authManager,
        DataManager& dataManager
    );
};

// 實作
inline Component MainScene::Create(
    std::function<void(const std::string&, const std::string&)> onStartGame,
    AuthManager& authManager,
    DataManager& dataManager
) {
    auto current_state = std::make_shared<MainSceneState>(MainSceneState::LOGIN);
    auto username = std::make_shared<std::string>("");
    auto password = std::make_shared<std::string>("");
    auto new_password1 = std::make_shared<std::string>("");
    auto new_password2 = std::make_shared<std::string>("");
    auto character_name = std::make_shared<std::string>("");
    auto login_attempts = std::make_shared<int>(0);
    auto error_message = std::make_shared<std::string>("");
    auto selected_character = std::make_shared<int>(0);
    auto selected_job = std::make_shared<int>(0);
    auto current_username = std::make_shared<std::string>("");
    
    static const std::vector<std::string> job_names = {"戰士", "法師", "弓箭手", "盜賊"};

    
    // ===== 登入畫面 =====
    Component username_input = Input(username.get(), "輸入使用者名稱");
    
    auto password_option = InputOption::Default();
    password_option.password = true;
    Component password_input = Input(password.get(), "輸入密碼", password_option);
    
    auto login_button = Button("登入", [&, username, password, login_attempts, error_message, current_state, current_username] {
        if (username->empty() || password->empty()) {
            *error_message = "請輸入使用者名稱和密碼！";
            return;
        }
        
        if (authManager.login(*username, *password)) {
            *error_message = "";
            *login_attempts = 0;
            *current_username = *username;
            *current_state = MainSceneState::SELECT_CHARACTER;
        } else {
            (*login_attempts)++;
            if (*login_attempts >= 3) {
                *error_message = "登入失敗 " + std::to_string(*login_attempts) + " 次";
            } else {
                *error_message = "帳號或密碼錯誤！(嘗試 " + std::to_string(*login_attempts) + "/3)";
            }
        }
    });
    
    auto register_button = Button("註冊", [current_state, username, password, error_message] {
        *username = "";
        *password = "";
        *error_message = "";
        *current_state = MainSceneState::REGISTER;
    });
    
    auto forgot_button = Button("忘記密碼", [current_state, username, password, error_message, new_password1, new_password2] {
        *password = "";
        *new_password1 = "";
        *new_password2 = "";
        *error_message = "";
        *current_state = MainSceneState::FORGOT_PASSWORD;
    });
    
    // ===== 註冊畫面 =====
    Component register_username_input = Input(username.get(), "輸入使用者名稱");
    Component register_password_input = Input(password.get(), "輸入密碼", password_option);
    
    auto register_confirm_button = Button("確認註冊", [&, username, password, error_message, current_state] {
        if (username->empty() || password->empty()) {
            *error_message = "請輸入使用者名稱和密碼！";
            return;
        }
        
        if (authManager.registerUser(*username, *password)) {
            *error_message = "註冊成功！請登入";
            *current_state = MainSceneState::LOGIN;
            *password = "";
        } else {
            *error_message = "使用者名稱已存在！";
        }
    });
    
    auto register_back_button = Button("返回", [current_state, username, password, error_message] {
        *username = "";
        *password = "";
        *error_message = "";
        *current_state = MainSceneState::LOGIN;
    });
    
    // ===== 忘記密碼畫面 =====
    Component forgot_username_input = Input(username.get(), "輸入使用者名稱");
    Component new_password1_input = Input(new_password1.get(), "輸入新密碼", password_option);
    Component new_password2_input = Input(new_password2.get(), "再次輸入新密碼", password_option);
    
    auto forgot_confirm_button = Button("確認修改", [&, username, password, new_password1, new_password2, error_message, current_state] {
        if (username->empty() || new_password1->empty() || new_password2->empty()) {
            *error_message = "請填寫所有欄位！";
            return;
        }
        
        if (*new_password1 != *new_password2) {
            *error_message = "兩次密碼不一致！";
            return;
        }
        
        if (authManager.forget(*username, *new_password1)) {
            *error_message = "密碼已重設！請登入";
            *current_state = MainSceneState::LOGIN;
            *password = "";
            *new_password1 = "";
            *new_password2 = "";
            // username 保留，方便用戶直接登入
        } else {
            *error_message = "使用者不存在！";
        }
    });
    
    auto forgot_back_button = Button("返回", [current_state, username, password, new_password1, new_password2, error_message] {
        *username = "";
        *password = "";
        *new_password1 = "";
        *new_password2 = "";
        *error_message = "";
        *current_state = MainSceneState::LOGIN;
    });
    
    // ===== 選擇角色畫面 =====
    auto character_select_button = Button("選擇角色", [&, current_username, selected_character, onStartGame, error_message] {
        auto characters = authManager.getCharacterNames(*current_username);
        if (*selected_character < 0 || *selected_character >= characters.size()) {
            *error_message = "請選擇有效的角色！";
            return;
        }
        
        onStartGame(*current_username, characters[*selected_character]);
    });
    
    auto character_create_button = Button("創建新角色", [&, current_state, current_username, error_message, character_name] {
        if (authManager.canCreateMoreCharacters(*current_username)) {
            *character_name = "";
            *error_message = "";
            *current_state = MainSceneState::CREATE_CHARACTER;
        } else {
            *error_message = "已達角色數量上限（3個）！";
        }
    });
    
    auto character_logout_button = Button("登出", [current_state, username, password, error_message, login_attempts] {
        *username = "";
        *password = "";
        *error_message = "";
        *login_attempts = 0;
        *current_state = MainSceneState::LOGIN;
    });
    
    // ===== 創建角色畫面 =====
    Component create_character_name_input = Input(character_name.get(), "輸入角色名稱");
    
    Component job_radiobox = Radiobox(&job_names, selected_job.get());
    
    auto create_confirm_button = Button("確認創建", [&, current_username, character_name, selected_job, error_message, current_state, onStartGame] {
        if (character_name->empty()) {
            *error_message = "請輸入角色名稱！";
            return;
        }
        
        // 首先嘗試在 AuthManager 中創建角色
        if (!authManager.createCharacter(*current_username, *character_name, *selected_job)) {
            *error_message = "角色名稱已存在或達到上限！";
            return;
        }
        
        // 然後創建存檔
        if (!dataManager.createNewCharacter(*current_username, *character_name, *selected_job)) {
            *error_message = "創建存檔失敗！請檢查 saves 目錄";
            // 回退：從 AuthManager 中刪除剛創建的角色
            // TODO: authManager.deleteCharacter(*current_username, *character_name);
            return;
        }
        
        // 成功創建角色和存檔，直接啟動遊戲
        *error_message = "";
        onStartGame(*current_username, *character_name);
    });
    
    auto create_back_button = Button("返回", [current_state, character_name, error_message, selected_job] {
        *character_name = "";
        *error_message = "";
        *selected_job = 0;
        *current_state = MainSceneState::SELECT_CHARACTER;
    });
    
    // ===== 容器 =====
    auto login_container = Container::Vertical({
        username_input, password_input, login_button, register_button, forgot_button
    });
    
    auto register_container = Container::Vertical({
        register_username_input, register_password_input,
        register_confirm_button, register_back_button
    });
    
    auto forgot_container = Container::Vertical({
        forgot_username_input, new_password1_input, new_password2_input,
        forgot_confirm_button, forgot_back_button
    });
    
    auto character_container = Container::Vertical({
        character_select_button, character_create_button, character_logout_button
    });
    
    auto create_container = Container::Vertical({
        create_character_name_input, job_radiobox,
        create_confirm_button, create_back_button
    });
    
    auto main_container = Container::Tab({
        login_container, register_container, forgot_container, character_container, create_container
    }, (int*)current_state.get());
    
    auto with_events = CatchEvent(main_container, [selected_character, current_state, current_username, &authManager](Event event) {
        if (*current_state == MainSceneState::SELECT_CHARACTER) {
            auto characters = authManager.getCharacterNames(*current_username);
            int max_idx = characters.size() > 0 ? characters.size() : 1;
            if (event == Event::ArrowDown || event == Event::Character('j')) {
                *selected_character = (*selected_character + 1) % max_idx;
                return true;
            }
            if (event == Event::ArrowUp || event == Event::Character('k')) {
                *selected_character = (*selected_character - 1 + max_idx) % max_idx;
                return true;
            }
        }
        return false;
    });
    
    return Renderer(with_events, [=, &authManager] {
        Element content;
        
        switch (*current_state) {
            case MainSceneState::LOGIN: {
                Elements buttons;
                buttons.push_back(login_button->Render() | size(WIDTH, EQUAL, 20));
                buttons.push_back(register_button->Render() | size(WIDTH, EQUAL, 20));
                if (*login_attempts >= 3) {
                    buttons.push_back(forgot_button->Render() | size(WIDTH, EQUAL, 20));
                }
                
                content = vbox({
                    text("使用者名稱:") | bold,
                    username_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    text("密碼:") | bold,
                    password_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    (*error_message != "" ? text(*error_message) | color(Color::Red) | center : text("")),
                    separator(),
                    vbox(buttons) | center,
                }) | border | size(WIDTH, EQUAL, 50);
                break;
            }
            
            case MainSceneState::REGISTER: {
                content = vbox({
                    text("註冊新帳號") | bold | center,
                    separator(),
                    text("使用者名稱:") | bold,
                    register_username_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    text("密碼:") | bold,
                    register_password_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    (*error_message != "" ? text(*error_message) | color(Color::Red) | center : text("")),
                    separator(),
                    hbox({
                        register_confirm_button->Render() | size(WIDTH, EQUAL, 15),
                        text("  "),
                        register_back_button->Render() | size(WIDTH, EQUAL, 15),
                    }) | center,
                }) | border | size(WIDTH, EQUAL, 50);
                break;
            }
            
            case MainSceneState::FORGOT_PASSWORD: {
                content = vbox({
                    text("重設密碼") | bold | center,
                    separator(),
                    text("使用者名稱:") | bold,
                    forgot_username_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    text("新密碼:") | bold,
                    new_password1_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    text("確認新密碼:") | bold,
                    new_password2_input->Render() | size(WIDTH, EQUAL, 30),
                    separator(),
                    (*error_message != "" ? text(*error_message) | color(Color::Red) | center : text("")),
                    separator(),
                    hbox({
                        forgot_confirm_button->Render() | size(WIDTH, EQUAL, 15),
                        text("  "),
                        forgot_back_button->Render() | size(WIDTH, EQUAL, 15),
                    }) | center,
                }) | border | size(WIDTH, EQUAL, 50);
                break;
            }
            
            case MainSceneState::SELECT_CHARACTER: {
                auto characters = authManager.getCharacterNames(*current_username);
                
                Elements character_list;
                if (characters.empty()) {
                    character_list.push_back(text("  尚無角色，請創建新角色") | dim);
                } else {
                    for (int i = 0; i < characters.size(); ++i) {
                        auto char_text = text((i == *selected_character ? "> " : "  ") + characters[i]);
                        if (i == *selected_character) {
                            char_text = char_text | bold | color(Color::Yellow);
                        }
                        character_list.push_back(char_text);
                    }
                }
                
                content = vbox({
                    text("選擇角色") | bold | center,
                    text("歡迎回來，" + *current_username + "！") | center,
                    separator(),
                    text("你的角色 (" + std::to_string(characters.size()) + "/3):") | bold,
                    vbox(character_list) | frame | size(HEIGHT, EQUAL, 8),
                    separator(),
                    (*error_message != "" ? text(*error_message) | color(Color::Red) | center : text("")),
                    separator(),
                    vbox({
                        (characters.size() > 0 ? character_select_button->Render() : text("選擇角色") | dim) | size(WIDTH, EQUAL, 20) | center,
                        character_create_button->Render() | size(WIDTH, EQUAL, 20) | center,
                        character_logout_button->Render() | size(WIDTH, EQUAL, 20) | center,
                    }),
                }) | border | size(WIDTH, EQUAL, 50);
                break;
            }
            
            case MainSceneState::CREATE_CHARACTER: {
                Elements create_elements;
                create_elements.push_back(text("創建新角色") | bold | center);
                create_elements.push_back(separator());
                create_elements.push_back(text("角色名稱:") | bold);
                create_elements.push_back(create_character_name_input->Render() | size(WIDTH, EQUAL, 30));
                create_elements.push_back(separator());
                create_elements.push_back(text("選擇職業:") | bold);
                create_elements.push_back(job_radiobox->Render() | frame | size(HEIGHT, EQUAL, 6));
                create_elements.push_back(separator());
                if (*error_message != "") {
                    create_elements.push_back(text(*error_message) | color(*error_message == "角色創建成功！" ? Color::Green : Color::Red) | center);
                } else {
                    create_elements.push_back(text(""));
                }
                create_elements.push_back(separator());
                create_elements.push_back(hbox({
                    create_confirm_button->Render() | size(WIDTH, EQUAL, 15),
                    text("  "),
                    create_back_button->Render() | size(WIDTH, EQUAL, 15)
                }) | center);
                
                content = vbox(create_elements) | border | size(WIDTH, EQUAL, 50);
                break;
            }
        }
        
        return vbox({
            filler(),
            text("木風之谷") | bold | center | size(HEIGHT, EQUAL, 3),
            filler(),
            content | center,
            filler(),
        }) | border;
    });
}

#endif // MAINSCENE_H
