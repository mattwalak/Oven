#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <iostream>
#include <fstream>
#include <float.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>

using namespace std;

int cores, xRes, yRes, startFrame, endFrame;
char * procName;


void renderNextFrame(int frame_num){
	cout << "Rendering frame" << frame_num << endl;
	char * args[] = {"sleep","5",NULL};
	execvp(args[0], args);
	cout << "ERROR IN EXECVPl bailing" << endl;
	exit(1);
}


void renderHorizontal(){
	int nextFrame = startFrame;
	int avaliableCores = cores;
	int * activePids = new int[cores];
	int * activeIndex = new int[cores]; // 1 = active, 0 = inactive

	while(nextFrame < endFrame){
		// See if we can start a new render
		if(avaliableCores > 0){
			int pid = fork();
			if(pid == -1){
				cout << "FORK ERROR; bailing" << endl;
				exit(0);
			}else if(pid == 0){
				renderNextFrame(nextFrame);
			}else{
				avaliableCores--;
				nextFrame++;
				int i = 0;
				while(activeIndex[i] == 1)
					i++;
				activeIndex[i] = 1;
				activePids[i] = pid;
				cout << "added pid " << pid << endl;
			}
		}

		// See if a render has finished
		for(int i = 0; i < cores; i++){
			if(activeIndex[i] == 1){
				int status;
				int result = waitpid(activePids[i], &status, WNOHANG);
				if(result == -1){
					cout << "big error oh no" << endl;
				}else if(result != 0){
					cout << "finished pid " << result << endl;
					avaliableCores++;
					activePids[i] = 0;
				}
			}
		}
	}

}

int main(int argc, char ** argv){
	if(argc != 7){
		cout << "You clearly don't know what you're doing" << endl;
	}

	cores = atoi(argv[1]);
	xRes = atoi(argv[2]);
	yRes = atoi(argv[3]);
	startFrame = atoi(argv[4]);
	endFrame = atoi(argv[5]);
	procName = argv[6];

	renderHorizontal();
}