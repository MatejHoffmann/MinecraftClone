#include "FunctionLibrary.h"

void Minecraft::AddCanvasPanelChild(UCanvasPanel* CanvasPanel, UWidget* ChildWidget, int32 ZOrder, FAnchors Anchors, FMargin Offset)
 {
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(CanvasPanel->AddChild(ChildWidget));
	CanvasPanelSlot->SetZOrder(ZOrder);
	CanvasPanelSlot->SetAnchors(Anchors);
	CanvasPanelSlot->SetOffsets(Offset);
 }

void Minecraft::AddSubmenuWidget(UCanvasPanel* CanvasPanel, UWidget* ChildWidget)
{
	const TArray<UPanelSlot*>& PanelSlots = CanvasPanel->GetSlots();
	int32 MaxZOrder = 0;

	// Find the highest ZOrder number
	for(auto& PanelSlot : PanelSlots)
	{
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(PanelSlot);
		MaxZOrder = CanvasPanelSlot->GetZOrder() > MaxZOrder ? CanvasPanelSlot->GetZOrder() : MaxZOrder;
	}
	// Add the submenu to the highest zorder position and stretch it across the whole screen
	AddCanvasPanelChild(CanvasPanel, ChildWidget, MaxZOrder + 1, FAnchors(0,0,1,1), FMargin(0));
}