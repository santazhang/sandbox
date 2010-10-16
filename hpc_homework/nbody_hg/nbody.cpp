#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <assert.h>
#include <math.h>

#include "bodysystem.h"
#include "bodyparam.h"

#include "time.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int numBodies = 1024;
int numIterations = 0; // run until exit

////////////////////////////////////////
// Demo Parameters
////////////////////////////////////////
/*

struct NBodyParams
{	   
	float m_timestep;
	float m_clusterScale;
	float m_velocityScale;
	float m_softening;
	float m_damping;

	void print() { printf("{ %f, %f, %f, %f, %f, %f },\n", 
				   m_timestep, m_clusterScale, m_velocityScale, 
				   m_softening, m_damping); }
};

NBodyParams demoParams[] = 
{
	{ 0.016f, 1.54f, 8.0f, 0.1f, 1.0f },
	{ 0.016f, 0.68f, 20.0f, 0.1f, 1.0f },
	{ 0.0006f, 0.16f, 1000.0f, 1.0f, 1.0f },
	{ 0.0006f, 0.16f, 1000.0f, 1.0f, 1.0f },
	{ 0.0019f, 0.32f, 276.0f, 1.0f, 1.0f },
	{ 0.0016f, 0.32f, 272.0f, 0.145f, 1.0f },
	{ 0.016000, 6.040000, 0.000000, 1.000000, 1.000000 },
};
*/
bool cycleDemo = true;
//int activeDemo = 0;
unsigned int demoTimer = 0;

NBodyParams activeParams;
BodySystem *nbody		 = 0;

float* hPos = 0;
float* hVel = 0;
float* hColor = 0;
/*
void reset(BodySystem *system, int numBodies, 
	   NBodyConfig config)
{
	// initalize the memory
	// reset(nbody, numBodies, NBODY_CONFIG_RANDOM);
	randomizeBodies(config, hPos, hVel, hColor, activeParams.m_clusterScale, 
					activeParams.m_velocityScale, numBodies);

	system->setArray(BodySystem::BODYSYSTEM_POSITION, hPos);
	system->setArray(BodySystem::BODYSYSTEM_VELOCITY, hVel);
}
*/
void loadfromfile(char * filename) {
	ifstream fin(filename);
	//int n;
	//fin.read((char *)&numBodies, sizeof(int));

	fin >> activeParams.m_timestep; 
	fin >> activeParams.m_clusterScale;
	fin >> activeParams.m_velocityScale;
	fin >> activeParams.m_softening;
	fin >> activeParams.m_damping;

	fin >> numBodies;
	//cout << numBodies << endl;
	
	nbody = new BodySystem(numBodies);
	hPos = new float[numBodies * 4];
	hVel = new float[numBodies * 4];
	nbody->setSoftening(activeParams.m_softening);

	for (int i = 0; i < numBodies * 4; ++i) fin >> hPos[i];
	for (int i = 0; i < numBodies * 4; ++i) fin >> hVel[i];
	nbody->setArray(BodySystem::BODYSYSTEM_POSITION, hPos);
	nbody->setArray(BodySystem::BODYSYSTEM_VELOCITY, hVel);

	fin.close();
}
/*
void savetofile(BodySystem *system, char * filename) {

	ofstream fout(filename);
	fout << numBodies << endl;
	//fout.write((char *)(&numBodies), sizeof(int));
	//cout << numBodies << endl;
	float* hPos = system->getArray(BodySystem::BODYSYSTEM_POSITION);
	float* hVel = system->getArray(BodySystem::BODYSYSTEM_VELOCITY);

	for (int i = 0; i < numBodies * 4; ++i) {
		//fout.write((char *)&hPos[i], sizeof(hPos[i]));
		//fout.write((char *)&hVel[i], sizeof(hVel[i]));
		//cout << hPos[i] << " " << hVel[i] << endl;
		fout << hPos[i] << " ";
	}
	fout << endl;
	for (int i = 0; i < numBodies * 4; ++i) fout << hVel[i] << " ";
	fout << endl;

	fout.close();
}

void init(int numBodies)
{
	nbody = new BodySystem(numBodies);
	// allocate host memory
	hPos = new float[numBodies*4];
	hVel = new float[numBodies*4];
	nbody->setSoftening(activeParams.m_softening);
}
*/

void computePerfStats(double &interactionsPerSecond, double &gflops, 
			  float milliseconds, int iterations)
{
	const int flopsPerInteraction = 20;
	interactionsPerSecond = (float)numBodies * (float)numBodies;
	interactionsPerSecond *= 1e-9 * iterations * 1000 / milliseconds;
	gflops = interactionsPerSecond * (float)flopsPerInteraction;
}

void runBenchmark(int iterations)
{
	double startTime=get_time();
	for (int i = 0; i < iterations; ++i)
	{
		nbody->update(activeParams.m_timestep);
	}
	double endTime=get_time();

	float milliseconds = (endTime-startTime)*1000;
	double interactionsPerSecond = 0;
	double gflops = 0;
	computePerfStats(interactionsPerSecond, gflops, milliseconds, iterations);
	
	printf("%d bodies, total time for %d iterations: %0.3f ms\n", 
		   numBodies, iterations, milliseconds);
	printf("= %0.3f billion interactions per second\n", interactionsPerSecond);
	printf("= %0.3f GFLOP/s at %d flops per interaction\n", gflops, 20);   
}

//////////////////////////////////////////////////////////////////////////////
// Program main
//////////////////////////////////////////////////////////////////////////////
int
main( int argc, char** argv) 
{
	
	if(argc != 3){
		cout<<"uasge: "<<argv[0]<<" <input data filename> <num iters>"<<endl;
		return 1;
	}
	

	//numBodies = 3; //atoi(argv[1]);
	numIterations = atoi(argv[2]);
	cout << "Iteration count: " << numIterations << endl;	
//********Load_From_File_Example*******************
	loadfromfile(argv[1]);
//  init and reset are not needed
//*************************************************


//*****************FIRST GENERATE******************
//	init(numBodies);
//	reset(nbody, numBodies, NBODY_CONFIG_RANDOM);
//	savetofile(nbody, "miao.txt");
//*************************************************
	runBenchmark(numIterations);
	const int N=numBodies>10?10:numBodies;
	float * pos=nbody->getArray(BodySystem::BODYSYSTEM_POSITION);
	for(int i=0;i<N;i++){
		cout<<"("<<pos[i*4+0]<<","<<pos[i*4+1]<<","<<pos[i*4+2]<<")"<<endl;
	}

	delete nbody;
	delete [] hPos;
	delete [] hVel;
	delete [] hColor;

	return 0;
}
