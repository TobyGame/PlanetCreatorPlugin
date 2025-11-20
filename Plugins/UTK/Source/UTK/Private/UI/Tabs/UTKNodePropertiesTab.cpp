#include "UI/Tabs/UTKNodePropertiesTab.h"

#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/NodeProperties/UTKNodePropertyProxy.h"
#include "UI/Toolkit/UTKEditorApp.h"

FUTKNodePropertiesTab::FUTKNodePropertiesTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("NodePropertiesTab"), InEditor), Editor(InEditor)
{
	TabLabel = FText::FromString(TEXT("Node Properties"));
	ViewMenuDescription = TabLabel;
	ViewMenuTooltip = FText::FromString(TEXT("Edit UTK node properties"));
}

TSharedRef<SWidget> FUTKNodePropertiesTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FUTKEditorApp> EditorPtr = Editor.Pin();
	if (!EditorPtr.IsValid())
	{
		return SNew(STextBlock)
			.Text(FText::FromString(TEXT("Invalid UTK editor")));
	}

	FPropertyEditorModule& PropertyEditorModule =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.bShowScrollBar = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	EditorPtr->OnSelectedNodeChanged().AddRaw(
		const_cast<FUTKNodePropertiesTab*>(this),
		&FUTKNodePropertiesTab::HandleSelectedNodeChanged
	);

	const_cast<FUTKNodePropertiesTab*>(this)->HandleSelectedNodeChanged(EditorPtr->GetSelectedNode());

	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}

void FUTKNodePropertiesTab::HandleSelectedNodeChanged(UUTKNode* NewNode)
{
	if (!DetailsView.IsValid())
		return;

	UUTKNodePropertyProxy* Proxy = NewObject<UUTKNodePropertyProxy>();

	if (NewNode)
		Proxy->InitializeFromNode(NewNode);

	DetailsView->SetObject(Proxy, true);
}