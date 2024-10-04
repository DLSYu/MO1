#ifndef MARQUEECONSOLE_H
#define MARQUEECONSOLE_H
#include <string>

class MarqueeConsole {
public:
    MarqueeConsole();
    void displayHeader();
    void setCursorPosition(int x, int y);
    void printMarquee();
    void checkCursorPosition();

    int coordX;
    int coordY;
    int xDirection;
    int yDirection;
    std::string state;
};



#endif //MARQUEECONSOLE_H
