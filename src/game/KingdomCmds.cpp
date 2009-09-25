#include "Common.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "KingdomMgr.h"

bool ChatHandler::HandleNpcKingdomCommand(const char* args)
{
    // .npc kingdom <kingdom id> [a/h/n/-]
    // prvni parametr udava kingdom id
    // druhy parametr prislusny tym, bez vyplneni 
    // odstrani bind creatury na kindgom
    // ! po provedeni prikazu je vyzadovan restart !
    // .npc kingdom
    // uvede aktualni bind creatury na kingdom

    Creature* creature = getSelectedCreature();
    if (!creature)
    {
        PSendSysMessage("Musis vybrat creaturu");
        return false;
    }

    if (!*args)
    {
        QueryResult* result = WorldDatabase.PQuery("SELECT kid, team FROM kingdom_creature WHERE guid = %u", creature->GetDBTableGUIDLow());
        if (!result)
        {
            PSendSysMessage("Zvolena creature nema bind na kralovstvi");
            return true;
        }

        Field* f = result->Fetch();
        uint32 kid = f[0].GetUInt32();
        uint8 team = f[1].GetUInt8();
        char teamc = KingdomMgr::TeamToChar(team);

        PSendSysMessage("Zvolena creatura ma bind na kralovstvi %u (tym %c)", kid, teamc);

        delete result;
        return true;
    } 

    char* kidc = strtok((char*)args, " ");
    char* rest = strtok(NULL, " ");

    uint32 kid = atoi(kidc);
    if (!kid)
    {
        PSendSysMessage("Neplatne id kralovstvi");
        return false;
    }

    uint32 guid = creature->GetDBTableGUIDLow();
    uint8 team = KINGDOM_TEAM_MAX;
    if (rest)
        team = KingdomMgr::CharToTeam(rest[0]);

    WorldDatabase.PExecute("DELETE FROM kingdom_creature WHERE guid = %u", guid);
    if (team != KINGDOM_TEAM_MAX)
    {
        WorldDatabase.PExecute("INSERT INTO kingdom_creature VALUES (%u,%u,%u)", kid, team, guid);
        if (Kingdom* k = kingdommgr.Get(kid))
            k->AddNewUnit(guid, team);
    }

    PSendSysMessage("Bind na kralovstvi upraven");
    return true;
}

bool ChatHandler::HandleKingdomSetCommand(const char* args)
{
    if (!*args)
        return false;

    char* kidc = strtok((char*)args, " ");
    char* rest = strtok(NULL, " ");

    uint32 kid = atoi(kidc);
    if (!kid)
    {
        PSendSysMessage("Neplatne id kralovstvi");
        return false;
    }

    uint8 team = KINGDOM_TEAM_NEUTRAL;
    if (rest)
        team = KingdomMgr::CharToTeam(rest[0]);

    if (Kingdom* k = kingdommgr.Get(kid))
        k->Capture(team);
    else
        return false; // kralovstvi neexistuje
    return true;
}
