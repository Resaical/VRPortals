#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

class FPortalViewExtension : public FSceneViewExtensionBase
{
public:
    FPortalViewExtension(const FAutoRegister& AutoRegister);


    virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};

    virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)override;

    virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {};

    virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};

    virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override {};

    virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override { return true; };
};
