#include "Interface.h"
#include <vector>
#include <Windows.h>
#include <fstream>

using namespace std;

vector<Proc *> get_best_prio();
unsigned int index_by_id(unsigned int id);
void addProc(unsigned int & tact, unsigned int & id);
void output();

vector<Proc> Queue;
vector<Proc> ListCompleted;
Settings settings;

double UNUSED = 0; // Processor's time is not used

void run()
{
	unsigned int tact = 0;
	static unsigned int ID = 0;
	vector<Proc *> proc_best_prio;
	unsigned int last_priority = settings.MAX_PRIO + 1, tmp_index;

	srand(1000);
	while (settings.tasks_count || Queue.size())
	{
		// Add process with possibility
		addProc(tact, ID);

		// Find best priority process list
		proc_best_prio = get_best_prio();

		// Find next process index.		Priority changed:
		if (proc_best_prio.size() && proc_best_prio[0]->priority != last_priority)
			tmp_index = 0;
		else // Priority not changed
			tmp_index = (tmp_index < 0 || tmp_index >= proc_best_prio.size() - 1) ? 0 : tmp_index + 1;

		if (proc_best_prio.size())
		{
			Proc * proc = proc_best_prio[tmp_index];
			proc->time_left -= settings.QUANTUM;
			last_priority = proc->priority;

			// Rest are waiting
			for (auto & p : Queue)
				if (p.ID != proc->ID)
					p.time_waited += settings.QUANTUM;

			// Check if task completed
			if (proc->time_left <= 0)
			{
				proc->time_done = tact + settings.QUANTUM + proc->time_left;
				proc->time_left = 0;

				ListCompleted.push_back(*proc);
				Queue.erase(Queue.begin() + index_by_id(proc->ID));
				proc_best_prio.erase(proc_best_prio.begin() + tmp_index);
			}

			// Next processes arrive during the quantum
			for (unsigned int t0 = tact + 1; t0 < tact + settings.QUANTUM; ++t0)
				addProc(t0, ID);

			tact += settings.QUANTUM;
		}
		else  // No process in queue. Go to next tact
		{
			tact++;
			UNUSED++;
		}

		output();
		Sleep(settings.DELAY);
	}
	UNUSED /= tact;
}

vector<Proc *> get_best_prio()
{
	vector<Proc *> proc_max_priority;

	// Find max priority;
	unsigned int min = settings.MAX_PRIO + 1;
	for (auto & p : Queue)
		if (p.priority < min)
			min = p.priority;

	for (auto & p : Queue)
		if (p.priority == min)
			proc_max_priority.push_back(&p);

	return proc_max_priority;
}

unsigned int index_by_id(unsigned int id)
{
	int pos = 0;
	for (auto & v : Queue)
		if (v.ID == id)
			return pos;
		else
			pos++;
	return -1;
}

void addProc(unsigned int & tact, unsigned int & id)
{
	if (settings.tasks_count && tact % settings.INTENSITY_PERIOD == 0)
	{
		Proc p;
		p.time_left = p.complexity = rand() % settings.MAX_COMPLEX + 1;
		p.priority = rand() % settings.MAX_PRIO;
		p.time_joined = tact;
		p.ID = id++;
		p.time_waited = 0;

		Queue.push_back(p);
		settings.tasks_count--;
	}
}

void output()
{
	system("cls");
	//printf("%d / %d \n", ListCompleted.size(), Queue.size());
	printf("%3s %10s %6s %6s %6s %6s %6s\n", "ID", "priority", "join", "todo", "wait", "left", "done");

	for (auto & p : Queue)
		printf("%3d %10d %6d %6d %6d %6d \n", p.ID, p.priority, p.time_joined, p.complexity, p.time_waited, p.time_left);
	
	printf("__________________________________________\n");
	for (auto & p : ListCompleted)
		printf("%3d %10d %6d %6d %6d %6d %6d\n", p.ID, p.priority, p.time_joined, p.complexity, p.time_waited, p.time_left, p.time_done);
	
}

void outfile()
{
	ofstream f("Statistics.txt");

	f << "ID\t" << "prio\t" << "join\t" << "todo\t" << "wait\t" << "done\t" << endl;

	int average_wait = 0, average_complex = 0;
	for (auto & p : ListCompleted)
	{
		f << p.ID << '\t' << p.priority << '\t' << p.time_joined << '\t' << p.complexity 
			<< '\t' << p.time_waited << '\t' << p.time_done << endl;
		average_wait += p.time_waited;
		average_complex += p.complexity;
	}

	f << endl << "Average wait: " << (double)average_wait / ListCompleted.size() << endl;
	f << "Average intensity: " << 1 / (double)settings.INTENSITY_PERIOD << endl;
	f << "Average complexity: " << (double)average_complex / ListCompleted.size() << endl;
	f << "Unused resources time: " << 100 * UNUSED << endl;

	// For f(priorities)
	int * priorities = new int[settings.MAX_PRIO], * n = new int[settings.MAX_PRIO];
	for (unsigned int i = 0; i < settings.MAX_PRIO; ++i)
		priorities[i] = n[i] = 0;

	for (auto & p : ListCompleted)
		for (unsigned int i = 0; i < settings.MAX_PRIO; ++i)
			if (i == p.priority)
			{
				n[i]++;
				priorities[i] += p.time_waited;
			}

	f << endl;
	for (unsigned int i = 0; i < settings.MAX_PRIO; ++i)
		f << i << '=' << priorities[i] / n[i] << endl;

	f.close();
}