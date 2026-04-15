#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "WorldSelectionMenuWidget.generated.h"


/*
 * This class represents the individually listed worlds (items) in the world selection menu
 */
UCLASS()
class MINECRAFT_API UWorldItemWidget : public UUserWidget
{
	GENERATED_BODY()
	DECLARE_DELEGATE_OneParam(FButtonClicked, UWorldItemWidget*);

public:
	virtual void NativeConstruct() override;

	/*
	 * Set the name of the loaded world
	 */
	UFUNCTION(BlueprintCallable)
	void SetWorldName(const FString Name);

	/*
	 * Handle when player clicks on he world (item) and thus selecting it
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleWorldButtonClick();

	/*
	 * Deselect this world
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DeSelectItem();

	/*
	 * Returns the name of the world
	 */
	UFUNCTION()
	FString GetWorldName();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> WorldName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UButton> WorldButton;

public:
	FButtonClicked OnSelected;
};

UCLASS()
class MINECRAFT_API UWorldSelectionMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/*
	 * Handle when the player clicks on a world in the list
	 */
	UFUNCTION()
	void HandleSelectedWorld(UWorldItemWidget* World);

	/*
	 * Handle when the player clicks on the back button
	 */
	UFUNCTION()
	void HandleBackButtonClick();

	/*
	 * Handle when the player clicks on the delete world button, deleting the selected world
	 */
	UFUNCTION()
	void HandleDeleteWorldButtonClick();

	/*
	 * Handle when the player clicks on the create new world button, opening new menu
	 */
	UFUNCTION()
	void HandleCreateNewWorldButtonClick();

	/*
	 * Handle when the player clicks on the play selected wordl button, opening new level with the selected world
	 */
	UFUNCTION()
	void HandlePlaySelectedWorldButtonClick();

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UScrollBox> ExistingWorlds;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> PlaySelectedWorldButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> DeleteSelectedWorldButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> CreateNewWorldButton;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSubclassOf<UUserWidget> WorldCreationMenuClass;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSubclassOf<UWorldItemWidget> WorldItemClass;

	UPROPERTY(EditDefaultsOnly, Category=Settings)
	TSoftObjectPtr<UWorld> LevelToOpen;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UWorldItemWidget> SelectedWorld;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UUserWidget> WorldCreationMenu;
};
