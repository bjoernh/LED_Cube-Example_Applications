#include "CubeTest.h"
#include <chrono>
#include <iostream>
#include <thread>

#define OVERSAMPLING 1




CubeTest::CubeTest(std::string serverUri) : CubeApplication(30, serverUri){
    // Audio params — tunable live from the param webapp.
    params.registerFloat("audioGain",     "Audio Gain",     1.0f, 200.0f, 50.0f, 1.0f,   "Audio");
    params.registerFloat("beatThreshold", "Beat Threshold", 1.1f, 2.5f,   1.4f,  0.05f,  "Audio");

    // Play beep before opening the mic so the speaker output doesn't saturate
    // the capture channel. playBeep() initialises SDL audio on its own.
    audio_.playBeep(440.0f, 1000.0f, 0.3f);
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    if (audio_.init()) {
        audio_.setGain(params.getFloat("audioGain"));
        audio_.setBeatThreshold(params.getFloat("beatThreshold"));
        std::cout << "AudioInput: init OK, gain=" << params.getFloat("audioGain") << std::endl;
    } else {
        std::cerr << "AudioInput: init FAILED — mic bars will be silent" << std::endl;
    }
}
/*CubeTest::CubeTest(int fps, std::string ip, std::string port) : CubeApplication(fps, ip, port) {
    std::cout << "CubeTest constructor ip: " << ip << std::endl;
};*/



bool CubeTest::loop() {
    static int loopcount = 0;

    // Sync audio tuning from params each frame so the webapp can adjust live.
    audio_.setGain(params.getFloat("audioGain"));
    audio_.setBeatThreshold(params.getFloat("beatThreshold"));
    const bool beat = audio_.consumeBeat();

    clear();
//    if(loopcount%2 == 0){
//        fillAll(Color::white());
//    }
//    if(loopcount%2 == 0){
        drawLine3D(Vector3i(0,0,CUBESIZE-loopcount%CUBESIZE),Vector3i(CUBESIZE,0,CUBESIZE-loopcount%CUBESIZE), Color::red());
        drawLine3D(Vector3i(loopcount%CUBESIZE,0,CUBESIZE),Vector3i(loopcount%CUBESIZE,0,0), Color::blue());
//    }
    drawText(ScreenNumber::front, Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), Color::white(), "Screen 0 front");
    drawText(ScreenNumber::right, Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), Color::white(), "Screen 1 right");
    drawText(ScreenNumber::back, Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), Color::white(), "Screen 2 back");
    drawText(ScreenNumber::left, Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), Color::white(), "Screen 3 left");
    drawText(ScreenNumber::top, Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), Color::white(), "Screen 4 top");
    // Mic bar indicator on bottom face: bass/mid/treble bands as filled bars.
    auto bands = audio_.getBands();
    auto drawBar = [&](int row, float level, Color col) {
        int w = static_cast<int>(level * 62.0f);
        if (w > 0)
            drawRect2D(ScreenNumber::top, 1, row, w, row + 7, col, true, col);
    };
    drawBar( 8, bands[0], Color::red());    // bass
    drawBar(20, bands[1], Color::green());  // mid
    drawBar(32, bands[2], Color::blue());   // treble
    // Show raw volume so we can diagnose even when bars are below pixel threshold.
    char volBuf[32];
    snprintf(volBuf, sizeof(volBuf), "V%.3f", audio_.getVolume());
    drawText(ScreenNumber::top, Vector2i(2, 46), Color::white(), volBuf);
    // Beat indicator: filled square top-right corner pulses on detected beat.
    if (beat)
        drawRect2D(ScreenNumber::top, 54, 46, 62, 54, Color::white(), true, Color::white());

    render();
    loopcount++;
    return true;
}