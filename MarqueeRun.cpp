#include "MarqueeRun.h"

#include <conio.h>
#include <iostream>
#include <thread>
#include <windows.h>

#include "MarqueeConsole.h"

void MarqueeRun::keyboardPollingDisplay(int* ctr, std::string* input, std::string* output) {
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
     GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
     COORD bottom;
     bottom.X = 0;
     bottom.Y = bufferInfo.dwSize.Y - *ctr;
     SetConsoleCursorPosition(hConsole, bottom);


     std::cout << "Enter a command for MARQUEE_CONSOLE: " << *input;
     std::cout << std::endl << *output;
}

void MarqueeRun::keyboardPolling(std::string* input, std::string* output, bool* runningProgram, int* ctr) {
     if (_kbhit()) {
          char ch = _getch();
          if (ch == '\r') {
               if (*input == "exit") {
                    *runningProgram = false;
               } else if (!input->empty()) {
                    *output = "Command processed in MARQUEE_CONSOLE: " + *input + "\n" + *output;
                    (*ctr)++;
               }
               input->clear();
          } else if (ch == '\b') {
               if (!input->empty()) {
                    input->pop_back();
               }
          }
          else {
               *input += ch;
          }
     }
}


void MarqueeRun::RunMarquee() {
     MarqueeConsole *marquee_console = new MarqueeConsole;
     bool runningProgram = true;
     bool* ptrRunningProgram = &runningProgram;
     std::string input;
     std::string output;
     int ctr = 2;
     int* ptrCtr = &ctr;


     while (runningProgram) {
          marquee_console->displayHeader();
          marquee_console->checkCursorPosition();
          keyboardPollingDisplay(ptrCtr, &input, &output);
          keyboardPolling(&input, &output, ptrRunningProgram, ptrCtr);

          Sleep(50);

     }

}

std::thread MarqueeRun::marqueeThread() {
     MarqueeConsole *marquee_console = new MarqueeConsole;
          marquee_console->displayHeader();
          marquee_console->checkCursorPosition();
          // Sleep(500);
     return std::thread(0);

}

std::thread MarqueeRun::keyboardThread() {
     bool runningProgram = true;
     std::string input;
     std::string output;
     int ctr = 2;
          HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
          CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
          GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
          COORD bottom;
          bottom.X = 0;
          bottom.Y = bufferInfo.dwSize.Y - ctr;
          SetConsoleCursorPosition(hConsole, bottom);


          std::cout << "Enter a command for MARQUEE_CONSOLE: " << input;
          std::cout << std::endl << output;
          if (_kbhit()) {
               char ch = _getch();
               if (ch == '\r') {
                    if (input == "exit") {
                         std::cout<<"gout";
                    } else if (!input.empty()) {
                         output = "Command processed in MARQUEE_CONSOLE: " + input + "\n" + output;
                         (ctr)++;
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
          // Sleep(500);
     return std::thread(0);
}

void MarqueeRun::comboThread() {
     std::thread marquee(marqueeThread());
     std::thread keyboard(keyboardThread());
     marquee.join();
     keyboard.join();
}