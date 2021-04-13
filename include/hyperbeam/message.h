#pragma once

#include "il2cpp-appdata.h"
#include <hyperbeam/core.h>
#include <hyperbeam/reflect.h>
#include <hyperbeam/logging.h>

typedef MessageWindow_SystemVoice__Enum MessageSystemVoice;
typedef MessageWindow_NoticeMode__Enum MessageNoticeMode;

namespace hb::message {

    String* getCommonString(int hash) {
        auto textId = (TextId*) il2cpp_object_new(TextId__TypeInfo);
        TextId__ctor_2(textId, hash, nullptr);

        return TextId_GetText(textId, nullptr);
    }

    String* getScriptString(int hash) {
        auto textId = (TextId*) il2cpp_object_new(TextId__TypeInfo);
        TextId__ctor_2(textId, hash, nullptr);

        il2cpp_initialize_method_metadata(0x68FDu);

        auto string = TextPool_GetText_ErrorIsNull(textId, true, nullptr);
        if (!string)
        {
            return hb::createCSharpString("");
        }

        return string;
    }

    MessageWindow* getMessageWindowSingleton() {
        return (MessageWindow*) SingletonMonoBehaviour_1_window_WindowManager__get_Instance(
            SingletonMonoBehaviour_1_MessageWindow__get_Instance__MethodInfo);
    }

    Coroutine* talk(String* text, String* characterName, bool interactive, int framesPerChar = 1,
            int autoCloseFrames = -1, MessageSystemVoice sfx = MessageSystemVoice::SYSTEM_VOICE_TALK) {
        auto noticeMode = (MessageNoticeMode) ((int) MessageNoticeMode::NOTICE_MODE_WHITE_FONT
            | (int) MessageNoticeMode::NOTICE_MODE_TALK_WINDOW);

        if (!interactive) {
            noticeMode = (MessageNoticeMode) ((int) noticeMode
                | (int) MessageNoticeMode::NOTICE_MODE_NOT_CLOSE
                | (int) MessageNoticeMode::NOTICE_MODE_NOT_KEY_WAIT);
        }

        auto messageWindowSingleton = getMessageWindowSingleton();

        auto enumerator = MessageWindow_Talk_1(messageWindowSingleton, text, characterName, sfx, framesPerChar,
            noticeMode, autoCloseFrames, nullptr);
        return MonoBehaviour_StartCoroutine_2((MonoBehaviour*) messageWindowSingleton, enumerator,
            MonoBehaviour_StartCoroutine_2__MethodInfo);
    }

    Coroutine* talk(const char* text, const char* characterName, bool interactive = true, int framesPerChar = 1,
            int autoCloseFrames = -1, MessageSystemVoice sfx = MessageSystemVoice::SYSTEM_VOICE_TALK) {
        auto textString = text ? hb::createCSharpString(text) : nullptr;
        auto nameString = text ? hb::createCSharpString(characterName) : nullptr;

        return talk(textString, nameString, interactive, framesPerChar, autoCloseFrames, sfx);
    }

    void closeMessageWindow() {
        MessageWindow_Close(getMessageWindowSingleton(), false, nullptr);
    }

}
