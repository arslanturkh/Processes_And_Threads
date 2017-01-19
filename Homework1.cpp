
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std;
unsigned int microseconds;

// paramaters for the threads functions
struct threadFuncktionParameters{
	string fileName[10];
	string directory;
	string programName;
	string type;
	int programCounter;
	int threadCounter[10];
	int myCounter = 0;
};

// threads will execute the function
void *ThreadFunction(void* param);


int main(int argc, char **argv){
	string directory = argv[1];
	string type;
	string programName = "Master";
	DIR *dir;
	struct dirent *dirpro;
	int pid, fileNumber = 0, programCounter = 0;
	
	//cout << "Enter folder name with path: ";
	//cin >> directory;
	cout << programName << ": Start" << endl;
	
	type = "Project";
	pid = fork(); // slave 1  programCounter=0 pid =0
	
	if(pid != 0){
		//waitpid(pid,NULL,0); // waits slave 1
		type = "Midterm";
		programCounter = 1;
		pid = fork(); // slave 2 pro = 1 pid = 0
	}
	
	if(pid != 0){
		//waitpid(pid,NULL,0); // waits slave 2
		type = "Final";
		programCounter = 2;
		pid = fork(); // slave 3 pro = 2 pid = 0
	}
	
	if (pid == 0){
		string fileName[10], threadName;
		int tid;
		stringstream s;
		s << (programCounter+1); // int to string
		programName = "Slave" + s.str();
		
		cout << programName << ": " << type << " statistics" << endl;
		
		dir = opendir(directory.c_str());
		
		if(dir == NULL){
			cout << "Opening Error" << endl;
		}
		
		while ((dirpro = readdir(dir))){
			if (dirpro->d_name != NULL){
				if (dirpro->d_name[0] != '.'){
					fileName[fileNumber] = dirpro->d_name;
					fileNumber++;
				}
			}
		}
		
		threadFuncktionParameters paramater;// create struct
		// struct value
		for (int i=0; i < fileNumber; i++){
			paramater.fileName[i] = fileName[i];
		}
		
		paramater.directory = directory;
		paramater.programName = programName;
		paramater.type = type;
		paramater.programCounter = programCounter;
		
		pthread_t threads[fileNumber]; 
		pthread_attr_t attr;
		
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		
		for (int i=0; i < fileNumber; i++){
			paramater.threadCounter[i] = i; // set thread counter to i
			// creating threads
			tid = pthread_create(&threads[i], &attr, ThreadFunction, (void*)&paramater);
			
			if (tid){
				cout << "Creating thread error " << endl;
				exit(-1);
			}
			
			//tid = pthread_join(threads[i], 0); // threads wait for each other
			//usleep(10000);
		}
		
		pthread_attr_destroy(&attr);
		for (int i = 0; i < fileNumber; i++){
			tid = pthread_join(threads[i], 0); // threads wait for each other
			
			if(tid){
				cout << "Error:unable to join, " << tid << endl;
				exit(-1);
			}
		}
		
		cout << programName << ": Done" << endl;
		closedir(dir);
	}// end slave process
	
	//waitpid (pid,NULL,0); // wait slave 3
	
	if (pid != 0) cout << programName << ": Finish" << endl;
}

void *ThreadFunction(void* param){
	struct threadFuncktionParameters* paramater;
	paramater = (struct threadFuncktionParameters*) param; //get inputs
	string line, lineNumber[3], name;
	ifstream file;
	int max = 0, min = 0, med = 0, lineCounter = 0, myNumber = 0, j, temp;
	int myArray[200];
	
	stringstream s1, s2;
    s1 << (paramater->programCounter+1);
    s2 << (paramater->threadCounter[paramater->myCounter]+1); 
	
    string threadName = "Thread" + s1.str() + "." + s2.str(); // thread name
	
	name = paramater->directory + "/" + paramater->fileName[paramater->threadCounter[paramater->myCounter]]; // file name
	paramater->myCounter++;	
	file.open(name.c_str());
	
	while(getline(file, line)){ // reads line
		istringstream lineArray(line);
		lineArray >> lineNumber[0] >> lineNumber[1] >> lineNumber[2];
		lineCounter++;
		
		myNumber = atoi(lineNumber[paramater->programCounter].c_str());
		
		if(lineCounter == 1){
			max = myNumber;
			min = myNumber;
		}
		else if(myNumber > max){
			max = myNumber;
		}
		else if(myNumber < min){
			min = myNumber;
		}
		
		myArray[lineCounter-1] = myNumber;
	}
	
	for (int i=0; i<lineCounter; i++){
		j=i;
		
		while ((j > 0) && (myArray[j]<myArray[j-1])){
			temp = myArray[j];
			myArray[j]=myArray[j-1];
			myArray[j-1]=temp;
			j--;
		} 
	}
	
	if(lineCounter%2 == 0) med = myArray[lineCounter/2];
	else med = myArray[(lineCounter+1)/2];
	
	cout << threadName << ": " << paramater->fileName[paramater->threadCounter[paramater->myCounter-1]] << " ";
	cout << paramater->type << " Min:" << min << " Max:" << max << " Med:" << med << endl;
	
	pthread_exit(NULL);
}

