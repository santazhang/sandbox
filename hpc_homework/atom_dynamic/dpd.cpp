#include"atom.h"

int main()
{
	time_t start_time,end_time;
	ATOM_SERIAL *atom;
	atom = new ATOM_SERIAL;
	
	start_time = clock();
	atom->LoadFromATO("water.ato","base.ato");
	atom->LoadFromParameter("parameter.txt");
	end_time = clock();
	
	printf("  Loading File costs : %f sec.\n",float(end_time-start_time)/1000);
	atom->Initialize();
	start_time = clock();
	printf("  Initialize costs : %f sec.\n",float(start_time-end_time)/1000);

	for (int i=1;i<atom->total_step+1;i++)
	{	
		start_time = clock();
		
		atom->VelocityVerletMover();
		
		if ((i%1000)==0 || ((i%100)==0&&i<1000) || ((i%10)==0&&i<100)) 
		{  
			char buf[64];
			sprintf(buf,"R%.7d.ATO",i);
			atom->SaveToATO(buf);
			printf("  %s is saved.\n",buf);
		}
		
		end_time = clock();
		printf("  time = %f (step:%d) , %d moved cost: %.2f sec \n",atom->total_time,i,atom->water_size,float(end_time-start_time)/1000);
	}
	
	start_time = clock();
	atom->Release();
	delete atom;
	end_time = clock();
	printf("  Release Memory costs : %f sec. \n",float(end_time-start_time)/1000);
	return 0;
}