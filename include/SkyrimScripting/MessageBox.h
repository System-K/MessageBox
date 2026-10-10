#pragma once

#include <functional>
#include <future>
#include <vector>

namespace SkyrimScripting {

    class SkyrimMessageBox {
        class MessageBoxResultCallback : public RE::IMessageBoxCallback {
            std::function<void(unsigned int)> _callback;

        public:
            ~MessageBoxResultCallback() override {}
            MessageBoxResultCallback(std::function<void(unsigned int)> callback) : _callback(callback) {}
            // CommonLibSSE-NG >= 11: Run() takes the pressed button index directly (was enum Message).
            void Run(std::uint8_t a_button) override { _callback(static_cast<unsigned int>(a_button)); }
        };

    public:
        static void Show(const std::string& bodyText, std::vector<std::string> buttonTextValues,
                         std::function<void(unsigned int)> callback) {
            auto* factoryManager = RE::MessageDataFactoryManager::GetSingleton();
            auto* uiStringHolder = RE::InterfaceStrings::GetSingleton();
            auto* factory = factoryManager->GetCreator<RE::MessageBoxData>(
                uiStringHolder->messageBoxData);  // "MessageBoxData" <--- can we just use this string?
            auto* messagebox = factory->Create();
            RE::BSTSmartPointer<RE::IMessageBoxCallback> messageCallback =
                RE::make_smart<MessageBoxResultCallback>(callback);
            messagebox->callback = messageCallback;
            messagebox->bodyText = bodyText;
            for (auto text : buttonTextValues) messagebox->buttonText.push_back(text.c_str());
            // CommonLibSSE-NG >= 11: same engine function (ID 51422/52271), now on MessageBoxMenu.
            RE::MessageBoxMenu::QueueMessage(messagebox);
        }
    };

    void ShowMessageBox(const std::string& bodyText, std::vector<std::string> buttonTextValues,
                        std::function<void(unsigned int)> callback) {
        SkyrimMessageBox::Show(bodyText, buttonTextValues, callback);
    }
}
