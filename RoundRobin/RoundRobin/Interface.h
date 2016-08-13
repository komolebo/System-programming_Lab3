struct Proc
{
	unsigned int ID;
	unsigned int complexity;
	unsigned int time_waited;
	unsigned int time_joined;
	unsigned int time_done;
	unsigned int priority;
	int time_left;
};

struct Settings
{
	unsigned int tasks_count = 30; // 100
	const unsigned int INTENSITY_PERIOD = 1; // 1..10
	const unsigned int MAX_COMPLEX = 15; //15
	const unsigned int MAX_PRIO = 3; // 3
	const unsigned int QUANTUM = 1;
	const unsigned int DELAY = 1000;
};

void run();
void outfile();