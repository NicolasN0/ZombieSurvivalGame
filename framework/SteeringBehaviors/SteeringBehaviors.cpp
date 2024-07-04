#include "stdafx.h"
#include "SteeringBehaviors.h"

void SetToWander(SteeringPlugin_Output& steering)
{

}

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent.MaxLinearSpeed;
		

	

	return steering;
}

//FLEE
//****
SteeringPlugin_Output Flee::CalculateSteering(float deltaT, AgentInfo pAgent)
{

	SteeringPlugin_Output steering = {};


	steering.LinearVelocity = -m_Target.Position + pAgent.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent.MaxLinearSpeed;

	//OLD VERSION PROB BETTER BUT DOESNT WORK XDXP (TO LOOK AT TARGET)
	//steering.AutoOrient = false;
	//
	////LOOK AT FLEEING TARGET

	////float rot = atan2(m_Target.Position.x, -m_Target.Position.y);
	////steering.AngularVelocity = rot;
	//////std::cout << "runnnn";
	//Elite::Vector2 direction = m_Target.Position - pAgent.Position;
	//direction.GetNormalized();
	//float currentOrientation = std::fmod(pAgent.Orientation + (float)M_PI, 2 * (float)M_PI);
	//if (currentOrientation < 0.f) 
	//{
	//	currentOrientation = 2 * (float)M_PI + currentOrientation;
	//}
	//float targetOrientation = Elite::GetOrientationFromVelocity(direction) + (float)M_PI;
	//float difference = currentOrientation - targetOrientation;

	//if (difference < 0.1f && difference > 0.1f) 
	//{
	//	steering.AngularVelocity = 0.f;
	//}
	//else if ((difference < 0 || difference >(float)M_PI) && difference > (float)M_PI) 
	//{
	//	steering.AngularVelocity = pAgent.MaxAngularSpeed;
	//}
	//else
	//{
	//	steering.AngularVelocity = -pAgent.MaxAngularSpeed;
	//}

	steering.AutoOrient = false;

	//LOOK AT CURRENT TARGET
	// FROM FACE FUNCTION
	//AgentInfo agent{};
	//m_pBlackboard->GetData("Agent", agent);
	//Elite::Vector2 target{};
	//if (m_IsLookingForItem)
	//{
	//	EntityInfo item;
	//	m_pBlackboard->GetData("ClosestItem", item);
	//	target = item.Location;
	//}
	//else
	//{

	//	m_pBlackboard->GetData("TargetHouse", target);
	//}

	Elite::Vector2 direction = m_Target.Position - pAgent.Position;
	direction.GetNormalized();
	//m_Direction = direction;
	float targetOrientation = Elite::GetOrientationFromVelocity(direction);
	float MidPointRight = (float)M_PI / 2;
	float MidPointLeft = -MidPointRight;
	float difference = pAgent.Orientation - targetOrientation;
	if (difference < 0.2f && difference > 0.2f)
	{
		steering.AngularVelocity = 0.f;
	}
	else if (targetOrientation > 0) //TARGET IS RIGHT SIDE SO ABOVE 0
	{
		if (pAgent.Orientation > 0)
		{
			if (pAgent.Orientation > targetOrientation)
			{
				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
			else
			{
				steering.AngularVelocity = pAgent.MaxAngularSpeed;
			}
		}//AGENT IS BELOW ZERO AND TARGET ABOVE
		else if (pAgent.Orientation < MidPointLeft) //FIRST AGENT IS LEFT TOP LOOKING ALREADY
		{
			if (targetOrientation < MidPointRight)
			{
				//OPPOSITE SIDES DOENST REALLY MATTER
				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
			else //AGENT TOP LEFT TARGET TOP RIGHT SO COUNTER CLOCKWISE SO -
			{
				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
		}
		else  //AGENT IS LEFT BOTTOM LOOKING
		{
			if (targetOrientation < MidPointRight)
			{
				steering.AngularVelocity = pAgent.MaxAngularSpeed;
			}
			else
			{
				//OPPOSITE SIDES DOENST REALLY MATTER
				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
		}

	}
	else if (targetOrientation < 0) //TARGET IS LEFT SIDE SO BELOW 0
	{
		if (pAgent.Orientation < 0)
		{
			if (pAgent.Orientation > targetOrientation) //AGENT IS LOOKING BELOW SO ROTATE CLOCKWISE SO -
			{
				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
			else //AGENT IS LOOKING ABOVE SO COUNTERCLOCKWISE SO +
			{
				steering.AngularVelocity = pAgent.MaxAngularSpeed;
			}
		}//AGENT IS ABOVE ZERO AND TARGET ABOVE
		else if (pAgent.Orientation < MidPointRight) //FIRST AGENT IS RIGHT TOP LOOKING ALREADY
		{
			if (targetOrientation < MidPointLeft) //TARGET TOP LEFT
			{

				steering.AngularVelocity = pAgent.MaxAngularSpeed;
			}
			else // OPPOSITE SIDES DOENST REALLY MATTER
			{

				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
		}
		else  //AGENT IS RIGHT BOTTOM LOOKING
		{
			if (targetOrientation < MidPointLeft) //OPPOSITE SIDES DOENST REALLY MATTER
			{
				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
			else
			{

				steering.AngularVelocity = -pAgent.MaxAngularSpeed;
			}
		}
	}


	return steering;
}

SteeringPlugin_Output Arrive::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering = {};



	const float maxSpeed = pAgent.MaxLinearSpeed;
	const float arrivalRadius = 1.f;
	const float slowRadius = 15.f;


	Elite::Vector2 toTarget = m_Target.Position - pAgent.Position;
	const float distance = toTarget.Magnitude();

	if (distance < arrivalRadius)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
		return steering;
	}



	Elite::Vector2 velocity = toTarget;
	velocity.Normalize();

	if (distance < slowRadius)
	{
		velocity *= maxSpeed * distance / slowRadius;
	}
	else
	{
		velocity *= maxSpeed;
	}


	steering.LinearVelocity = velocity;





	return steering;
}


SteeringPlugin_Output Face::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	//works first time, then becomes buggy after using other behaviour

	SteeringPlugin_Output steering = {};

	steering.LinearVelocity = Elite::ZeroVector2;
	steering.AutoOrient = false;


	float rot = atan2(m_Target.Position.x, -m_Target.Position.y);
	steering.AngularVelocity = rot;


	return steering;
}
SteeringPlugin_Output Wander::CalculateSteering(float deltaT, AgentInfo pAgent)
{

	SteeringPlugin_Output steering = {};

	Elite::Vector2 target{};


	const float angleChange{ m_MaxAngleChange * (Elite::randomFloat(2.f) - 1.f) };
	//float wanderAngle = 0;
	m_WanderAngle += angleChange;
	//wanderAngle += angleChange;
	//pAgent.Orientation = wanderAngle;

	const Elite::Vector2 centreToTarget{ m_Radius * cos(m_WanderAngle), m_Radius * sin(m_WanderAngle) };
	steering.LinearVelocity = centreToTarget;
	steering.LinearVelocity *= pAgent.MaxLinearSpeed;

	steering.AutoOrient = false;
	float angSpeed = 1.f;
	steering.AngularVelocity = angSpeed;

	return steering;



}

//PURSUIT
//*******
//SteeringPlugin_Output Pursuit::CalculateSteering(float deltaT, AgentInfo pAgent)
//{
//
//	if (pAgent.MaxLinearSpeed == 0.f)
//	{
//		return SteeringPlugin_Output(Elite::ZeroVector2, 0.f, false);
//	}
//
//	//... logic
//	Elite::Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
//	float distanceToTarget = toTarget.Magnitude();
//	float speedRatio = m_Target.LinearVelocity.Magnitude() / pAgent->GetMaxLinearSpeed();
//	float distancePredictedPos = distanceToTarget * speedRatio;
//	if (distancePredictedPos >= distanceToTarget) distancePredictedPos = distanceToTarget - 1;
//
//	m_Target.Position += m_Target.LinearVelocity.GetNormalized() * distancePredictedPos;
//	SteeringPlugin_Output steering = { Seek::CalculateSteering(deltaT, pAgent) };
//
//	if (pAgent->CanRenderBehavior())
//	{
//		DEBUGRENDERER2D->DrawCircle(m_Target.Position, 0.3f, Elite::Color{ 1, 0, 1, 1 }, 0);
//	}
//
//	return steering;
//}

//EVADE
//*****
//SteeringPlugin_Output Evade::CalculateSteering(float deltaT, AgentInfo pAgent)
//{
//	float distanceToTarget = Distance(pAgent.Position, m_Target.Position);
//	SteeringPlugin_Output steering = { Pursuit::CalculateSteering(deltaT, pAgent) };
//	if (distanceToTarget > m_FleeRadius)
//	{
//		steering.LinearVelocity = Elite::ZeroVector2;
//		return steering;
//	}
//
//	
//
//	//... logic
//	steering.LinearVelocity *= -1;
//
//	return steering;
//}
