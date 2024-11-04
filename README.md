# MO1
#### Machine Project Group 13
#### Names: <br/>
* AGERO, JANELLE EINSLEY SALVADOR<br/>
* DELOS REYES, JOMAR MAGTALAS<br/>
* GAMBOA, MIKKEL DOMINIC VERAR<br/>
* YU, HANZ PATRICK OCHONG<br/>

<br/>
This is an implementation of an OS Emulator coded using C++, which was created as a major output for the course CSOPESY. The main programs that were made are the Processes Scheduler and the Command-Line Interpreter.
<br/>
<br/>

The `main` function can be found in the file called `MO1.cpp`

#### Instructions
To run the program, the `initialize` command must first be executed to load the parameters from the `config.txt`<br/>
After running the command, the user is able to run the following commands to simulate the OS Emulator.
Command | Description
-|-
initialize | Loads the config file into the emulator
screen -s | When the user types `screen -s <process name>` from the main menu console, the console will clear its contents and the screen of the process that was just created will appear. From there the user will have two options: (1) **process-smi** : prints simple information about the current process, and (2) **exit** : returns the user to the main menu
screen -r | When the user types `screen -r <process name>`, it allows them to access a process already created.
scheduler-test | Generates dummy processes into the ready queue. The frequency can be updated in the `config.txt`.
scheduler-stop | Stops generating dummy processes.
screen -ls | This lists the CPU utilization, cores used, and cores available, as well as a summary of the running and finished processes
report-util | Saves the same content from screen -ls into a file called `csopesy-log.txt `
clear | Clears the screen of any clutter
exit | Exits the program

<br/><br/>

The config file contains the following parameters:
Parameter |	Description
----------|------------
num-cpu	| Number of CPUs available. The range is [1, 128].
scheduler |	The scheduler algorithm: “fcfs” or “rr”.
quantum-cycles |	The time slice is given for each processor if a round-robin scheduler is used. Has no effect on other schedulers. The range is [1, 2^32].
batch-process-freq |	The frequency of generating processes in the “scheduler-test” command in CPU cycles. The range is [1, 2^32]. If one, a new process is generated at the end of each CPU cycle.
min-ins |	The minimum instructions/command per process. The range is [1, 2^32].
max-ins |	The maximum instructions/command per process. The range is [1, 2^32].
delays-per-exec |	Delay before executing the next instruction in CPU cycles. The delay is a “busy-waiting” scheme wherein the process remains in the CPU. The range is [0, 2^32]. If zero, each instruction is executed per CPU cycle.



 
