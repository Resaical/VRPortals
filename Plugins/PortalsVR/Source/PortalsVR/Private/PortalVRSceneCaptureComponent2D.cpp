// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalVRSceneCaptureComponent2D.h"
#include "SceneView.h"
#include "SceneViewExtension.h"
#include "RendererInterface.h"
#include "Engine/TextureRenderTarget2D.h"


void UPortalVRSceneCaptureComponent2D::UpdateSceneCaptureContents(FSceneInterface* Scene)
{
	Scene->UpdateSceneCaptureContents(this);
}