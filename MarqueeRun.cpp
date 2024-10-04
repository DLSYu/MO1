#include "MarqueeRun.h"

#include <conio.h>
#include <iostream>
#include <windows.h>

#include "MarqueeConsole.h"


void MarqueeRun::RunMarquee() {
     MarqueeConsole *marquee_console = new MarqueeConsole;
     bool runningProgram = true;
     std::string input;
     std::string output;
     int ctr = 2;

     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

     while (runningProgram) {
          marquee_console->displayHeader();
          marquee_console->checkCursorPosition();

          GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
          COORD bottom;
          bottom.X = 0;
          bottom.Y = bufferInfo.dwSize.Y - ctr;
          SetConsoleCursorPosition(hConsole, bottom);


          std::cout << "Enter a command for MARQUEE_CONSOLE: " << input;
          std::cout << std::endl << output;

          if (_kbhit()) {
               char ch = _getch();
               if (ch == '\r') { // Enter key
                    if (input == "exit") {
                         runningProgram = false;
                    } else if (!input.empty()) {
                         output = "Command processed in MARQUEE_CONSOLE: " + input + "\n" + output;
                         ctr++;
                    }
                    input.clear();
               } else if (ch == '\b') {
                    if (!input.empty()) {
                         input.pop_back();
                    }
               }
               else {
                    input += ch;
               }
          }

          Sleep(50);

     }

}
