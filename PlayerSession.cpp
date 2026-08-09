#include "stdafx.h"
#include "PlayerSession.h"
#include <cmath>

using namespace Ponpoko;

void CPlayerSession::Reset(SPos InStartPos, int InStartRow)
{
    m_StartPos = InStartPos;
    m_nStartRow = InStartRow;
    m_fX = InStartPos.x;
    m_nRow = InStartRow;
    m_nScore = 0;
    m_fFacingDir = 1.0f;
    m_bJumping = false;
    m_fJumpTimer = 0.0f;
    m_fJumpOffsetY = 0.0f;
    m_bRowTransition = false;
    m_bFalling = false;
    m_fRowOffsetY = 0.0f;
}

bool CPlayerSession::TryGetPlatformBounds(const SMapData& InMapData, float InX, float& OutStartX, float& OutEndX) const
{
    float fY = InMapData.GetRowY(m_nRow);

    if (fY == SMapData::m_fFloorY)
    {
        OutStartX = m_fFloorLeftBound;
        OutEndX = m_fFloorRightBound;
        return true;
    }

    for (const auto& platform : InMapData.Platforms)
    {
        if (platform.fY == fY && InX >= platform.fStartX && InX <= platform.fEndX)
        {
            OutStartX = platform.fStartX;
            OutEndX = platform.fEndX;
            return true;
        }
    }
    return false;
}

bool CPlayerSession::HasLadderAt(const SMapData& InMapData, int nFloor) const
{
    for (const auto& ladder : InMapData.Ladders)
    {
        if (ladder.nFloor == nFloor && fabsf(ladder.fX - m_fX) <= m_fLadderTolerance)
            return true;
    }
    return false;
}

void CPlayerSession::StartRowTransition(int nToRow, bool bFalling)
{
    m_bRowTransition = true;
    m_bFalling = bFalling;
    m_fRowTransitionTimer = 0.0f;
    m_nTransitionFromRow = m_nRow;
    m_nTransitionToRow = nToRow;
}

void CPlayerSession::ApplyInput(EInputType InInput, SMapData& InMapData)
{
    if ((m_bJumping || m_bRowTransition) && InInput != EInputType::None)
        return;

    switch (InInput)
    {
    case EInputType::MoveLeft:
    case EInputType::MoveRight:
    {
        float fDir = (InInput == EInputType::MoveLeft) ? -1.0f : 1.0f;
        m_fFacingDir = fDir;
        float fNextX = m_fX + fDir * m_fMoveStep;

        float fStartX, fEndX;
        if (!TryGetPlatformBounds(InMapData, m_fX, fStartX, fEndX))
            break;

        if (fNextX < fStartX) fNextX = fStartX;
        if (fNextX > fEndX) fNextX = fEndX;
        m_fX = fNextX;
        break;
    }
    case EInputType::ClimbUp:
        if (m_nRow > 0 && HasLadderAt(InMapData, m_nRow - 1))
            StartRowTransition(m_nRow - 1, false);
        break;
    case EInputType::ClimbDown:
        if (m_nRow < InMapData.GetRowCount() && HasLadderAt(InMapData, m_nRow))
            StartRowTransition(m_nRow + 1, false);
        break;
    case EInputType::Jump:
        m_bJumping = true;
        m_fJumpTimer = 0.0f;
        m_fJumpStartX = m_fX;
        break;
    default:
        break;
    }

    if (InInput != EInputType::Jump && InInput != EInputType::ClimbUp && InInput != EInputType::ClimbDown)
        CheckPickupAndHazards(InMapData);
}

void CPlayerSession::Update(float fDelta, SMapData& InMapData)
{
    if (m_bRowTransition)
    {
        float fDuration = m_bFalling ? m_fFallDuration : m_fClimbDuration;
        m_fRowTransitionTimer += fDelta;
        float fT = (m_fRowTransitionTimer >= fDuration) ? 1.0f : m_fRowTransitionTimer / fDuration;

        float fFromY = InMapData.GetRowY(m_nTransitionFromRow);
        float fToY = InMapData.GetRowY(m_nTransitionToRow);
        m_fRowOffsetY = (fToY - fFromY) * fT;

        if (fT >= 1.0f)
        {
            m_nRow = m_nTransitionToRow;
            m_fRowOffsetY = 0.0f;
            m_bRowTransition = false;

            float fStartX, fEndX;
            if (m_bFalling && !TryGetPlatformBounds(InMapData, m_fX, fStartX, fEndX))
            {
                StartRowTransition(m_nRow + 1, true);
                return;
            }

            m_bFalling = false;
            CheckPickupAndHazards(InMapData);
        }
        return;
    }

    if (!m_bJumping)
        return;

    m_fJumpTimer += fDelta;
    float fT = (m_fJumpTimer >= m_fJumpDuration) ? 1.0f : m_fJumpTimer / m_fJumpDuration;

    m_fX = m_fJumpStartX + m_fFacingDir * m_fJumpDistance * fT;
    if (m_fX < m_fFloorLeftBound) 
        m_fX = m_fFloorLeftBound;
    if (m_fX > m_fFloorRightBound) 
        m_fX = m_fFloorRightBound;

    if (m_fJumpTimer >= m_fJumpDuration)
    {
        m_bJumping = false;
        m_fJumpOffsetY = 0.0f;

        float fStartX, fEndX;
        if (!TryGetPlatformBounds(InMapData, m_fX, fStartX, fEndX))
        {
            StartRowTransition(m_nRow + 1, true);
            return;
        }

        CheckPickupAndHazards(InMapData);
        return;
    }

    m_fJumpOffsetY = 4.0f * m_fJumpHeight * fT * (1.0f - fT);
}

void CPlayerSession::CheckPickupAndHazards(SMapData& InMapData)
{
    float fY = InMapData.GetRowY(m_nRow);

    for (auto it = InMapData.Items.begin(); it != InMapData.Items.end(); ++it)
    {
        if (it->y == fY && fabsf(it->x - m_fX) <= m_fPickupRadius)
        {
            m_nScore += it->nScore;
            InMapData.Items.erase(it);
            break;
        }
    }

    for (const auto& spike : InMapData.Spikes)
    {
        if (spike.y == fY && fabsf(spike.x - m_fX) <= m_fPickupRadius)
        {
            Respawn();
            return;
        }
    }

    for (const auto& enemy : InMapData.Enemies)
    {
        if (enemy.y == fY && fabsf(enemy.x - m_fX) <= m_fPickupRadius)
        {
            Respawn();
            return;
        }
    }
}

void CPlayerSession::Respawn()
{
    m_fX = m_StartPos.x;
    m_nRow = m_nStartRow;
    m_bJumping = false;
    m_fJumpOffsetY = 0.0f;
    m_bRowTransition = false;
    m_bFalling = false;
    m_fRowOffsetY = 0.0f;
}