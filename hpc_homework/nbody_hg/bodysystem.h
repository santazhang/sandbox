#ifndef __BODYSYSTEM_H__
#define __BODYSYSTEM_H__

enum NBodyConfig
{
	NBODY_CONFIG_RANDOM,
	NBODY_CONFIG_SHELL,
	NBODY_CONFIG_EXPAND,
	NBODY_NUM_CONFIGS
};

// utility function
void randomizeBodies(NBodyConfig config, float* pos, float* vel, float* color, float clusterScale, 
			 float velocityScale, int numBodies);

// CPU Body System
class BodySystem
{
public:
	BodySystem(int numBodies);
	virtual ~BodySystem();

	virtual void update(float deltaTime);

	virtual void setSoftening(float softening) { m_softeningSquared = softening * softening; }
	virtual void setDamping(float damping)	 { m_damping = damping; }

	enum BodyArray 
	{
		BODYSYSTEM_POSITION,
		BODYSYSTEM_VELOCITY,
	};

	virtual float* getArray(BodyArray array);
	virtual void   setArray(BodyArray array, const float* data);

	virtual unsigned int getCurrentReadBuffer() const { return m_currentRead; }

protected: // methods
	BodySystem() {} // default constructor

	virtual void _initialize(int numBodies);
	virtual void _finalize();

	void _computeNBodyGravitation();
	void _integrateNBodySystem(float deltaTime);
	
protected: // data
	float* m_pos[2];
	float* m_vel[2];
	float* m_force;

	float m_softeningSquared;
	float m_damping;

	unsigned int m_currentRead;
	unsigned int m_currentWrite;

	unsigned int m_timer;

	int m_numBodies;
	bool m_bInitialized;
};

#endif // __BODYSYSTEM_H__
