#include "joystick.h"
#include "LED_matrix.h"
#include "common.h"
#include "sampler.h"
#include "analysis.h"
#include "shutdown.h"
#include "LED_output.h"
#include "user_button.h"

int main(){

    Sampler_startSampling();
    sleepForMs(1000);           //very important, sleep for 1 second

    userButton_initButton();
    LEDMatrix_init();
    LEDOutput_startDisplaying();
    Analysis_init();

    init_shutdown();
    waitForShutdown();
    userButton_shutdownButton();

    return 0;
}





