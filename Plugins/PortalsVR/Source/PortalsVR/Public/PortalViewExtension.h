#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

#define PORTAL_VIEW_EXTENSION_STEREOSCOPIC_IN_CHARGE_GAME_THREAT 0
#define PORTAL_VIEW_EXTENSION_STEREOSCOPIC_IN_CHARGE_RENDER_THREAT 1
#define PORTAL_REVERT_LATE_XRHMD_UPDATE 0



class FPortalViewExtension : public FSceneViewExtensionBase
{
public:
    FPortalViewExtension(const FAutoRegister& AutoRegister);


    virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};

    virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)override;

    virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {};

    virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};

    virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override;

    virtual void PreRenderBasePass_RenderThread(FRDGBuilder& GraphBuilder, bool bDepthBufferIsPopulated) override;


    virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override { return true; };


};
