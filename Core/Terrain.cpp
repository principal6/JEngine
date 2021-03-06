#include "Terrain.h"
#include "Game.h"
#include "Material.h"
#include "Math.h"
#include "PrimitiveGenerator.h"
#include "../Model/MeshPorter.h"
#include "../Model/Object2D.h"

using std::max;
using std::min;
using std::vector;
using std::string;
using std::to_string;
using std::make_unique;

CTerrain::CTerrain(ID3D11Device* const PtrDevice, ID3D11DeviceContext* const PtrDeviceContext, CGame* const PtrGame) :
	m_PtrDevice{ PtrDevice }, m_PtrDeviceContext{ PtrDeviceContext }, m_PtrGame{ PtrGame }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);
}

CTerrain::~CTerrain()
{
}

void CTerrain::Create(const XMFLOAT2& TerrainSize, const CMaterialData& MaterialData, uint32_t MaskingDetail, float UniformScaling)
{
	m_vFoliages.clear();
	
	m_TerrainFileData = make_unique<STERRData>(KTessFactorMin, KTessFactorMin, KMaskingDefaultDetail, KDefaultFoliagePlacingDetail);
	m_TerrainFileData->SizeX = TerrainSize.x;
	m_TerrainFileData->SizeZ = TerrainSize.y;
	m_TerrainFileData->HeightRange = KHeightRange;
	m_TerrainFileData->MaskingDetail = max(min(MaskingDetail, KMaskingMaxDetail), KMaskingMinDetail);
	m_TerrainFileData->UniformScalingFactor = UniformScaling;

	m_CBTerrainData.TerrainSizeX = m_TerrainFileData->SizeX;
	m_CBTerrainData.TerrainSizeZ = m_TerrainFileData->SizeZ;
	m_CBTerrainData.TerrainHeightRange = m_TerrainFileData->HeightRange;

	vector<CMaterialData> vMaterialData{};
	vMaterialData.emplace_back(MaterialData);
	CreateTerrainObject3D(vMaterialData);

	m_Object2DTextureRep = make_unique<CObject2D>("TextureRepresentation", m_PtrDevice, m_PtrDeviceContext);
	m_Object2DTextureRep->Create(Generate2DRectangle(XMFLOAT2(600, 480)), true);

	CreateHeightMapTexture(true);
	CreateMaskingTexture(true);

	CreateWater();

	Scale(XMVectorSet(m_TerrainFileData->UniformScalingFactor, m_TerrainFileData->UniformScalingFactor, m_TerrainFileData->UniformScalingFactor, 0));

	m_TerrainFileData->bShouldSave = true;
}

void CTerrain::Load(const string& FileName)
{
	m_vFoliages.clear();

	m_TerrainFileData = make_unique<STERRData>(KTessFactorMin, KTessFactorMin, KMaskingDefaultDetail, KDefaultFoliagePlacingDetail);
	m_TerrainFileData->FileName = FileName;

	CMeshPorter MeshPorter{};
	MeshPorter.ImportTerrain(FileName, *m_TerrainFileData);

	m_CBTerrainData.TerrainSizeX = m_TerrainFileData->SizeX;
	m_CBTerrainData.TerrainSizeZ = m_TerrainFileData->SizeZ;
	m_CBTerrainData.TerrainHeightRange = m_TerrainFileData->HeightRange;

	CreateTerrainObject3D(m_TerrainFileData->vMaterialData);

	m_Object2DTextureRep = make_unique<CObject2D>("TextureRepresentation", m_PtrDevice, m_PtrDeviceContext);
	m_Object2DTextureRep->Create(Generate2DRectangle(XMFLOAT2(600, 480)), true);

	CreateHeightMapTexture(false);
	CreateMaskingTexture(false);

	CreateWater();

	Scale(XMVectorSet(m_TerrainFileData->UniformScalingFactor, m_TerrainFileData->UniformScalingFactor, m_TerrainFileData->UniformScalingFactor, 0));

	if (m_TerrainFileData->vFoliageData.size())
	{
		CreateFoliageCluster();
	}
}

bool CTerrain::Save(const string& FileName)
{
	if (!m_Object3DTerrain) return false;

	m_TerrainFileData->FileName = FileName;
	m_TerrainFileData->vMaterialData = m_Object3DTerrain->GetModel().vMaterialData;

	CMeshPorter MeshPorter{};
	for (size_t iFoliage = 0; iFoliage < m_vFoliages.size(); ++iFoliage)
	{
		m_TerrainFileData->vFoliageData[iFoliage].vInstanceData = m_vFoliages[iFoliage]->GetInstanceCPUDataVector();
	}
	
	MeshPorter.ExportTerrain(FileName, *m_TerrainFileData);

	m_TerrainFileData->bShouldSave = false;

	return true;
}

void CTerrain::Scale(const XMVECTOR& Scaling)
{
	if (m_Object3DTerrain) 
	{
		m_Object3DTerrain->ScaleTo(Scaling);
		m_Object3DTerrain->UpdateWorldMatrix();
	}

	if (m_Object3DWater)
	{
		m_Object3DWater->ScaleTo(Scaling);
		m_Object3DWater->UpdateWorldMatrix();
	}
}

void CTerrain::RegisterChange()
{
	m_TerrainFileData->bShouldSave = true;
}

bool CTerrain::ShouldSave() const
{
	return m_TerrainFileData->bShouldSave;
}

void CTerrain::CreateFoliageCluster()
{
	srand((unsigned int)GetTickCount64());

	CreateFoliagePlacingTexutre(true);

	m_vFoliages.clear();
	for (const auto& FoliageData : m_TerrainFileData->vFoliageData)
	{
		m_vFoliages.emplace_back(make_unique<CObject3D>("Foliage", m_PtrDevice, m_PtrDeviceContext));
		m_vFoliages.back()->CreateFromFile(FoliageData.FileName, false);
		m_vFoliages.back()->CreateInstances(FoliageData.vInstanceData);
	}

	m_TerrainFileData->bHasFoliageCluster = true;

	CreateWindRepresentation();

	RegisterChange();
}

void CTerrain::CreateFoliageCluster(const std::vector<std::string>& vFoliageFileNames, uint32_t PlacingDetail)
{
	srand((unsigned int)GetTickCount64());

	m_TerrainFileData->vFoliageData.clear();
	m_TerrainFileData->vFoliageData.resize(vFoliageFileNames.size());
	for (size_t iFoliageData = 0; iFoliageData < m_TerrainFileData->vFoliageData.size(); ++iFoliageData)
	{
		m_TerrainFileData->vFoliageData[iFoliageData].FileName = vFoliageFileNames[iFoliageData];
	}

	m_TerrainFileData->FoliagePlacingDetail = PlacingDetail;

	CreateFoliagePlacingTexutre(true);

	m_vFoliages.clear();
	for (const auto& FoliageData : m_TerrainFileData->vFoliageData)
	{
		m_vFoliages.emplace_back(make_unique<CObject3D>("Foliage", m_PtrDevice, m_PtrDeviceContext));
		m_vFoliages.back()->CreateFromFile(FoliageData.FileName, false);
		m_vFoliages.back()->CreateInstances(FoliageData.vInstanceData);
	}

	m_TerrainFileData->bHasFoliageCluster = true;

	CreateWindRepresentation();

	RegisterChange();
}

void CTerrain::SetFoliageDensity(float Density)
{
	m_TerrainFileData->FoliageDenstiy = Density;
}

float CTerrain::GetFoliageDenstiy() const
{
	return m_TerrainFileData->FoliageDenstiy;
}

void CTerrain::SetWindVelocity(const XMFLOAT3& Velocity)
{
	XMVECTOR xmvVelocity{ XMLoadFloat3(&Velocity) };
	m_CBWindData.Velocity = xmvVelocity;
}

void CTerrain::SetWindVelocity(const XMVECTOR& Velocity)
{
	m_CBWindData.Velocity = Velocity;
}

const CTerrain::SCBWindData& CTerrain::GetWindData() const
{
	return m_CBWindData;
}

const XMVECTOR& CTerrain::GetWindVelocity() const
{
	return m_CBWindData.Velocity;
}

void CTerrain::SetWindRadius(float Radius)
{
	Radius = max(Radius, KMinWindRadius);
	m_CBWindData.Radius = Radius;
}

float CTerrain::GetWindRadius() const
{
	return m_CBWindData.Radius;
}

void CTerrain::CreateTerrainObject3D(const std::vector<CMaterialData>& vMaterialData)
{
	SMESHData Model{};

	Model.vMeshes.clear();
	Model.vMeshes.emplace_back(GenerateTerrainBase(XMFLOAT2(m_TerrainFileData->SizeX, m_TerrainFileData->SizeZ), KTextureSubdivisionDetail)); // @important
	Model.vMaterialData = vMaterialData;
	Model.bUseMultipleTexturesInSingleMesh = true; // @important

	m_Object3DTerrain = make_unique<CObject3D>("Terrain", m_PtrDevice, m_PtrDeviceContext);
	m_Object3DTerrain->Create(Model);
	m_Object3DTerrain->ShouldTessellate(true); // @important
}

void CTerrain::CreateHeightMapTexture(bool bShouldClear)
{
	m_HeightMapTexture = make_unique<CTexture>(m_PtrDevice, m_PtrDeviceContext);

	m_HeightMapTextureSize = XMFLOAT2(m_TerrainFileData->SizeX + 1.0f, m_TerrainFileData->SizeZ + 1.0f);
	m_HeightMapTexture->CreateBlankTexture(CTexture::EFormat::Pixel8Int, m_HeightMapTextureSize);
	m_HeightMapTexture->SetSlot(0);
	m_HeightMapTexture->SetShaderType(EShaderType::VertexShader);
	m_HeightMapTexture->Use();

	if (bShouldClear)
	{
		m_TerrainFileData->vHeightMapTextureRawData.clear();
		m_TerrainFileData->vHeightMapTextureRawData.resize(static_cast<size_t>(m_HeightMapTextureSize.x)* static_cast<size_t>(m_HeightMapTextureSize.y));
		for (auto& Data : m_TerrainFileData->vHeightMapTextureRawData)
		{
			Data.R = 127;
		}
	}

	// Update HeightMap Texture
	m_HeightMapTexture->UpdateTextureRawData(&m_TerrainFileData->vHeightMapTextureRawData[0]);
}

void CTerrain::CreateMaskingTexture(bool bShouldClear)
{
	m_MaskingTexture = make_unique<CTexture>(m_PtrDevice, m_PtrDeviceContext);
	
	m_MaskingTextureSize = XMFLOAT2(m_TerrainFileData->SizeX * m_TerrainFileData->MaskingDetail, m_TerrainFileData->SizeZ * m_TerrainFileData->MaskingDetail);
	m_MaskingTexture->CreateBlankTexture(CTexture::EFormat::Pixel32Int, m_MaskingTextureSize);
	m_MaskingTexture->SetSlot(KMaskingTextureSlot);
	m_MaskingTexture->Use();

	if (bShouldClear)
	{
		m_TerrainFileData->vMaskingTextureRawData.clear();
		m_TerrainFileData->vMaskingTextureRawData.resize(static_cast<size_t>(m_MaskingTextureSize.x) * static_cast<size_t>(m_MaskingTextureSize.y));
	}

	// Update Masking Texture
	m_MaskingTexture->UpdateTextureRawData(&m_TerrainFileData->vMaskingTextureRawData[0]);

	XMMATRIX Translation{ XMMatrixTranslation(m_TerrainFileData->SizeX / 2.0f, 0, m_TerrainFileData->SizeZ / 2.0f) };
	XMMATRIX Scaling{ XMMatrixScaling(1 / m_TerrainFileData->SizeX, 1.0f, 1 / m_TerrainFileData->SizeZ) };
	m_MatrixMaskingSpace = Translation * Scaling;
}

void CTerrain::CreateWater()
{
	static constexpr XMVECTOR KWaterColor{ 0.0f, 0.5f, 0.75f, 0.8125f };

	m_Object3DWater = make_unique<CObject3D>("Water", m_PtrDevice, m_PtrDeviceContext);

	SMesh WaterMesh{ GenerateTerrainBase(XMFLOAT2(m_TerrainFileData->SizeX, m_TerrainFileData->SizeZ), KTextureSubdivisionDetail, KWaterColor) };

	CMaterialData WaterMaterialData{};
	WaterMaterialData.SetTextureFileName(ETextureType::DiffuseTexture, "Asset\\water_diffuse.jpg");
	WaterMaterialData.SetTextureFileName(ETextureType::NormalTexture, "Asset\\water_normal.jpg");
	WaterMaterialData.SetTextureFileName(ETextureType::DisplacementTexture, "Asset\\water_displacement.jpg");

	m_Object3DWater->Create(WaterMesh, WaterMaterialData);
	m_Object3DWater->ShouldTessellate(true);
}

void CTerrain::CreateFoliagePlacingTexutre(bool bShouldClear)
{
	m_FoliagePlacingTexture = make_unique<CTexture>(m_PtrDevice, m_PtrDeviceContext);
	
	m_FoliagePlacingTextureSize = 
		XMFLOAT2(m_TerrainFileData->SizeX * m_TerrainFileData->FoliagePlacingDetail, m_TerrainFileData->SizeZ * m_TerrainFileData->FoliagePlacingDetail);
	m_FoliagePlacingTexture->CreateBlankTexture(CTexture::EFormat::Pixel8Int, m_FoliagePlacingTextureSize);

	if (bShouldClear)
	{
		m_TerrainFileData->vFoliagePlacingTextureRawData.clear();
		m_TerrainFileData->vFoliagePlacingTextureRawData.resize(static_cast<size_t>(m_FoliagePlacingTextureSize.x)* static_cast<size_t>(m_FoliagePlacingTextureSize.y));
	}
	else
	{
		// Update Foliage Placing Texture
		m_FoliagePlacingTexture->UpdateTextureRawData(&m_TerrainFileData->vFoliagePlacingTextureRawData[0]);
	}
}

void CTerrain::CreateWindRepresentation()
{
	m_Object3DWindRep = make_unique<CObject3D>("WindRepr", m_PtrDevice, m_PtrDeviceContext);
	m_Object3DWindRep->Create(GenerateSphere());
}

void CTerrain::AddMaterial(const CMaterialData& MaterialData)
{
	assert(m_Object3DTerrain);
	if ((int)m_Object3DTerrain->GetModel().vMaterialData.size() == KMaterialMaxCount) return;

	m_Object3DTerrain->AddMaterial(MaterialData);

	RegisterChange();
}

void CTerrain::SetMaterial(int MaterialID, const CMaterialData& NewMaterialData)
{
	assert(m_Object3DTerrain);
	assert(MaterialID >= 0 && MaterialID < 4);

	m_Object3DTerrain->SetMaterial(MaterialID, NewMaterialData);

	RegisterChange();
}

const CMaterialData& CTerrain::GetMaterial(size_t Index) const
{
	assert(m_Object3DTerrain);
	return m_Object3DTerrain->GetModel().vMaterialData[Index];
}

CMaterialData& CTerrain::GetMaterial(size_t Index)
{
	assert(m_Object3DTerrain);
	return m_Object3DTerrain->GetModel().vMaterialData[Index];
}

CMaterialTextureSet* CTerrain::GetMaterialTextureSet(size_t MaterialID) const
{
	return m_Object3DTerrain->GetMaterialTextureSet(MaterialID);
}

size_t CTerrain::GetMaterialCount() const
{
	assert(m_Object3DTerrain);
	return m_Object3DTerrain->GetMaterialCount();
}

void CTerrain::Select(const XMVECTOR& PickingRayOrigin, const XMVECTOR& PickingRayDirection, bool bShouldEdit, bool bIsLeftButton)
{
	if (!m_Object3DTerrain) return;

	UpdateSelectionPosition(PickingRayOrigin, PickingRayDirection);

	if (bShouldEdit)
	{
		if (m_eEditMode == EEditMode::Masking)
		{
			if (bIsLeftButton)
			{
				UpdateMasking(m_eMaskingLayer, m_MaskingRatio);
			}
			else
			{
				UpdateMasking(m_eMaskingLayer, 0.0f, true);
			}
		}
		else if (m_eEditMode == EEditMode::FoliagePlacing)
		{
			UpdateFoliagePlacing(!bIsLeftButton);
		}
		else
		{
			UpdateHeights(bIsLeftButton);
		}
	}
}

void CTerrain::UpdateSelectionPosition(const XMVECTOR& PickingRayOrigin, const XMVECTOR& PickingRayDirection)
{
	XMVECTOR PlaneT{};
	if (IntersectRayPlane(PickingRayOrigin, PickingRayDirection, XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 1, 0, 0), &PlaneT))
	{
		const XMFLOAT2 KTerrainHalfSize{ m_TerrainFileData->SizeX / 2.0f, m_TerrainFileData->SizeZ / 2.0f };
		const XMVECTOR KPointOnPlane{ PickingRayOrigin + PickingRayDirection * PlaneT };

		m_CBTerrainSelectionData.Position.x = XMVectorGetX(KPointOnPlane);
		m_CBTerrainSelectionData.Position.y = XMVectorGetZ(KPointOnPlane);
	}
}

void CTerrain::UpdateHeights(bool bIsLeftButton)
{
	const XMMATRIX KInverseWorld{ XMMatrixInverse(nullptr, m_CBTerrainSelectionData.TerrainWorld) };
	const float KLocalSelectionRadius{ m_CBTerrainSelectionData.SelectionRadius / XMVectorGetX(m_Object3DTerrain->GetTransform().Scaling) };
	const float KLocalRadiusSquare{ KLocalSelectionRadius * KLocalSelectionRadius };
	const int KTerrainSizeX{ (int)m_TerrainFileData->SizeX };
	const int KTerrainSizeZ{ (int)m_TerrainFileData->SizeZ };
	
	XMVECTOR LocalSelectionPosition{ m_CBTerrainSelectionData.Position.x, 0, m_CBTerrainSelectionData.Position.y, 1 };
	LocalSelectionPosition = XMVector3TransformCoord(LocalSelectionPosition, KInverseWorld);
	const int KCenterU{ (int)(XMVectorGetX(LocalSelectionPosition) + 0.5f) + KTerrainSizeX / 2 };
	const int KCenterV{ (int)(-XMVectorGetZ(LocalSelectionPosition) + 0.5f) + KTerrainSizeZ / 2 };

	int AverageHeightInRange{};
	if (m_eEditMode == EEditMode::AverageHeight)
	{
		int PixelInRangeCount{};
		int HeightSum{};
		for (int iPixel = 0; iPixel < (int)m_TerrainFileData->vHeightMapTextureRawData.size(); ++iPixel)
		{
			int U{ iPixel % (int)m_HeightMapTextureSize.x };
			int V{ iPixel / (int)m_HeightMapTextureSize.x };

			float dU{ float(U - KCenterU) };
			float dV{ float(V - KCenterV) };
			float DistanceSquare{ dU * dU + dV * dV };
			if (DistanceSquare <= KLocalRadiusSquare)
			{
				HeightSum += m_TerrainFileData->vHeightMapTextureRawData[iPixel].R;
				++PixelInRangeCount;
			}
		}
		float fAverageHeight{ (float)HeightSum / (float)PixelInRangeCount };
		AverageHeightInRange = (int)fAverageHeight;
	}

	for (int iPixel = 0; iPixel < (int)m_TerrainFileData->vHeightMapTextureRawData.size(); ++iPixel)
	{
		int U{ iPixel % (int)m_HeightMapTextureSize.x };
		int V{ iPixel / (int)m_HeightMapTextureSize.x };

		float dU{ float(U - KCenterU) };
		float dV{ float(V - KCenterV) };
		float DistanceSquare{ dU * dU + dV * dV };
		if (DistanceSquare <= KLocalRadiusSquare)
		{
			switch (m_eEditMode)
			{
			case EEditMode::SetHeight:
				{
					float NewY{ (m_SetHeightValue + KHeightRangeHalf) / KHeightRange };
					NewY = min(max(NewY, 0.0f), 1.0f);

					m_TerrainFileData->vHeightMapTextureRawData[iPixel].R = static_cast<uint8_t>(NewY * 255);
				}
				break;
			case EEditMode::AverageHeight:
				{
					int Deviation{ (int)m_TerrainFileData->vHeightMapTextureRawData[iPixel].R - AverageHeightInRange };
					Deviation = (int)((float)Deviation * 0.875f);

					m_TerrainFileData->vHeightMapTextureRawData[iPixel].R = (uint8_t)(AverageHeightInRange + Deviation);
				}
				break;
			case EEditMode::DeltaHeight:
				{
					if (bIsLeftButton)
					{
						int NewY{ m_TerrainFileData->vHeightMapTextureRawData[iPixel].R + 1 };
						NewY = min(NewY, 255);
						m_TerrainFileData->vHeightMapTextureRawData[iPixel].R = static_cast<uint8_t>(NewY);
					}
					else
					{
						int NewY{ m_TerrainFileData->vHeightMapTextureRawData[iPixel].R - 1 };
						NewY = max(NewY, 0);
						m_TerrainFileData->vHeightMapTextureRawData[iPixel].R = static_cast<uint8_t>(NewY);
					}
				} 
				break;
			default:
				break;
			}
		}
	}

	// Update HeightMap Texture
	m_HeightMapTexture->UpdateTextureRawData(&m_TerrainFileData->vHeightMapTextureRawData[0]);

	RegisterChange();
}

void CTerrain::UpdateMasking(EMaskingLayer eLayer, float Value, bool bForceSet)
{
	const XMMATRIX KInverseWorld{ XMMatrixInverse(nullptr, m_CBTerrainSelectionData.TerrainWorld) };
	const float KLocalSelectionRadius{ m_CBTerrainSelectionData.SelectionRadius / XMVectorGetX(m_Object3DTerrain->GetTransform().Scaling) };
	const float KDetailSquare{ (float)m_TerrainFileData->MaskingDetail * (float)m_TerrainFileData->MaskingDetail };
	const float KLocalRadiusSquare{ KLocalSelectionRadius * KLocalSelectionRadius * KDetailSquare };
	const int KTerrainSizeX{ (int)m_TerrainFileData->SizeX };
	const int KTerrainSizeZ{ (int)m_TerrainFileData->SizeZ };

	XMVECTOR LocalSelectionPosition{ m_CBTerrainSelectionData.Position.x, 0, m_CBTerrainSelectionData.Position.y, 1 };
	LocalSelectionPosition = XMVector3TransformCoord(LocalSelectionPosition, KInverseWorld);
	const int KCenterU{ static_cast<int>((+m_TerrainFileData->SizeX / 2.0f + XMVectorGetX(LocalSelectionPosition)) * m_TerrainFileData->MaskingDetail) };
	const int KCenterV{ static_cast<int>(-(-m_TerrainFileData->SizeZ / 2.0f + XMVectorGetZ(LocalSelectionPosition)) * m_TerrainFileData->MaskingDetail) };

	for (int iPixel = 0; iPixel < (int)m_TerrainFileData->vMaskingTextureRawData.size(); ++iPixel)
	{
		int U{ iPixel % (int)m_MaskingTextureSize.x };
		int V{ iPixel / (int)m_MaskingTextureSize.x };

		float dU{ float(U - KCenterU) };
		float dV{ float(V - KCenterV) };
		float DistanceSquare{ dU * dU + dV * dV };
		if (DistanceSquare <= KLocalRadiusSquare)
		{
			float Factor{ 1.0f -
				(sqrt(DistanceSquare / KDetailSquare) / KLocalSelectionRadius) * m_MaskingAttenuation - // Distance attenuation
				((DistanceSquare / KDetailSquare) / KLocalSelectionRadius) * m_MaskingAttenuation }; // Distance square attenuation
			Factor = max(Factor, 0.0f);
			Factor = min(Factor, 1.0f);

			if (bForceSet)
			{
				switch (eLayer)
				{
				case EMaskingLayer::LayerR:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].R = static_cast<uint8_t>(Value * Factor * 255.0f);
					break;
				case EMaskingLayer::LayerG:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].G = static_cast<uint8_t>(Value * Factor * 255.0f);
					break;
				case EMaskingLayer::LayerB:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].B = static_cast<uint8_t>(Value * Factor * 255.0f);
					break;
				case EMaskingLayer::LayerA:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].A = static_cast<uint8_t>(Value * Factor * 255.0f);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (eLayer)
				{
				case EMaskingLayer::LayerR:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].R = 
						max(m_TerrainFileData->vMaskingTextureRawData[iPixel].R, static_cast<uint8_t>(Value * Factor * 255.0f));
					break;
				case EMaskingLayer::LayerG:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].G = 
						max(m_TerrainFileData->vMaskingTextureRawData[iPixel].G, static_cast<uint8_t>(Value * Factor * 255.0f));
					break;
				case EMaskingLayer::LayerB:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].B = 
						max(m_TerrainFileData->vMaskingTextureRawData[iPixel].B, static_cast<uint8_t>(Value * Factor * 255.0f));
					break;
				case EMaskingLayer::LayerA:
					m_TerrainFileData->vMaskingTextureRawData[iPixel].A =
						max(m_TerrainFileData->vMaskingTextureRawData[iPixel].A, static_cast<uint8_t>(Value * Factor * 255.0f));
					break;
				default:
					break;
				}
			}
		}
	}

	// Update Masking Texture
	m_MaskingTexture->UpdateTextureRawData(&m_TerrainFileData->vMaskingTextureRawData[0]);

	RegisterChange();
}

void CTerrain::UpdateFoliagePlacing(bool bErase)
{
	if (!m_FoliagePlacingTexture) return;

	const XMMATRIX KInverseWorld{ XMMatrixInverse(nullptr, m_CBTerrainSelectionData.TerrainWorld) };
	const float KScalingX{ XMVectorGetX(m_Object3DTerrain->GetTransform().Scaling) };
	const float KScalingZ{ XMVectorGetX(m_Object3DTerrain->GetTransform().Scaling) };
	const float KLocalSelectionRadius{ m_CBTerrainSelectionData.SelectionRadius / KScalingX };
	const float KDetailSquare{ (float)m_TerrainFileData->FoliagePlacingDetail * (float)m_TerrainFileData->FoliagePlacingDetail };
	const float KLocalRadiusSquare{ KLocalSelectionRadius * KLocalSelectionRadius * KDetailSquare };
	const int KTerrainSizeX{ (int)m_TerrainFileData->SizeX };
	const int KTerrainSizeZ{ (int)m_TerrainFileData->SizeZ };

	XMVECTOR LocalSelectionPosition{ m_CBTerrainSelectionData.Position.x, 0, m_CBTerrainSelectionData.Position.y, 1 };
	LocalSelectionPosition = XMVector3TransformCoord(LocalSelectionPosition, KInverseWorld);
	const int KCenterU{ static_cast<int>((+m_TerrainFileData->SizeX / 2.0f + XMVectorGetX(LocalSelectionPosition)) * m_TerrainFileData->FoliagePlacingDetail) };
	const int KCenterV{ static_cast<int>(-(-m_TerrainFileData->SizeZ / 2.0f + XMVectorGetZ(LocalSelectionPosition)) * m_TerrainFileData->FoliagePlacingDetail) };

	for (int iPixel = 0; iPixel < (int)m_TerrainFileData->vFoliagePlacingTextureRawData.size(); ++iPixel)
	{
		int U{ iPixel % (int)(m_FoliagePlacingTextureSize.x) };
		int V{ iPixel / (int)(m_FoliagePlacingTextureSize.x) };

		float dU{ float(U - KCenterU) };
		float dV{ float(V - KCenterV) };
		float DistanceSquare{ dU * dU + dV * dV };
		if (DistanceSquare <= KLocalRadiusSquare)
		{
			if (bErase)
			{
				// 지우기
				if (m_TerrainFileData->vFoliagePlacingTextureRawData[iPixel].R == 255)
				{
					m_TerrainFileData->vFoliagePlacingTextureRawData[iPixel].R = 0;

					const string KInstanceName{ "Fol_" + to_string(U) + "_" + to_string(V) };
					for (auto& Foliage : m_vFoliages)
					{
						Foliage->DeleteInstance(KInstanceName);
					}
				}
			}
			else
			{
				// 그리기
				float InverseDensity{ 1.0f - m_TerrainFileData->FoliageDenstiy };
				float Exponent{ GetRandom(6.0f, 8.0f) };
				int DensityModular{ iPixel % (int)(pow(InverseDensity + 1.0f, Exponent)) };
				if (m_TerrainFileData->vFoliagePlacingTextureRawData[iPixel].R != 255 && DensityModular == 0)
				{
					m_TerrainFileData->vFoliagePlacingTextureRawData[iPixel].R = 255;

					const string KInstanceName{ "Fol_" + to_string(U) + "_" + to_string(V) };
					const float Interval{ 1.0f / (float)m_TerrainFileData->FoliagePlacingDetail };
					for (auto& Foliage : m_vFoliages)
					{
						if (Foliage->InsertInstance(KInstanceName))
						{
							float XDisplacement{ GetRandom(-0.2f, +0.2f) };
							float YDisplacement{ GetRandom(-0.1f, 0.0f) };
							float ZDisplacement{ GetRandom(-0.2f, +0.2f) };

							Foliage->TranslateInstanceTo(
								KInstanceName,
								XMVectorSet
								(
									XDisplacement + (U - (int)(m_FoliagePlacingTextureSize.x * 0.5f)) * KScalingX * Interval,
									YDisplacement,
									ZDisplacement - (V - (int)(m_FoliagePlacingTextureSize.y * 0.5f)) * KScalingZ * Interval,
									1
								));

							float YRotationAngle{ GetRandom(0.0f, XM_2PI) };
							Foliage->RotateInstanceYawTo(KInstanceName, YRotationAngle);
							Foliage->UpdateInstanceWorldMatrix(KInstanceName);
						}
					}
				}
			}
		}
	}
	
	// Update Foliage Placing Texture
	m_FoliagePlacingTexture->UpdateTextureRawData(&m_TerrainFileData->vFoliagePlacingTextureRawData[0]);

	RegisterChange();
}

void CTerrain::SetMaskingLayer(EMaskingLayer eLayer)
{
	m_eMaskingLayer = eLayer;
}

CTerrain::EMaskingLayer CTerrain::GetMaskingLayer() const
{
	return m_eMaskingLayer;
}

void CTerrain::SetMaskingAttenuation(float Attenuation)
{
	m_MaskingAttenuation = Attenuation;
}

float CTerrain::GetMaskingAttenuation() const
{
	return m_MaskingAttenuation;
}

void CTerrain::SetSelectionRadius(float Radius)
{
	Radius = max(min(Radius, KMaxSelectionRadius), KMinSelectionRadius);
	
	m_CBTerrainSelectionData.SelectionRadius = Radius;
}

float CTerrain::GetSelectionRadius() const
{
	return m_CBTerrainSelectionData.SelectionRadius;
}

void CTerrain::SetSetHeightValue(float Value)
{
	m_SetHeightValue = Value;
}

float CTerrain::GetSetHeightValue() const
{
	return m_SetHeightValue;
}

void CTerrain::SetDeltaHeightValue(float Value)
{
	m_DeltaHeightValue = Value;
}

float CTerrain::GetDeltaHeightValue() const
{
	return m_DeltaHeightValue;
}

void CTerrain::SetMaskingRatio(float Value)
{
	m_MaskingRatio = Value;
}

float CTerrain::GetMaskingRatio() const
{
	return m_MaskingRatio;
}

void CTerrain::ShouldTessellate(bool Value)
{
	if (m_Object3DTerrain) m_Object3DTerrain->ShouldTessellate(Value);
}

bool CTerrain::ShouldTessellate() const
{
	if (m_Object3DTerrain) return m_Object3DTerrain->ShouldTessellate();
	return false;
}

void CTerrain::ShouldDrawWater(bool Value)
{
	m_TerrainFileData->bShouldDrawWater = Value;
}

bool CTerrain::ShouldDrawWater() const
{
	return m_TerrainFileData->bShouldDrawWater;
}

void CTerrain::ShouldShowSelection(bool Value)
{
	m_CBTerrainSelectionData.bShowSelection = ((Value == true) ? TRUE : FALSE);
}

void CTerrain::SetEditMode(EEditMode Mode)
{
	m_eEditMode = Mode;
}

CTerrain::EEditMode CTerrain::GetEditMode()
{
	return m_eEditMode;
}

void CTerrain::SetWaterHeight(float Value)
{
	m_TerrainFileData->WaterHeight = Value;
}

float CTerrain::GetWaterHeight() const
{
	return m_TerrainFileData->WaterHeight;
}

void CTerrain::SetTerrainTessFactor(float Value)
{
	m_TerrainFileData->TerrainTessellationFactor = Value;
	
	if (m_Object3DTerrain) m_Object3DTerrain->SetTessFactorData(CObject3D::SCBTessFactorData(Value));
}

float CTerrain::GetTerrainTessFactor() const
{
	return m_TerrainFileData->TerrainTessellationFactor;
}

const CObject3D::SCBTessFactorData& CTerrain::GetTerrainTessFactorData() const
{
	return m_Object3DTerrain->GetTessFactorData();
}

const CObject3D::SCBDisplacementData& CTerrain::GetTerrainDisplacementData() const
{
	return m_Object3DTerrain->GetDisplacementData();
}

void CTerrain::SetWaterTessFactor(float Value)
{
	m_TerrainFileData->WaterTessellationFactor = Value;
	if (m_Object3DWater) m_Object3DWater->SetTessFactorData(CObject3D::SCBTessFactorData(Value));
}

float CTerrain::GetWaterTessFactor() const
{
	return m_TerrainFileData->WaterTessellationFactor;
}

const CObject3D::SCBTessFactorData& CTerrain::GetWaterTessFactorData() const
{
	return m_Object3DWater->GetTessFactorData();
}

const CObject3D::SCBDisplacementData& CTerrain::GetWaterDisplacementData() const
{
	return m_Object3DWater->GetDisplacementData();
}

XMFLOAT2 CTerrain::GetSize() const
{
	return XMFLOAT2(m_TerrainFileData->SizeX, m_TerrainFileData->SizeZ);
}

int CTerrain::GetMaskingDetail() const
{
	return m_TerrainFileData->MaskingDetail;
}

bool CTerrain::HasFoliageCluster() const
{
	return m_TerrainFileData->bHasFoliageCluster;
}

const string& CTerrain::GetFileName() const
{
	return m_TerrainFileData->FileName;
}

const XMFLOAT2& CTerrain::GetSelectionPosition() const
{
	return m_CBTerrainSelectionData.Position;
}

uint32_t CTerrain::GetFoliagePlacingDetail() const
{
	return m_TerrainFileData->FoliagePlacingDetail;
}

float CTerrain::GetTerrainHeightAt(float X, float Z)
{
	if (m_TerrainFileData->vHeightMapTextureRawData.size())
	{
		int iTerrainHalfSizeX{ (int)(m_TerrainFileData->SizeX * 0.5f) };
		int iTerrainHalfSizeZ{ (int)(m_TerrainFileData->SizeZ * 0.5f) };
		int iX{ (int)floor(X) + iTerrainHalfSizeX }; // [0, TerrainSizeX + 1]
		int iZ{ -(int)floor(Z) + iTerrainHalfSizeZ }; // [0, TerrainSizeZ + 1]
		float dX{ X - floor(X) };
		float dZ{ Z - floor(Z) };
		
		if (dX == 0.0f && dZ == 0.0f)
		{
			return GetTerrainHeightAt(iX, iZ);
		}
		else
		{
			int idX{}, idZ{};
			if (dX != 0.0f)
			{
				idX = (int)(dX / dX);
				if (dX < 0.0f) idX = -idX;
			}
			if (dZ != 0.0f)
			{
				idZ = (int)(dZ / dZ);
				if (dZ < 0.0f) idZ = -idZ;
			}
			int iCmpX{ iX + idX };
			int iCmpZ{ iZ - idZ };
			float Height{ GetTerrainHeightAt(iX, iZ) };
			float CmpHeightX{ GetTerrainHeightAt(iCmpX, iZ) };
			float CmpHeightZ{ GetTerrainHeightAt(iX, iCmpZ) };

			float XLerp{ Lerp(Height, CmpHeightX, abs(dX)) };
			float ZLerp{ Lerp(Height, CmpHeightZ, abs(dZ)) };

			if (dX == 0.0f) return ZLerp;
			if (dZ == 0.0f) return XLerp;

			float Tan{ abs(dZ) / abs(dX) };
			float Theta{ atan(Tan) };
			float Weight{ Theta / XM_PIDIV4 }; // [0.0f, 1.0f]

			return Lerp(XLerp, ZLerp, Weight);
		}
	}
	return 0.0f;
}

float CTerrain::GetTerrainHeightAt(int iX, int iZ)
{
	if (m_TerrainFileData->vHeightMapTextureRawData.size())
	{
		const int KHeightMapSizeX{ (int)m_HeightMapTextureSize.x };
		const int KHeightMapSizeZ{ (int)m_HeightMapTextureSize.y };

		if (iX < 0) iX = KHeightMapSizeX - 1;
		if (iZ < 0) iZ = KHeightMapSizeZ - 1;
		if (iX >= KHeightMapSizeX) iX = 0;
		if (iZ >= KHeightMapSizeZ) iZ = 0;

		int iPixel{ iZ * KHeightMapSizeX + iX };
		float NormalizeHeight{ (float)m_TerrainFileData->vHeightMapTextureRawData[iPixel].R / 255.0f };
		float Height{ NormalizeHeight * KHeightRange }; // [0, KHeightRange]
		Height += KMinHeight; // [-KMinHeight, +KMaxHeight]
		return Height;
	}
	return 0.0f;
}

const CTerrain::SCBTerrainData& CTerrain::GetTerrainData() const
{
	return m_CBTerrainData;
}

const DirectX::XMMATRIX& CTerrain::GetMaskingSpaceData() const
{
	return m_MatrixMaskingSpace;
}

const CTerrain::SCBTerrainSelectionData& CTerrain::GetSelectionData()
{
	m_CBTerrainSelectionData.TerrainWorld = m_Object3DTerrain->GetWorldMatrix();
	m_CBTerrainSelectionData.InverseTerrainWorld = XMMatrixInverse(nullptr, m_CBTerrainSelectionData.TerrainWorld);

	return m_CBTerrainSelectionData;
}

const DirectX::XMMATRIX& CTerrain::GetWaterWorldMatrix() const
{
	return m_Object3DWater->GetWorldMatrix();
}

const DirectX::XMMATRIX& CTerrain::GetWindRepresentationWorldMatrix() const
{
	return m_Object3DWindRep->GetWorldMatrix();
}

void CTerrain::UpdateWind(float DeltaTime)
{
	XMVECTOR xmvPosition{ XMLoadFloat3(&m_CBWindData.Position) };
	xmvPosition += m_CBWindData.Velocity * DeltaTime;	
	XMStoreFloat3(&m_CBWindData.Position, xmvPosition);
	
	float HalfTerrainSizeX{ m_TerrainFileData->SizeX * 0.5f };
	float HalfTerrainSizeZ{ m_TerrainFileData->SizeZ * 0.5f };
	if (m_CBWindData.Position.x < -HalfTerrainSizeX - m_CBWindData.Radius)
	{
		m_CBWindData.Position.x = +HalfTerrainSizeX + m_CBWindData.Radius;
	}
	else if (m_CBWindData.Position.x > +HalfTerrainSizeX + m_CBWindData.Radius)
	{
		m_CBWindData.Position.x = -HalfTerrainSizeX - m_CBWindData.Radius;
	}
	if (m_CBWindData.Position.z < -HalfTerrainSizeZ - m_CBWindData.Radius)
	{
		m_CBWindData.Position.z = +HalfTerrainSizeZ + m_CBWindData.Radius;
	}
	else if (m_CBWindData.Position.z > +HalfTerrainSizeZ + m_CBWindData.Radius)
	{
		m_CBWindData.Position.z = -HalfTerrainSizeZ - m_CBWindData.Radius;
	}
	m_CBWindData.Position.y = GetTerrainHeightAt(m_CBWindData.Position.x, m_CBWindData.Position.z);
}

void CTerrain::DrawTerrain(bool bDrawNormals)
{
	if (!m_Object3DTerrain) return;

	m_HeightMapTexture->SetShaderType(EShaderType::VertexShader);
	m_HeightMapTexture->Use();
	m_MaskingTexture->Use();

	if (bDrawNormals)
	{
		m_PtrGame->GetBaseShader(CGame::EBaseShader::GSNormal)->Use();
	}

	m_Object3DTerrain->Draw();

	if (bDrawNormals)
	{
		m_PtrDeviceContext->GSSetShader(nullptr, nullptr, 0);
	}
}

void CTerrain::DrawWater()
{
	m_Object3DWater->SetTransform(m_Object3DTerrain->GetTransform());
	m_Object3DWater->Translate(XMVectorSet(0, m_TerrainFileData->WaterHeight, 0, 1));
	m_Object3DWater->UpdateWorldMatrix();

	//m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetDepthStencilStateLessEqualNoWrite(), 0);

	m_PtrGame->GetBaseShader(CGame::EBaseShader::VSBase)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::HSWater)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::DSWater)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::PSWater)->Use();

	m_Object3DWater->Draw();

	//m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthDefault(), 0);

	m_PtrDeviceContext->HSSetShader(nullptr, nullptr, 0);
	m_PtrDeviceContext->DSSetShader(nullptr, nullptr, 0);
}

void CTerrain::DrawFoliageCluster()
{
	if (m_vFoliages.empty()) return;
	if (m_vFoliages[0]->GetInstanceCount() == 0) return;

	m_PtrDeviceContext->RSSetState(m_PtrGame->GetCommonStates()->CullNone());
	m_PtrDeviceContext->OMSetBlendState(m_PtrGame->GetBlendStateAlphaToCoverage(), nullptr, 0xFFFFFFFF);

	m_HeightMapTexture->SetShaderType(EShaderType::VertexShader);
	m_HeightMapTexture->Use();
	
	m_PtrGame->GetBaseShader(CGame::EBaseShader::VSFoliage)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::PSFoliage)->Use();

	m_PtrDeviceContext->HSSetShader(nullptr, nullptr, 0);
	m_PtrDeviceContext->DSSetShader(nullptr, nullptr, 0);

	for (const auto& Foliage : m_vFoliages)
	{
		Foliage->Draw();
	}

	m_PtrGame->SetUniversalRSState();
}

void CTerrain::DrawWindRepresentation()
{
	// Wind sphere represenatation
	if (m_Object3DWindRep)
	{
		m_PtrDeviceContext->RSSetState(m_PtrGame->GetCommonStates()->Wireframe());

		m_Object3DWindRep->TranslateTo(XMLoadFloat3(&m_CBWindData.Position));
		m_Object3DWindRep->ScaleTo(XMVectorSet(m_CBWindData.Radius, m_CBWindData.Radius, m_CBWindData.Radius, 0));
		m_Object3DWindRep->UpdateWorldMatrix();

		m_PtrGame->GetBaseShader(CGame::EBaseShader::VSBase)->Use();
		m_PtrGame->GetBaseShader(CGame::EBaseShader::PSBase_RawVertexColor)->Use();

		m_Object3DWindRep->Draw();

		m_PtrGame->SetUniversalRSState();
	}
}

void CTerrain::DrawHeightMapTexture()
{
	if (!m_Object2DTextureRep) return;

	m_HeightMapTexture->SetShaderType(EShaderType::PixelShader);
	m_HeightMapTexture->Use();

	m_PtrDeviceContext->RSSetState(m_PtrGame->GetCommonStates()->CullCounterClockwise());
	m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthNone(), 0);

	m_PtrGame->GetBaseShader(CGame::EBaseShader::VSBase2D)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::PSHeightMap2D)->Use();

	m_Object2DTextureRep->Draw();

	m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthDefault(), 0);
}

void CTerrain::DrawMaskingTexture()
{
	if (!m_Object2DTextureRep) return;

	m_MaskingTexture->Use(0);

	m_PtrDeviceContext->RSSetState(m_PtrGame->GetCommonStates()->CullCounterClockwise());
	m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthNone(), 0);

	m_PtrGame->GetBaseShader(CGame::EBaseShader::VSBase2D)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::PSMasking2D)->Use();

	m_Object2DTextureRep->Draw();

	m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthDefault(), 0);
}

void CTerrain::DrawFoliagePlacingTexture()
{
	if (!m_FoliagePlacingTexture) return;
	if (!m_Object2DTextureRep) return;

	m_FoliagePlacingTexture->SetShaderType(EShaderType::PixelShader);
	m_FoliagePlacingTexture->Use();

	m_PtrDeviceContext->RSSetState(m_PtrGame->GetCommonStates()->CullCounterClockwise());
	m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthNone(), 0);

	m_PtrGame->GetBaseShader(CGame::EBaseShader::VSBase2D)->Use();
	m_PtrGame->GetBaseShader(CGame::EBaseShader::PSHeightMap2D)->Use();

	m_Object2DTextureRep->Draw();

	m_PtrDeviceContext->OMSetDepthStencilState(m_PtrGame->GetCommonStates()->DepthDefault(), 0);
}
