#include "MarqueeRun.h"

#include <windows.h>

#include "MarqueeConsole.h"

void MarqueeRun::RunMarquee() {
     MarqueeConsole *marquee_console = new MarqueeConsole;
     bool runningProgram = true;

     while (runningProgram) {
          marquee_console->displayHeader();
          marquee_console->checkCursorPosition();
          Sleep(25);
     }

}
