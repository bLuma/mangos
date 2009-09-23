#ifndef MANGOS_KINGDOM_H
#define MANGOS_KINGDOM_H

#include "SharedDefines.h"
#include <set>
#include "GameObject.h"
#include "Player.h"

// interni oznaceni hordy a aliance
enum KingdomTeams 
{
    KINGDOM_TEAM_ALLIANCE = 0,
    KINGDOM_TEAM_HORDE = 1,
    KINGDOM_TEAM_NEUTRAL = 2,
    KINGDOM_TEAM_MAX = 3
};

// datovy typ pro seznam guid npc
typedef std::set<uint32> KingdomCreatureList;
typedef KingdomCreatureList::iterator KingdomCLIterator;

// datovy typ pro seznam guid gameobjektu
typedef std::set<uint32> KingdomGameObjectList;
typedef KingdomGameObjectList::iterator KingdomGLIterator;

class Kingdom 
{
    public:
        Kingdom();
        ~Kingdom();
        void LoadFromDB(uint32 id, uint8 defaultOwner);
        
        uint8 GetEnemyTeam() 
        { 
            if (m_currentOwner == KINGDOM_TEAM_NEUTRAL) 
                return KINGDOM_TEAM_NEUTRAL; 
            return (m_currentOwner == KINGDOM_TEAM_ALLIANCE ? KINGDOM_TEAM_HORDE : KINGDOM_TEAM_ALLIANCE); 
        }
        // zajmuti kralovstvi - zmena spawnu, ulozeni do db
        void Capture(uint8 team);
        void Capture();
       
        // obsahuje toto kralovstvi jednotku s guid ?
        bool HasUnit(uint32 guid);

    protected:
        // spawn/despawn npc
        void SpawnDespawn(uint8 team, bool spawn);
        void SpawnGuid(uint32 guid);
        void DespawnGuid(uint32 guid);

        // spawn/despawn gameobjetu
        void SpawnDespawnObject(uint8 team, bool spawn);
        void SpawnObject(uint32 guid);
        void DespawnObject(uint32 guid);

        KingdomCreatureList& GetCreatureListByTeam(uint8 team);
        KingdomGameObjectList& GetGameObjectListByTeam(uint8 team);

    private:
        // id kralovstvi - cislujte od 1...
        uint32 m_id;

        // zakladni vlastnik (bez dat v character db tab. kingdom)
        uint8 m_currentOwner;
        // soucastny vlatnik
        uint8 m_defaultOwner;

        // guid sesznamy npc a gameobjektu
        // todo?: presunout do array
        KingdomCreatureList m_allianceSpawns;
        KingdomCreatureList m_hordeSpawns;
        KingdomCreatureList m_neutralSpawns;

        KingdomGameObjectList m_allianceObjects;
        KingdomGameObjectList m_hordeObjects;
        KingdomGameObjectList m_neutralObjects;
};

#endif
