#include "UI/Tabs/UTKViewportTab.h"
#include "Graph/Nodes/UTKNode.h"
#include "UI/Toolkit/UTKEditorApp.h"
#include "UI/Viewport/SUTK3DViewport.h"

FUTKViewportTab::FUTKViewportTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("ViewportTab"), InEditor), Editor(InEditor)
{
	if (InEditor.IsValid())
	{
		SelectedNodeChangedHandle = InEditor->OnSelectedNodeChanged().AddLambda([this](UUTKNode* /*NewNode*/){
			this->RefreshOutputPinOptions();

			if (this->OutputComboBox.IsValid())
			{
				this->OutputComboBox->RefreshOptions();
				this->OutputComboBox->SetSelectedItem(this->CurrentOutputSelection);
			}
		});
	}
}

FUTKViewportTab::~FUTKViewportTab()
{
	TSharedPtr<FUTKEditorApp> PinnedEditor = Editor.Pin();

	if (PinnedEditor.IsValid() && SelectedNodeChangedHandle.IsValid())
	{
		PinnedEditor->OnSelectedNodeChanged().Remove(SelectedNodeChangedHandle);
		SelectedNodeChangedHandle.Reset();
	}

	OutputComboBox.Reset();
	CurrentOutputSelection.Reset();
	OutputPinOptions.Reset();
}

const FSlateBrush* FUTKViewportTab::GetPreviewBrush() const
{
	TSharedPtr<FUTKEditorApp> PinnedEditor = Editor.Pin();

	UTexture2D* Texture = PinnedEditor.IsValid() ? PinnedEditor->GetPreviewTexture() : nullptr;

	if (Texture)
	{
		if (PreviewBrush.GetResourceObject() != Texture)
		{
			PreviewBrush = FSlateBrush();
			PreviewBrush.SetResourceObject(Texture);
			PreviewBrush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
		}

		return &PreviewBrush;
	}

	return nullptr;
}

FText FUTKViewportTab::GetCurrentOutputSelectionText() const
{
	if (CurrentOutputSelection.IsValid())
		return FText::FromName(*CurrentOutputSelection);

	return FText::FromString(TEXT("<no outputs>"));
}

void FUTKViewportTab::RefreshOutputPinOptions() const
{
	OutputPinOptions.Reset();
	CurrentOutputSelection.Reset();

	TSharedPtr<FUTKEditorApp> PinnedEditor = Editor.Pin();
	if (!PinnedEditor.IsValid())
		return;

	UUTKNode* PreviewNode = PinnedEditor->GetFocusedNode();
	if (!PreviewNode)
		PreviewNode = PinnedEditor->GetSelectedNode();

	if (!PreviewNode)
		return;

	const FUTKNodeDefinition& Def = PreviewNode->GetDefinition();
	const FName OverrideName = PinnedEditor->GetPreviewOutputPinOverrideForNode(PreviewNode);

	TSharedPtr<FName> FirstItem;

	for (const FUTKNodePinDefinition& Pin : Def.Pins)
	{
		if (!Pin.bInput)
		{
			TSharedPtr<FName> Item = MakeShared<FName>(Pin.Name);
			if (!FirstItem.IsValid())
				FirstItem = Item;

			OutputPinOptions.Add(Item);

			if (!OverrideName.IsNone() && OverrideName == Pin.Name)
				CurrentOutputSelection = Item;
		}
	}

	if (!CurrentOutputSelection.IsValid())
		CurrentOutputSelection = FirstItem;
}

TSharedRef<SWidget> FUTKViewportTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FUTKEditorApp> PinnedEditor = Editor.Pin();
	RefreshOutputPinOptions();

	return SNew(SBorder)
		.Padding(4.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 0.4f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Preview Output")))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(8.0f, 0.0f)
				.VAlign(VAlign_Center)
				[
					SAssignNew(OutputComboBox, SComboBox<TSharedPtr<FName>>)
					.OptionsSource(&OutputPinOptions)
					.InitiallySelectedItem(CurrentOutputSelection)
					.OnGenerateWidget_Lambda([](TSharedPtr<FName> Item){
						const FText Text = Item.IsValid()
							? FText::FromName(*Item)
							: FText::FromString(TEXT("<None>"));
						return SNew(STextBlock).Text(Text);
					})
					.OnSelectionChanged_Lambda([this, WeakEditor = TWeakPtr<FUTKEditorApp>(PinnedEditor)](TSharedPtr<FName> NewItem, ESelectInfo::Type){
						TSharedPtr<FUTKEditorApp> Ed = WeakEditor.Pin();
						if (!Ed.IsValid())
							return;

						UUTKNode* PreviewNode = Ed->GetFocusedNode();
						if (!PreviewNode)
							PreviewNode = Ed->GetSelectedNode();

						if (!PreviewNode)
							return;

						if (!NewItem.IsValid())
						{
							Ed->SetPreviewOutputPinOverrideForNode(PreviewNode, NAME_None);
							this->CurrentOutputSelection.Reset();
						}
						else
						{
							Ed->SetPreviewOutputPinOverrideForNode(PreviewNode, *NewItem);
							this->CurrentOutputSelection = NewItem;
						}

						Ed->EvaluateCurrentSelectionForPreview();
					})
					.Content()
					[
						SNew(STextBlock)
						.Text(this, &FUTKViewportTab::GetCurrentOutputSelectionText)
					]
				]
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SBorder)
				[
					SNew(SUTK3DViewport)
					.EditorApp(Editor)
				]
			]
		];
}