#pragma once

#define NOMINMAX 0

#include "SharedHeader.h"

#include <Windows.h>
#include <chrono>

#include "GUIConstants.h"
#include "Math.h"
#include "Camera.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Material.h"
#include "PrimitiveGenerator.h"
#include "Terrain.h"
#include "Billboard.h"
#include "Light.h"
#include "CascadedShadowMap.h"
#include "FullScreenQuad.h"
#include "BFNTBaker.h"
#include "BFNTRenderer.h"
#include "DynamicPool.h"
#include "../Model/Object3D.h"
#include "../Model/Object3DLine.h"
#include "../Model/Object2D.h"
#include "../Model/MeshPorter.h"
#include "../Editor/CubemapRep.h"
#include "../Editor/IBLBaker.h"
#include "../Editor/Gizmo3D.h"
#include "../Physics/PhysicsEngine.h"
#include "../AI/Intelligence.h"
#include "../AI/Pattern.h"
#include "../AI/MonsterSpawner.h"

#include "TinyXml2/tinyxml2.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

class CGame
{
public:
	enum class EBaseShader
	{
		VSAnimation,
		VSBase,
		VSBase2D,
		VSFoliage,
		VSLight,
		VSLine,
		VSSky,
		VSTerrain,

		HSPointLight,
		HSSpotLight,
		HSStatic,
		HSTerrain,
		HSWater,

		DSPointLight,
		DSSpotLight,
		DSStatic,
		DSTerrain,
		DSWater,

		GSNormal,

		PSBase,
		PSBase_GBuffer,
		PSBase_RawVertexColor,
		PSBase_RawDiffuseColor,
		PSBase_Void,
		PSBase2D,
		PSBase2D_RawVertexColor,
		PSCamera,
		PSCloud,
		PSDirectionalLight,
		PSDirectionalLight_NonIBL,
		PSDynamicSky,
		PSFoliage,
		PSEdgeDetector,
		PSHeightMap2D,
		PSLine,
		PSMasking2D,
		PSPointLight,
		PSPointLight_Volume,
		PSSky,
		PSSpotLight,
		PSSpotLight_Volume,
		PSTerrain,
		PSTerrain_GBuffer,
		PSWater
	};

	enum class EObjectType
	{
		NONE,

		Object3D,
		Object3DInstance, // Instance of Object3D
		Object3DLine,
		Object2D,
		EditorCamera,
		Camera,
		Light
	};

	struct SCBSpaceData // Update every frame (because Camera/View is seriously dynamic)
	{
		XMMATRIX	World{};
		XMMATRIX	View{};
		XMMATRIX	Projection{};
		XMMATRIX	ViewProjection{};
		XMMATRIX	WVP{};
		XMVECTOR	EyePosition{};
	};

	struct SCBGlobalLightData // Update only when there is a change
	{
		XMVECTOR	DirectionalLightDirection{ XMVectorSet(0, 1, 0, 0) };
		XMFLOAT3	DirectionalLightColor{ 1, 1, 1 };
		float		Exposure{ 1.0 };
		XMFLOAT3	AmbientLightColor{ 1, 1, 1 };
		float		AmbientLightIntensity{ 0.5f };
		uint32_t	IrradianceTextureMipLevels{};
		uint32_t	PrefilteredRadianceTextureMipLevels{};
		XMFLOAT2	Reserved{};
	};

	struct SCBAnimationBonesData // Update every frame (when CPU skinning is used)
	{
		XMMATRIX	BoneMatrices[CObject3D::KMaxBoneMatrixCount]{};
	};

	struct SCBSkyTimeData
	{
		float		SkyTime{};
		float		Pads[3]{};
	};

	struct SCBTerrainMaskingSpaceData
	{
		XMMATRIX	Matrix{};
	};

	struct SCBWaterTimeData
	{
		float		Time{};
		float		Pads[3]{};
	};

	struct SCBEditorTimeData
	{
		float		NormalizedTime{};
		float		NormalizedTimeHalfSpeed{};
		float		Reserved[2]{};
	};

	struct SCBCameraInfoData
	{
		uint32_t	CurrentCameraID{};
		float		Pads[3]{};
	};

	struct SCBGBufferUnpackingData
	{
		XMFLOAT4	PerspectiveValues{};
		XMMATRIX	InverseViewMatrix{};
		XMFLOAT2	ScreenSize{};
		float		Reserved[2]{};
	};

	struct SCBSceneMaterialData
	{
		uint32_t	FlagsHasSceneTexture;
		uint32_t	FlagsIsSceneTextureSRGB;
		float		Reserved[2]{};
	};

	enum class EFlagsRendering
	{
		None = 0x0000,
		DrawWireFrame = 0x0001,
		DrawNormals = 0x0002,
		Use3DGizmos = 0x0004,
		DrawMiniAxes = 0x0008,
		DrawPickingData = 0x0010,
		DrawBoundingVolumes = 0x0020,
		DrawTerrainHeightMapTexture = 0x0040,
		DrawTerrainMaskingTexture = 0x0080,
		DrawTerrainFoliagePlacingTexture = 0x0100,
		DrawGrid = 0x0200,
		TessellateTerrain = 0x0400, // 내부적으로만 사용하는 플래그!
		UseLighting = 0x0800,
		UsePhysicallyBasedRendering = 0x1000,
		DrawLightVolumes = 0x2000,
		DrawDirectionalLightShadowMap = 0x4000,
		DrawIdentifiers = 0x8000,
	};

	enum class ERasterizerState
	{
		CullNone,
		CullClockwise,
		CullCounterClockwise,
		WireFrame
	};

	enum class EMode
	{
		Edit,
		Test,
		Play
	};

	enum class EEditMode
	{
		EditObject,
		EditTerrain
	};

	struct SSkyData
	{
		struct SSkyObjectData
		{
			XMFLOAT2	UVOffset{};
			XMFLOAT2	UVSize{};
			float		WidthHeightRatio{};
		};

		bool			bIsDataSet{ false };
		bool			bIsDynamic{ false };
		std::string		TextureFileName{};
		SSkyObjectData	Sun{};
		SSkyObjectData	Moon{};
		SSkyObjectData	Cloud{};
	};

	struct SEditorGUIBools
	{
		bool bShowWindowPropertyEditor{ true };
		bool bShowWindowSceneEditor{ true };
		bool bShowPopupTerrainGenerator{ false };
		bool bShowPopupObjectAdder{ false };
		bool bShowPopupObjectRenamer{ false };
		bool bShowPopupMaterialNameChanger{ false };
		bool bShowPopupMaterialTextureExplorer{ false };
	};

	struct SObject3DPickingCandiate
	{
		SObject3DPickingCandiate() {};
		SObject3DPickingCandiate(CObject3D* const _PtrObject3D, XMVECTOR _T) : PtrObject3D{ _PtrObject3D }, T{ _T } {}
		SObject3DPickingCandiate(CObject3D* const _PtrObject3D, const std::string& _InstanceName, XMVECTOR _T) :
			PtrObject3D{ _PtrObject3D }, InstanceName{ _InstanceName }, T{ _T } {}

		CObject3D*	PtrObject3D{};
		std::string	InstanceName{};
		XMVECTOR	T{};
		bool		bHasFailedPickingTest{ false };
	};

	// EditorCamera
	// Camera - Name
	// Light - Name, Extra(== CLight::EType)
	// Object3D - Name, PtrObject3D
	// Object3DInstance - Name, PtrObject3D
	// Object3DLine - Name, PtrObject3DLine
	// Object2D - Name, PtrObject2D
	struct SSelectionData
	{
		SSelectionData() {}

		// For deselecting
		SSelectionData(EObjectType _eObjectType) : eObjectType{ _eObjectType } {}

		SSelectionData(EObjectType _eObjectType, const std::string& _Name) :
			eObjectType{ _eObjectType }, Name{ _Name } {}
		SSelectionData(EObjectType _eObjectType, const std::string& _Name, void* _PtrObject) :
			eObjectType{ _eObjectType }, Name{ _Name }, PtrObject{ _PtrObject } {}
		SSelectionData(EObjectType _eObjectType, const std::string& _Name, uint32_t _Extra) :
			eObjectType{ _eObjectType }, Name{ _Name }, Extra{ _Extra } {}

		EObjectType	eObjectType{};
		std::string	Name{};
		void*		PtrObject{};
		uint32_t	Extra{};
	};

	enum class ESelectionMode
	{
		Override,
		Additive,
		Subtractive
	};

	enum class EAxis
	{
		X,
		Y,
		Z
	};

	struct SCopyObject3D
	{
		SCopyObject3D() {}
		SCopyObject3D(const std::string& _Name, const SMESHData& _Model,
			const SComponentTransform& _ComponentTransform, const SComponentPhysics& _ComponentPhysics,
			const SComponentRender& _ComponentRender, const std::vector<SObject3DInstanceCPUData> _vInstanceCPUData) :
			Name{ _Name }, Model{ _Model },
			ComponentTransform{ _ComponentTransform }, ComponentPhysics{ _ComponentPhysics }, ComponentRender{ _ComponentRender },
			vInstanceCPUData{ _vInstanceCPUData } {}

		std::string								Name{};
		SMESHData								Model{};
		SComponentTransform						ComponentTransform{};
		SComponentPhysics						ComponentPhysics{};
		SComponentRender						ComponentRender{};
		std::vector<SObject3DInstanceCPUData>	vInstanceCPUData{};

		size_t									PasteCounter{};
	};

	struct SCopyObject3DInstance
	{
		SCopyObject3DInstance() {}
		SCopyObject3DInstance(const SObject3DInstanceCPUData& _InstanceCPUData, CObject3D* const _PtrObject3D) :
			InstanceCPUData{ _InstanceCPUData }, PtrObject3D{ _PtrObject3D } {}

		SObject3DInstanceCPUData	InstanceCPUData{};
		CObject3D* const			PtrObject3D{};
		size_t						PasteCounter{};
	};

	struct SCopyLight
	{
		SCopyLight() {}
		SCopyLight(const CLight::SInstanceCPUData& _InstanceCPUData, const CLight::SInstanceGPUData& _InstanceGPUData) :
			InstanceCPUData{ _InstanceCPUData }, InstanceGPUData{ _InstanceGPUData } {}

		CLight::SInstanceCPUData	InstanceCPUData{};
		CLight::SInstanceGPUData	InstanceGPUData{};
		size_t						PasteCounter{};
	};

	struct SGeometryBuffers
	{
		// GBuffer #0 - Depth 24 Stencil 8
		ComPtr<ID3D11Texture2D>				DepthStencilBuffer{};
		ComPtr<ID3D11DepthStencilView>		DepthStencilDSV{};
		ComPtr<ID3D11ShaderResourceView>	DepthStencilSRV{};

		// GBuffer #1 - Base color 24 & Roughness 8
		ComPtr<ID3D11Texture2D>				BaseColorRoughBuffer{};
		ComPtr<ID3D11RenderTargetView>		BaseColorRoughRTV{};
		ComPtr<ID3D11ShaderResourceView>	BaseColorRoughSRV{};

		// GBuffer #2 - Normal 32
		ComPtr<ID3D11Texture2D>				NormalBuffer{};
		ComPtr<ID3D11RenderTargetView>		NormalRTV{};
		ComPtr<ID3D11ShaderResourceView>	NormalSRV{};

		// GBuffer #3 - Metalness 8 & AmbientOcclusion 8
		ComPtr<ID3D11Texture2D>				MetalAOBuffer{};
		ComPtr<ID3D11RenderTargetView>		MetalAORTV{};
		ComPtr<ID3D11ShaderResourceView>	MetalAOSRV{};
	};

public:
	CGame(HINSTANCE hInstance, const XMFLOAT2& WindowSize);
	~CGame();

public:
	void CreateWin32(WNDPROC const WndProc, const std::string& WindowName, bool bWindowed, bool bCreateEditor);
	void Destroy();
	bool IsDestroyed() const;

private:
	void CreateWin32Window(WNDPROC const WndProc, const std::string& WindowName);
	void InitializeDirectX(bool bWindowed);
	void InitializeGameData();
	void InitializeEditorAssets(bool bCreateEditor);
	void InitializeImGui(const std::string& FontFileName, float FontSize);

private:
	void _CreateSwapChain(bool bWindowed);
	void _CreateViews();
	void _InitializeViewports();
	void _CreateDepthStencilStates();
	void _CreateBlendStates();
	void _CreateRasterizerStates();
	void _CreateInputDevices();
	void _CreateConstantBuffers();
	void _CreateBaseShaders();
	void _CreateMiniAxes();
	void _CreatePickingRay();
	void _CreatePickedTriangle();

public:
	void SetAssetDirectory(const std::string& Directory);
	void SetSceneDirectory(const std::string& Directory);
	const std::string& GetAssetDirectory() const;
	const std::string& GetSceneDirectory() const;

public:
	void EmptyScene();
	void LoadScene(const std::string& FileName);
	void LoadScene(const std::string& FileName, const std::string& SceneContentDirectory);
	void SaveScene(const std::string& FileName, const std::string& SceneContentDirectory);

// Advanced settings
public:
	void SetProjectionMatrices(float FOV, float NearZ, float FarZ);
	void SetRenderingFlags(EFlagsRendering Flags);
	void ToggleRenderingFlag(EFlagsRendering Flags);
	void TurnOnRenderingFlag(EFlagsRendering Flags);
	void TurnOffRenderingFlag(EFlagsRendering Flags);
	EFlagsRendering GetRenderingFlags() const;
	void SetUniversalRSState();
	CommonStates* GetCommonStates() const { return m_CommonStates.get(); }

private:
	void UpdateCBSpace(const XMMATRIX& World = KMatrixIdentity);
	void UpdateCBSpace(const XMMATRIX& World, const XMMATRIX& Projection);
	void UpdateCBAnimationBoneMatrices(const XMMATRIX* const BoneMatrices);
	void UpdateCBAnimationData(const CObject3D::SCBAnimationData& Data);
	void UpdateCBTerrainData(const CTerrain::SCBTerrainData& Data);
	void UpdateCBWindData(const CTerrain::SCBWindData& Data);

	void UpdateCBTessFactorData(const CObject3D::SCBTessFactorData& Data);
	void UpdateCBDisplacementData(const CObject3D::SCBDisplacementData& Data);

	void UpdateCBTerrainMaskingSpace(const XMMATRIX& Matrix);

	// This function also updates terrain's world matrix by calling UpdateCBSpace() from inside.
	void UpdateCBTerrainSelection(const CTerrain::SCBTerrainSelectionData& Selection);

	void UpdateCBGlobalLightProbeData();

	void UpdateSceneMaterial();

public:
	void CreateDynamicSky(const std::string& SkyDataFileName, float ScalingFactor);
	void CreateStaticSky(float ScalingFactor);

private:
	void LoadSkyObjectData(const tinyxml2::XMLElement* const xmlSkyObject, SSkyData::SSkyObjectData& SkyObjectData);

public:
	void SetDirectionalLight(const XMVECTOR& LightSourcePosition, const XMFLOAT3& Color);
	void SetDirectionalLightDirection(const XMVECTOR& LightSourcePosition);
	void SetDirectionalLightColor(const XMFLOAT3& Color);
	const XMVECTOR& GetDirectionalLightDirection() const;
	const XMFLOAT3& GetDirectionalLightColor() const;

	void SetAmbientlLight(const XMFLOAT3& Color, float Intensity);
	const XMFLOAT3& GetAmbientLightColor() const;
	float GetAmbientLightIntensity() const;

	void SetExposure(float Value);
	float GetExposure();

public:
	void CreateTerrain(const XMFLOAT2& TerrainSize, uint32_t MaskingDetail, float UniformScaling);
	void LoadTerrain(const std::string& TerrainFileName);
	void SaveTerrain(const std::string& TerrainFileName);
	CTerrain* GetTerrain() const { return m_Terrain.get(); }

// Object pool
public:
	void ClearCopyList();
	void CopySelectedObject();
	void PasteCopiedObject();
	void DeleteSelectedObjects();

public:
	bool InsertCamera(const std::string& Name, CCamera::EType eType);
	void DeleteCamera(const std::string& Name);
	void ClearCameras();
	CCamera* GetCamera(const std::string& Name, bool bShowWarning = true) const;
	const std::map<std::string, size_t>& GetCameraMap() const { return m_mapCameraNameToIndex; }

private:
	size_t GetCameraID(const std::string Name) const;

public:
	CShader* AddCustomShader();
	CShader* GetCustomShader(size_t Index) const;
	CShader* GetBaseShader(EBaseShader eShader) const;

	bool IsObject3DNameInsertable(const std::string& Name, bool bShowWarning = true);
	bool InsertObject3D(const std::string& Name);
	bool ChangeObject3DName(const std::string& OldName, const std::string& NewName);
	void DeleteObject3D(const std::string& Name);
	void ClearObject3Ds();
	CObject3D* GetObject3D(const std::string& Name, bool bShowWarning = true) const;
	const std::map<std::string, size_t>& GetObject3DMap() const { return m_mapObject3DNameToIndex; }

	void DeleteObject3DInstance(CObject3D* const Object3D, const std::string& Name);
	void ClearObject3DInstances(CObject3D* const Object3D);

	bool InsertObject3DLine(const std::string& Name, bool bShowWarning = true);
	void ClearObject3DLines();
	CObject3DLine* GetObject3DLine(const std::string& Name, bool bShowWarning = true) const;

	bool InsertObject2D(const std::string& Name);
	void DeleteObject2D(const std::string& Name);
	void ClearObject2Ds();
	CObject2D* GetObject2D(const std::string& Name, bool bShowWarning = true) const;
	const std::map<std::string, size_t>& GetObject2DMap() const { return m_mapObject2DNameToIndex; }

public:
	void WalkPlayerToPickedPoint(float WalkSpeed);
	void JumpPlayer(float JumpSpeed);

public:
	bool InsertLight(CLight::EType eType, const std::string& Name);
	void DeleteLight(CLight::EType eType, const std::string& Name);
	void ClearLights();

public:
	bool InsertPattern(const std::string& FileName);
	void DeletePattern(const std::string& FileName);
	void ClearPatterns();
	CPattern* GetPattern(const std::string& FileName);

public:
	bool InsertBFNTRenderer(const std::string& BFNTRendererName, const std::string& BFNT_FileName);
	void DeleteBFNTRenderer(const std::string& BFNTRendererName);
	void ClearBFNTRenderers();
	CBFNTRenderer* GetBFNTRenderer(const std::string& BFNTRendererName);

public:
	bool InsertMonsterSpawner(const std::string& Name, const SMonsterSpawnerData& Data);
	void DeleteMonsterSpawner(const std::string& Name);
	void ClearMonsterSpanwers();

public:
	bool SetMode(EMode eMode);
	EMode GetMode() const { return m_eMode; }

	void SetEditMode(EEditMode eEditMode, bool bForcedSet = false);
	EEditMode GetEditMode() const { return m_eEditMode; }

public:
	void NotifyMouseLeftDown();
	void NotifyMouseLeftUp();

private:
	void Select();
	void SelectMultipleObjects(ESelectionMode eSelectionMode);
	void SelectObject(const SSelectionData& SelectionData, ESelectionMode eSelectionMode = ESelectionMode::Override);
	void DeselectObject(const SSelectionData& SelectionData);
	void DeselectType(EObjectType eObjectType);
	void DeselectAll();
	std::string GetSelectionName(const SSelectionData& SelectionData) const;
	bool IsAnythingSelected() const;
	void TranslateSelectionTo(EAxis eAxis, float Prime);

private:
	void CastPickingRay();
	void PickBoundingSphere(ESelectionMode eSelectionMode);
	bool PickObject3DTriangle(ESelectionMode eSelectionMode);

private:
	CCamera* GetCurrentCamera();
	void UseEditorCamera();
	void UseCamera(CCamera* const Camera);
	void SetPlayerCamera(CCamera* const Camera);
	CCamera* GetPlayerCamera() const;
	bool IsEditorCamera(CCamera* const Camera) const;
	bool IsCurrentCamera(CCamera* const Camera) const;
	bool IsPlayerCamera(CCamera* const Camera) const;

public:
	void Capture3DGizmoTranslation();
	void Update3DGizmos();

private:
	void SelectTerrain(bool bShouldEdit, bool bIsLeftButton);

private:
	bool IsInsideSelectionRegion(const XMFLOAT2& ProjectionSpacePosition);

public:
	void BeginRendering(const FLOAT* ClearColor);
	void Update();
	void Draw();
	void EndRendering();

private:
	void SetForwardRenderTargets(bool bClearViews = false);
	void SetDeferredRenderTargets(bool bClearViews = false);

public:
	auto GethWnd() const->HWND;
	auto GetDevicePtr() const->ID3D11Device*;
	auto GetDeviceContextPtr() const->ID3D11DeviceContext*;
	auto GetKeyState() const->Keyboard::State;
	auto GetMouseState() const->Mouse::State;
	auto GetWindowSize() const->const XMFLOAT2&;
	auto GetDepthStencilStateLessEqualNoWrite() const->ID3D11DepthStencilState*;
	auto GetBlendStateAlphaToCoverage() const->ID3D11BlendState*;
	auto GetDeltaTime() const->float;

private:
	void DrawOpaqueObject3Ds(bool bIgnoreOwnTexture = false, bool bUseVoidPS = false);
	void DrawObject3D(CObject3D* const PtrObject3D,
		EFlagsObject3DRendering eFlagsRendering = EFlagsObject3DRendering::None, size_t OneInstanceIndex = 0);
	void DrawBoundingSphereRep(const XMVECTOR& Center, float Radius);
	void DrawAxisAlignedBoundingBoxRep(const XMVECTOR& Center, float HalfSizeX, float HalfSizeY, float HalfSizeZ);

	void DrawObject3DLines();

	void DrawObject2Ds();

	void DrawMiniAxes();

	void DrawPickingRay();
	void DrawPickedTriangle();

	void DrawGrid();

	void DrawShadowMapReps();

	void DrawSky(float DeltaTime);
	void DrawTerrainOpaqueParts(float DeltaTime);
	void DrawTerrainTransparentParts();

	void Draw3DGizmos();
	void DrawCameraRep();
	void DrawLightRep();
	void DrawMultipleSelectionRep();

	void DrawIdentifiers();

	void DrawEditorGUI();
	void DrawEditorGUIMenuBar();
	void DrawEditorGUIPopupTerrainGenerator();
	void DrawEditorGUIPopupObjectAdder();
	void DrawEditorGUIWindowPropertyEditor();

	// return true if any interaction is required
	bool DrawEditorGUIWindowPropertyEditor_MaterialData(CMaterialData& MaterialData, CMaterialTextureSet* const TextureSet,
		ETextureType& eSeletedTextureType, float ItemsOffsetX, bool bShowOnlyTextureData = false);
	void DrawEditorGUIPopupMaterialNameChanger(CMaterialData*& capturedMaterialData);
	bool DrawEditorGUIPopupMaterialTextureExplorer(CMaterialData* const capturedMaterialData, CMaterialTextureSet* const capturedMaterialTextureSet,
		ETextureType eSelectedTextureType);
	void DrawEditorGUIWindowSceneEditor();

private:
	void GenerateEnvironmentCubemapFromHDRi();
	void GenerateIrradianceMap(float RangeFactor);
	void GeneratePrefilteredRadianceMap(float RangeFactor);
	void GenerateIntegratedBRDFMap();

public:
	XMVECTOR GetObject3DNDCPosition(const SObjectIdentifier& Identifier) const;
	XMFLOAT2 GetScreenPixelPositionFromNDCPosition(const XMVECTOR& NDCPosition) const;
	const CPhysicsEngine* GetPhysicsEngine() const;

public:
	static constexpr float KTranslationMinLimit{ -1000.0f };
	static constexpr float KTranslationMaxLimit{ +1000.0f };
	static constexpr float KTranslationDelta{ CGizmo3D::KMovementFactorBase };
	static constexpr float KRotationDelta{ 22.5f };
	static constexpr int KRotation360MaxLimit{ 360 };
	static constexpr int KRotation360MinLimit{ 360 };
	static constexpr int KRotation360Unit{ 1 };
	static constexpr float KRotation360To2PI{ 1.0f / 360.0f * XM_2PI };
	static constexpr float KRotation2PITo360{ 1.0f / XM_2PI * 360.0f };
	static constexpr float KScalingDelta{ CGizmo3D::KMovementFactorBase };
	static constexpr float KBSCenterOffsetMinLimit{ -10.0f };
	static constexpr float KBSCenterOffsetMaxLimit{ +10.0f };
	static constexpr float KBSCenterOffsetDelta{ +0.01f };
	static constexpr float KBSRadiusMinLimit{ 0.001f };
	static constexpr float KBSRadiusMaxLimit{ 10.0f };
	static constexpr float KBSRadiusDelta{ +0.01f };
	static constexpr float KBSRadiusBiasMinLimit{ 0.001f };
	static constexpr float KBSRadiusBiasMaxLimit{ 1000.0f };
	static constexpr float KBSRadiusBiasDelta{ +0.01f };
	static constexpr float KCameraRepSelectionRadius{ 0.25f };
	static constexpr int KAssetNameMaxLength{ 100 };

private:
	static constexpr float KDefaultFOV{ 50.0f / 360.0f * XM_2PI };
	static constexpr float KDefaultNearZ{ 0.1f };
	static constexpr float KDefaultFarZ{ 1000.0f };
	static constexpr float KSkyDistance{ 30.0f };
	static constexpr float KSkyTimeFactorAbsolute{ 0.04f };
	static constexpr float KPickingRayLength{ 1000.0f };
	static constexpr uint32_t KSkySphereSegmentCount{ 32 };
	static constexpr XMVECTOR KColorWhite{ 1.0f, 1.0f, 1.0f, 1.0f };
	static constexpr XMVECTOR KSkySphereColorUp{ 0.1f, 0.5f, 1.0f, 1.0f };
	static constexpr XMVECTOR KSkySphereColorBottom{ 1.2f, 1.2f, 1.2f, 1.0f };
	static constexpr int KEnvironmentTextureSlot{ 50 };
	static constexpr int KIrradianceTextureSlot{ 51 };
	static constexpr int KPrefilteredRadianceTextureSlot{ 52 };
	static constexpr int KIntegratedBRDFTextureSlot{ 53 };
	static constexpr int KEditorCameraID{ -999 };
	static constexpr float KEditorCameraDefaultMovementFactor{ 3.0f };
	static constexpr size_t KInvalidIndex{ SIZE_T_MAX };

	static constexpr char KTextureDialogFilter[45]{ "JPG 파일\0*.jpg\0PNG 파일\0*.png\0모든 파일\0*.*\0" };
	static constexpr char KTextureDialogTitle[16]{ "텍스쳐 불러오기" };

// Shader
private:
	std::vector<std::unique_ptr<CShader>>	m_vCustomShaders{};

	std::unique_ptr<CShader>				m_VSAnimation{};
	std::unique_ptr<CShader>				m_VSBase{};
	std::unique_ptr<CShader>				m_VSBase_Instanced{};
	std::unique_ptr<CShader>				m_VSBase2D{};
	std::unique_ptr<CShader>				m_VSFoliage{};
	std::unique_ptr<CShader>				m_VSLight{};
	std::unique_ptr<CShader>				m_VSLine{};
	std::unique_ptr<CShader>				m_VSSky{};
	std::unique_ptr<CShader>				m_VSTerrain{};

	std::unique_ptr<CShader>				m_HSPointLight{};
	std::unique_ptr<CShader>				m_HSSpotLight{};
	std::unique_ptr<CShader>				m_HSStatic{};
	std::unique_ptr<CShader>				m_HSTerrain{};
	std::unique_ptr<CShader>				m_HSWater{};

	std::unique_ptr<CShader>				m_DSPointLight{};
	std::unique_ptr<CShader>				m_DSSpotLight{};
	std::unique_ptr<CShader>				m_DSStatic{};
	std::unique_ptr<CShader>				m_DSTerrain{};
	std::unique_ptr<CShader>				m_DSWater{};

	std::unique_ptr<CShader>				m_GSNormal{};

	std::unique_ptr<CShader>				m_PSBase{};
	std::unique_ptr<CShader>				m_PSBase_GBuffer{};
	std::unique_ptr<CShader>				m_PSBase_Void{};
	std::unique_ptr<CShader>				m_PSBase_RawVertexColor{};
	std::unique_ptr<CShader>				m_PSBase_RawDiffuseColor{};
	std::unique_ptr<CShader>				m_PSBase2D{};
	std::unique_ptr<CShader>				m_PSBase2D_RawVertexColor{};
	std::unique_ptr<CShader>				m_PSCamera{};
	std::unique_ptr<CShader>				m_PSCloud{};
	std::unique_ptr<CShader>				m_PSDirectionalLight{};
	std::unique_ptr<CShader>				m_PSDirectionalLight_NonIBL{};
	std::unique_ptr<CShader>				m_PSDynamicSky{};
	std::unique_ptr<CShader>				m_PSEdgeDetector{};
	std::unique_ptr<CShader>				m_PSFoliage{};
	std::unique_ptr<CShader>				m_PSHeightMap2D{};
	std::unique_ptr<CShader>				m_PSLine{};
	std::unique_ptr<CShader>				m_PSMasking2D{};
	std::unique_ptr<CShader>				m_PSPointLight{};
	std::unique_ptr<CShader>				m_PSPointLight_Volume{};
	std::unique_ptr<CShader>				m_PSSky{};
	std::unique_ptr<CShader>				m_PSSpotLight{};
	std::unique_ptr<CShader>				m_PSSpotLight_Volume{};
	std::unique_ptr<CShader>				m_PSTerrain{};
	std::unique_ptr<CShader>				m_PSTerrain_gbuffer{};
	std::unique_ptr<CShader>				m_PSWater{};

// Constant buffer
private:
	std::unique_ptr<CConstantBuffer>		m_CBSpace{};
	std::unique_ptr<CConstantBuffer>		m_CBAnimationBones{};
	std::unique_ptr<CConstantBuffer>		m_CBAnimation{};
	std::unique_ptr<CConstantBuffer>		m_CBTerrain{};
	std::unique_ptr<CConstantBuffer>		m_CBWind{};
	std::unique_ptr<CConstantBuffer>		m_CBTessFactor{};
	std::unique_ptr<CConstantBuffer>		m_CBDisplacement{};
	std::unique_ptr<CConstantBuffer>		m_CBGlobalLight{};
	std::unique_ptr<CConstantBuffer>		m_CBTerrainMaskingSpace{};
	std::unique_ptr<CConstantBuffer>		m_CBTerrainSelection{};
	std::unique_ptr<CConstantBuffer>		m_CBSkyTime{};
	std::unique_ptr<CConstantBuffer>		m_CBWaterTime{};
	std::unique_ptr<CConstantBuffer>		m_CBEditorTime{};
	std::unique_ptr<CConstantBuffer>		m_CBCameraInfo{};
	std::unique_ptr<CConstantBuffer>		m_CBGBufferUnpacking{};
	std::unique_ptr<CConstantBuffer>		m_CBShadowMap{};
	std::unique_ptr<CConstantBuffer>		m_CBSceneMaterial{};

	SCBSpaceData							m_CBSpaceData{};
	SCBAnimationBonesData					m_CBAnimationBonesData{};
	CObject3D::SCBAnimationData				m_CBAnimationData{};
	CTerrain::SCBTerrainData				m_CBTerrainData{};
	CTerrain::SCBWindData					m_CBWindData{};
	CObject3D::SCBTessFactorData			m_CBTessFactorData{};
	CObject3D::SCBDisplacementData			m_CBDisplacementData{};
	SCBGlobalLightData						m_CBGlobalLightData{};
	SCBTerrainMaskingSpaceData				m_CBTerrainMaskingSpaceData{};
	CTerrain::SCBTerrainSelectionData		m_CBTerrainSelectionData{};
	SCBSkyTimeData							m_CBSkyTimeData{};
	SCBWaterTimeData						m_CBWaterTimeData{};
	SCBEditorTimeData						m_CBEditorTimeData{};
	SCBCameraInfoData						m_CBCameraInfoData{};
	SCBGBufferUnpackingData					m_CBGBufferUnpackingData{};
	CCascadedShadowMap::SCBShadowMapData	m_CBShadowMapData{};
	SCBSceneMaterialData					m_CBSceneMaterialData{};

// Object pool
private:
	std::vector<std::unique_ptr<CObject3D>>		m_vObject3Ds{};
	size_t										m_Object3DTotalInstanceCount{};
	size_t										m_Object3DCulledInstanceCount{};

	std::vector<std::unique_ptr<CObject3DLine>>	m_vObject3DLines{};
	std::vector<std::unique_ptr<CObject2D>>		m_vObject2Ds{};

	std::vector<std::unique_ptr<CObject3D>>		m_vMiniAxes{};
	std::unique_ptr<CObject3D>					m_BoundingSphereRep{};
	std::unique_ptr<CObject3D>					m_AxisAlignedBoundingBoxRep{};

	std::unique_ptr<CObject3DLine>				m_Grid{};

	std::map<std::string, size_t>				m_mapObject3DNameToIndex{};
	std::map<std::string, size_t>				m_mapObject3DLineNameToIndex{};
	std::map<std::string, size_t>				m_mapObject2DNameToIndex{};
	std::map<std::string, size_t>				m_mapCameraNameToIndex{};
	size_t										m_PrimitiveCreationCounter{};

private:
	CPhysicsEngine							m_PhysicsEngine{};
	std::unique_ptr<CObject3D>				m_AClosestPointRep{};
	std::unique_ptr<CObject3D>				m_BClosestPointRep{};
	std::unique_ptr<CObject3D>				m_PickedPointRep{};

private:
	std::unique_ptr<CIntelligence>					m_Intelligence{};

private:
	std::vector<std::unique_ptr<CMonsterSpawner>>	m_vMonsterSpawners{};
	std::map<std::string, size_t>					m_mapMonsterSpawnerNameToIndex{};

// Shadow map
private:
	std::unique_ptr<CCascadedShadowMap>		m_CascadedShadowMap{};

// Light
private:
	std::unique_ptr<CLight>					m_LightArray[2]{}; // 0 for PointLight, 1 for SpotLight
	std::unique_ptr<CBillboard>				m_LightRep{};
	size_t									m_LightCreationCounter{};
	std::unordered_map<std::string, size_t>	m_umapLightNames{};

// Sky
private:
	std::string								m_SkyFileName{};
	float									m_SkyScalingFactor{};
	SSkyData								m_SkyData{};
	CMaterialData							m_SkyMaterialData{};
	std::unique_ptr<CObject3D>				m_Object3DSkySphere{};
	std::unique_ptr<CObject3D>				m_Object3DSun{};
	std::unique_ptr<CObject3D>				m_Object3DMoon{};
	std::unique_ptr<CObject3D>				m_Object3DCloud{};

// 3D Gizmo
private:
	std::unique_ptr<CGizmo3D>				m_Gizmo3D{};

// Camera
private:
	std::unique_ptr<CCamera>				m_EditorCamera{};
	std::vector<std::unique_ptr<CCamera>>	m_vCameras{};
	CCamera*								m_PtrCurrentCamera{};
	int										m_CurrentCameraID{ -1 };
	CCamera*								m_PtrPlayerCamera{};
	std::unique_ptr<CObject3D>				m_CameraRep{};


// BFNT renderer
private:
	CDynamicPool<CBFNTRenderer>					m_BFNTRendererPool{};
	std::unique_ptr<CBFNTRenderer>				m_BFNT_Identifiers{};
	//std::vector<std::unique_ptr<CBFNTRenderer>>	m_vBFNTRenderers_Identifier{};

// Scene testing
private:
	CCamera*								m_SavedCurrentCamera{};
	SComponentTransform						m_SavedPlayerTransform{};
	SComponentPhysics						m_SavedPlayerPhysics{};
	EFlagsRendering							m_SavedRenderingFlags{};

// Picking
private:
	std::unique_ptr<CObject3DLine>			m_PickingRayRep{};
	std::unique_ptr<CObject3D>				m_PickedTriangleRep{};
	XMVECTOR								m_PickingRayWorldSpaceOrigin{};
	XMVECTOR								m_PickingRayWorldSpaceDirection{};
	std::vector<SObject3DPickingCandiate>	m_vObject3DPickingCandidates{};
	XMVECTOR								m_PickedTriangleV0{};
	XMVECTOR								m_PickedTriangleV1{};
	XMVECTOR								m_PickedTriangleV2{};
	XMVECTOR								m_PickedPoint{};

	// Multiple selection
	bool									m_bIsMultipleSelectionChanging{ false };
	XMFLOAT2								m_MultipleSelectionTopLeft{};
	XMFLOAT2								m_MultipleSelectionBottomRight{};
	XMFLOAT2								m_MultipleSelectionXYNormalized{};
	XMFLOAT2								m_MultipleSelectionXYPrimeNormalized{};
	XMFLOAT2								m_MultipleSelectionSizeNormalized{};
	XMVECTOR								m_MultipleSelectionWorldCenter{};
	std::unique_ptr<CObject2D>				m_MultipleSelectionRep{};

	std::vector<SSelectionData>				m_vSelectionData{};
	// @important: below are for faster search & display in Editor GUI!
	// <Key | Index in m_vSelectionData>
	size_t									m_EditorCameraSelectionIndex{ KInvalidIndex };
	std::unordered_map<std::string, size_t>	m_umapSelectionObject3D{}; // @important: Instance [Object3DName + InstanceName]
	std::unordered_map<std::string, size_t>	m_umapSelectionObject3DLine{};
	std::unordered_map<std::string, size_t>	m_umapSelectionObject2D{};
	std::unordered_map<std::string, size_t>	m_umapSelectionCamera{};
	std::unordered_map<std::string, size_t>	m_umapSelectionLight{};

// Copy & paste
private:
	std::vector<SCopyObject3D>				m_vCopyObject3Ds{};
	std::vector<SCopyObject3DInstance>		m_vCopyObject3DInstances{};
	std::vector<SCopyLight>					m_vCopyLights{};
	
// Terrain
private:
	std::unique_ptr<CTerrain>				m_Terrain{};
	std::unique_ptr<CMaterialData>			m_TerrainMaterialDefault{};

// Time
private:
	std::chrono::steady_clock				m_Clock{};
	long long								m_TimeNow_ms{};
	long long								m_TimePrev_ms{};
	long long								m_Timer_Frame_ms{};
	long long								m_Timer_Test_ms{};
	long long								m_FPS{};
	long long								m_FrameCounter{};
	float									m_DeltaTime_s{};
	float									m_Test_DeltaTime_s{ 0.02f };
	float									m_Test_SlowFactor{ 1.0f };
	bool									m_bIsTestTimerPaused{ false };
	bool									m_bShouldAdvanceTestTimer{ false };

// Editor
private:
	ERasterizerState						m_eRasterizerState{ ERasterizerState::CullCounterClockwise };
	EFlagsRendering							m_eFlagsRendering{};
	bool									m_bIsDeferredRenderTargetsSet{ false };
	bool									m_bIsDestroyed{ false };
	CMeshPorter								m_MeshPorter{};
	ImFont*									m_EditorGUIFont{};
	SEditorGUIBools							m_EditorGUIBools{};
	EMode									m_eMode{};
	EEditMode								m_eEditMode{};
	EGUILanguageID							m_eLanguage{ EGUILanguageID::English };

// Scene
private:
	std::unique_ptr<CMaterialData>			m_SceneMaterial{};
	std::unique_ptr<CMaterialTextureSet>	m_SceneMaterialTextureSet{};
	std::vector<std::unique_ptr<CPattern>>	m_vPatterns{};
	std::unordered_map<std::string, size_t> m_umapPatternFileNameToIndex{};

// IBL
private:
	std::unique_ptr<CTexture>				m_EnvironmentTexture{};
	std::unique_ptr<CTexture>				m_IrradianceTexture{};
	std::unique_ptr<CTexture>				m_PrefilteredRadianceTexture{};
	std::unique_ptr<CTexture>				m_IntegratedBRDFTexture{};

	std::unique_ptr<CCubemapRep>			m_EnvironmentCubemapRep{};
	std::unique_ptr<CCubemapRep>			m_IrradianceCubemapRep{};
	std::unique_ptr<CCubemapRep>			m_PrefilteredRadianceCubemapRep{};

	std::unique_ptr<CIBLBaker>				m_IBLBaker{};

private:
	std::unique_ptr<CFullScreenQuad>		m_EdgeDetectorFSQ{};
	std::unique_ptr<CFullScreenQuad>		m_DirectionalLightFSQ{};

// DirectX
private:
	ComPtr<IDXGISwapChain>					m_SwapChain{};
	ComPtr<ID3D11Device>					m_Device{};
	ComPtr<ID3D11DeviceContext>				m_DeviceContext{};

	ComPtr<ID3D11Texture2D>					m_BackBuffer{};
	ComPtr<ID3D11RenderTargetView>			m_BackBufferRTV{};

	ComPtr<ID3D11Texture2D>					m_EdgeDetectorTexture{};
	ComPtr<ID3D11RenderTargetView>			m_EdgeDetectorRTV{};
	ComPtr<ID3D11ShaderResourceView>		m_EdgeDetectorSRV{};

	SGeometryBuffers						m_GBuffers{};

	ComPtr<ID3D11DepthStencilState>			m_DepthStencilStateLessEqualNoWrite{};
	ComPtr<ID3D11DepthStencilState>			m_DepthStencilStateGreaterEqual{};
	ComPtr<ID3D11DepthStencilState>			m_DepthStencilStateAlways{};

	ComPtr<ID3D11BlendState>				m_BlendAlphaToCoverage{};
	ComPtr<ID3D11BlendState>				m_BlendAdditiveLighting{};

	ComPtr<ID3D11RasterizerState>			m_RSCCWCullFront{};

	std::unique_ptr<CommonStates>			m_CommonStates{};

	std::vector<D3D11_VIEWPORT>				m_vViewports{};
	XMMATRIX								m_MatrixProjection{};
	XMMATRIX								m_MatrixProjection2D{};
	float									m_NearZ{};
	float									m_FarZ{};
	XMMATRIX								m_MatrixView{};

// Input
private:
	std::unique_ptr<Keyboard>				m_Keyboard{};
	std::unique_ptr<Mouse>					m_Mouse{};
	Keyboard::State							m_CapturedKeyboardState{};
	Mouse::State							m_CapturedMouseState{};
	bool									m_bLeftButtonPressedOnce{ false };
	bool									m_bLeftButtonUpOnce{ false };
	int										m_PrevCapturedMouseX{};
	int										m_PrevCapturedMouseY{};

// Windows
private:
	HWND									m_hWnd{};
	HINSTANCE								m_hInstance{};
	XMFLOAT2								m_WindowSize{};
	std::string								m_AssetDirectory{ "Asset\\" };
	std::string								m_SceneDirectory{ "Scene\\" };
};

ENUM_CLASS_FLAG(CGame::EFlagsRendering)