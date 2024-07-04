#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "Behaviors.h"
#include "framework/Data/EBlackboard.h"
//#include "framework/BT/EBehaviorTree.h"
//#include "framework/Data/EBlackboard.h"

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "NicolasNeveTEST";
	info.Student_FirstName = "Nicolas";
	info.Student_LastName = "Neve";
	info.Student_Class = "2DAE07";


	
	auto agentInfo = m_pInterface->Agent_GetInfo();
	//m_RandomPoint = GetNewRandomPoint();
	m_pBlackboard = CreateBlackboard(agentInfo);
	m_pBehaviourTree = new Elite::BehaviorTree(m_pBlackboard,
		new Elite::BehaviorSelector
		({
			new Elite::BehaviorSequence(
			{
				new Elite::BehaviorConditional(IsInPurgeZone),
				new Elite::BehaviorAction(FleeFromPurgeZone)
			}),
			new Elite::BehaviorSequence(
			{
				new Elite::BehaviorConditional(IsHealthLow),
				new Elite::BehaviorAction(UseMedkit)
			}),
			new Elite::BehaviorSequence(
			{
				new Elite::BehaviorConditional(IsEnergyLow),
				new Elite::BehaviorAction(EatFood)
			}),
			new Elite::BehaviorSequence(
			{
					new Elite::BehaviorConditional(IsEnemyClose),
					new Elite::BehaviorAction(ChangeToFlee),
					new Elite::BehaviorConditional(IsInShootLine),
					new Elite::BehaviorAction(ShootGun)
			}
			),
			new Elite::BehaviorSequence(
			{
				new Elite::BehaviorConditional(IsItemClose),
				new Elite::BehaviorAction(ChangeToSeekItem)
			}),
			//TEST GO STRAIGHT TO SEEK RANDOM
				new Elite::BehaviorSequence(
			{
				new Elite::BehaviorConditional(IsInCenterHouse),
				new Elite::BehaviorAction(ChangeToSeekRandom)
			}),
			new Elite::BehaviorSequence(
			{
				new Elite::BehaviorConditional(IsHouseClose),
				new Elite::BehaviorAction(ChangeToSeekHouse),
			}),
			new Elite::BehaviorAction(ChangeToSeekRandom)
			})
	);
	
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called when the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//RESET VARIABLEs
	m_IsLookingRandom = false;

	auto steering = SteeringPlugin_Output();
	
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();
	
	auto nextTargetPos = m_Target; //To start you can use the mouse position as guidance

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	for (auto& e : vEntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
			std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y <<  " ---EntityHash: " << e.EntityHash << "---Radius: "<< zoneInfo.Radius << std::endl;
		}
	}
	
	
	//INVENTORY USAGE DEMO
	//********************

	//if (m_GrabItem)
	//{
	//	ItemInfo item;
	//	//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
	//	//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
	//	//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
	//	//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
	//	if (m_pInterface->Item_Grab({}, item))
	//	{
	//		//Once grabbed, you can add it to a specific inventory slot
	//		//Slot must be empty
	//		m_pInterface->Inventory_AddItem(0, item);
	//	}
	//}

	if (m_UseItem)
	{
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterface->Inventory_UseItem(0);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(0);
	}


	//RESET AND UPDATE DATA
	m_IsLookingForHouse = false;
	m_IsLookingForItem = false;
	UpdateBlackboard();
	CleanupInventory();
	if(Elite::DistanceSquared(agentInfo.Position,m_RandomPoint) < sqrtf(250))
	{
		m_RandomPoint = GetNewRandomPoint();
	}
	m_pBehaviourTree->Update(dt);

	//SET STEERING FROM BLACKBOARD
	std::string steeringString{};
	m_pBlackboard->GetData("SteeringString", steeringString);
	steering = m_SteeringBehaviorsMap[steeringString]->CalculateSteering(dt, agentInfo);


	//CONTROL RUNNING
	if(steeringString == "Flee")
	{
		m_CanRun = true;
	} 
	else 
	{
		if (agentInfo.WasBitten)
		{
			m_CanRun = true;
		}

	}
	
	if (agentInfo.Stamina < 1) 
	{
		m_CanRun = false;
	}
	steering.RunMode = m_CanRun;


	//ADD ITEMS
	if(steeringString == "Seek" && m_IsLookingForItem == true)
	{

		AddItem();
	}


	//LOOK AROUND WHEN GOING TO RANDOM POS
	if (steeringString == "Seek" && !m_IsLookingForItem && !m_IsLookingForHouse)
	{

			steering.AutoOrient = false;
			steering.AngularVelocity = agentInfo.MaxAngularSpeed;

	}


	//FACE CURRENT TARGET 
	if (m_IsLookingForItem == true || m_IsLookingForHouse == true) 
	{
		//if (GetTotalAmmo() < 10) 
		//{

			FaceCurrenTarget(steering);
		//}
	}


	//LET AGENT ROTATE IF ATTACKED BY ZOMBIES EVEN WHEN GETTING TO ITEMS
	if (agentInfo.WasBitten && HasGun()) 
	{
		m_isRotatingBitten = true;
	}

	if (m_isRotatingBitten == true) 
	{
		steering.AutoOrient = false;
		steering.AngularVelocity = agentInfo.MaxAngularSpeed;
		m_BittenRotateTimer += dt;
		if (m_BittenRotateTimer > m_RotateTimerMax) 
		{
			m_BittenRotateTimer = 0;
			m_isRotatingBitten = false;
		}
	}


	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	//m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });

	//TARGET RANDOM
	Elite::Vector2 r;
	m_pBlackboard->GetData("RandomTarget", r);
	m_pInterface->Draw_SolidCircle(r, 1.f, { 0,0 }, { 0, 0, 1 });

	//TARGET HOUSE
	Elite::Vector2 h;
	m_pBlackboard->GetData("TargetHouse", h);
	m_pInterface->Draw_Circle(h, 5.f, { 1, 1, 1 });

	//TARGET ITEM
	EntityInfo item;
	m_pBlackboard->GetData("ClosestItem", item);
	m_pInterface->Draw_Circle(item.Location, 1.5f, { 0, 1, 1 });

	//INSIDE HOUSe
	Elite::Vector2 inside = GetInsideHouse().Center;
	m_pInterface->Draw_Circle(inside, 0.7f, { 0.6f,0.6f,0.6f });

	//LAST VISITED
	for (size_t i{}; i < m_LastThreeVisited.size(); i++) 
	{
		Elite::Vector2 last = m_LastThreeVisited.at(i).Center;
		m_pInterface->Draw_Circle(last, 1.5f, { 0.5f,0.5f,0 });
	}
	//Elite::Vector2 last = GetLastVisited().Center;

	//DRAW DIRECTION
	AgentInfo a;
	m_pBlackboard->GetData("Agent", a);
	m_pInterface->Draw_Direction(a.Position, m_Direction, 50, { 1,1,1 });

}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}

vector<EnemyInfo> Plugin::GetEnemiesInFov() const
{
	vector<EnemyInfo> vEnemiesInFov = {};
	vector<EntityInfo> ei{};
	ei = GetEntitiesInFOV();
	for (size_t i{}; i < ei.size(); i++)
	{
		if(ei.at(i).Type == eEntityType::ENEMY)
		{
			EnemyInfo e;
			m_pInterface->Enemy_GetInfo(ei.at(i), e);
			vEnemiesInFov.push_back(e);
		}
	}
	return vEnemiesInFov;
}

vector<EntityInfo> Plugin::GetItemsInFov() const
{
	//RETURN ENITTIES NOT ITEMS
	vector<EntityInfo> vItemsInFov = {};
	vector<EntityInfo> ei{};
	ei = GetEntitiesInFOV();
	for (size_t i{}; i < ei.size(); i++)
	{
		if (ei.at(i).Type == eEntityType::ITEM)
		{
			ItemInfo item;
			
			m_pInterface->Item_GetInfo(ei.at(i), item);
			if (item.Type != eItemType::GARBAGE) 
			{
				vItemsInFov.push_back(ei.at(i));
			}
			
		}
	}
	return vItemsInFov;
}

Elite::Blackboard* Plugin::CreateBlackboard(AgentInfo a)
{
	m_RandomPoint = GetNewRandomPoint();
	Elite::Blackboard* pBlackboard = new Elite::Blackboard();
	pBlackboard->AddData("Agent", a);
	pBlackboard->AddData("EntitiesVec", Plugin::GetEntitiesInFOV());
	pBlackboard->AddData("Steering", m_Steering);
	pBlackboard->AddData("SteeringString", std::string{});
	pBlackboard->AddData("TargetHouse", Elite::Vector2{});
	pBlackboard->AddData("AgentFleeTarget", Elite::Vector2{}); // Needs the cast for the type
	pBlackboard->AddData("Map", m_SteeringBehaviorsMap);
	pBlackboard->AddData("Plugin", this);
	pBlackboard->AddData("HousesVec", Plugin::GetHousesInFOV());
	pBlackboard->AddData("ClosestItem", EntityInfo{});
	pBlackboard->AddData("RandomTarget", m_RandomPoint);
	pBlackboard->AddData("RandomTargetPurgeFree", Elite::Vector2{});
	pBlackboard->AddData("ClosestEnemy", EnemyInfo{});
	pBlackboard->AddData("PurgeFleeTarget", PurgeZoneInfo{});
	return pBlackboard;
}

void Plugin::UpdateBlackboard() 
{
	m_pBlackboard->ChangeData("EntitiesVec", Plugin::GetEntitiesInFOV());
	m_pBlackboard->ChangeData("HousesVec", Plugin::GetHousesInFOV());
	m_pBlackboard->ChangeData("Agent", m_pInterface->Agent_GetInfo());
	Elite::Vector2 r;
	m_pBlackboard->GetData("RandomTarget", r);
	if(m_pInterface->Agent_GetInfo().Position.DistanceSquared(r) < sqrtf(250.f))
	{
		m_pBlackboard->ChangeData("RandomTarget", GetNewRandomPoint());
	}

	//RESET SO THE BEHAVIORCONDITIONAL WORKS FROM PURGE ZONES

	if (m_pInterface->Agent_GetInfo().Position.DistanceSquared(r) < sqrtf(250.f))
	{
		m_IsFleeingFromPurge = false;
		m_pBlackboard->ChangeData("RandomTargetPurgeFree", Elite::Vector2{});
	}
	
}

Elite::Vector2 Plugin::GetNewRandomPoint() const
{
	float xWorldSize = m_pInterface->World_GetInfo().Dimensions.x;
	float yWorldSize = m_pInterface->World_GetInfo().Dimensions.y;
	float maxX = xWorldSize / 2;
	float minX = -maxX;
	float maxY = yWorldSize / 2;
	float minY = -maxY;
	float randomX = Elite::randomFloat(minX, maxX);
	float randomY = Elite::randomFloat(minY, maxY);
	Elite::Vector2 random{ randomX,randomY };
	return Elite::Vector2{ Elite::randomFloat(minX, maxX), Elite::randomFloat(minY, maxY) };

}

bool Plugin::HasGun() 
{
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};

		if (m_InventoryFilled.at(i)) 
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::PISTOL)
				{
					if (GetInterface()->Weapon_GetAmmo(item) > 0) 
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Plugin::CleanupInventory() 
{
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};
		if (m_InventoryFilled.at(i)) 
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				switch(item.Type)
				{
				case eItemType::PISTOL:
					if (GetInterface()->Weapon_GetAmmo(item) == 0) 
					{
						GetInterface()->Inventory_RemoveItem(i);
						m_InventoryFilled.at(i) = false;
					}
					break;
				case eItemType::FOOD:
					if (GetInterface()->Food_GetEnergy(item) == 0) 
					{
						GetInterface()->Inventory_RemoveItem(i);
						m_InventoryFilled.at(i) = false;
					}
					break;
				case eItemType::MEDKIT:
					if (GetInterface()->Medkit_GetHealth(item) == 0)
					{
						GetInterface()->Inventory_RemoveItem(i);
						m_InventoryFilled.at(i) = false;
					}
					break;
				}
			}
		}
	}

	return false;
}

void Plugin::AddItem()
{
	ItemInfo info{};
	EntityInfo e{};
	m_pBlackboard->GetData("ClosestItem", e);
	if (m_pInterface->Item_Grab(e, info))
	{
		int count{};
		for (size_t i{}; i < m_InventoryFilled.size(); i++) 
		{
			if(m_InventoryFilled.at(i) == false)
			{
				count = i;
				m_InventoryFilled.at(i) = true;
				break;
			}
		}
		m_pInterface->Inventory_AddItem(count, info);
	}
}

bool Plugin::CheckIfInventoryFull() const 
{
	int count{};
	for (size_t i{}; i < m_InventoryFilled.size(); i++) 
	{
		if(m_InventoryFilled.at(i) == true)
		{
			count++;
		}
	}
	if (count == 5) 
	{
		return true;
	}
	return false;
}

bool Plugin::HasMedcine() 
{
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};

		if (m_InventoryFilled.at(i))
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::MEDKIT)
				{
					if (GetInterface()->Medkit_GetHealth(item) > 0)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Plugin::HasFood() 
{
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};

		if (m_InventoryFilled.at(i))
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::FOOD)
				{
					if (GetInterface()->Food_GetEnergy(item) > 0)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

void  Plugin::FaceCurrenTarget(SteeringPlugin_Output& steering)
{
	steering.AutoOrient = false;

	//LOOK AT CURRENT TARGET
	AgentInfo agent{};
	m_pBlackboard->GetData("Agent", agent);
	Elite::Vector2 target{};
	if (m_IsLookingForItem) 
	{
		EntityInfo item;
		m_pBlackboard->GetData("ClosestItem", item);
		target = item.Location;
	}
	else
	{
		
		m_pBlackboard->GetData("TargetHouse", target);
	}
	Elite::Vector2 direction = target - agent.Position;
	direction.GetNormalized();
	m_Direction = direction;
	float currentOrientation = std::fmod(agent.Orientation + (float)M_PI, 2 * (float)M_PI);
	if (currentOrientation < 0.f)
	{
		currentOrientation = 2 * (float)M_PI + currentOrientation;
	}
	float targetOrientation = Elite::GetOrientationFromVelocity(direction) + (float)M_PI;
	float difference = currentOrientation - targetOrientation;

	if (difference < 0.1f && difference > -0.1f)
	{
		steering.AngularVelocity = 0.f;
	}
	else if ((difference < 0 || difference >(float)M_PI) && difference > (float)-M_PI)
	{
		steering.AngularVelocity = agent.MaxAngularSpeed;
	}
	else
	{
		steering.AngularVelocity = -agent.MaxAngularSpeed;
	}

}

vector<EntityInfo> Plugin::GetPurgeZones() const 
{
	vector<EntityInfo> zones{};
	vector<EntityInfo> allEntities = GetEntitiesInFOV();
	for (size_t i{}; i < allEntities.size(); i++) 
	{
		if (allEntities.at(i).Type == eEntityType::PURGEZONE) 
		{
			zones.push_back(allEntities.at(i));
		}
	}
	return zones;
}

int Plugin::GetTotalAmmo() 
{
	int total{};
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};

		if (m_InventoryFilled.at(i))
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::PISTOL)
				{
					total += GetInterface()->Weapon_GetAmmo(item);
					
				}
			}
		}
	}

	return total;
}

void Plugin::AddHouseLastVisited(HouseInfo h)
{
	if (m_LastThreeVisited.size() == 3) 
	{
		m_LastThreeVisited.pop_front();
	}
	m_LastThreeVisited.push_back(h);
}


int Plugin::GetMedkitCount()
{
	int count{};
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};

		if (m_InventoryFilled.at(i))
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::MEDKIT)
				{
					count++;
				}
			}
		}
	}
	return count;
}

int Plugin::GetFoodCount() 
{
	int count{};
	for (size_t i{}; i < GetInterface()->Inventory_GetCapacity(); i++)
	{
		ItemInfo item{};

		if (m_InventoryFilled.at(i))
		{
			if (GetInterface()->Inventory_GetItem(i, item))
			{
				if (item.Type == eItemType::FOOD)
				{
					count++;
				}
			}
		}
	}
	return count;
}