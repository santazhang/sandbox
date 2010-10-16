#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include <math.h>

#include "bodysystem.h"
#include "bodyparam.h"
#include <iostream>
#include <fstream>
using namespace std;

////////////////////////////////////////
// Demo Parameters
////////////////////////////////////////

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

//bool cycleDemo = true;
int activeDemo = 0;
//unsigned int demoTimer = 0;
NBodyParams activeParams = demoParams[activeDemo];

int numBodies = 0;
BodySystem *nbody = 0;

float* hPos = 0;
float* hVel = 0;
float* hColor = 0;

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

void savetofile(BodySystem *system, char * filename) {

	ofstream fout(filename);
	fout << activeParams.m_timestep << endl;
	fout << activeParams.m_clusterScale << endl;
	fout << activeParams.m_velocityScale << endl;
	fout << activeParams.m_softening << endl;
	fout << activeParams.m_damping << endl;

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

int main(int argc, char** argv) {
	if (argc != 3) {
		cout << "usage: generate <output filename> <num of bodies>" << endl;
		return 1;
	}
	numBodies = atoi(argv[2]);
	cout << "nbodies = " << argv[2] << endl;
	init(numBodies);
	reset(nbody, numBodies, NBODY_CONFIG_RANDOM);
	savetofile(nbody, argv[1]);
	//numIterations = 80; //atoi(argv[2]);
	return 0;
}
