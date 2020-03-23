// 2020 Sticky Snout Studio (Jonas Mølgaard)

#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_UBIKSolver.h"
#include "AnimGraphNode_UBIKSolver.generated.h"

/**
 *
 */
UCLASS(meta = (Keywords = "UBIK Inverse Kinematics Solve IK Upper Body"))
class UBIKEDITOR_API UAnimGraphNode_UBIKSolver : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = Settings)
		FAnimNode_UBIKSolver Node;
	
public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface

};
