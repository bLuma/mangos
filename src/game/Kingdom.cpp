#include "Kingdom.h"
#include "ObjectMgr.h"
#include "World.h"
#include "MapManager.h"

Kingdom::Kingdom() { }

Kingdom::~Kingdom() 
{
    m_allianceSpawns.clear();
    m_hordeSpawns.clear();
    m_neutralSpawns.clear();
    m_allianceObjects.clear();
    m_hordeObjects.clear();
    m_neutralObjects.clear();
}

void Kingdom::LoadFromDB(uint32 id, uint8 defaultOwner) 
{
    m_id = id;
    m_defaultOwner = defaultOwner;

    QueryResult* result = CharacterDatabase.PQuery("SELECT team FROM kingdom WHERE kid = %u", m_id);
    if (!result) 
    {
        m_currentOwner = defaultOwner;
    } 
    else 
    {
        Field* f = result->Fetch();

        m_currentOwner = f[0].GetUInt8();
        if (m_currentOwner >= KINGDOM_TEAM_MAX) 
        {
            m_currentOwner = m_defaultOwner;
            sLog.outError("Neplatny tym u kralovstvi %u, resetovan na zakladni!", m_id);
        }

        delete result;
    }

    QueryResult* creatureResult = WorldDatabase.PQuery("SELECT team, guid FROM kingdom_creature WHERE kid = %u", m_id);
    if (creatureResult)
    {
        do {
            Field* f = creatureResult->Fetch();
            uint8 team = f[0].GetUInt8();
            uint32 guid = f[1].GetUInt32();

            if (team >= KINGDOM_TEAM_MAX)
            {
                sLog.outErrorDb("Neplatny tym u potvory (guid:%u) z kralovstvi %u, potvora preskocena!", guid, m_id);
                continue;
            }

            GetCreatureListByTeam(team).insert(guid);
        } while(creatureResult->NextRow());

        delete creatureResult;
    }

    QueryResult* gameobjectResult = WorldDatabase.PQuery("SELECT team, guid FROM kingdom_gameobject WHERE kid = %u", m_id);
    if (gameobjectResult)
    {
        do {
            Field* f = gameobjectResult->Fetch();
            uint8 team = f[0].GetUInt8();
            uint32 guid = f[1].GetUInt32();

            if (team >= KINGDOM_TEAM_MAX)
            {
                sLog.outErrorDb("Neplatny tym u gameobjectu (guid:%u) z kralovstvi %u, gameobject preskocen!", guid, m_id);
                continue;
            }

            GetGameObjectListByTeam(team).insert(guid);
        } while(gameobjectResult->NextRow());

        delete gameobjectResult;
    }
  
    SpawnDespawn(m_currentOwner, true);
    SpawnDespawnObject(m_currentOwner, true);
}

void Kingdom::SpawnDespawn(uint8 team, bool spawn)
{
    KingdomCreatureList& creatures = GetCreatureListByTeam(team);

    for (KingdomCLIterator it = creatures.begin(); it != creatures.end(); it++)
    {
        if (spawn)
            SpawnGuid(*it);
        else
            DespawnGuid(*it);
    }
}

void Kingdom::SpawnGuid(uint32 guid)
{
    CreatureData const* data = objmgr.GetCreatureData(guid);
    if (data)
    {
        objmgr.AddCreatureToGrid(guid, data);

        // Spawn if necessary (loaded grids only)
        Map* map = const_cast<Map*>(MapManager::Instance().CreateBaseMap(data->mapid));
        // We use spawn coords to spawn
        if (!map->Instanceable() && !map->IsRemovalGrid(data->posX, data->posY))
        {
            Creature* pCreature = new Creature;
            //sLog.outDebug("Spawning creature %u",guid);
            if (!pCreature->LoadFromDB(guid, map))
            {
                delete pCreature;
            }
            else
            {
                map->Add(pCreature);
                if (!pCreature->isAlive())
                    pCreature->Respawn();
            }
        }
        return;
    }
    return;
}

void Kingdom::DespawnGuid(uint32 guid)
{
    if (CreatureData const* data = objmgr.GetCreatureData(guid))
    {
        objmgr.RemoveCreatureFromGrid(guid, data);

        if (Creature* pCreature = ObjectAccessor::Instance().GetObjectInWorld(MAKE_NEW_GUID(guid, data->id, HIGHGUID_UNIT), (Creature*)NULL))
        {
            pCreature->ForcedDespawn();
            pCreature->AddObjectToRemoveList();
        }
    }
}

void Kingdom::SpawnDespawnObject(uint8 team, bool spawn)
{
    KingdomGameObjectList& gameobjects = GetGameObjectListByTeam(team);

    for (KingdomGLIterator it = gameobjects.begin(); it != gameobjects.end(); it++)
    {
        if (spawn)
            SpawnObject(*it);
        else
            DespawnObject(*it);
    }
}


void Kingdom::SpawnObject(uint32 guid)
{
    GameObjectData const* data = objmgr.GetGOData(guid);
    if (data)
    {
        objmgr.AddGameobjectToGrid(guid, data);
        // Spawn if necessary (loaded grids only)
        // this base map checked as non-instanced and then only existed
        Map* map = const_cast<Map*>(MapManager::Instance().CreateBaseMap(data->mapid));
        // We use current coords to unspawn, not spawn coords since creature can have changed grid
        if (!map->Instanceable() && !map->IsRemovalGrid(data->posX, data->posY))
        {
            GameObject* pGameobject = new GameObject;
            //sLog.outDebug("Spawning gameobject %u", guid);
            if (!pGameobject->LoadFromDB(guid, map))
            {
                delete pGameobject;
            }
            else
            {
                if (pGameobject->isSpawnedByDefault())
                    map->Add(pGameobject);
            }
        }
        return;
    }
    return;
}

void Kingdom::DespawnObject(uint32 guid)
{
    if (GameObjectData const* data = objmgr.GetGOData(guid))
    {
        objmgr.RemoveGameobjectFromGrid(guid, data);

        if (GameObject* pGameobject = ObjectAccessor::Instance().GetObjectInWorld(MAKE_NEW_GUID(guid, data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
            pGameobject->AddObjectToRemoveList();
    }
}

void Kingdom::Capture()
{
    Capture(GetEnemyTeam());
}

void Kingdom::Capture(uint8 team)
{
    if (team == m_currentOwner)
        return;

    // despawn soucasnych objektu
    SpawnDespawn(m_currentOwner, false);
    SpawnDespawnObject(m_currentOwner, false);

    m_currentOwner = team;

    // spawn novych objektu
    SpawnDespawn(m_currentOwner, true);
    SpawnDespawnObject(m_currentOwner, true);

    // zapsani do db
    CharacterDatabase.PExecute("DELETE FROM kingdom WHERE kid = %u", m_id);
    CharacterDatabase.PExecute("INSERT INTO kingdom VALUES (%u, %u)", m_id, m_currentOwner);
}

bool Kingdom::HasUnit(uint32 guid)
{
    KingdomCLIterator it = m_allianceSpawns.find(guid);
    if (it != m_allianceSpawns.end())
        return true;

    KingdomCLIterator itb = m_hordeSpawns.find(guid);
    if (itb != m_hordeSpawns.end())
        return true;

    KingdomCLIterator itc = m_neutralSpawns.find(guid);
    if (itc != m_neutralSpawns.end())
        return true;

    return false;
}

KingdomCreatureList& Kingdom::GetCreatureListByTeam(uint8 team)
{
    switch (team)
    {
        case KINGDOM_TEAM_ALLIANCE: 
            return m_allianceSpawns;
        case KINGDOM_TEAM_HORDE:
            return m_hordeSpawns;
        case KINGDOM_TEAM_NEUTRAL:
            return m_neutralSpawns;
        default:
            sLog.outError("Kingdom::GetCreatureListByTeam() zavolan pro neznamy tym (%u)!", team);
            return KingdomCreatureList();
    }
}

KingdomGameObjectList& Kingdom::GetGameObjectListByTeam(uint8 team)
{
    switch (team)
    {
        case KINGDOM_TEAM_ALLIANCE: 
            return m_allianceObjects;
        case KINGDOM_TEAM_HORDE:
            return m_hordeObjects;
        case KINGDOM_TEAM_NEUTRAL:
            return m_neutralObjects;
        default:
            sLog.outError("Kingdom::GetGameObjectListByTeam() zavolan pro neznamy tym (%u)!", team);
            return KingdomGameObjectList();
    }
}