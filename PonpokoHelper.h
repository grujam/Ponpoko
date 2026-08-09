#pragma once
#include <vector>
#include <windows.h>

namespace Ponpoko
{
    enum class EItemType
    {
        CARROT,
        CHERRY,
        ENUM_END
    };

    struct SPos
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct SPlatform
    {
        float fY = 0.0f;
        float fStartX = 0.0f;
        float fEndX = 0.0f;
    };

    struct SLadder
    {
        float fX = 0.0f;
        int nFloor = 0;
    };

    struct SSpike : public SPos
    {
    };

    struct SItemData : public SPos
    {
        EItemType eType = EItemType::CARROT;
        int nScore = 10;
    };

    struct SEnemyData : public SPos
    {
        constexpr static float fSpeed = 40.0f;
        constexpr static float fTickDuration = 1.0f / 60.0f;


        float fPatrolRange = 10.0f;
        float GetCurrentX(int nTick) const
        {
            if (fPatrolRange <= 0.0f)
                return x;

            float fDist = fmodf(nTick * fTickDuration * fSpeed, fPatrolRange * 4.0f);
            if (fDist < 0) fDist += fPatrolRange * 4.0f;

            float fOffset;
            if (fDist < fPatrolRange)
                fOffset = fDist;
            else if (fDist < fPatrolRange * 3.0f)
                fOffset = fPatrolRange * 2.0f - fDist;
            else
                fOffset = fDist - fPatrolRange * 4.0f;

            return x + fOffset;
        }
    };

    struct SMapData
    {
        int nStageLevel = 0;

        SPos StartPos;

        std::vector<SPlatform> Platforms;
        std::vector<SLadder> Ladders;
        std::vector<SSpike> Spikes;

        std::vector<SItemData> Items;
        std::vector<SEnemyData> Enemies;

        bool SaveMapData(const std::wstring& path);
        bool LoadMapData(const std::wstring& path);
        void DrawMapData(IDirect3DTexture9* pTexture);

        float GetRowY(int nRowIndex) const;
        int GetRowCount() const { return m_nPlatformRowCount; }

        constexpr static float m_fCeilingY = 150.0f;
        constexpr static float m_fFloorY = 750.0f;
        constexpr static int m_nPlatformRowCount = 4;
        constexpr static float m_fLadderWidth = 30.0f;
        constexpr static float m_fLadderHeightScale = 1.15f;
        constexpr static float m_fSpikeWidth = 20.0f;
        constexpr static float m_fSpikeHeight = 24.0f;
        constexpr static float m_fStartSize = 20.0f;
        constexpr static float m_fItemSize = 24.0f;
        constexpr static float m_fEnemySize = 24.0f;
        constexpr static float m_fCharacterSize = 24.0f;
    };
}