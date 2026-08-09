#include "stdafx.h"
#include "PonpokoHelper.h"
#include <fstream>

namespace Ponpoko
{
    template<typename T>
    void WriteVector(std::ofstream& file, const std::vector<T>& vec)
    {
        int nCount = (int)vec.size();
        file.write(reinterpret_cast<const char*>(&nCount), sizeof(nCount));
        if (nCount > 0)
            file.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * nCount);
    }

    template<typename T>
    void ReadVector(std::ifstream& file, std::vector<T>& vec)
    {
        int nCount = 0;
        file.read(reinterpret_cast<char*>(&nCount), sizeof(nCount));
        vec.resize(nCount);
        if (nCount > 0)
            file.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * nCount);
    }

    bool SMapData::SaveMapData(const std::wstring& path)
	{
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
            return false;

        file.write(reinterpret_cast<const char*>(&nStageLevel), sizeof(nStageLevel));
        file.write(reinterpret_cast<const char*>(&StartPos), sizeof(StartPos));

        WriteVector(file, Platforms);
        WriteVector(file, Ladders);
        WriteVector(file, Spikes);
        WriteVector(file, Items);
        WriteVector(file, Enemies);

        return true;
	}

	bool SMapData::LoadMapData(const std::wstring& path)
	{
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return false;

        file.read(reinterpret_cast<char*>(&nStageLevel), sizeof(nStageLevel));
        file.read(reinterpret_cast<char*>(&StartPos), sizeof(StartPos));

        ReadVector(file, Platforms);
        ReadVector(file, Ladders);
        ReadVector(file, Spikes);
        ReadVector(file, Items);
        ReadVector(file, Enemies);

        return true;
	}

    void SMapData::DrawMapData(IDirect3DTexture9* pTexture)
    {
        for (const auto& platform : Platforms)
            GET_DX_MGR()->DrawTexture(pTexture, platform.fStartX, platform.fY, 255, platform.fEndX - platform.fStartX);

        for (const auto& item : Items)
        {
            IDirect3DTexture9* pItemTexture = (item.eType == EItemType::CARROT)
                ? GET_DX_MGR()->GetItem1Texture()
                : GET_DX_MGR()->GetItem2Texture();

            GET_DX_MGR()->DrawTexture(pItemTexture, item.x - m_fItemSize / 2.0f, item.y - m_fItemSize, 255, m_fItemSize, m_fItemSize);
        }

        for (const auto& ladder : Ladders)
        {
            float fTopY = GetRowY(ladder.nFloor);
            float fBottomY = GetRowY(ladder.nFloor + 1);
            GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetLadderTexture(), ladder.fX, fTopY, 255, m_fLadderWidth, fBottomY - fTopY);
        }

        for (const auto& spike : Spikes)
            GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetSpikeTexture(), spike.x, spike.y - m_fSpikeHeight, 255, m_fSpikeWidth, m_fSpikeHeight);

        for (const auto& enemy : Enemies)
        {
            GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetEnemyTexture(),
                enemy.x - m_fEnemySize / 2.0f, enemy.y - m_fEnemySize, 255, m_fEnemySize, m_fEnemySize);
        }

        GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetFloorCeilingTexture(), 0, m_fCeilingY);
        GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetFloorCeilingTexture(), 0, m_fFloorY);

        for (int i = 0; i < m_nPlatformRowCount; ++i)
            GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetRowGuideTexture(), 0, GetRowY(i));

        GET_DX_MGR()->DrawTexture(GET_DX_MGR()->GetStartTexture(),
            StartPos.x - m_fStartSize / 2.0f, StartPos.y - m_fStartSize, 255, m_fStartSize, m_fStartSize);
    }
    float SMapData::GetRowY(int nRowIndex) const
    {
        float fSpacing = (m_fFloorY - m_fCeilingY) / (m_nPlatformRowCount + 1);
        return m_fCeilingY + fSpacing * (nRowIndex + 1);
    }
}



