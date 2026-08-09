#pragma once
#include "PonpokoHelper.h"

enum class EInputType : int
{
    None,
    MoveLeft,
    MoveRight,
    ClimbUp,
    ClimbDown,
    Jump,
    Max
};

class CPlayerSession
{
public:
    void Reset(Ponpoko::SPos InStartPos, int InStartRow);
    void ApplyInput(EInputType InInput, Ponpoko::SMapData& InMapData);
    void Update(float fDelta, Ponpoko::SMapData& InMapData);

    float GetX() const { return m_fX; }
    int GetRow() const { return m_nRow; }
    int GetScore() const { return m_nScore; }
    float GetJumpOffsetY() const { return m_fJumpOffsetY; }
    float GetRowOffsetY() const { return m_fRowOffsetY; }
    bool IsJumping() const { return m_bJumping; }

private:
    bool TryGetPlatformBounds(const Ponpoko::SMapData& InMapData, float InX, float& OutStartX, float& OutEndX) const;
    bool HasLadderAt(const Ponpoko::SMapData& InMapData, int nFloor) const;
    void CheckPickupAndHazards(Ponpoko::SMapData& InMapData);
    void StartRowTransition(int nToRow, bool bFalling);
    void Respawn();

private:
    float m_fX = 0.0f;
    int m_nRow = 0;
    int m_nScore = 0;
    float m_fFacingDir = 1.0f;

    bool m_bJumping = false;
    float m_fJumpTimer = 0.0f;
    float m_fJumpOffsetY = 0.0f;
    float m_fJumpStartX = 0.0f;

    bool m_bRowTransition = false;
    bool m_bFalling = false;
    float m_fRowTransitionTimer = 0.0f;
    int m_nTransitionFromRow = 0;
    int m_nTransitionToRow = 0;
    float m_fRowOffsetY = 0.0f;

    Ponpoko::SPos m_StartPos;
    int m_nStartRow = 0;

private:
    constexpr static float m_fMoveStep = 6.0f;
    constexpr static float m_fLadderTolerance = 20.0f;
    constexpr static float m_fPickupRadius = 20.0f;
    constexpr static float m_fFloorLeftBound = 20.0f;
    constexpr static float m_fFloorRightBound = 980.0f;
    constexpr static float m_fJumpDuration = 0.5f;
    constexpr static float m_fJumpHeight = 60.0f;
    constexpr static float m_fJumpDistance = 80.0f;
    constexpr static float m_fClimbDuration = 0.3f;
    constexpr static float m_fFallDuration = 0.15f;
};