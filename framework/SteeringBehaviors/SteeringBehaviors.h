#pragma once
#include <Exam_HelperStructs.h>
#include "SteeringHelpers.h"
//Global functions
void SetToWander(SteeringPlugin_Output& steering);

//Class
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) = 0;

	//Seek Functions
	void SetTarget(const Elite::Vector2& target) { m_Target = target; }
	TargetData GetTarget() { return m_Target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{
		return static_cast<T*>(this);
	}

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
};

/////////////////////////
//FLEE
//****
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Seek Behavior
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

private:
	float m_FleeRadius = 10.f;
};

class Arrive :public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	//Seek Behavior
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

};
class Face : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() = default;

	//Seek Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
};

class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Seek Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

	void SetWanderOffset(float offset) { m_OffSetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }

protected:

	float m_OffSetDistance = 6.f; //Offset (agent direction)
	float m_Radius = 4.f; //Wanderradius
	float m_MaxAngleChange = Elite::ToRadians(5); //Max WanderAngle change per frame
	float m_WanderAngle = 0.f; //Internal

};
class Pursuit : public Seek
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	//Seek Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
};
class Evade : public Pursuit
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	//Seek Behaviour

	SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

private:
	float m_FleeRadius = 15.0f;
};



