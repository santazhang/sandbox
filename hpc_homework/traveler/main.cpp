/*
旅行推销员问题（Traveling Salesman Problem）是一个多局部最优的最优化问题：
有n个城市，一个推销员要从其中某一个城市出发，唯一走遍所有的城市，求最短的路线。
*/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
//#include <windows.h>
#include <sys/types.h>
#include <time.h>
/*
DWORD timeGetTime(VOID); 
  Header: Declared in Mmsystem.h; include Windows.h.
  Library: Use Winmm.lib.
*/


#define MAX_SITES	100		// 城市个数
#define MAX_BUFFER	10

using namespace std;

float site_x[MAX_SITES];			// 城市坐标X
float site_y[MAX_SITES];			// 城市坐标Y
float dist[MAX_SITES][MAX_SITES];	// 城市之间的距离
int best_path[MAX_SITES];			// 最佳路线
float min_dist = -1;				// 最佳路线的长度

void getShortPath(int n)
{
	int curr_path[MAX_SITES] = {0};		// 当前路线，尝试从城市0开始
	float curr_dist[MAX_SITES] = {0};	// 当前路线的长度
	int visited[MAX_SITES] = {0};		// 标记已走过的城市
	int ptr = 0;
	int path_too_long = 0;				// 当前路线长度是否比最佳路线长
	int site_not_visited;
	
	visited[curr_path[ptr]] = 1;

	do {
		if (ptr + 1 < n && path_too_long == 0)
		{
			site_not_visited = 0;
			while (visited[site_not_visited])
			{
				site_not_visited++;
			}
			visited[site_not_visited] = 1;
			curr_path[++ptr] = site_not_visited;
		}
		else
		{
			path_too_long = 0;

			do {
				visited[curr_path[ptr]] = 0;
				site_not_visited = curr_path[ptr] + 1;
				
				while (site_not_visited < n && visited[site_not_visited])
				{
					site_not_visited++;
				}

				if (site_not_visited < n)
				{
					curr_path[ptr] = site_not_visited;
					visited[site_not_visited] = 1;
				}
				else
				{
					ptr--;
				}
			} while (site_not_visited >= n && ptr >= 0);
		}

		if (ptr > 0)
		{
			// 计算目前路线的总长度
			curr_dist[ptr] = curr_dist[ptr - 1] + dist[curr_path[ptr]][curr_path[ptr -1]];
			if (min_dist > 0 && curr_dist[ptr] > min_dist)
			{
				path_too_long = 1;
			}
		}

		if (ptr == n - 1 && path_too_long == 0)
		{
			// 找到一条更短的路线
			min_dist = curr_dist[ptr];
			for (int idx=0; idx<n; idx++)
			{
				best_path[idx] = curr_path[idx];
			}
		}
	} while (ptr >= 0);
}

int main(int argc, char *argv[])
{
	int i;
	char buf1[MAX_BUFFER + 1];
	char buf2[MAX_BUFFER + 1];
	int lineNum, siteNum=0;		// 城市的总数
	char *dataFile;  

	if (argc < 2)
	{
		dataFile = "genes.dat";
	}
	else
	{
		dataFile = argv[1];
	}

	ifstream dataStr(dataFile);
	if (!dataStr)
	{
		cerr << "Error: Couldn't find gene data file `" << dataFile << "'" << endl;
		return -1;
	}

	i = 0;
	while (dataStr.getline(buf1, MAX_BUFFER))
		i++;

	lineNum = (unsigned int)i + 1;

	dataStr.clear();
	dataStr.seekg(0, ios::beg);

	for (i = 0; i < (lineNum - 1); i++)
    {
		if (!(dataStr.getline(buf1, MAX_BUFFER, ',')))
		{
			cerr << "Error: Bad format or unexpected end of file at line " << i << " in file `" << dataFile << "'" << endl;
			break;
		}
		if (!(dataStr.getline(buf2, MAX_BUFFER)) && !(dataStr.eof()))	  
		{
			cerr << "Error: Bad format or unexpected end of file at line " << i << " in file `" << dataFile << "'" << endl;
			break;
		}
		site_x[i] = (float)atof(buf1);
		site_y[i] = (float)atof(buf2);
		siteNum = i;
    }

	siteNum += 1;

//	DWORD startTime=timeGetTime();

	for (int s1=0; s1<siteNum; s1++)
	for (int s2=s1+1; s2<siteNum; s2++)
	{
		float d = sqrt( (site_x[s1] - site_x[s2])*(site_x[s1] - site_x[s2]) +
						(site_y[s1] - site_y[s2])*(site_y[s1] - site_y[s2]) );
		dist[s1][s2] = dist[s2][s1] = d;
	}

	getShortPath(siteNum);

//	DWORD endTime=timeGetTime();
	for (i=0; i<siteNum; i++)
	{
		cout << best_path[i] << " ";
	}
	cout << endl << min_dist;
//	cout << endl << "Calculations took " << endTime-startTime << "ms.\n";

	return 0;
}
