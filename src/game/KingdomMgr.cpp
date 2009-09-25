#include "KingdomMgr.h"
#include "ObjectMgr.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(KingdomMgr);

void KingdomMgr::LoadFromDB()
{
    QueryResult* result = WorldDatabase.Query("SELECT kid, team, name FROM kingdom_template");
    if (!result)
        return;

    do {
        Field* fields = result->Fetch();
        uint32 kid = fields[0].GetUInt32();
        uint8 team = fields[1].GetUInt8();
        std::string name = fields[2].GetCppString();

        if (team >= KINGDOM_TEAM_MAX)
        {
            sLog.outErrorDb("Neplatny tym u kralovstvi %u, kralovstvi nebylo vytvoreno", kid);
            continue;
        }

        Kingdom* k = new Kingdom(kid, team, name);
        k->LoadFromDB();

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
        if (it->second->HasActiveUnit(guid))
        {
            uint8 team = KINGDOM_TEAM_NEUTRAL;
            if (player)
            {
                uint32 plteam = player->GetTeam();
                switch (plteam)
                {
                    case HORDE:
                        team = KINGDOM_TEAM_HORDE;
                        break;

                    case ALLIANCE:
                        team = KINGDOM_TEAM_ALLIANCE;
                        break;
                }
            }

            it->second->Capture(team);
            return;
        }
    }
}

Kingdom* KingdomMgr::Get(uint32 id)
{
    KingdomListIterator it = m_kingdoms.find(id);
    if (it == m_kingdoms.end())
        return NULL;

    return it->second;
}

char KingdomMgr::TeamToChar(uint8 team)
{
    switch (team)
    {
        case KINGDOM_TEAM_ALLIANCE: return 'a';
        case KINGDOM_TEAM_HORDE:    return 'h';
        case KINGDOM_TEAM_NEUTRAL:  return 'n';
    }

    return '-';
}

uint8 KingdomMgr::CharToTeam(char team)
{
    switch (team)
    {
        case 'a': case 'A': return KINGDOM_TEAM_ALLIANCE;
        case 'h': case 'H': return KINGDOM_TEAM_HORDE;
        case 'n': case 'N': default: return KINGDOM_TEAM_NEUTRAL;
    }
}
