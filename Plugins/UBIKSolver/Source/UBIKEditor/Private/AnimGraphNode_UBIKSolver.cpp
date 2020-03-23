// 2020 Sticky Snout Studio (Jonas M�lgaard)

#include "..\Public\AnimGraphNode_UBIKSolver.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

FText UAnimGraphNode_UBIKSolver::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UBIKSolver", "UBIK Solver");
}

FText UAnimGraphNode_UBIKSolver::GetTooltipText() const
{
	return LOCTEXT("UBIKSolver_Tooltip", "Solve inverse kinematics for UpperBody.");
}

#undef LOCTEXT_NAMESPACE
