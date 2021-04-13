#pragma once

#include <hyperbeam/core.h>
#include <hyperbeam/fs.h>
#include <hyperbeam/logging.h>
#include <hyperbeam/message.h>
#include <hyperbeam/unity.h>
#include <cstdint>

const float TEXT_DURATION = 3.2f;
const float TEXT_DURATION_LONG = 5.f;
const float ROUND_TIME = 15.f;
const int ROUNDS = 6;

// When the hints should be displayed
const float PORTRAIT_HINT_TIME = 2.5f;
const float TEXT_HINT_1_TIME = 6.f;
const float TEXT_HINT_2_TIME = 11.5f;

int MIN_POINTS = 500;
int MAX_POINTS = 1500;

struct SentryDutyData {
    char footprintAssetName[8];
    char creatureSymbol[32];
    int nameHash;
    char16_t hints[4][96];
};

enum class SentryDutyState {
    NONE,
    START_TEXT,         // Text "Look at the footprint..."
    ROUND_START,        // Text "Here comes a Pokémon"
    SELECTING_ANSWER,   // The player is guessing
    ROUND_END_1,        // Text "The footprint is Pokémon's..."
    ROUND_END_2,        // Loudred tells the player if the choice was correct
    ROUND_END_WRONG,    // On a wrong selection, Loudred tells us the right choice
    END_RESULT,         // Text "[partner] and [hero]![R] That'll do it!"
    NEW_HIGH_SCORE,     // If it's new highscore: Text "[hero]! We did it! It's a new record!"
    FINISHED
};

enum class SentryDutyHintState {
    NONE,
    PORTRAITS,
    TEXT_HINT_1,
    TEXT_HINT_2
};

class SentryDutyMinigame : private hb::NonCopyable {

public:

    SentryDutyMinigame() {
        // Load data from binary file
        char* dataFile = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/sentry_duty.bin");
        if (!dataFile) {
            LOG("Failed to read sentry_duty.bin!");
            return;
        }

        hb::memcpy(&dataCount, dataFile, sizeof(uint32_t));
        dataEntries = new SentryDutyData[dataCount];
        hb::memcpy(dataEntries, dataFile + sizeof(uint32_t), sizeof(SentryDutyData) * dataCount);
        delete dataFile;

        loadHighscore();

        // Get an instance of the input and sound system
        // The methods are shared with other singleton objects so the names are wrong, this will be fixed later.
        input = (InputSystem*) Singleton_1_NativeMessageWindowCtrl__get_Instance(
            Singleton_1_InputSystem__get_Instance__MethodInfo);

        sound = (SoundSystem*) SingletonMonoBehaviour_1_window_WindowManager__get_Instance(
            SingletonMonoBehaviour_1_SoundSystem__get_Instance__MethodInfo);

        // Load the UI Canvas from an AssetBundle and spawn it
        sentryDutyAssetBundle = new hb::unity::AssetBundleWrapper("sentry_duty");
        footprintsAssetBundle = new hb::unity::AssetBundleWrapper("footprints");
        auto canvasPrefab = sentryDutyAssetBundle->loadAsset("SentryDutyCanvas");
        canvas = (GameObject*) hb::unity::instantiate(canvasPrefab);

        auto canvasTransform = GameObject_get_transform(canvas, nullptr);

        // Get the image object where the footprint is displayed
        auto rawImageType = hb::reflect::getUnityUIType("UnityEngine.UI", "RawImage");
        auto footprintImageTransform = hb::unity::findTransform(canvasTransform, "FootprintContainer/Footprint");
        footprintImage = hb::unity::getComponent<RawImage>((UnityComponent*) footprintImageTransform, rawImageType);

        // Get the time bar
        auto imageType = hb::reflect::getUnityUIType("UnityEngine.UI", "Image");
        auto timeBarTransform = hb::unity::findTransform(canvasTransform, "TimeBar/TimeBarGauge");
        timeBarImage = hb::unity::getComponent<Image>((UnityComponent*) timeBarTransform, imageType);

        // Get the rounds and score labels
        auto textMeshProType = hb::reflect::getType("Unity.TextMeshPro", "TMPro", "TextMeshProUGUI");

        auto scoreTransform = hb::unity::findTransform(canvasTransform, "ScoreBG/Score");
        scoreText = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) scoreTransform, textMeshProType);
        auto roundsTransform = hb::unity::findTransform(canvasTransform, "RoundsBG/Rounds");
        roundText = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) roundsTransform, textMeshProType);
        auto highscoreContainerTransform = hb::unity::findTransform(canvasTransform, "HighscoreContainer");
        auto highscoreTransform = hb::unity::findTransform(highscoreContainerTransform, "HighscoreBG/Highscore");
        highscoreText = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) highscoreTransform, textMeshProType);
        auto highscoreTitleTransform = hb::unity::findTransform(highscoreContainerTransform, "Text");
        auto highscoreTitle = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) highscoreTitleTransform,
            textMeshProType);

        // Get the buttons and portrait images
        auto animationType = hb::reflect::getUnityType("UnityEngine", "Animation");

        auto option0 = hb::unity::findTransform(canvasTransform, "Option0");
        auto option1 = hb::unity::findTransform(canvasTransform, "Option1");
        auto option2 = hb::unity::findTransform(canvasTransform, "Option2");
        auto option3 = hb::unity::findTransform(canvasTransform, "Option3");
        optionAnimationComponents[0] = hb::unity::getComponent<Animation>((UnityComponent*) option0, animationType);
        optionAnimationComponents[1] = hb::unity::getComponent<Animation>((UnityComponent*) option1, animationType);
        optionAnimationComponents[2] = hb::unity::getComponent<Animation>((UnityComponent*) option2, animationType);
        optionAnimationComponents[3] = hb::unity::getComponent<Animation>((UnityComponent*) option3, animationType);

        auto option0Image = hb::unity::findTransform(option0, "Face");
        auto option1Image = hb::unity::findTransform(option1, "Face");
        auto option2Image = hb::unity::findTransform(option2, "Face");
        auto option3Image = hb::unity::findTransform(option3, "Face");
        optionImages[0] = hb::unity::getComponent<RawImage>((UnityComponent*) option0Image, rawImageType);
        optionImages[1] = hb::unity::getComponent<RawImage>((UnityComponent*) option1Image, rawImageType);
        optionImages[2] = hb::unity::getComponent<RawImage>((UnityComponent*) option2Image, rawImageType);
        optionImages[3] = hb::unity::getComponent<RawImage>((UnityComponent*) option3Image, rawImageType);

        auto option0Name = hb::unity::findTransform(option0, "Name");
        auto option1Name = hb::unity::findTransform(option1, "Name");
        auto option2Name = hb::unity::findTransform(option2, "Name");
        auto option3Name = hb::unity::findTransform(option3, "Name");

        optionNameLabels[0] = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) option0Name, textMeshProType);
        optionNameLabels[1] = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) option1Name, textMeshProType);
        optionNameLabels[2] = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) option2Name, textMeshProType);
        optionNameLabels[3] = hb::unity::getComponent<TextMeshProUGUI>((UnityComponent*) option3Name, textMeshProType);

        // Get the font from ResourceCache since it's already loaded at this point and set it on all text components.
        // We can't use our own font since the shader is incompatible, so Unity displays the pink error shader
        auto uiBundle = (ResourceCache_AssetBundleResource*) ResourceCache_GetBundle(
            hb::createCSharpString("ui"), nullptr);
        auto fontAssetType = hb::reflect::getType("Unity.TextMeshPro", "TMPro", "TMP_FontAsset");
        auto font = (TMP_FontAsset*) ResourceCache_AssetBundleResource_FindObject(
            uiBundle, hb::createCSharpString("SystemMenuFont SDF"), fontAssetType, nullptr);

        TMP_Text_set_font((TMP_Text*) scoreText, font, nullptr);
        TMP_Text_set_font((TMP_Text*) roundText, font, nullptr);

        TMP_Text_set_font((TMP_Text*) optionNameLabels[0], font, nullptr);
        TMP_Text_set_font((TMP_Text*) optionNameLabels[1], font, nullptr);
        TMP_Text_set_font((TMP_Text*) optionNameLabels[2], font, nullptr);
        TMP_Text_set_font((TMP_Text*) optionNameLabels[3], font, nullptr);

        TMP_Text_set_font((TMP_Text*) highscoreText, font, nullptr);
        TMP_Text_set_font((TMP_Text*) highscoreTitle, font, nullptr);

        updateHighscoreText();

        highscoreContainerAnimation = hb::unity::getComponent<Animation>((UnityComponent*) highscoreContainerTransform,
            animationType);

        setElementsVisible(false, false);
        transitionState(SentryDutyState::START_TEXT);

        // Setting the time scale to a very low value effectively pauses the game
        //Time_set_timeScale(0.000000001f, nullptr);
    }

    void destroy() {
        // This should be a destructor, but implementing it as a destructor somehow
        // causes the game to crash at startup (???????)
        hb::unity::destroy((UnityObject*) canvas);

        delete sentryDutyAssetBundle;
        delete footprintsAssetBundle;
        delete[] dataEntries;

        for (int i = 0; i < 4; i++) {
            if (loadedPortraitAssetBundles[i] != nullptr) {
                delete loadedPortraitAssetBundles[i];
            }
        }

        //Time_set_timeScale(1.0f, nullptr);
    }

    void update() {
        // Get the time that has passed since the previous frame
        timeSinceStateTransition += Time_get_unscaledDeltaTime(nullptr);

        switch (state) {
            case SentryDutyState::NONE:
            case SentryDutyState::FINISHED: {
                break;
            }
            case SentryDutyState::START_TEXT: {
                // Show another textbox after some time has passed
                if (timeSinceStateTransition > TEXT_DURATION_LONG) {
                    transitionState(SentryDutyState::ROUND_START);
                }
                break;
            }
            case SentryDutyState::ROUND_START: {
                if (timeSinceStateTransition > TEXT_DURATION) {
                    transitionState(SentryDutyState::SELECTING_ANSWER);
                }
                break;
            }
            case SentryDutyState::SELECTING_ANSWER: {
                handleInput();
                updateTimeBar();

                if (timeSinceStateTransition > ROUND_TIME) {
                    // Timeout if the player took longer than ROUND_TIME seconds
                    selectedButton = -1;
                    transitionState(SentryDutyState::ROUND_END_2);
                }

                // Show hints
                auto hints = dataEntries[currentDataEntryIndex].hints;
                if (hintState == SentryDutyHintState::NONE && timeSinceStateTransition > PORTRAIT_HINT_TIME) {
                    setElementsVisible(true, true);
                    hintState = SentryDutyHintState::PORTRAITS;
                } else if (hintState == SentryDutyHintState::PORTRAITS && timeSinceStateTransition > TEXT_HINT_1_TIME) {
                    // Pick hint 0, 1 or 2 at random
                    auto hint = hints[hb::randomIntInRange(0, 3)];
                    hb::message::talk(hb::createCSharpString(hint), hb::createCSharpString(""), false, 0);
                    hintState = SentryDutyHintState::TEXT_HINT_1;
                } else if (hintState == SentryDutyHintState::TEXT_HINT_1 && timeSinceStateTransition > TEXT_HINT_2_TIME) {
                    // Show hint 3
                    hb::message::talk(hb::createCSharpString(hints[3]), hb::createCSharpString(""), false, 0);
                    hintState = SentryDutyHintState::TEXT_HINT_2;
                }
                break;
            }
            case SentryDutyState::ROUND_END_1: {
                if (timeSinceStateTransition > TEXT_DURATION) {
                    transitionState(SentryDutyState::ROUND_END_2);
                }
                break;
            }
            case SentryDutyState::ROUND_END_2: {
                if (timeSinceStateTransition > TEXT_DURATION) {
                    if (selectedButton == correctChoice) {
                        transitionState(currentRoundNum >= ROUNDS 
                            ? SentryDutyState::END_RESULT : SentryDutyState::ROUND_START);
                    } else {
                        transitionState(SentryDutyState::ROUND_END_WRONG);
                    }
                }
                break;
            }
            case SentryDutyState::ROUND_END_WRONG: {
                if (timeSinceStateTransition > TEXT_DURATION) {
                    transitionState(currentRoundNum >= ROUNDS ? SentryDutyState::END_RESULT : SentryDutyState::ROUND_START);
                }
                break;
            }
            case SentryDutyState::END_RESULT: {
                if (timeSinceStateTransition > TEXT_DURATION_LONG) {
                    transitionState(score > highscore ? SentryDutyState::NEW_HIGH_SCORE : SentryDutyState::FINISHED);
                }
                break;
            }
            case SentryDutyState::NEW_HIGH_SCORE: {
                if (timeSinceStateTransition > TEXT_DURATION_LONG) {
                    transitionState(SentryDutyState::FINISHED);
                }
                break;
            }
        }
    }

    bool isFinished() {
        return state == SentryDutyState::FINISHED;
    }

private:

    void transitionState(SentryDutyState newState) {
        auto playerSelection = dataEntries[buttonEntryIndices[selectedButton]];

        switch (newState) {
            case SentryDutyState::NONE: {
                break;
            }
            case SentryDutyState::START_TEXT: {
                hb::message::talk("Look at the footprint at the top of the screen, OK?[R]Then identify the Pokémon!",
                    "Loudred", false);
                break;
            }
            case SentryDutyState::ROUND_START: {
                setElementsVisible(false, false); // Hide portraits and footprint
                setSelectedButton(-1, false); // Unselect the current button
                setRoundNum(++currentRoundNum);
                Image_set_fillAmount(timeBarImage, 1.f, nullptr); // Fully fill the time bar
                Animation_Play_3(highscoreContainerAnimation, hb::createCSharpString("HideHighscore"), nullptr);
                hb::message::talk("Here comes a Pokémon! Check its footprint and tell me what it is!", "Loudred", false);
                break;
            }
            case SentryDutyState::SELECTING_ANSWER: {
                loadNextRound();
                hintState = SentryDutyHintState::NONE;
                setElementsVisible(true, false);
                hb::message::talk("Whose footprint is this?", "[partner]", false);
                break;
            }
            case SentryDutyState::ROUND_END_1: {
                // Add the new points to the score
                if (selectedButton == correctChoice) {
                    score += calculatePoints();
                }

                // Build the string "The footprint is [Pokémon]'s! The footprint is [Pokémon]'s!"
                auto pokemonName = hb::cSharpStringToTempCharPtr(hb::message::getCommonString(playerSelection.nameHash));
                char* buffer = (char*) alloca(200);
                rtdx_snprintf(buffer, 200, "The footprint is %s's![R]The footprint is %s's!", pokemonName, pokemonName);

                hb::message::talk(buffer, "[hero]", false);
                break;
            }
            case SentryDutyState::ROUND_END_2: {
                // Show right and wrong answers
                String* animCorrect = hb::createCSharpString("OptionCorrect");
                String* animWrong = hb::createCSharpString("OptionWrong");

                for (int i = 0; i < 4; i++) {
                    // Play the correct "animation" (only a single frame that will be displayed forever)
                    Animation_Play_3(optionAnimationComponents[i], i == correctChoice ? animCorrect : animWrong, nullptr);
                }

                setElementsVisible(true, true); // Show portraits and footprint

                if (selectedButton == correctChoice) {
                    updateScoreText();
                    SoundSystem_PlaySe(sound, hb::createCSharpString("SE_SYS_CHIME"), nullptr);
                    hb::message::talk("Yep! Looks like you're right!", "Loudred", false);
                } else if (selectedButton == -1) {
                    // Timed out
                    SoundSystem_PlaySe(sound, hb::createCSharpString("SE_AT_MISS_PLAYER"), nullptr);
                    hb::message::talk("Out of time! Pick up the pace!", "Loudred", false);
                } else {
                    // Wrong choice
                    SoundSystem_PlaySe(sound, hb::createCSharpString("SE_AT_MISS_PLAYER"), nullptr);
                    hb::message::talk("Huh?! Looks wrong to me!", "Loudred", false);
                }

                break;
            }
            case SentryDutyState::ROUND_END_WRONG: {
                auto correctEntry = dataEntries[currentDataEntryIndex];
                auto pokemonName = hb::cSharpStringToTempCharPtr(hb::message::getCommonString(correctEntry.nameHash));
                char* buffer = (char*) alloca(200);
                rtdx_snprintf(buffer, 200, "The correct answer is %s![R]Buck up! And snap to it!", pokemonName);               

                hb::message::talk(buffer, "Loudred", false);
                break;
            }
            case SentryDutyState::END_RESULT: {
                Animation_Play_3(highscoreContainerAnimation, hb::createCSharpString("ShowHighscore"), nullptr);
                hb::message::talk("[partner] and [hero]![R] That'll do it!", "Loudred", false);
                break;
            }
            case SentryDutyState::NEW_HIGH_SCORE: {
                highscore = score;
                updateHighscoreText();
                saveHighscore();
                SoundSystem_StopBgm(sound, BGMChannel__Enum::CH_0, 0.3f, nullptr);
                SoundSystem_PlaySe(sound, hb::createCSharpString("SE_MINIGAME_PERFECT"), nullptr);
                hb::message::talk("[hero]! We did it! It's a new record!", "[partner]", false);
                break;
            }
            case SentryDutyState::FINISHED: {
                hb::message::closeMessageWindow();
                break;
            }
        }

        LOGF("State transition: %d -> %d\n", state, newState);

        state = newState;
        timeSinceStateTransition = 0.f;
    }

    void loadNextRound() {
        auto textureType = hb::reflect::getUnityType("UnityEngine", "Texture");

        // Randomly select a new data entry
        int nextDataIndex;
        do {
            nextDataIndex = hb::randomIntInRange(0, dataCount);
        } while (nextDataIndex == currentDataEntryIndex);
        currentDataEntryIndex = nextDataIndex;

        auto dataEntry = dataEntries[nextDataIndex];
        auto footprintTexture = (Texture*) footprintsAssetBundle->loadAsset(dataEntry.footprintAssetName);
        if (footprintTexture) {
            RawImage_set_texture(footprintImage, footprintTexture, nullptr);
        }

        // Determine which slot (0-3) will hold the correct answer
        correctChoice = hb::randomIntInRange(0, 4);

        // Determine the other random choices
        for (int i = 0; i < 4; i++) {
            if (i == correctChoice) {
                buttonEntryIndices[i] = nextDataIndex;
            } else {
                bool duplicate;
                do {
                    buttonEntryIndices[i] = hb::randomIntInRange(0, dataCount);
                    duplicate = buttonEntryIndices[i] == nextDataIndex;
                    // Check if the index was already rolled
                    for (int j = 0; j < i; j++) {
                        if (buttonEntryIndices[i] == buttonEntryIndices[j] ) {
                            duplicate = true;
                            break;
                        }
                    }
                } while (duplicate);
            }

            // Load the portrait
            auto dataEntry = dataEntries[buttonEntryIndices[i]];
            
            if (loadedPortraitAssetBundles[i] != nullptr) {
                delete loadedPortraitAssetBundles[i];
                loadedPortraitAssetBundles[i] = nullptr;
            }

            // If the asset bundle with the portrait was already loaded by the game, we must
            // not load it again because Unity doesn't allow it. We can retrieve it from
            // ResourceCache instead.
            Texture* portraitTexture = nullptr;
            auto symbolString = hb::createCSharpString(dataEntry.creatureSymbol);
            bool bundleInResourceCache = ResourceCache_IsBundleLoaded(symbolString, nullptr);

            if (bundleInResourceCache) {
                auto cachedResource = (ResourceCache_AssetBundleResource*) ResourceCache_GetBundle(symbolString, nullptr);
                if (cachedResource) {
                    portraitTexture = (Texture*) ResourceCache_AssetBundleResource_FindObject(
                        cachedResource, symbolString, textureType, nullptr);

                    if (!portraitTexture) {
                        LOGF("Failed to get texture from AssetBundleResource.\n");
                    }
                } else {
                    LOGF("Could not retrieve cached bundle.\n");
                }
            } else {
                // Not present in ResourceCache, so we can load it manually
                auto portraitAssetBundle = new hb::unity::AssetBundleWrapper(dataEntry.creatureSymbol);
                if (portraitAssetBundle->assetBundle) {
                    portraitTexture = (Texture*) portraitAssetBundle->loadAsset(dataEntry.creatureSymbol);
                    loadedPortraitAssetBundles[i] = portraitAssetBundle;
                } else {
                    LOGF("Failed to load portrait AssetBundle: '%s'.\n", dataEntry.creatureSymbol);
                }
            }

            RawImage_set_texture(optionImages[i], portraitTexture, nullptr);

            // Set the Pokémon name
            // TextMeshProUGUI is a child class of TMP_Text, so we can call TMP_Text.set_text
            auto nameString = hb::message::getCommonString(dataEntry.nameHash);
            TMP_Text_set_text((TMP_Text*) optionNameLabels[i], nameString, nullptr);
        }

        setSelectedButton(0, false);
    }

    void handleInput() {
        bool aButton = InputSystem_IsPad_MenuDecide(input, nullptr);
        bool up = InputSystem_IsPad_MenuUp(input, nullptr);
        bool down = InputSystem_IsPad_MenuDown(input, nullptr);
        bool left = InputSystem_IsPad_MenuLeft(input, nullptr);
        bool right = InputSystem_IsPad_MenuRight(input, nullptr);

        switch (selectedButton) {
            case 0:
                if (right) {
                    setSelectedButton(1, true);
                } else if (down) {
                    setSelectedButton(2, true);
                }
                break;
            case 1:
                if (left) {
                    setSelectedButton(0, true);
                } else if (down) {
                    setSelectedButton(3, true);
                }
                break;
            case 2:
                if (up) {
                    setSelectedButton(0, true);
                } else if (right) {
                    setSelectedButton(3, true);
                }
                break;
            case 3:
                if (up) {
                    setSelectedButton(1, true);
                } else if (left) {
                    setSelectedButton(2, true);
                }
                break;
        }

        if (aButton) {
            SoundSystem_PlaySe(sound, hb::createCSharpString("SE_SYS_DECIDE"), nullptr);
            transitionState(SentryDutyState::ROUND_END_1);
        }
    }

    float getRelativeRemainingTime() {
        float relativeTime = Mathf_Clamp01(timeSinceStateTransition / ROUND_TIME, nullptr);
        return 1.f - relativeTime;
    }

    int calculatePoints() {
        return MIN_POINTS + (((float) (MAX_POINTS - MIN_POINTS)) * getRelativeRemainingTime());
    }

    void loadHighscore() {
        // Load the highscore from the sentry_duty_highscore.bin save file
        char* contents = hb::fs::readEntireFile("PEGASUS:/sentry_duty_highscore.bin");
        if (contents) {
            hb::memcpy(&highscore, contents, sizeof(int));
            LOGF("Loaded highscore from file: %d\n", highscore);
            delete contents;
        } else {
            LOG("Failed to load highscore, the file is probably not yet present.\n");
        }
    }

    void saveHighscore() {
        hb::fs::writeFile("PEGASUS:/sentry_duty_highscore.bin", &highscore, sizeof(int));

        // Save data must be commited, otherwise the change won't have an effect
        hb::fs::commitSaveData();
    }
 
    void setSelectedButton(int buttonIndex, bool playSound) {
        if (selectedButton == buttonIndex) {
            return;
        }

        selectedButton = buttonIndex;

        String* animNormal = hb::createCSharpString("OptionNormal");
        String* animSelected = hb::createCSharpString("OptionSelected");

        for (int i = 0; i < 4; i++) {
            Animation_Play_3(optionAnimationComponents[i], i == buttonIndex ? animSelected : animNormal, nullptr);
        }

        if (playSound) {
            SoundSystem_PlaySe(sound, hb::createCSharpString("SE_SYS_CURSOR"), nullptr);
        }
    }

    void setElementsVisible(bool footprintAndNamesVisible, bool portraitsVisible) {
        Color white = Color_get_white(nullptr);
        Color transparent;
        transparent.r = 1.f;
        transparent.g = 1.f;
        transparent.b = 1.f;
        transparent.a = 0.f;

        // Turns the UI objects invisible by setting the color to transparent
        for (int i = 0; i < 4; i++) {
            TMP_Text_set_color((TMP_Text*) optionNameLabels[i], footprintAndNamesVisible ? white : transparent, nullptr);
            Graphic_set_color((Graphic*) optionImages[i], portraitsVisible ? white : transparent, nullptr);
        }
        Graphic_set_color((Graphic*) footprintImage, footprintAndNamesVisible ? white : transparent, nullptr);
    }

    void updateTimeBar() {
        Image_set_fillAmount(timeBarImage, getRelativeRemainingTime(), nullptr);
    }

    void setRoundNum(int roundNum) {
        currentRoundNum = roundNum;

        char* buffer = (char*) alloca(5);
        rtdx_snprintf(buffer, 5, "%d/%d", roundNum, ROUNDS);  

        TMP_Text_set_text((TMP_Text*) roundText, hb::createCSharpString(buffer), nullptr);
    }

    void updateScoreText() {
        char* buffer = (char*) alloca(10);
        rtdx_snprintf(buffer, 10, "%dP", score);  

        TMP_Text_set_text((TMP_Text*) scoreText, hb::createCSharpString(buffer), nullptr);
    }

    void updateHighscoreText() {
        char* buffer = (char*) alloca(10);
        rtdx_snprintf(buffer, 10, "%dP", highscore);  

        TMP_Text_set_text((TMP_Text*) highscoreText, hb::createCSharpString(buffer), nullptr);
    }

private:
    uint32_t dataCount;
    SentryDutyData* dataEntries;
    hb::unity::AssetBundleWrapper* sentryDutyAssetBundle;
    hb::unity::AssetBundleWrapper* footprintsAssetBundle;
    hb::unity::AssetBundleWrapper* loadedPortraitAssetBundles[4] = {};

    InputSystem* input;
    SoundSystem* sound;
    GameObject* canvas;
    RawImage* footprintImage;
    Animation* optionAnimationComponents[4];
    RawImage* optionImages[4];
    TextMeshProUGUI* optionNameLabels[4];
    Image* timeBarImage;
    TextMeshProUGUI* scoreText;
    TextMeshProUGUI* roundText;
    TextMeshProUGUI* highscoreText;
    Animation* highscoreContainerAnimation;

    SentryDutyState state = SentryDutyState::NONE;
    SentryDutyHintState hintState = SentryDutyHintState::NONE;
    float timeSinceStateTransition = 0.f;
    int selectedButton = 0;
    int correctChoice = 0;
    int buttonEntryIndices[4] = {};
    int currentDataEntryIndex = -1;
    int currentRoundNum = 0;
    int score = 0;
    int highscore = 0;
};
