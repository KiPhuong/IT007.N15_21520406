#include <iostream>
#include <queue>
#include <iomanip>
#include <vector>
#define INF 1e9
using namespace std;


int num_process; //so process

struct process {
	int id;
	char name[10];
	float arr, brust, finish;
	float wait_time, rp_time, tround_time;
	float remain_time;
};

queue<process> qlist;

void swap(struct process* a, struct process* b) {
	struct process tmp = *a;
	*a = *b;
	*b = tmp;
}

// sort increasing id
void sort(process P[]) {	
	for (int i = 0; i < num_process - 1; i++)
		for (int j = i + 1; j < num_process; j++)
			if (P[j].id < P[i].id) swap(&P[i], &P[j]);
}

void show(struct process P[]) {
	sort(P);
	cout << left << setw(6) << "Name" << left << setw(15) << "Arrival Time" << left << setw(15) << "Burst Time"
		<< left << setw(15) << "Complete Time" << left << setw(20) << "Turnaround Time"
		<< left << setw(15) << "Waiting Time" << left << setw(15) << "Respone Time" << "\n";
	float avg_waiting_time = 0;
	float avg_turnaround = 0;
	float avg_respone_time = 0;
	for (int i = 0; i < num_process; i++) {
		cout << setw(6) << P[i].name << setw(15) <<P[i].arr  << setw(15) << P[i].brust << setw(15) << P[i].finish
			<< setw(20) << P[i].tround_time << setw(15) <<  P[i].wait_time << setw(15) << P[i].rp_time<<"\n";
		avg_waiting_time += P[i].wait_time;
		avg_turnaround += P[i].tround_time;
		avg_respone_time += P[i].rp_time;
	}
	avg_waiting_time /= num_process;
	avg_turnaround /= num_process;
	avg_respone_time /= num_process;

	cout << "\nAverage waiting time : " << avg_waiting_time << "\n";
	cout << "Average turn around time: " << avg_turnaround << "\n";
	cout << "Average respone time: " << avg_respone_time << "\n";
}

void SJF(struct process P[]) {

	vector<int> done;
	for (int i = 0; i < num_process; i++) done.push_back(0);

	float time_elapsed = P[0].arr + P[0].brust;
	P[0].finish = time_elapsed;
	P[0].tround_time = P[0].finish - P[0].arr;
	P[0].wait_time = P[0].tround_time - P[0].brust;
	P[0].rp_time = P[0].wait_time;

	done[0] = 1;
	for (int i = 1; i < num_process; i++) {
		float shortestBrust = INF;
		int idx = -1;
		for (int j = 0; j < num_process; j++) {
			if (!done[j] && time_elapsed >= P[j].arr && shortestBrust > P[j].brust) {
				shortestBrust = P[j].brust;
				idx = j;
			}
		}
		if (idx == -1) {
			float shortestarr = INF;
			for (int j = 0; j < num_process; j++)
				if (!done[j] && shortestarr > P[j].arr && P[j].arr > time_elapsed) {
					shortestarr = P[j].arr;
					idx = j;
				}
			if (idx == -1) break;
			time_elapsed = shortestarr;

		}
		time_elapsed += P[idx].brust;
		P[idx].finish = time_elapsed;
		P[idx].tround_time = P[idx].finish - P[idx].arr;
		P[idx].wait_time = P[idx].tround_time - P[idx].brust;
		P[idx].rp_time = P[idx].wait_time;
		done[idx] = 1;
	}
	show(P);
}

void RR(struct process P[]) {
	float Quantum_time;
	printf("\nEnter Quantum time: ");
	cin >> Quantum_time;

	queue<process> Q;

	float time_elapsed = 0;
	vector<int> done;
	for (int i = 0; i < num_process; i++) done.push_back(0);

	int numP = 0;
	int count = num_process;
	while (count > 0) {
		while (numP < num_process) {
			if (done[numP] == 0) {
				Q.push(P[numP]);
				time_elapsed = P[numP++].arr;
				break;
			}
			numP++;
		}
		while (!Q.empty()) {
			int idx = Q.front().id;
			Q.pop();
			if (P[idx].remain_time == P[idx].brust) {
				P[idx].rp_time = time_elapsed - P[idx].arr;
			}

			int rtime = P[idx].remain_time;
			if (rtime > Quantum_time) rtime = Quantum_time;

			time_elapsed += rtime;
			P[idx].remain_time -= rtime;

			while (numP < num_process && P[numP].arr <= time_elapsed) {
				Q.push(P[numP]);
				numP++;
			}

			if (P[idx].remain_time == 0) {
				P[idx].finish = time_elapsed;
				P[idx].tround_time = P[idx].finish - P[idx].arr;
				P[idx].wait_time = P[idx].tround_time - P[idx].brust;
				done[idx] = 1;
				count--;
			}
			else Q.push(P[idx]);
		}
	}
	show(P);
}

int SRTJob(const struct process P[], int time_elapsed, const vector<int> done) {
	int jobIndex = -1;
	float shortestTime = INF;
	// find minimun arr_time and rei
	for (int i = 0; i < num_process; i++) {
		if (!done[i] && P[i].arr <= time_elapsed && P[i].remain_time < shortestTime) {
			jobIndex = i;
			shortestTime = P[i].remain_time;
		}
	}
	return jobIndex;
}

void SRTF(struct process P[]) {
	float time_elapsed = P[0].arr;
	int numP = 1;
	int cntP = num_process;
	int preidx = 0;

	vector<int> done;
	for (int i = 0; i < num_process; i++) done.push_back(0);


	while (cntP > 0) {
		//printf("%d %f %d %.2f\n",numP,time_elapsed,preidx,P[preidx].rp_time);
		if (P[preidx].brust == P[preidx].remain_time) P[preidx].rp_time = time_elapsed - P[preidx].arr;
		if (numP < num_process && time_elapsed + P[preidx].remain_time >= P[numP].arr) {
			float rtime = time_elapsed + P[preidx].remain_time - P[numP].arr;
			time_elapsed = P[numP].arr;
			P[preidx].remain_time = rtime;
			if (rtime == 0) {
				P[preidx].finish = time_elapsed;
				P[preidx].tround_time = P[preidx].finish - P[preidx].arr;
				P[preidx].wait_time = P[preidx].tround_time - P[preidx].brust;
				done[preidx] = 1;
				cntP--;
			}

			if (P[numP].brust < rtime) {
				preidx = numP;
			}
			numP++;
		}
		else {
			int idx = SRTJob(P, time_elapsed, done);
			if (numP < num_process) {
				if (idx == -1) {
					time_elapsed = P[numP].arr;
					preidx = numP;
					numP++;
				}
				else {
					if (time_elapsed + P[idx].remain_time < P[numP].arr) {
						if (P[idx].brust == P[idx].remain_time) P[idx].rp_time = time_elapsed - P[idx].arr;

						time_elapsed += P[idx].remain_time;
						P[idx].remain_time = 0;
						P[idx].finish = time_elapsed;
						P[idx].tround_time = P[idx].finish - P[idx].arr;
						P[idx].wait_time = P[idx].tround_time - P[idx].brust;
						done[idx] = 1;
						cntP--;
					}
					preidx = idx;
				}
			}
			else {
				if (P[idx].brust == P[idx].remain_time) P[idx].rp_time = time_elapsed - P[idx].arr;

				time_elapsed += P[idx].remain_time;
				P[idx].remain_time = 0;
				P[idx].finish = time_elapsed;
				P[idx].tround_time = P[idx].finish - P[idx].arr;
				P[idx].wait_time = P[idx].tround_time - P[idx].brust;
				done[idx] = 1;
				preidx = idx;
				cntP--;
			}
		}
	}
	show(P);
}


int main() {
	struct process P[15];
	printf("Enter number of process: ");
	cin >> num_process;
	for (int i = 0; i < num_process; i++) {
		printf("Enter Process Name: "); cin>> P[i].name;
		printf("Enter Arrival Time: "); cin >> P[i].arr;
		printf("Enter Burst Time  : "); cin>>P[i].brust;
		P[i].finish = P[i].tround_time = P[i].wait_time = P[i].rp_time = 0;
		P[i].remain_time = P[i].brust;
		P[i].id = i;
		printf("\n");
	}
	int type;
	do {
		cout << "Enter algorithm: 1.SJF \t 2.RR \t 3.SRTF \n";
		cin >> type;
	} while (type != 1 && type != 2 && type != 3);
	if (type == 1) SJF(P);
	else if (type == 2) RR(P);
	else SRTF(P);
	return 0;
}



