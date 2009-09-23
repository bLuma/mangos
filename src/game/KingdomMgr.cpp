#include "KingdomMgr.h"
#include "ObjectMgr.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(KingdomMgr);

void KingdomMgr::LoadFromDB()
{
    QueryResult* result = WorldDatabase.Query("SELECT kid, team FROM kingdom_template");
    if (!result) 
        return;

    do {
        Field* fields = result->Fetch();
        uint32 kid = fields[0].GetUInt32();
        uint8 team = fields[1].GetUInt8();

        if (team >= KINGDOM_TEAM_MAX) 
        {
            sLog.outErrorDb("Neplatny tym u kralovstvi %u, kralovstvi nebylo vytvoreno", kid);
            continue;
        }

        Kingdom* k = new Kingdom;
        k->LoadFromDB(kid, team);

        m_kingdoms[kid] = k;
    } while(result->NextRow());

    delete result;
}

KingdomMgr::~KingdomMgr()
{
    for(KingdomListIterator it = m_kingdoms.begin(); it != m_kingdoms.end(); it++)
    {
        delete it->second;
    }

    m_kingdoms.clear();
}

void KingdomMgr::MajordomoDied(uint32 guid, Player* player)
{
    for (KingdomListIterator it = m_kingdoms.begin(); it != m_kingdoms.end(); it++)
    {
        if (it->second->HasUnit(guid))
        {
            uint8 team = KINGDOM_TEAM_NEUTRAL;
            if (player)
            {
                uint32 plteam = player->GetTeam();
                switch (plteam)
                {
                    case HORDE: team = KINGDOM_TEAM_HORDE; break;
                    case ALLIANCE: team = KINGDOM_TEAM_ALLIANCE; break;
                }
            }

            it->second->Capture(team);
            return;
        }
    }
}
