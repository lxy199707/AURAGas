#include "Player/AuraPlayerController.h"   // 包含自定义的玩家控制器头文件
#include "EnhancedInputSubsystems.h"       // 包含增强输入子系统的头文件
#include <EnhancedInputComponent.h>

AAuraPlayerController::AAuraPlayerController()
{
    // 设置此玩家控制器为复制模式
    // 这样该对象会进行网络复制，在多人游戏中允许状态同步
    //bReplicates = true;
}

// BeginPlay 是 Unreal 中的一个生命周期函数，会在玩家控制器开始播放时调用
void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 获取当前本地玩家的 Enhanced Input 子系统
    // Enhanced Input 是 UE5 引入的输入系统，用于更灵活和复杂的输入管理
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

    // 检查 AuraContext 是否有效，确保我们有一个有效的输入上下文
    check(AuraContext);

    // 将 AuraContext 映射上下文添加到输入子系统中，优先级为 0
    Subsystem->AddMappingContext(AuraContext, 0);

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    // 设置输入模式为游戏和 UI 模式
    FInputModeGameAndUI InputModeData;
    // 设定鼠标不会锁定到视口，允许鼠标自由移动
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    // 鼠标捕获时不隐藏光标
    InputModeData.SetHideCursorDuringCapture(false);
    // 将此输入模式应用到玩家控制器
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn* ControlledPawn = GetPawn<APawn>()) 
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);

    }
}
