# Five State Process Simulator

### What's included

1.	processes.txt - Input file

1.	gantt_chart.txt - Gantt chart from run state is outputted

1.	exited_processes.txt - Holds process information after completion
	(waiting, turnaround, burst, completion time..)

1.	process_stats.txt - Holds process statistics outputted every 30 seconds
	(avg waiting, turnaround, throughput...)


### To Compile:

g++ exit_state.cpp -o exit_state
g++ blocked_state.cpp -pthread -o blocked_state
g++ run_state.cpp -o run_state
g++ ready_state.cpp -pthread -o ready_state
g++ new_state.cpp -o new_state
g++ 5_state_model_simulator.cpp -o 5_state_model_simulator

### To Run:
./5_state_model_simulator
