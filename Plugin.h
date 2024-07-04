#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
//#include "framework/Data/EBlackboard.h"
//#include "Behaviors.h"
#include "framework/SteeringBehaviors/SteeringBehaviors.h"
#include <map>

class IBaseInterface;
class IExamInterface;
//class ISteeringBehavior; 
namespace Elite {
	class BehaviorTree;
	class Blackboard;
}

class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

	IExamInterface* GetInterface() { return m_pInterface; }
	std::deque<HouseInfo> GetLastThreeVisited() const { return m_LastThreeVisited; }
	void AddHouseLastVisited(HouseInfo h);

	HouseInfo GetInsideHouse() const { return m_InsideHouse; }
	void SetInsideHouse(HouseInfo& h) { m_InsideHouse = h; }
	Elite::Vector2 GetLastPosOutside() { return m_LastPositionOutHouse; }
	void SetLastPosOutside(Elite::Vector2& pos) { m_LastPositionOutHouse = pos; }
	bool GetIsLookingForItem() { return m_IsLookingForItem; }
	void SetIsLookingForItem(bool b) { m_IsLookingForItem = b; }
	bool GetIsLookingForHouse() { return m_IsLookingForHouse; }
	void SetIsLookingForHouse(bool b) { m_IsLookingForHouse = b; }
	bool GetIsInPurgeZone() { return m_IsInPurgeZone; }
	void SetIsInPurgeZone(bool b) { m_IsInPurgeZone = b; }
	bool GetIsFleeingFromPurge() { return m_IsFleeingFromPurge; }
	void SetIsFleeingFromPurge(bool b) { m_IsFleeingFromPurge = b; }

	vector<EnemyInfo> GetEnemiesInFov() const;
	vector<EntityInfo> GetItemsInFov() const;
	vector<EntityInfo> GetPurgeZones() const;
	Elite::Vector2 GetNewRandomPoint() const;
	void SetIsLookingRandom(bool isLookingRandom) { m_IsLookingRandom = isLookingRandom; }
	bool CheckIfInventoryFull() const;
	std::vector<bool> GetInventoryFilled() const { return m_InventoryFilled; }
	bool HasGun();
	bool HasMedcine();
	bool HasFood();
	int GetTotalAmmo();
	int GetMedkitCount();
	int GetFoodCount();
private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	vector<HouseInfo> GetHousesInFOV() const;
	vector<EntityInfo> GetEntitiesInFOV() const;
	void AddItem();
	void FaceCurrenTarget(SteeringPlugin_Output& steering);
	Elite::Blackboard* CreateBlackboard(AgentInfo a);
	void UpdateBlackboard();
	
	bool CleanupInventory();

	
	Elite::Vector2 m_Direction{};
	Elite::Vector2 m_LastPositionOutHouse{};
	HouseInfo m_InsideHouse{};
	float m_BittenRotateTimer{};
	float m_RotateTimerMax{ 3.f };
	bool m_isRotatingBitten{};
	std::deque<HouseInfo> m_LastThreeVisited{};
	Elite::Vector2 m_Target = {};
	
	bool m_IsFleeingFromPurge{};
	bool m_IsLookingForItem{};
	bool m_IsLookingForHouse{};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose
	bool m_IsInPurgeZone{};
	std::vector<bool> m_InventoryFilled{ false,false,false,false,false };
	

	bool m_IsLookingRandom{};
	Elite::Vector2 m_RandomPoint{};

	ISteeringBehavior* m_Steering{nullptr};
	ISteeringBehavior* pWander{ nullptr };
	Elite::Blackboard* m_pBlackboard{ nullptr };
	Elite::BehaviorTree* m_pBehaviourTree{ nullptr };
	std::map<std::string, ISteeringBehavior*> m_SteeringBehaviorsMap{ {"Wander",new Wander()},{"Flee",new Flee()},{"Seek",new Seek},};

	int m_InventoryCount{};
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}

