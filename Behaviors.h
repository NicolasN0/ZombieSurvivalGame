#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/BT/EBehaviorTree.h"
//#include "framework/SteeringBehaviors/SteeringBehaviors.h"
#include "Plugin.h"
#include <algorithm>
#include <iterator>
//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;

	std::string steeringString{ "Wander" };
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToSeekHouse(Elite::Blackboard* pBlackboard)
{
	std::cout << "SeekingHouse" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Elite::Vector2 target{};
	pBlackboard->GetData("TargetHouse", target);

	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;

	Plugin* pPlugin{ nullptr };
	pBlackboard->GetData("Plugin", pPlugin);
	Elite::Vector2 closest = pPlugin->GetInterface()->NavMesh_GetClosestPathPoint(target);


	std::string steeringString{ "Seek" };
	std::map<std::string, ISteeringBehavior*> map;
	pBlackboard->GetData("Map", map);
	map[steeringString]->SetTarget(closest);
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToSeekItem(Elite::Blackboard* pBlackboard)
{
	std::cout << "SeekingItem" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	EntityInfo item{};
	pBlackboard->GetData("ClosestItem", item);
	Elite::Vector2 itemPos{ item.Location };

	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;

	Plugin* pPlugin{ nullptr };
	pBlackboard->GetData("Plugin", pPlugin);
	Elite::Vector2 closest = pPlugin->GetInterface()->NavMesh_GetClosestPathPoint(itemPos);


	std::string steeringString{ "Seek" };
	std::map<std::string, ISteeringBehavior*> map;
	pBlackboard->GetData("Map", map);
	map[steeringString]->SetTarget(closest);
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
{
	std::cout << "Fleeing" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Elite::Vector2 target{};
	pBlackboard->GetData("AgentFleeTarget", target);

	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;


	std::string steeringString{ "Flee" };
	std::map<std::string, ISteeringBehavior*> map;
	pBlackboard->GetData("Map", map);
	map[steeringString]->SetTarget(target);
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToSeekRandom(Elite::Blackboard* pBlackboard)
{
	std::cout << "SeekingRandom" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Elite::Vector2 target{};
	pBlackboard->GetData("RandomTarget", target);

	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;

	Plugin* pPlugin{ nullptr };
	pBlackboard->GetData("Plugin", pPlugin);
	//SET OTHER IS LOOKING FALSE TO BE SURE
	//pPlugin->SetIsLookingForHouse(false);
	//pPlugin->SetIsLookingForItem(false);

	Elite::Vector2 closest = pPlugin->GetInterface()->NavMesh_GetClosestPathPoint(target);
	pPlugin->SetIsLookingRandom(true);

	std::string steeringString{ "Seek" };
	std::map<std::string, ISteeringBehavior*> map;
	pBlackboard->GetData("Map", map);
	map[steeringString]->SetTarget(closest);
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ChangeToSeekRandomPurgeFree(Elite::Blackboard* pBlackboard)
{
	std::cout << "SeekingRandomPurgeFree" << std::endl;
	//TODO IN CONDITIONAL PURGE ZONE SET RANDOMTARGETFROMPURGE TO ZERO IF HE IS OUT OF PURGE AND ONLY MAKE NEW ONE IF HE IS IN AND EXISTING ONE IS ZERO
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Elite::Vector2 target{};
	pBlackboard->GetData("RandomTargetPurgeFree", target);

	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;

	Plugin* pPlugin{ nullptr };
	pBlackboard->GetData("Plugin", pPlugin);
	//SET OTHER IS LOOKING FALSE TO BE SURE
	pPlugin->SetIsLookingForHouse(false);
	pPlugin->SetIsLookingForItem(false);

	Elite::Vector2 closest = pPlugin->GetInterface()->NavMesh_GetClosestPathPoint(target);
	pPlugin->SetIsLookingRandom(true);

	std::string steeringString{ "Seek" };
	std::map<std::string, ISteeringBehavior*> map;
	pBlackboard->GetData("Map", map);
	map[steeringString]->SetTarget(closest);
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState ShootGun(Elite::Blackboard* pBlackboard)
{
	std::cout << "Shooting" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	std::vector<bool> inventory = pPlugin->GetInventoryFilled();
	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;


	for (size_t i{}; i < pPlugin->GetInterface()->Inventory_GetCapacity(); i++) 
	{

		if (inventory.at(i))
		{
			ItemInfo item{};
			if (pPlugin->GetInterface()->Inventory_GetItem(i, item)) 
			{
				if (item.Type == eItemType::PISTOL) 
				{
					pPlugin->GetInterface()->Inventory_UseItem(i);
				}

			}

		}
	}
	
	
	return Elite::BehaviorState::Success;
}

Elite::BehaviorState EatFood(Elite::Blackboard* pBlackboard)
{
	std::cout << "Eating" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	std::vector<bool> inventory = pPlugin->GetInventoryFilled();
	
	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;

	for (size_t i{}; i < pPlugin->GetInterface()->Inventory_GetCapacity(); i++)
	{
		if (inventory.at(i)) 
		{
			ItemInfo item{};
			if (pPlugin->GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::FOOD)
				{
					pPlugin->GetInterface()->Inventory_UseItem(i);
				}
			}
		}
	}


	return Elite::BehaviorState::Success;
}

Elite::BehaviorState UseMedkit(Elite::Blackboard* pBlackboard)
{
	std::cout << "MedKitting" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	std::vector<bool> inventory = pPlugin->GetInventoryFilled();
	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;
	for (size_t i{}; i < pPlugin->GetInterface()->Inventory_GetCapacity(); i++)
	{
		if (inventory.at(i))
		{
			ItemInfo item{};
			if (pPlugin->GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::MEDKIT)
				{
					pPlugin->GetInterface()->Inventory_UseItem(i);
				}
			}
		}
	}


	return Elite::BehaviorState::Success;
}

Elite::BehaviorState FleeFromPurgeZone(Elite::Blackboard* pBlackboard)
{
	std::cout << "FleeingFromPurge" << std::endl;
	AgentInfo pAgent{};
	pBlackboard->GetData("Agent", pAgent);
	PurgeZoneInfo target{};
	pBlackboard->GetData("PurgeFleeTarget", target);

	if (pAgent.Death == true)
		return Elite::BehaviorState::Failure;


	std::string steeringString{ "Flee" };
	std::map<std::string, ISteeringBehavior*> map;
	pBlackboard->GetData("Map", map);
	map[steeringString]->SetTarget(target.Center);
	pBlackboard->ChangeData("SteeringString", steeringString);
	return Elite::BehaviorState::Success;

}
//// CONDITIONS

bool IsEnemyClose(Elite::Blackboard* pBlackboard)
{
	
	AgentInfo pAgent;
	pBlackboard->GetData("Agent", pAgent);
	std::vector<EnemyInfo> entities{};

	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	entities = pPlugin->GetEnemiesInFov();

	if (entities.size() < 1)
	{
		return false;
	}
	std::cout << "Enemies" << std::endl;
	EnemyInfo closestEntity = entities[0];
	float closestDistance{ Elite::DistanceSquared(pAgent.Position, closestEntity.Location) };
	for (size_t i = 1; i < entities.size(); i++)	// loop over al food items starting from the second
	{
		float distanceSquared{ Elite::DistanceSquared(pAgent.Position, entities[i].Location) };
		if (distanceSquared < closestDistance)	// if smaller change value to new food
		{
			closestEntity = entities[i];
			closestDistance = Elite::DistanceSquared(pAgent.Position, closestEntity.Location);
		}
	}



	if (pBlackboard->ChangeData("AgentFleeTarget", closestEntity.Location)) 
	{
		pBlackboard->ChangeData("ClosestEnemy", closestEntity);
		return true;
	}
	
	
	return false;
}

bool AreMultipleEnemiesClose(Elite::Blackboard* pBlackboard)
{

	AgentInfo pAgent;
	pBlackboard->GetData("Agent", pAgent);

	std::vector<EnemyInfo> entities{};

	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	entities = pPlugin->GetEnemiesInFov();
	if (entities.size() < 1)
	{
		return false;
	}
	

	if (pPlugin->GetEnemiesInFov().size() >= 3) 
	{
		pBlackboard->ChangeData("RandomTarget", pPlugin->GetNewRandomPoint());
		return true;
	}
	return false;
}

bool IsHouseClose(Elite::Blackboard* pBlackboard)
{
	
	AgentInfo pAgent;
	pBlackboard->GetData("Agent", pAgent);

	std::vector<HouseInfo> houses{};
	pBlackboard->GetData("HousesVec", houses);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	pPlugin->SetIsLookingForItem(false);
	//DELETE LAST THREE VISITED HOUSE FROM THE LIST
	std::vector<int> indexesToRemove;
	int size = houses.size();
	for (int i{}; i < size; i++)
	{
	
		for (size_t j{}; j < pPlugin->GetLastThreeVisited().size(); j++) 
		{
			if (houses.at(i).Center == pPlugin->GetLastThreeVisited().at(j).Center) 
			{
				houses.erase(houses.begin() + i); //DELETING IN LOOP IS RANGE ERROS SO CHANGE SIZE AND BREAK
				size--;
				break;

			}
		}
	}

	if (houses.size() < 1)
	{
		return false;
	}
	
	HouseInfo closestHouse = houses[0];
	float closestDistance{ Elite::DistanceSquared(pAgent.Position, closestHouse.Center) };
	for (size_t i = 1; i < houses.size(); i++)	// loop over al food items starting from the second
	{
		float distanceSquared{ Elite::DistanceSquared(pAgent.Position, houses[i].Center) };
		if (distanceSquared < closestDistance)	// if smaller change value to new food
		{
			closestHouse = houses[i];
			closestDistance = Elite::DistanceSquared(pAgent.Position, closestHouse.Center);
		}
	}

	if(pBlackboard->ChangeData("TargetHouse", closestHouse.Center))
	{
		pPlugin->SetIsLookingForHouse(true);
		pPlugin->SetInsideHouse(closestHouse);
		return true;
	}
	return false;
}

bool IsItemClose(Elite::Blackboard* pBlackboard)
{

	AgentInfo pAgent;
	pBlackboard->GetData("Agent", pAgent);


	std::vector<EntityInfo> items{};
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	items = pPlugin->GetItemsInFov();
	pPlugin->SetIsLookingForItem(false);

	if (items.size() < 1)
	{
		return false;
	}

	if (items.size() < 1)
	{
		return false;
	}


	EntityInfo closestEntity = items[0];
	float closestDistance{ Elite::DistanceSquared(pAgent.Position, closestEntity.Location) };
	for (size_t i = 1; i < items.size(); i++)	// loop over al food items starting from the second
	{
		float distanceSquared{ Elite::DistanceSquared(pAgent.Position, items[i].Location) };
		if (distanceSquared < closestDistance)	// if smaller change value to new food
		{
			closestEntity = items[i];
			closestDistance = Elite::DistanceSquared(pAgent.Position, closestEntity.Location);
		}
	}

	//IF THE CLOSEST IS A TYPE YOU ALREADY HAVE TWO OFF USE THEM SO YOU HAVE SPACE
	ItemInfo itemInf{};
	pPlugin->GetInterface()->Item_GetInfo(closestEntity, itemInf);
	if (itemInf.Type == eItemType::FOOD) 
	{
		if (pPlugin->GetFoodCount() > 1) 
		{
			std::vector<bool> inventory = pPlugin->GetInventoryFilled();

			for (size_t i{}; i < pPlugin->GetInterface()->Inventory_GetCapacity(); i++)
			{
				if (inventory.at(i))
				{
					ItemInfo item{};
					if (pPlugin->GetInterface()->Inventory_GetItem(i, item))
					{
						if (item.Type == eItemType::FOOD)
						{
							pPlugin->GetInterface()->Inventory_UseItem(i);
						}
					}
				}
			}
		}
	}
	//SAME FOR MEDKIT
	if (itemInf.Type == eItemType::MEDKIT)
	{
		if (pPlugin->GetMedkitCount() > 1)
		{
			std::vector<bool> inventory = pPlugin->GetInventoryFilled();

			for (size_t i{}; i < pPlugin->GetInterface()->Inventory_GetCapacity(); i++)
			{
				if (inventory.at(i))
				{
					ItemInfo item{};
					if (pPlugin->GetInterface()->Inventory_GetItem(i, item))
					{
						if (item.Type == eItemType::MEDKIT)
						{
							pPlugin->GetInterface()->Inventory_UseItem(i);
						}
					}
				}
			}
		}
	}

	if (pBlackboard->ChangeData("ClosestItem", closestEntity))
	{
		pPlugin->SetIsLookingForItem(true);
		return true;
	}

	return false;
}

bool IsInCenterHouse(Elite::Blackboard* pBlackboard)
{
	AgentInfo pAgent;
	pBlackboard->GetData("Agent", pAgent);

	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	HouseInfo house = pPlugin->GetInsideHouse();
	float distanceSquared{ house.Center.DistanceSquared(house.Center + house.Size/16) };

	//TEST GO STRAIGHT TO RANDOM TARGET

	if ((pAgent.Position.DistanceSquared(house.Center) < distanceSquared) && (house.Center != Elite::ZeroVector2))
	{
		Elite::Vector2 newRandom{ pPlugin->GetNewRandomPoint() };
		
		//CHECK WITH CENTER CAUSE NO WAY YET TO COMPARE HOUSEINFO?
		std::deque<HouseInfo> lastThree = pPlugin->GetLastThreeVisited();
		std::vector<Elite::Vector2> lastThreeCenters{};
		for (size_t i{}; i < lastThree.size(); i++) 
		{
			lastThreeCenters.push_back(lastThree.at(i).Center);
		}

		if(std::find(lastThreeCenters.begin(), lastThreeCenters.end(),house.Center) == lastThreeCenters.end())
		{
			pPlugin->AddHouseLastVisited(pPlugin->GetInsideHouse());
		}
		pBlackboard->ChangeData("RandomTarget", newRandom);
		return true;
	}
	return false;
}

bool IsInShootLine(Elite::Blackboard* pBlackboard)
{
	
	EnemyInfo enemy;
	pBlackboard->GetData("ClosestEnemy", enemy);
	AgentInfo agent;
	pBlackboard->GetData("Agent", agent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	if (!pPlugin->HasGun()) 
	{
		return false;
	}

	//ONLY SHOOT FAST OR BIG GUYS OR MORE THAN 5 BULLETS
	if (enemy.Type == eEnemyType::ZOMBIE_NORMAL) 
	{
		if (pPlugin->GetTotalAmmo() < 5) 
		{
			return false;
		}
	}

	Elite::Vector2 lineP1 = agent.Position;
	Elite::Vector2 lineP2 = (agent.Position + (Elite::OrientationToVector(agent.Orientation) * agent.FOV_Range));

	Elite::Vector2 n = Elite::ZeroVector2;
	float t = 0;
	Elite::Vector2 ap = enemy.Location - lineP1;
	Elite::Vector2 ab = lineP2 - lineP1;

	float dab = Elite::Dot(ab, ab);
	if (dab == 0.0f) return false;

	t = Elite::Clamp(Dot(ap, ab) / dab, 0.0f, 1.0f);
	n = enemy.Location - (lineP1 + ab * t);
	t = n.Magnitude();

	if (t > (enemy.Size * enemy.Size))
		return false;

	n = n.GetNormalized();
	t = enemy.Size - sqrtf(t);
	return true;
}

bool IsHealthLow(Elite::Blackboard* pBlackboard)
{
	//ONLY SAY TRUE WHEN YOU HAVE MEDKIT
	AgentInfo agent;
	pBlackboard->GetData("Agent", agent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	if (!pPlugin->HasMedcine()) 
	{
		return false;
	}

	if (agent.Health < 5.f) 
	{

		return true;
	}
	return false;
}

bool IsEnergyLow(Elite::Blackboard* pBlackboard)
{
	//ONLY SAY TRUE WHEN YOU HAVE FOOD
	AgentInfo agent;
	pBlackboard->GetData("Agent", agent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	if (!pPlugin->HasFood())
	{
		return false;
	}

	if (agent.Energy < 5.f)
	{

		return true;
	}
	return false;
}

bool IsInPurgeZone(Elite::Blackboard* pBlackboard)
{
	AgentInfo agent;
	pBlackboard->GetData("Agent", agent);
	Plugin* pPlugin{};
	pBlackboard->GetData("Plugin", pPlugin);
	std::vector<EntityInfo> zones = pPlugin->GetPurgeZones();
	if (zones.size() < 1) 
	{
		return false;
	}
	
	std::vector<PurgeZoneInfo> infos(zones.size());

	for(size_t i{}; i<zones.size();i++)
	{
		pPlugin->GetInterface()->PurgeZone_GetInfo(zones.at(i), infos.at(i));
	}
	

	for (size_t i{}; i < infos.size(); i++)
	{
		if (agent.Position.Distance(infos.at(i).Center) < infos.at(i).Radius)
		{
			pBlackboard->ChangeData("PurgeFleeTarget", infos.at(i));
			pBlackboard->ChangeData("RandomTarget", pPlugin->GetNewRandomPoint());
			return true;
		}
	}


	return false;
}
#endif