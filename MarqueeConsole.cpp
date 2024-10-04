#include "MarqueeConsole.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>

MarqueeConsole::MarqueeConsole() {
    this->coordX = 40;
    this->coordY = 3;
    this->state = "";
    this->xDirection = 1;
    this->yDirection = 1;
}

void MarqueeConsole::displayHeader() {
    system("cls");
    std::cout << "****************************************" << std::endl;
    std::cout << "* Displaying a marquee console! *" << std::endl;
    std::cout << "****************************************  ";
}

void MarqueeConsole::setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printMarquee();
}

void MarqueeConsole::printMarquee() {
    std::cout << "Hello world in marquee!"<< std::endl;
}

void MarqueeConsole::checkCursorPosition() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
    int consoleWidth = bufferInfo.dwSize.X;
    int consoleHeight = bufferInfo.dwSize.Y;

    if (this->coordX == 40) {
        xDirection = 1;
    } else if (this->coordX == consoleWidth-35) {
        xDirection = 0;
    }

    if (this->coordY == 0) {
        yDirection = 1;
    } else if (this->coordY == consoleHeight-2) {
        yDirection = 0;
    }

    if (xDirection == 1 && yDirection == 1) {
        this->coordX++;
        this->coordY++;
        this->state = "rightDown";
    } else if (xDirection == 0 && yDirection == 1) {
        this->coordX--;
        this->coordY++;
        this->state = "leftDown";
    } else if (xDirection == 1 && yDirection == 0) {
        this->coordX++;
        this->coordY--;
        this->state = "rightUp";
    } else if (xDirection == 0 && yDirection == 0) {
        this->coordX--;
        this->coordY--;
        this->state = "leftUp";
    }

    setCursorPosition(this->coordX, this->coordY);
}



