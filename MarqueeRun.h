#ifndef MARQUEERUN_H
#define MARQUEERUN_H
#include <thread>
#include <xstring>


class MarqueeRun {
public:
    void keyboardPollingDisplay(int *ctr, std::string *input, std::string *output);

    void keyboardPolling(std::string *input, std::string *output, bool *runningProgram, int *ctr);

    void RunMarquee();

    std::thread marqueeThread();

    std::thread keyboardThread();

    void comboThread();

};



#endif //MARQUEERUN_H