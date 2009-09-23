#ifndef MANGOS_KINGDOMMGR_H
#define MANGOS_KINGDOMMGR_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "Kingdom.h"

class KingdomMgr {
    // seznam kralovstvi (asociativni pole: id kralovstvi -> pointer)
    typedef UNORDERED_MAP<uint32, Kingdom*> KingdomList;
    typedef KingdomList::iterator KingdomListIterator;

    public:
        // nacteni kralovstvi z db
        void LoadFromDB();
        // destruktor
        ~KingdomMgr();

        // event pri zabiti npc s flagem majordoma
        void MajordomoDied(uint32 guid, Player* player);

    private:
        // seznam kralovstvi
        KingdomList m_kingdoms;

};

// definice zkraceneho volani singletonu
#define kingdommgr MaNGOS::Singleton<KingdomMgr>::Instance()
#endif
