#ifndef __BODYPARAM_H__
#define __BODYPARAM_H__

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
#endif