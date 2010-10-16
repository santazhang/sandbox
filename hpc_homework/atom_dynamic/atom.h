#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <fstream>
#include<iostream>
#include<map>

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define DATATYPE double
#define Power2(a) ( (a)*(a) )
#define GO_NEXT(set)\
        set = set->next
#define DESTORY_LIST(TYPE,t_head)                       \
if (1) {  TYPE *t_tmp;                                  \
   while (t_head){  t_tmp= t_head;          	        \
                    GO_NEXT(t_head) ;                	\
                    delete  t_tmp;}               		\
}
#define Vector_Differ(C,A,B)\
       ( (C)[0] = ( (A)[0]- (B)[0] ), \
         (C)[1] = ( (A)[1]- (B)[1] ), \
         (C)[2] = ( (A)[2]- (B)[2] ) )  
#define DOT2(A,B) ( ((A)[0]*(B)[0]) + ((A)[1]*(B)[1]) )
#define DOT3(A,B) ( ((A)[0]*(B)[0]) + ((A)[1]*(B)[1]) + ((A)[2]*(B)[2]) )
#define DOT4(A,B) ( ((A)[0]*(B)[0]) + ((A)[1]*(B)[1]) + ((A)[2]*(B)[2]) + ((A)[3]*(B)[3]) )         
#define CDOT(A,B,R)\
  ( (R)[0] = ( ((A)[1]*(B)[2]) - ((B)[1]*(A)[2]) ) ,\
    (R)[1] = ( ((A)[2]*(B)[0]) - ((A)[0]*(B)[2]) ) ,\
    (R)[2] = ( ((A)[0]*(B)[1]) - ((A)[1]*(B)[0]) ) )
#define weight_C(A) ( ( (A) < (rCut) ) ? ( 1 - ((A)/(rCut)) ):(0.0) )
#define weight_C2(A) ( Power2( (weight_C(A)) ) )
#define weight_CD(A) ( ( (A) < (rDifferent) ) ? ( 1 - ((A)/(rDifferent)) ):(0.0) )
#define weight_CD2(A) ( Power2( (weight_CD(A)) ) )
#define weight_R(A) ( ( (A) < (rCut) ) ? ( 1 - ((A)/(rCut)) ):(0.0) )
#define weight_D(A) ( Power2( (weight_R(A)) ) )
double G_TABLE[25]=
{-3     ,
-2.75   ,
-2.5	,
-2.25	,
-2	,
-1.75	,
-1.5	,
-1.25	,
-1	,
-0.75	,
-0.5	,
-0.25	,
0	,
0.25	,
0.5	,
0.75	,
1	,
1.25	,
1.5	,
1.75	,
2	,
2.25	,
2.5	,
2.75	,
3	};

double U_TABLE[25]=
{0.001,
0.003,
0.005,
0.012,
0.023,
0.04 ,
0.067,
0.106,
0.159,
0.227,
0.309,
0.401,
0.5  ,
0.599,
0.691,
0.773,
0.841,
0.894,
0.933,
0.96 ,
0.977,
0.988,
0.995,
0.997,
0.999};

/*
double Gaussian_unit(unsigned long int *seedA,unsigned long int *seedB)
{
	double x,y,r;
		do{   x=LCG_FunctionD_Sign(seedA);
		      y=LCG_FunctionD_Sign(seedB);
		      r=x*x+y*y; } while (r>=1.0);
	return y*sqrt(-2.0*log(r)/r);
}
*/
void Normalize(DATATYPE *a)
{
  DATATYPE tmp = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
  if ( tmp > 1e-12 ) tmp = (1+1e-12)*sqrt(tmp);
  if ( tmp > 1e-12 ) 
  {
	  a[0] /= tmp;
	  a[1] /= tmp;
	  a[2] /= tmp;
  }
}

DATATYPE weight_linear(DATATYPE A)
{
	if (( A < 1 ) && ( A >= 0 ) )
		return ( 1 - A );

	return 0.0;
}

DATATYPE weight_linear2(DATATYPE A)
{
	if (( A < 1 ) && ( A >= 0 ) )
		return (( 1 - A )*( 1 - A ));
	return 0.0;
}

DATATYPE Distance(DATATYPE *A,DATATYPE *B)
{
	DATATYPE a;
	a = Power2((A[0]-B[0])) + Power2((A[1]-B[1]))  + Power2((A[2]-B[2])) ;

	if ( a > 0.0000000001 ) return sqrt(a);
	return 0;
}


DATATYPE Distance2D(DATATYPE *A,DATATYPE *B)
{
	DATATYPE a;
	a = Power2((A[0]-B[0])) + Power2((A[1]-B[1]))  ;

	if ( a > 0.0000000001 ) return sqrt(a);
	return 0;
}

DATATYPE SQUAREROOT(DATATYPE A,DATATYPE B)
{
	DATATYPE a = Power2(A) + Power2(B);
	if (a>0.00000000001) return sqrt(a);
	return 0;
}

void Select_UV(DATATYPE *N,DATATYPE *U,DATATYPE *V)
{
      DATATYPE buf[3];

      buf[0] = -N[1];
      buf[1] =  N[2];
      buf[2] = -N[0];

      CDOT(N,buf,U);
      Normalize(U);
      CDOT(N,U,V);
      Normalize(V);
}

double LCG_FunctionD(unsigned long int *current_value)
{
    *current_value = (16807**current_value + 123)%2147483647;
    return double(*current_value)/2147483646;
}

double Gaussian_unit(double A)
{
   if ( A < U_TABLE[0] )  return -3.1;
   if ( A >= U_TABLE[24] ) return 3.1;
   int i=0; while (!( (A>=U_TABLE[i])&&(A<U_TABLE[i+1]) ) ) i++;
   return ( G_TABLE[i] + (A-U_TABLE[i])*(G_TABLE[i+1]-G_TABLE[i])/(U_TABLE[i+1]-U_TABLE[i]) );
}

class INDEX_LIST
{
   public:
          unsigned int ID;
          DATATYPE *r;
          INDEX_LIST *next;
          INDEX_LIST() { next = NULL; ID = 0; r = NULL; }
          INDEX_LIST(unsigned int i) { ID = i ; next = NULL; r = NULL;  }
          INDEX_LIST(unsigned int i,unsigned int rsize) { ID = i ; next = NULL; r = new DATATYPE[rsize]; }
          ~INDEX_LIST() {   if (r) delete[] r; }
          bool Is_ID_exist(unsigned int QID)
          {
			  if (this == NULL)
				  return false;
             if (QID==ID) return true;
             INDEX_LIST *p;
             p = next;
             while (p) { if (p->ID==QID) return true; GO_NEXT(p);}
             return false; } ;
};

class ATOM_SERIAL
{
public: 
	double boundary1[3],boundary2[3],**original,top_x,top_y,sumv2;
	double **atom_array,**force,**forcedt,**v_array,**vbar,**density,upper[3],lower[3],dt,total_time;
	double debug_msg[20];
	double rCut,rDifferent,rE,gamma,sigma,amplitude_A,amplitude_B,gravityforce;
	double amplitude_A0,amplitude_B0;
	unsigned int size,water_size,total_step;
	unsigned long int seed1,seed2,seed3;
	unsigned int sumparticle;
	INDEX_LIST *Neighbor,*NeighborFix;

	ATOM_SERIAL() 
	{ 
		size = 0;
		water_size = 0;
		total_time = 0;
		sumv2=0;
		sumparticle=0;
		dt = 0.005;
		original=NULL;
		Neighbor = NULL;
		NeighborFix = NULL;
		rCut = 1.0;
		rDifferent = 0.5;
		rE=1;
		amplitude_A = -40;
		amplitude_B =  25;
		amplitude_A0 = -10;
		amplitude_B0 = -20;
		sigma = 7.45;
		gamma = 27.75125;
		gravityforce = 0;
		force = NULL;
		forcedt = NULL;
		vbar = NULL;
		density = NULL;
		total_step = 50000;
		seed1 =     1010;
		seed2 =  1010101;
		seed3 = 90221230;
	}

	~ATOM_SERIAL() {  Release();    }
	void Release();

	void boundaryATO();
	void LoadFromATO(char *filenameW,char *filenameB);
	void LoadFromParameter(char *filename);
	bool Create_Array(unsigned int num,unsigned int total_num);
	void SetNeighbor();
	void UpdateNeighbor();
	void DensityProfile(char *filename);
	void RetrieveSurfaceTension();
	void LSSurfaceTension();
	void LVSurfaceTension();
	void SaveToATO(char *filename);
	void CalculateLocalDensity();
	void CalculateForce();
	void Initialize();
	void VelocityVerletMover();
	void UpdateNearPostion();
	//        void PrtInfo();
};


void do_work1(double ** atom_array, unsigned w_s1,unsigned t_s1, unsigned int *sectA, unsigned int *sectB,
  INDEX_LIST* Neighbor, INDEX_LIST* NeighborFix, DATATYPE TwoSpan, DATATYPE rCut2) {
  int i, j;
  INDEX_LIST* pt;
  DATATYPE XL, XR, YL, YR, ZL, ZR, tmp;

  printf("doing work 1\n");
  for (i=0;i<w_s1;i++)
  {
    XL = atom_array[sectA[i]][0] - TwoSpan;  XR = atom_array[sectA[i]][0] + TwoSpan;
    YL = atom_array[sectA[i]][1] - TwoSpan;  YR = atom_array[sectA[i]][1] + TwoSpan;
    ZL = atom_array[sectA[i]][2] - TwoSpan;  ZR = atom_array[sectA[i]][2] + TwoSpan;

    for (j=i+1;j<w_s1;j++)
    {
      if (( atom_array[sectA[j]][0] >= XR )||( atom_array[sectA[j]][0] <= XL )) continue;
      if (( atom_array[sectA[j]][1] >= YR )||( atom_array[sectA[j]][1] <= YL )) continue;
      if (( atom_array[sectA[j]][2] >= ZR )||( atom_array[sectA[j]][2] <= ZL )) continue;

      pt = new INDEX_LIST(sectA[j],4);

      Vector_Differ(pt->r,atom_array[sectA[j]],atom_array[sectA[i]]);
      tmp = DOT3(pt->r,pt->r);
      if ( tmp < rCut2 ) 
      {
        pt->r[3] = sqrt(tmp);
        pt->next = Neighbor[sectA[i]].next;
        Neighbor[sectA[i]].next = pt;
      }
      else {
        delete pt;
      }
    }

    for (;j<t_s1;j++)
    {

      if (( atom_array[sectA[j]][1] >= YR )||( atom_array[sectA[j]][1] <= YL )) continue;
      if (( atom_array[sectA[j]][2] >= ZR )||( atom_array[sectA[j]][2] <= ZL )) continue;
      if (( atom_array[sectA[j]][0] >= XR )||( atom_array[sectA[j]][0] <= XL )) continue;

      pt = new INDEX_LIST(sectA[j],4);

      Vector_Differ(pt->r,atom_array[sectA[j]],atom_array[sectA[i]]);
      tmp = DOT3(pt->r,pt->r);
      if ( tmp < rCut2 )
      {
        pt->r[3] = sqrt(tmp);
        pt->next = NeighborFix[sectA[i]].next;
        NeighborFix[sectA[i]].next = pt;
      }
      else delete pt;
    }           
  }
}

void do_work2(double ** atom_array, unsigned w_s2,unsigned t_s2, unsigned int *sectA, unsigned int *sectB,
  INDEX_LIST* Neighbor, INDEX_LIST* NeighborFix, DATATYPE TwoSpan, DATATYPE rCut2) {
  int i, j;
  INDEX_LIST* pt;
  DATATYPE XL, XR, YL, YR, ZL, ZR, tmp;
  printf("doing work 2\n");
    for (i=0;i<w_s2;i++)
    {
      XL = atom_array[sectB[i]][0] - TwoSpan;  XR = atom_array[sectB[i]][0] + TwoSpan;
      YL = atom_array[sectB[i]][1] - TwoSpan;  YR = atom_array[sectB[i]][1] + TwoSpan;
      ZL = atom_array[sectB[i]][2] - TwoSpan;  ZR = atom_array[sectB[i]][2] + TwoSpan;
      for (j=i+1;j<w_s2;j++)
      {
        if (( atom_array[sectB[j]][0] >= XR )||( atom_array[sectB[j]][0] <= XL )) continue;
        if (( atom_array[sectB[j]][1] >= YR )||( atom_array[sectB[j]][1] <= YL )) continue;
        if (( atom_array[sectB[j]][2] >= ZR )||( atom_array[sectB[j]][2] <= ZL )) continue;

        pt = new INDEX_LIST(sectB[j],4);

        Vector_Differ(pt->r,atom_array[sectB[j]],atom_array[sectB[i]]);
        tmp = DOT3(pt->r,pt->r);
        if ( tmp < rCut2 ) 
        {
          pt->r[3] = sqrt(tmp);
          pt->next = Neighbor[sectB[i]].next;
          Neighbor[sectB[i]].next = pt;
        }
        else delete pt;
      }

      for (;j<t_s2;j++)
      {
        if (( atom_array[sectB[j]][0] >= XR )||( atom_array[sectB[j]][0] <= XL )) continue;
        if (( atom_array[sectB[j]][1] >= YR )||( atom_array[sectB[j]][1] <= YL )) continue;
        if (( atom_array[sectB[j]][2] >= ZR )||( atom_array[sectB[j]][2] <= ZL )) continue;

        pt = new INDEX_LIST(sectB[j],4);

        Vector_Differ(pt->r,atom_array[sectB[j]],atom_array[sectB[i]]);
        tmp = DOT3(pt->r,pt->r);
        if ( tmp < rCut2 )
        {
          pt->r[3] = sqrt(tmp);
          pt->next = NeighborFix[sectB[i]].next;
          NeighborFix[sectB[i]].next = pt;
        }
        else delete pt;
      }           
    }
}




void do_work3(double ** atom_array, unsigned w_s1, unsigned w_s2, unsigned t_s2, unsigned int *sectA, unsigned int *sectB,
  INDEX_LIST* Neighbor, INDEX_LIST* NeighborFix, DATATYPE TwoSpan, DATATYPE rCut2) {
  int i, j;
  INDEX_LIST* pt;
  DATATYPE XL, XR, YL, YR, ZL, ZR, tmp;

  printf("doing work 3\n");

    for (i=0;i<w_s1;i++)
    {
      XL = atom_array[sectA[i]][0] - TwoSpan;  XR = atom_array[sectA[i]][0] + TwoSpan;
      YL = atom_array[sectA[i]][1] - TwoSpan;  YR = atom_array[sectA[i]][1] + TwoSpan;
      ZL = atom_array[sectA[i]][2] - TwoSpan;  ZR = atom_array[sectA[i]][2] + TwoSpan;
      for (j=0;j<w_s2;j++)
      {
        if (( atom_array[sectB[j]][1] >= YR )||( atom_array[sectB[j]][1] <= YL )) continue;
        if (( atom_array[sectB[j]][2] >= ZR )||( atom_array[sectB[j]][2] <= ZL )) continue;
        if (( atom_array[sectB[j]][0] >= XR )||( atom_array[sectB[j]][0] <= XL )) continue;

        pt = new INDEX_LIST(sectB[j],4);

        Vector_Differ(pt->r,atom_array[sectB[j]],atom_array[sectA[i]]);
        tmp = DOT3(pt->r,pt->r);
        if ( tmp < rCut2 )
        {
          pt->r[3] = sqrt(tmp);
          pt->next = Neighbor[sectA[i]].next;
          Neighbor[sectA[i]].next = pt;
        }
        else delete pt;
      }

      for (;j<t_s2;j++)
      {
        if (( atom_array[sectB[j]][2] >= ZR )||( atom_array[sectB[j]][2] <= ZL )) continue;
        if (( atom_array[sectB[j]][1] >= YR )||( atom_array[sectB[j]][1] <= YL )) continue;
        if (( atom_array[sectB[j]][0] >= XR )||( atom_array[sectB[j]][0] <= XL )) continue;

        pt = new INDEX_LIST(sectB[j],4);

        Vector_Differ(pt->r,atom_array[sectB[j]],atom_array[sectA[i]]);
        tmp = DOT3(pt->r,pt->r);
        if ( tmp < rCut2 )
        {
          pt->r[3] = sqrt(tmp);
          pt->next = NeighborFix[sectA[i]].next;
          NeighborFix[sectA[i]].next = pt;
        }
        else delete pt;
      }
    }

}


void do_work4(double ** atom_array, unsigned w_s1,  unsigned w_s2,unsigned t_s1, unsigned int *sectA, unsigned int *sectB,
  INDEX_LIST* Neighbor, INDEX_LIST* NeighborFix, DATATYPE TwoSpan, DATATYPE rCut2) {
  int i, j;
  INDEX_LIST* pt;
  DATATYPE XL, XR, YL, YR, ZL, ZR, tmp;
  printf("doing work 4\n");
    for (i=0;i<w_s2;i++)
    {
      XL = atom_array[sectB[i]][0] - TwoSpan;  XR = atom_array[sectB[i]][0] + TwoSpan;
      YL = atom_array[sectB[i]][1] - TwoSpan;  YR = atom_array[sectB[i]][1] + TwoSpan;
      ZL = atom_array[sectB[i]][2] - TwoSpan;  ZR = atom_array[sectB[i]][2] + TwoSpan;

      for (j=w_s1;j<t_s1;j++)
      {
        if (( atom_array[sectA[j]][1] >= YR )||( atom_array[sectA[j]][1] <= YL )) continue;
        if (( atom_array[sectA[j]][2] >= ZR )||( atom_array[sectA[j]][2] <= ZL )) continue;
        if (( atom_array[sectA[j]][0] >= XR )||( atom_array[sectA[j]][0] <= XL )) continue;

        pt = new INDEX_LIST(sectA[j],4);

        Vector_Differ(pt->r,atom_array[sectA[j]],atom_array[sectB[i]]);
        tmp = DOT3(pt->r,pt->r);
        if ( tmp < rCut2 )
        {
          pt->r[3] = sqrt(tmp);
          pt->next = NeighborFix[sectB[i]].next;
          NeighborFix[sectB[i]].next = pt;
        }
        else delete pt;
      }
    }
}


/**
 * Regenerate the neighbor list
 */
void ATOM_SERIAL::SetNeighbor()
{
	INDEX_LIST *pt,*pt_head;
	unsigned int i,j,t_s,w_s1,t_s1,w_s2,t_s2,*sectA,*sectB;
	DATATYPE bitsection,XL,XR,YL,YR,ZL,ZR,rCut2,tmp,TwoSpan;
	double cutoff=(rCut+rE)*0.5;

	//	PrtInfo(); // xuewei

	for (i=0;i<water_size;i++)
	{
		original[i][0]=atom_array[i][0];
		original[i][1]=atom_array[i][1];
		original[i][2]=atom_array[i][2];
		pt_head = Neighbor[i].next;
		Neighbor[i].next = NULL;
		while (pt_head)
		{
			pt = pt_head->next;
			delete pt_head;
			pt_head = pt;
		}

		pt_head = NeighborFix[i].next;
		NeighborFix[i].next = NULL;
		while(pt_head)
		{
			pt = pt_head->next;
			delete pt_head;
			pt_head = pt;
		}
	}

	//======
	rCut2 = 4*cutoff*cutoff;
	TwoSpan = 2*cutoff;

	bitsection=0;
	for (i=0;i<size;i++)
		bitsection += atom_array[i][0];
	bitsection /= double(size);

	t_s =0; 
	for (i=0;i<size;i++)
		if (atom_array[i][0]>bitsection) t_s++;

	sectA = new unsigned int[t_s];
	sectB = new unsigned int[size-t_s];
	t_s1=0; t_s2=0;
	for (i=0;i<water_size;i++) 
	{
		if (atom_array[i][0]>bitsection) sectA[t_s1++] = i;
		else sectB[t_s2++] = i;
	}
	w_s1 = t_s1;
	w_s2 = t_s2;
	for (;i<size;i++) 
	{
		if (atom_array[i][0]>bitsection) sectA[t_s1++] = i;
		else sectB[t_s2++] = i;
	}

  #pragma omp parallel sections
  {

  #pragma omp section
  do_work1(atom_array, w_s1, t_s1, sectA, sectB,
    Neighbor, NeighborFix, TwoSpan,rCut2);

  #pragma omp section
  do_work2(atom_array, w_s2, t_s2, sectA, sectB,
    Neighbor, NeighborFix, TwoSpan,rCut2);

  }

	XR = bitsection + 1.01*TwoSpan; 
	XL = bitsection - 1.01*TwoSpan; 

	t_s =0; 
	for (i=0;i<w_s1;i++)
		if (atom_array[sectA[i]][0]<XR) sectA[t_s++] = sectA[i];
	w_s1 = t_s;
	for (;i<t_s1;i++)
		if (atom_array[sectA[i]][0]<XR) sectA[t_s++] = sectA[i];
	t_s1 = t_s;

	t_s =0; 
	for (i=0;i<w_s2;i++)
		if (atom_array[sectB[i]][0]>XL) sectB[t_s++] = sectB[i];
	w_s2 = t_s;
	for (;i<t_s2;i++)
		if (atom_array[sectB[i]][0]>XL) sectB[t_s++] = sectB[i];
	t_s2 = t_s;

  #pragma omp parallel sections
  {
    
#pragma omp section
  do_work3(atom_array, w_s1, w_s2, t_s2, sectA, sectB,
    Neighbor, NeighborFix, TwoSpan,rCut2);

      #pragma omp section
  do_work4(atom_array,w_s1, w_s2, t_s1, sectA, sectB,
    Neighbor, NeighborFix, TwoSpan,rCut2);
  }

	delete[] sectA;	sectA = NULL;
	delete[] sectB;	sectB = NULL;
}

void ATOM_SERIAL::UpdateNeighbor()
{
	INDEX_LIST *pt;
	unsigned int i;

  #pragma omp parallel for
	for (i=0;i<water_size;i++)
	{
		pt = Neighbor[i].next;

		while (pt)
		{
			Vector_Differ(pt->r,atom_array[pt->ID],atom_array[i]);
			pt->r[3] = sqrt(DOT3(pt->r,pt->r));
			GO_NEXT(pt);
		}

		pt = NeighborFix[i].next;
		while(pt)
		{
			Vector_Differ(pt->r,atom_array[pt->ID],atom_array[i]);
			pt->r[3] = sqrt(DOT3(pt->r,pt->r));
			GO_NEXT(pt);
		}           
	}
}


void ATOM_SERIAL::CalculateLocalDensity()
{
	double cell_factorB;
	INDEX_LIST *pt;
	unsigned int i;

	cell_factorB = 2.3873241463784300365332564505877/(rDifferent*rDifferent*rDifferent);  // 15/(2*pi*R^3)

  #pragma omp parallel for
	for(i=0;i<water_size;i++)
	{   density[i][0] = 1.0;  }

  #pragma omp parallel for
	for(i=0;i<water_size;i++)
	{
		pt = Neighbor[i].next;
		while (pt)
		{
			if ( pt->r[3] > rDifferent ) { GO_NEXT(pt); continue; }
			density[i][0] += weight_CD2(pt->r[3]); // for B
			density[pt->ID][0] += weight_CD2(pt->r[3]); //for B
			GO_NEXT(pt);
		}
	}

  #pragma omp parallel for shared(cell_factorB)
	for(i=0;i<water_size;i++)
		density[i][0] *= cell_factorB;

	cell_factorB = 0;
  #pragma omp parallel for reduction(+:cell_factorB)
	for (i=0;i<water_size;i++)	cell_factorB += density[i][0];

	printf("Density=%lf.",cell_factorB/double(water_size));
}

void ATOM_SERIAL::CalculateForce()
{
	unsigned int i,j;
	double tmp2;
	INDEX_LIST *pt;

	for (i=0;i<water_size;i++)
	{   
		force[i][0] = 0 ;
		force[i][1] = 0 ;
		force[i][2] = gravityforce ; 
	}

	tmp2 = sigma;
	sigma /= sqrt(dt);

  #pragma omp parallel for
	for (i=0;i<water_size;i++)
	{
    double tmp, e_ij[3], vd[3];
		pt = Neighbor[i].next;
		while (pt)
		{
			if ( (pt->r[3]) > rCut ) {	GO_NEXT(pt);  continue;	} 

			for (j=0;j<3;j++) e_ij[j] = -(pt->r[j])/pt->r[3] ;

			tmp = amplitude_A* weight_C(pt->r[3])
				+  amplitude_B*( density[i][0] + density[pt->ID][0] )*weight_CD(pt->r[3]) ;

			//Random force      // Gaussian random number
			tmp += sigma*Gaussian_unit(LCG_FunctionD(&seed1))*weight_R(pt->r[3]);

			//Dissipative force
			for (j=0;j<3;j++) vd[j] = v_array[i][j] - v_array[pt->ID][j];
			tmp -= gamma*DOT3(e_ij,vd)*weight_D(pt->r[3]);
			for (j=0;j<3;j++)
			{
				force[i][j] +=  tmp *e_ij[j] ;
				force[pt->ID][j] -=   tmp *e_ij[j] ;     
			}

			GO_NEXT(pt);
		} //END of while

		pt = NeighborFix[i].next;
		while (pt)
		{
			if ( pt->r[3] > rCut ) {GO_NEXT(pt);continue;} 

			for (j=0;j<3;j++) e_ij[j] = -(pt->r[j])/pt->r[3] ;

			tmp = amplitude_A0* weight_C(pt->r[3]) + amplitude_B0 * weight_CD(pt->r[3]) ;
			//													using random factor
			tmp += sigma*Gaussian_unit(LCG_FunctionD(&seed1))*weight_R(pt->r[3]);
			
			tmp -= gamma*DOT3(e_ij,v_array[i])*weight_D(pt->r[3]);

			for (j=0;j<3;j++) force[i][j] += tmp *e_ij[j] ;

			GO_NEXT(pt);
		} //END of while
	}
	sigma = tmp2;
}

void ATOM_SERIAL::Initialize()
{
	SetNeighbor();
	CalculateLocalDensity();
	CalculateForce();
}

void ATOM_SERIAL::VelocityVerletMover()
{
	unsigned int i,j;

	double tmp,**swap_force,**swap_v;

	tmp =dt*dt*0.5;
#pragma omp parallel for
	for (i=0;i<water_size;i++)
		for (j=0;j<3;j++)
			atom_array[i][j] = atom_array[i][j] + ( v_array[i][j]*dt ) + ( force[i][j]*tmp) ;

	tmp = dt*0.5;
#pragma omp parallel for
	for(i=0;i<water_size;i++)
		for (j=0;j<3;j++)
			vbar[i][j] = v_array[i][j] + force[i][j]*tmp;

	swap_v = vbar ; vbar = v_array;  v_array = swap_v;
	swap_force = force; force = forcedt;  forcedt = swap_force;

	UpdateNeighbor();
	CalculateLocalDensity();
	CalculateForce();

	double x=0,y=0,z=0;
	tmp = dt*0.5;

#pragma omp parallel for
	for(i=0;i<water_size;i++)
		for (j=0;j<3;j++)
			v_array[i][j] = vbar[i][j] + (force[i][j]+forcedt[i][j])*tmp;
#pragma omp parallel for
	for(i=0;i<water_size;i++)
	{
		x+=atom_array[i][0];
		y+=atom_array[i][1];
		z+=atom_array[i][2];
	}
	x/=water_size;
	y/=water_size;
	z/=water_size;
	sumv2=0;
	sumparticle=0;
	for(i=0;i<water_size;i++)
	{
		if(atom_array[i][0]<x+1 && atom_array[i][0]>x-1 && atom_array[i][1]<y+1 &&atom_array[i][1]>y-1 && atom_array[i][2]<z+1 && atom_array[i][2]>z-1)
		{
			sumv2+=(Power2(v_array[i][0])+Power2(v_array[i][1])+Power2(v_array[i][2]));
			sumparticle+=1;
		}
	}
	printf("\t%lf\t",sumv2/sumparticle);
	total_time += dt;
	
	double temp=0.5*rE;
	for(i=0;i<water_size;i++)
	{
		double displacement= Distance(original[i],atom_array[i]);
		if(displacement>temp) {
			SetNeighbor();
			//modified for tuning
			printf("Entering into SetNeighbor for %d",i);
			//			break;
		}
	}
}

bool ATOM_SERIAL::Create_Array(unsigned int num,unsigned int total_num)
{
	if (num<=0)  return 0;
	Release();

	unsigned int i;
	size = total_num;
	water_size = num;
	original=new double*[water_size];
	for(i=0;i<water_size;i++)
		original[i]=new double[3];
	atom_array = new double*[size]; for (i=0;i<size;i++) atom_array[i] = new double[3];

	v_array = new double*[water_size]; for (i=0;i<water_size;i++) v_array[i] = new double[3];

	force = new double*[water_size]; for (i=0;i<water_size;i++) force[i] = new double[3];
	forcedt = new double*[water_size]; for (i=0;i<water_size;i++) forcedt[i] = new double[3];
	vbar = new double*[water_size]; for (i=0;i<water_size;i++) vbar[i] =new double[3];
	density = new double*[water_size]; for (i=0;i<water_size;i++) density[i] = new double[1];

	for (i=0;i<water_size;i++) 
		for (int j=0;j<3;j++) 
		{
			v_array[i][j] = 0;
			vbar[i][j] = 0;
			force[i][j] = 0;
			forcedt[i][j] = 0;	
		}

		Neighbor = new INDEX_LIST[water_size];
		NeighborFix = new INDEX_LIST[water_size];

		return 1;
}


void ATOM_SERIAL::boundaryATO()
{
	top_x=atom_array[0][0];
	top_y=atom_array[0][1];
	for(int j=0;j<3;j++)
	{
		boundary1[j]=atom_array[0][j];
		boundary2[j]=atom_array[0][j];
	}
	for(unsigned int i=1;i<water_size;i++)
	{
		if(boundary2[2]<atom_array[i][2])
		{
			boundary2[2]=atom_array[i][2];
			top_x=atom_array[i][0];
			top_y=atom_array[i][1];
		}
		for(int j=0;j<2;j++)
		{
			boundary1[j]=MIN(boundary1[j],atom_array[i][j]);
			boundary2[j]=MAX(boundary2[j],atom_array[i][j]);
		}
		boundary1[2]=MIN(boundary1[2],atom_array[i][2]);
	}
}

void ATOM_SERIAL::DensityProfile(char *filename)
{
	unsigned int i, run=100;
	int j=0,is,js;
	double x,y;
	srand((unsigned int)time(NULL));
	double displace_x,displace_y,h=boundary2[2]+1,den_a[50000]={0.0};
	FILE *stream;
	std::vector<int> clustID;

	DATATYPE qt[3]={0,0,0},den_p=1,dist;
	stream = fopen(filename,"w");

	x=top_x;
	y=top_y;
	for(unsigned int k=0;k<run;k++)
	{
		printf("run %d\n",k);
		displace_x=x+(rand()%101)/100.0-0.5,displace_y=y+(rand()%101)/100.0-0.5;
		clustID.clear();
		qt[2]=0;
		j=0;
		for (i=0;i<water_size;i++)
		{
			if ( ( atom_array[i][0] > 2.0+displace_x ) || ( atom_array[i][0] < -2.0+displace_x ) ) continue;
			if ( ( atom_array[i][1] > 2.0+displace_y ) || ( atom_array[i][1] < -2.0+displace_y ) ) continue;
			clustID.push_back(i);
		}

		while (qt[2] < h)
		{
			den_p = 0;
			for (is=-3;is<4;is++)
			{
				qt[0] = is*0.2+displace_x;
				for (js=-3;js<4;js++)
				{
					qt[1] = js *0.2+displace_y;
					for (i=0;i<clustID.size();i++)
					{
						if ( ( atom_array[clustID[i]][2] - qt[2] > 1 ) || ( atom_array[clustID[i]][2] - qt[2] < -1 ) ) continue;
						dist = Distance(qt,atom_array[clustID[i]]);
						if (dist < 1 )	den_p += weight_linear2(dist);
					}
				}
			}
			den_a[j++]+=den_p;
			qt[2] += 0.01;
		}
	}
	for(qt[2]=0,j=0;qt[2]<h;qt[2]+=0.01,j++)
		fprintf(stream,"%f\t%f\n",qt[2],2.3873241463784300365332564505877*den_a[j]/49/run);
	fclose(stream);
	clustID.clear();
}

void ATOM_SERIAL::RetrieveSurfaceTension()
{
	rE=0;
	SetNeighbor();
	CalculateLocalDensity();
	std::map<double,double> tensionMap;

	double boundary1[3],boundary2[3];
	double x,y;
	x=atom_array[0][0];
	y=atom_array[0][1];
	boundary1[0] = boundary2[0] = atom_array[0][0];
	boundary1[1] = boundary2[1] = atom_array[0][1];
	boundary1[2] = boundary2[2] = atom_array[0][2];
	for(unsigned int i=1;i<water_size;i++)
	{
		x+=atom_array[i][0];
		y+=atom_array[i][1];
		boundary1[0] = MIN(boundary1[0],atom_array[i][0]);
		boundary1[1] = MIN(boundary1[1],atom_array[i][1]);
		boundary1[2] = MIN(boundary1[2],atom_array[i][2]);
		boundary2[0] = MAX(boundary2[0],atom_array[i][0]);
		boundary2[1] = MAX(boundary2[1],atom_array[i][1]);
		boundary2[2] = MAX(boundary2[2],atom_array[i][2]);
	}
	x/=water_size;
	y/=water_size;
	//	boundary1[0]=x-0.75;
	boundary1[1]=y-1;
	//	boundary2[0]=x+0.75;
	boundary2[1]=y+1;
	double area = (boundary2[0]-boundary1[0]) * (boundary2[1]-boundary1[1]);
	std::cout<<"\n"
		<<boundary1[0]<<"\t"<<boundary1[1]<<"\t"<<boundary1[2]<<std::endl
		<<boundary2[0]<<"\t"<<boundary2[1]<<"\t"<<boundary2[2]<<std::endl;

	for(unsigned int i=0;i<water_size;i++)
	{
		double PN=0,PT=0,force;
		if(	atom_array[i][0] < boundary1[0] ||
			atom_array[i][1] < boundary1[1] ||
			atom_array[i][0] > boundary2[0] ||
			atom_array[i][1] > boundary2[1])
			continue;
		for(unsigned int j=0;j<water_size;j++)
		{
			if(i == j)
				continue;
			double	xdis=atom_array[i][0]-atom_array[j][0],
				ydis=atom_array[i][1]-atom_array[j][1],
				zdis=atom_array[i][2]-atom_array[j][2];
			if(xdis>rCut || ydis>rCut || zdis>rCut)
				continue;
			double rij = sqrt(xdis*xdis + ydis*ydis + zdis*zdis);
			if(rij>rCut)
				continue;

			force = amplitude_A* weight_C(rij) +  amplitude_B*( density[i][0] + density[j][0] )*weight_CD(rij) ;
			force *= 0.5;
			PN += force * zdis * zdis / rij;
			PT += 0.5 * force * ( xdis * xdis + ydis * ydis ) / rij;
		}
		INDEX_LIST *pt = NeighborFix[i].next;
		while(pt)
		{
			force = amplitude_A0* weight_C(pt->r[3]) +  amplitude_B0 * weight_CD(pt->r[3]);
			force *= 0.5;
			PN += force * pt->r[2] * pt->r[2] / pt->r[3];
			PT += 0.5 * force * ( pt->r[0] * pt->r[0] + pt->r[1] * pt->r[1] ) / pt->r[3];
			GO_NEXT(pt);
		}

		tensionMap[atom_array[i][2]] += (PN - PT) / area;
	}
	std::ofstream fout("tension.txt");
	std::map<double,double>::iterator Imap;
	for(Imap = tensionMap.begin();Imap!=tensionMap.end();Imap++)
		fout<<Imap->first<<"\t"<<Imap->second<<std::endl;
	fout.close();
}


void ATOM_SERIAL::Release()
{
	if ( size>0)
	{
		unsigned int i;

		for (i=0;i<size;i++) delete[] atom_array[i];
		delete[] atom_array; atom_array = NULL;
		for (i=0;i<water_size;i++) delete[] v_array[i];
		delete[] v_array; v_array = NULL;
		for (i=0;i<water_size;i++) delete[] force[i];
		delete[] force; force = NULL;
		for (i=0;i<water_size;i++) delete[] forcedt[i];
		delete[] forcedt; forcedt = NULL;
		for (i=0;i<water_size;i++) delete[] vbar[i];
		delete[] vbar; vbar = NULL;
		for (i=0;i<water_size;i++) delete[] density[i];
		delete[] density; density = NULL;
		for(i=0;i<water_size;i++) delete[] original[i];
		delete[] original; original=NULL;

		INDEX_LIST *pt,*pt_head;
		for (i=0;i<water_size;i++)
		{
			pt_head = Neighbor[i].next;
			Neighbor[i].next = NULL;
			while(pt_head)
			{
				pt = pt_head->next;
				delete pt_head;
				pt_head = pt;
			}

			pt_head = NeighborFix[i].next;
			NeighborFix[i].next = NULL;
			while(pt_head)
			{
				pt = pt_head->next;
				delete pt_head;
				pt_head = pt;
			}
		}

		delete[] Neighbor; 
		Neighbor = NULL;
		delete[] NeighborFix;
		NeighborFix = NULL;

		size =0; 
	}
}

void ATOM_SERIAL::LoadFromATO(char *filenameW,char *filenameB)
{
	FILE *streamB,*streamW;
	unsigned int i,j;
	unsigned int base_size;
	float tmp;

	streamB = fopen(filenameB,"rb");
	fread(&base_size,4,1,streamB);

	streamW = fopen(filenameW,"rb");
	fread(&water_size,4,1,streamW);

	Create_Array(water_size,water_size+base_size);

	for (i=0;i<base_size;i++)
		for (j=0;j<3;j++)
		{ fread(&tmp,4,1,streamB);
	atom_array[i+water_size][j] = tmp; }

	fclose(streamB);

	for (i=0;i<water_size;i++)
		for (j=0;j<3;j++)
		{ fread(&tmp,4,1,streamW); atom_array[i][j] = tmp; }

		for (i=0;i<water_size;i++)
			for (j=0;j<3;j++)
			{ fread(&tmp,4,1,streamW); v_array[i][j] = tmp; }


			fclose(streamW);
}

void ATOM_SERIAL::LoadFromParameter(char *filename)
{
	FILE *stream = fopen(filename,"r");
	char dummy[128];

	while ( fscanf( stream ,"%s",dummy ) != EOF)
	{
		if (strcmp(dummy,"dt") == 0) {fscanf(stream,"%lf",&dt);continue;}
		if (strcmp(dummy,"rCut") == 0) { fscanf( stream,"%lf",&rCut); printf("rCut\t\t=%lf\n",rCut);continue;}
		if (strcmp(dummy,"rDifferent") == 0) { fscanf( stream,"%lf",&rDifferent); printf("rDifferent\t=%lf\n",rDifferent);continue; }
		if (strcmp(dummy,"rE") == 0) { fscanf( stream,"%lf",&rE); printf("rE\t=%lf\n",rE);continue; }
		if (strcmp(dummy,"amplitude_A") == 0) { fscanf( stream,"%lf",&amplitude_A); printf("amplitude_A\t=%lf\n",amplitude_A);continue; }
		if (strcmp(dummy,"amplitude_B") == 0) { fscanf( stream,"%lf",&amplitude_B); printf("amplitude_B\t=%lf\n",amplitude_B);continue; }
		if (strcmp(dummy,"gamma") == 0) { fscanf( stream,"%lf",&gamma); printf("gamma\t\t=%lf\n",gamma);continue; }
		if (strcmp(dummy,"sigma") == 0) { fscanf( stream,"%lf",&sigma); printf("sigma\t\t=%lf\n",sigma);continue; }
		//if (strcmp(dummy,"gravity_effect") == 0) { fscanf( stream,"%d",&gravity_effect); printf("gravity_effect\t=%d\n",gravity_effect);continue; }
		if (strcmp(dummy,"gravityforce") == 0) { fscanf( stream,"%lf",&gravityforce); printf("gravityforce\t=%lf\n",gravityforce);continue; }
		if (strcmp(dummy,"amplitude_A0") == 0) { fscanf( stream,"%lf",&amplitude_A0); printf("amplitude_A0\t=%lf\n",amplitude_A0);continue; }
		if (strcmp(dummy,"amplitude_B0") == 0) { fscanf( stream,"%lf",&amplitude_B0); printf("amplitude_B0\t=%lf\n",amplitude_B0);continue; }
		if (strcmp(dummy,"total_step") == 0) { fscanf( stream,"%d",&total_step); printf("\ntotal_step\t=%d\n",total_step);continue; }
	} 

	//kT =  0.5*sigma*sigma/gamma;

	fclose(stream);
}


void ATOM_SERIAL::SaveToATO(char *filename)
{
	FILE *stream;
	unsigned int i,j;
	float tmp_f;

	stream = fopen(filename,"wb");

	fwrite(&water_size,4,1,stream);
	for (i=0;i<water_size;i++)
		for (j=0;j<3;j++)
		{
			tmp_f = (float)atom_array[i][j];
			fwrite(&tmp_f,4,1,stream);
		}

		for (i=0;i<water_size;i++)
			for (j=0;j<3;j++)
			{
				tmp_f = (float)v_array[i][j];
				fwrite(&tmp_f,4,1,stream);
			}

			fclose(stream);

}


/*
void ATOM_SERIAL::PrtInfo()
{
	FILE *fp=NULL, *fp2=NULL;
	unsigned int	i;
	static int flag=0;
	char filename[200], filename2[200];
	double boundary1[3],boundary2[3];



	sprintf(filename,"coordinates%d.m",flag);
	sprintf(filename2,"statics.txt");
	
	fp = fopen(filename,"w");
	fp2= fopen(filename2,"a+");
	
	if( NULL == fp ) {
		printf("Can not open file:%s for writing\n",filename);
		exit( 0 );
	}
	if( NULL == fp2 ) {
		printf("Can not open file:%s for writing\n",filename2);
		exit( 0 );
	}

	if( flag == 0 ) {
		fprintf(fp2,"the water size is %d and the base size is %d\n",
				water_size, size );
		fprintf(fp2," rCut=%lf, rDifferent=%lf, Twospan=%lf\n",
				rCut,
				rDifferent,
				(rCut+rE) );
	}

	boundary1[0] = boundary2[0] = atom_array[0][0];
	boundary1[1] = boundary2[1] = atom_array[0][1];
	boundary1[2] = boundary2[2] = atom_array[0][2];
	for(i=1;i<size;i++)
	{
		boundary1[0] = MIN(boundary1[0],atom_array[i][0]);
		boundary1[1] = MIN(boundary1[1],atom_array[i][1]);
		boundary1[2] = MIN(boundary1[2],atom_array[i][2]);
		boundary2[0] = MAX(boundary2[0],atom_array[i][0]);
		boundary2[1] = MAX(boundary2[1],atom_array[i][1]);
		boundary2[2] = MAX(boundary2[2],atom_array[i][2]);
	}
	fprintf(fp2,"flag:%d, X: %lf -- %lf\n", flag, boundary1[0], boundary2[0]); 
	fprintf(fp2,"flag:%d, Y: %lf -- %lf\n", flag, boundary1[1], boundary2[1]); 
	fprintf(fp2,"flag:%d, Z: %lf -- %lf\n", flag, boundary1[2], boundary2[2]); 

	boundary1[0] = boundary2[0] = atom_array[0][0];
	boundary1[1] = boundary2[1] = atom_array[0][1];
	boundary1[2] = boundary2[2] = atom_array[0][2];
	for(i=1;i<water_size;i++)
	{
		boundary1[0] = MIN(boundary1[0],atom_array[i][0]);
		boundary1[1] = MIN(boundary1[1],atom_array[i][1]);
		boundary1[2] = MIN(boundary1[2],atom_array[i][2]);
		boundary2[0] = MAX(boundary2[0],atom_array[i][0]);
		boundary2[1] = MAX(boundary2[1],atom_array[i][1]);
		boundary2[2] = MAX(boundary2[2],atom_array[i][2]);
	}
	fprintf(fp2,"water flag:%d, X: %lf -- %lf\n", flag, boundary1[0], boundary2[0]); 
	fprintf(fp2,"water flag:%d, Y: %lf -- %lf\n", flag, boundary1[1], boundary2[1]); 
	fprintf(fp2,"water flag:%d, Z: %lf -- %lf\n", flag, boundary1[2], boundary2[2]); 

	for(i=0;i<water_size;i++)
		fprintf(fp,"plot3(%lf,%lf,%lf);\n",
					atom_array[i][0],
					atom_array[i][1],
					atom_array[i][2]);

	fclose(fp2);
	fclose(fp);

	flag ++;

	return;
}
*/
