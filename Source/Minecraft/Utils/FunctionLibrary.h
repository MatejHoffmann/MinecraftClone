#pragma once

#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"

namespace Minecraft
{
	/*
	 * Add new widget to the given Canvas panel. The newly created slot inside the panel will have the given ZOrder, Anchors and Offset.
	 */
	 void AddCanvasPanelChild(UCanvasPanel* CanvasPanel, UWidget* ChildWidget, int32 ZOrder, FAnchors Anchors, FMargin Offset);

	/*
	 * Add a Submenu to the given Canvas panel. The submenu will be placed at the highest ZOrder position and will be stretched across the whole canvas
	 */
	 void AddSubmenuWidget(UCanvasPanel* CanvasPanel, UWidget* ChildWidget);
}
