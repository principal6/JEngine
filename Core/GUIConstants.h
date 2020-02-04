#pragma once

static const char* KGUIString_Menu[][2]
{
	{ "Terrain",								u8"����"								},
	{ "Create",									u8"�����"								},
	{ "Load",									u8"�ҷ�����"							},
	{ "Save",									u8"��������"							},
	{ "Window",									u8"â"									},
	{ "Property editor",						u8"�Ӽ� ������"							},
	{ "Scene editor",							u8"��� ������"							},
	{ "Quit",									u8"����"								},
};

static const char* KGUIString_PropertyEditor[][2]
{
	{ "Object",									u8"������Ʈ"							},
	{ "Terrain",								u8"����"								},
	{ "Scene",									u8"���"								},
	{ "IBL",									u8"IBL"									},
	{ "ETC",									u8"��Ÿ"								},

	{ "Physics Properties",						u8"���� �Ӽ�"							},
	{ "Bounding Volume",						u8"�ٿ�� ����"							},
	{ "Tessellation",							u8"�׼����̼�"							},
	{ "Material",								u8"����"								},
	{ "Animation",								u8"�ִϸ��̼�"							},
	{ "Object Intelligence",					u8"������Ʈ ����"						},
	{ "Instance Intelligence",					u8"�ν��Ͻ� ����"						},
};

static const char* KGUIString_Content[][2]
{
	// Object tab
	{ "<Please select an object>",				u8"<������Ʈ�� �����ϼ���>"				},
	{ "Selected object",						u8"���õ� ������Ʈ:"					},
	{ "Vertex count",							u8"���� ����"							},
	{ "Triangle count",							u8"�ﰢ�� ����"							},

	{ "<Please select an instance>",			u8"<�ν��Ͻ��� �����ϼ���>"				},
	{ "Selected instance",						u8"���õ� �ν��Ͻ�"						},

	{ "Tranlsation",							u8"��ġ"								},
	{ "Rotation",								u8"ȸ��"								},
	{ "Rotation Pitch",							u8"ȸ�� Pitch"							},
	{ "Rotation Yaw",							u8"ȸ�� Yaw"							},
	{ "Scaling",								u8"ũ��"								},

	{ "Role of the object",						u8"������Ʈ ����:"						},

	{ "None",									u8"����"								},
	{ "Player",									u8"�÷��̾�"							},
	{ "Enviroment",								u8"ȯ��"								},
	{ "Monster",								u8"����"								},

	{ "Velocity",								u8"�ӵ�"								},

	{ "BoundingSphere",							u8"�ٿ�� ��"							},
	{ "AxisAlignedBoundingBox",					u8"�ٿ�� AABB"							},

	{ "Outer bounding sphere",					u8"�ܺ� �ٿ�� ��"						},
	{ "Center",									u8"�߽�"								},
	{ "Radius bias",							u8"������ ����ġ"						},
	{ "Radius (auto)",							u8"������ (�ڵ�)"						},
	{ "Radius",									u8"������"								},
	{ "Inner bounding volumes",					u8"���� �ٿ�� ����"					},

	{ "Insert",									u8"�߰�"								},
	{ "Delete",									u8"����"								},
	{ "Edit",									u8"����"								},
	{ "Save",									u8"����"								},
	{ "Load",									u8"����"								},
	{ "Confirm",								u8"����"								},
	{ "Cancel",									u8"���"								},
	{ "Close",									u8"�ݱ�"								},
	{ "Bounding volume type",					u8"�ٿ�� ���� ����"					},
	{ "X half size",							u8"X ���� ũ��"							},
	{ "Y half size",							u8"Y ���� ũ��"							},
	{ "Z half size",							u8"Z ���� ũ��"							},

	{ "Use tessellation",						u8"�׼����̼� ��� ����"				},
	{ "Edge tessellation factor",				u8"�׼����̼� �� ���"					},
	{ "Inside tessellation factor",				u8"�׼����̼� ���� ���"				},
	{ "Displacement factor",					u8"���� ���"							},

	{ "Ignore scene material",					u8"��� ���� �����ϱ�"					},
	{ "Change material name",					u8"���� �̸� ����"						},
	{ "Base color",								u8"���̽� ����"							},
	{ "Diffuse color",							u8"��ǻ�� ����"							},
	{ "Ambient color",							u8"�ں��Ʈ ����"						},
	{ "Specular color",							u8"����ŧ�� ����"						},
	{ "Specular exponent",						u8"����ŧ�� ����"						},
	{ "Specular intensity",						u8"����ŧ�� ����"						},
	{ "Roughness",								u8"��ĥ��"								},
	{ "Metalness",								u8"�ݼӼ�"								},
	{ "Texture",								u8"�ؽ�ó"								},
	{ "Clear all the textures",					u8"��� �ؽ�ó ����"					},
	{ "Normal",									u8"����"								},
	{ "Opacity",								u8"������"							},
	{ "Ambient occlusion",						u8"�ں��Ʈ ��Ŭ����"					},
	{ "Displacement",							u8"����"								},

	{ "Is GPU skinned",							u8"GPU ��Ű�� ����"						},
	{ "Yes",									u8"��"									},
	{ "No",										u8"�ƴϿ�"								},
	{ "Animation count",						u8"�ִϸ��̼� ����"						},
	{ "Animation list",							u8"�ִϸ��̼� ���"						},
	{ "Instance animation ID",					u8"�ν��Ͻ� �ִϸ��̼� ID"				},
	{ "Object animation ID",					u8"������Ʈ �ִϸ��̼� ID"				},
	{ "Insert animation",						u8"�ִϸ��̼� �߰�"						},
	{ "Animation name",							u8"�ִϸ��̼� �̸�"						},
	{ "File name",								u8"���� �̸�"							},
	{ "Edit animation",							u8"�ִϸ��̼� ����"						},
	{ "Registered Type",						u8"��ϵ� ����"							},
	{ "Ticks per second",						u8"�ʴ� Tick ��"						},
	{ "Behavior start tick",					u8"�ൿ ���� Tick"						},

	{ "Pattern",								u8"����"								},
	{ "Set pattern",							u8"���� ����"							},
	{ "Free pattern",							u8"���� ����"							},
	{ "Pattern list",							u8"���� ���"							},
	{ "No pattern found",						u8"������ �������� �ʽ��ϴ�"			},

	{ "Current camera",							u8"���� ȭ�� ī�޶�"					},
	{ "Go back to Editor Camera",				u8"������ ī�޶�� ���ư���"			},
	{ "Selected camera",						u8"���õ� ī�޶�"						},
	{ "Camera type",							u8"ī�޶� ����"							},
	{ "First person",							u8"1��Ī"								},
	{ "Third person",							u8"3��Ī"								},
	{ "Free look",								u8"���� ����"							},
	{ "Zoom distance",							u8"�� �Ÿ�"								},
	{ "Movement speed",							u8"�̵� �ӷ�"							},
	{ "Be player camera",						u8"�÷��̾� ī�޶��"					},
	{ "Be current camera",						u8"���� ȭ�� ī�޶��"					},

	{ "Selected light",							u8"���õ� ����"							},
	{ "Light type",								u8"���� ����"							},
	{ "Point light",							u8"�� ����"								},
	{ "Spot light",								u8"����Ʈ ����Ʈ"						},
	{ "Color",									u8"����"								},
	{ "Orientation",							u8"����"								},
	{ "Range",									u8"����"								},
	{ "Theta",									u8"����(Theta)"							},

	// Terrain tab
	{ "Width * Height",							u8"���� * ����"							},
	{ "Masking detail",							u8"����ŷ ����"							},
	{ "Terrain tessellation factor",			u8"���� �׼����̼� ���"				},
	{ "Draw water",								u8"�� �׸���"							},
	{ "Water Y position",						u8"�� Y ��ġ"							},
	{ "Water tessellation factor",				u8"�� �׼����̼� ���"					},

	{ "<Set-height mode>",						u8"<���� ���� ���>"					},
	{ "<Change-height mode>",					u8"<���� ���� ���>"					},
	{ "<Average-height mode>",					u8"<���� ��� ���>"					},
	{ "<Masking mode>",							u8"<����ŷ ���>"						},
	{ "<Foliage placing mode>",					u8"<�ʸ� ��ġ ���>"					},

	{ "Height prime",							u8"������ ����"							},
	{ "Masking layer",							u8"����ŷ ���̾�"						},
	{ "Masking ratio",							u8"����ŷ ������"						},
	{ "Masking attenuation",					u8"����ŷ ����"							},

	{ "Generate foliage cluster",				u8"�ʸ� Ŭ������ ����"					},
	{ "Foliage placing detail",					u8"�ʸ� ��ġ ����"						},
	{ "Foliage density",						u8"�ʸ� �е�"							},
	{ "Wind velocity",							u8"�ٶ� �ӵ�"							},
	{ "Wind radius",							u8"�ٶ� ������"							},
	{ "Selection radius",						u8"���� ������"							},
	{ "Selection position",						u8"���� ��ġ"							},
	{ "Foliage generator",						u8"�ʸ� ������"							},

	{ "Insert material",						u8"���� �߰�"							},
	{ "<Please create or load terrain>",		u8"<������ �����ϰų� �ҷ�������>"		},
	{ "Load texture from file",					u8"���Ͽ��� �ؽ�ó �ҷ�����"			},
	{ "Free texture",							u8"�ؽ�ó �����ϱ�"						},
	{ "New name",								u8"�� �̸�"								},
	{ "Use default",							u8"�⺻�� ���"							},

	// Scene tab
	{ "World floor height",						u8"���� �ٴ� ����"						},
	{ "Scene material",							u8"��� ����"							},
	{ "Intelligence pattern",					u8"���� ����"							},
	{ "Content",								u8"����"								},
	{ "Pattern file viewer",					u8"���� ���� ���"						},

	// IBL tab
	{ "Environment map",						u8"ȯ�� ��"								},
	{ "Current map",							u8"���� ��"								},
	{ "Irradiance map",							u8"���� ��"								},
	{ "Generate map",							u8"�� �����ϱ�"							},
	{ "Prefiltered radiance map",				u8"�������͸��� �ֵ� ��"				},
	{ "Integrated BRDF map",					u8"������ BRDF ��"						},

	// ETC tab
	{ "Light",									u8"����"								},
	{ "Directional light position",				u8"�𷺼ų� ����Ʈ ��ġ"				},
	{ "Directional light color",				u8"�𷺼ų� ����Ʈ ����"				},
	{ "Ambient light color",					u8"�ں��Ʈ ����Ʈ ����"				},
	{ "Ambient light intensity",				u8"�ں��Ʈ ����Ʈ ����"				},
	{ "Exposure (HDR)",							u8"���� (HDR)"							},
	{ "Frames per second (FPS)",				u8"�ʴ� ������ (FPS)"					},
	{ "Editor flags",							u8"������ �÷���"						},
	{ "Wire frame",								u8"���̾� ������"						},
	{ "Draw normals",							u8"���� ǥ��"							},
	{ "Draw mini-axes",							u8"���� ��ǥ�� ǥ��"					},
	{ "Draw bounding volumes",					u8"�ٿ�� ���� ǥ��"					},
	{ "Draw terrain height map",				u8"���� ���̸� ǥ��"					},
	{ "Draw terrain masking map",				u8"���� ����ŷ�� ǥ��"					},
	{ "Draw terrain foliage map",				u8"���� �ʸ�� ǥ��"					},
	{ "Draw identifiers",						u8"���̵�(ID) ǥ��"						},
	{ "Engine flags",							u8"���� �÷���"							},
	{ "Draw lighting volumes",					u8"���� ���� ǥ��"						},
	{ "Draw 3D Gizmos",							u8"3D ����� ǥ��"						},
	{ "Draw picking data",						u8"��ŷ ������ ǥ��"					},
	{ "Draw grid",								u8"�׸��� ǥ��"							},
	{ "Draw shadow map",						u8"�׸��� �� ǥ��"						},

	// Scene editor
	{ "Clear scene",							u8"��� ����"							},
	{ "Save scene",								u8"��� ����"							},
	{ "Load scene",								u8"��� ����"							},
	{ "Insert object",							u8"������Ʈ �߰�"						},
	{ "Save object",							u8"������Ʈ ����"						},
	{ "Delete object",							u8"������Ʈ ����"						},
	{ "Object & Instance",						u8"������Ʈ & �ν��Ͻ�"					},
	{ "To do",									u8"����"								},
	{ "3D Object",								u8"3D ������Ʈ"							},
	{ "+Inst",									u8"+�ν�"								},
	{ "-Inst",									u8"-�ν�"								},
	{ "Change name",							u8"�̸� ����"							},
	{ "Change object/instance name",			u8"������Ʈ/�ν��Ͻ� �̸� ����"			},
	{ "2D Object",								u8"2D ������Ʈ"							},
	{ "Camera",									u8"ī�޶�"								},

	// Test window
	{ "Init Test",								u8"�׽�Ʈ ����"							},
	{ "Quit Test",								u8"�׽�Ʈ ����"							},
	{ "Control panel",							u8"������"								},
	{ "Physics engine",							u8"���� ����"							},
	{ "Stop updating",							u8"���� ����"							},
	{ "Start updating",							u8"���� �簳"							},
	{ "Update speed",							u8"���� �ӷ�"							},
	{ "To next frame",							u8"���� ����������"						},
	{ "Delta time (s)",							u8"��Ÿ Ÿ�� (s)"						},

	// Terrain generator
	{ "Terrain generator",						u8"���� ������"							},
	{ "Width",									u8"����"								},
	{ "Height",									u8"����"								},

	// Object generator
	{ "Object generator",						u8"������Ʈ ������"						},
	{ "3D primitive",							u8"3D ����"								},
	{ "3D model file",							u8"3D �� ����"						},
	{ "2D primitive",							u8"2D ����"								},
	{ "Square (XY)",							u8"���簢�� (XY)"						},
	{ "Square (XZ)",							u8"���簢�� (XZ)"						},
	{ "Square (YZ)",							u8"���簢�� (YZ)"						},
	{ "Circle",									u8"��"									},
	{ "Cube",									u8"������ü"							},
	{ "Pyramid",								u8"����"								},
	{ "Prism",									u8"�����"								},
	{ "Sphere",									u8"��"									},
	{ "Torus",									u8"����"								},
	{ "Object name",							u8"������Ʈ �̸�"						},
	{ "Radius factor",							u8"������ �μ�"							},
	{ "Side count",								u8"���� ��"								},
	{ "Inner radius",							u8"�� ������"							},
	{ "Segment count",							u8"���׸�Ʈ ��"							},
	{ "Load model file",						u8"�� ���� �ҷ�����"					},
	{ "Is rigged",								u8"���� ����"							},
	{ "Square",									u8"���簢��"							},

	{ "Animation texture",						u8"�ִϸ��̼� �ؽ�ó"					},
	{ "Foliage model",							u8"�ʸ� ��"							},
};

static const char* KGUIString_MB[][2]
{
	{ "SpriteFont creation",						"��������Ʈ ��Ʈ ����"						},
	{ "No created device",							"����̽��� �������� �ʾҽ��ϴ�"			},
	{ "Creation failure",							"���� ����"									},
	{ "SkyData loading",							"��ī�� ������ �ε�"						},
	{ "Failed to create sprite font",				"��������Ʈ ��Ʈ ���� ����"					},
	{ "Failed to find sky data file",				"��ī�� ������ ������ ã�� �� �����ϴ�"		},

	{ "Camera creation",							"ī�޶� ����"								},
	{ "Unusable name",								"�� �� ���� �̸��Դϴ�"					},
	{ "Name collision",								"�ߺ��� �̸��Դϴ�"							},
	{ "Name too long",								"�̸��� �ʹ� ��ϴ�"						},
	{ "Name can't be null string",					"�̸��� ������ �� �����ϴ�"					},

	{ "Camera deletion",							"ī�޶� ����"								},
	{ "Failed to find the name",					"�������� �ʴ� �̸��Դϴ�"					},

	{ "Camera getting",								"ī�޶� ������"							},

	{ "Object3D creation",							"������Ʈ3D ����"							},
	{ "Object3D name change",						"������Ʈ3D �̸� ����"						},
	{ "Object3D deletion",							"������Ʈ3D ����"							},
	{ "Object3D getting",							"������Ʈ3D ������"						},

	{ "Object3DLine creation",						"������Ʈ3D���� ����"						},
	{ "Object3DLine getting",						"������Ʈ3D���� ������"					},

	{ "Object2D creation",							"������Ʈ2D ����"							},
	{ "Object2D deletion",							"������Ʈ2D ����"							},
	{ "Object2D getting",							"������Ʈ2D ������"						},

	{ "BMFontRendeer creation",						"BMFont������ ����"							},

	{ "Mode setting",								"��� ����"									},
	{ "First set a player camera",					"���� �÷��̾� ī�޶� ������ �ּ���"		},
	{ "First set a player object",					"���� �÷��̾� ������Ʈ�� ������ �ּ���"	},

	{ "Load a model file",							"�� ������ �ҷ�������"					},
	{ "Model loading",								"�� �ҷ�����"								},
};

static const char* KGUIString_DLG[][2]
{
	{	"Terrain file(*.terr)\0*.terr\0",
		"���� ����(*.terr)\0*.terr\0"	},
	{	"FBX file\0*.fbx\0MESH file\0*.mesh\0OB3D file\0*.ob3d\0All files\0*.*\0",
		"FBX ����\0*.fbx\0MESH ����\0*.mesh\0OB3D ����\0*.ob3d\0��� ����\0*.*\0"	},
	{	"Animation texture file(*.dds)\0*.dds\0",
		"�ִϸ��̼� �ؽ�ó ����(*.dds)\0*.dds\0"	},
	{	"FBX file\0*.fbx\0",
		"FBX ����\0*.fbx\0"	},
	{	"Pattern file(*.ptrn)\0*.ptrn\0",
		"���� ����(*.ptrn)\0*.ptrn\0"	},
	{	"DDS file\0*.dds\0HDR file\0*.hdr\0",
		"DDS ����\0*.dds\0HDR ����\0*.hdr\0"	},
	{	"DDS file\0*.dds\0",
		"DDS ����\0*.dds\0"	},
	{	"Scene file(*.scene)\0*.scene\0",
		"��� ����(*.scene)\0*.scene\0"	},
	{	"MESH file\0*.mesh\0OB3D file\0*.ob3d\0",
		"MESH ����\0*.mesh\0OB3D ����\0*.ob3d\0"	},
};

static const char* KGUIString_DLG_Caption[][2]
{
	{	"Terrain file",							"���� ����"						},
	{	"3D model file",						"3D �� ����"					},
	{	"Animation texture file",				"�ִϸ��̼� �ؽ�ó ����"		},
	{	"Foliage model file",					"�ʸ� �� ����"				},
	{	"Pattern file",							"���� ����"						},
	{	"Environment map file",					"ȯ�� �� ����"					},
	{	"Irradiance map file",					"���� �� ����"					},
	{	"Prefiltered radiance map file",		"�������͸��� �ֵ� �� ����"		},
	{	"Integrated BRDF map file",				"������ BRDF �� ����"			},
	{	"Scene file",							"��� ����"						},
};

enum class EGUILanguageID
{
	English,
	Korean
};

enum class EGUIString_Menu
{
	Terrain,
	Terrain_Create,
	Terrain_Load,
	Terrain_Save,
	Window,
	Window_PropertyEditor,
	Window_SceneEditor,
	Quit
};

enum class EGUIString_PropertyEditor
{
	Tab_Object,
	Tab_Terrain,
	Tab_Scene,
	Tab_IBL,
	Tab_ETC,

	Item_PhysicsProperties,
	Item_BoundingVolume,
	Item_Tessellation,
	Item_Material,
	Item_Animation,
	Item_ObjectIntelligence,
	Item_InstanceIntelligence,
};

enum class EGUIString_Content
{
	// Object tab
	PleaseSelectAnObject,
	SelectedObject,
	VertexCount,
	TriangleCount,

	PleaseSelectAnInstance,
	SelectedInstance,

	Translation,
	Rotation,
	RotationPitch,
	RotationYaw,
	Scaling,

	RoleOfTheObject,
	
	None,
	Player,
	Environment,
	Monster,

	Velocity,

	BoundingSphere,
	AxisAlignedBoundingBox,

	OuterBoundingSphere,
	Center,
	RadiusBias,
	Radius_Auto,
	Radius,
	InnerBoundingVolumes,

	Insert,
	Delete,
	Edit,
	Save,
	Load,
	Confirm,
	Cancel,
	Close,
	BoundingVolumeType,
	XHalfSize,
	YHalfSize,
	ZHalfSize,

	UseTessellation,
	EdgeTessellationFactor,
	InsideTessellationFactor,
	DisplacementFactor,

	IgnoreSceneMaterial,
	ChangeMaterialName,

	BaseColor,
	DiffuseColor,
	AmbientColor,
	SpecularColor,
	SpecularExponent,
	SpecularIntensity,
	Roughness,
	Metalness,
	Texture,
	ClearAllTheTextures,
	Normal,
	Opacity,
	AmbientOcclusion,
	Displacement,

	IsGPUSkinned,
	Yes,
	No,
	AnimationCount,
	AnimationList,
	InstanceAnimationID,
	ObjectAnimationID,
	InsertAnimation,
	AnimationName,
	FileName,
	EditAnimation,
	RegisteredType,
	TicksPerSecond,
	BehaviorStartTick,

	Pattern,
	SetPattern,
	FreePattern,
	PatternList,
	NoPatternFound,

	CurrentCamera,
	GoBackToEditorCamera,
	SelectedCamera,
	CameraType,
	FirstPerson,
	ThirdPerson,
	FreeLook,
	ZoomDistance,
	MovementSpeed,
	BePlayerCamera,
	BeCurrentCamera,

	SelectedLight,
	LightType,
	PointLight,
	SpotLight,
	Color,
	Orientation,
	Range,
	Theta,

	// Terrain tab
	Width_Height,
	MaskingDetail,
	TerrainTessellationFactor,
	DrawWater,
	WaterYPosition,
	WaterTessellationFactor,

	SetHeightMode,
	ChangeHeightMode,
	AverageHeightMode,
	MaskingMode,
	FoliagePlacingMode,

	HeightPrime,
	MaskingLayer,
	MaskingRatio,
	MaskingAttenuation,

	GenerateFoliageCluster,
	FoliagePlacingDetail,
	FoliageDensity,
	WindVelocity,
	WindRadius,
	SelectionRadius,
	SelectionPosition,
	FoliageGenerator,

	InsertMaterial,
	PleaseCreateOrLoadTerrain,
	LoadTextureFromFile,
	FreeTexture,
	NewName,
	UseDefault,

	// Scene tab
	WorldFloorHeight,
	SceneMaterial,
	IntelligencePattern,
	Content,
	PatternFileViewer,

	// IBL tab
	EnvironmentMap,
	CurrentMap,
	IrradianceMap,
	GenerateMap,
	PrefilteredRadianceMap,
	IntegratedBRDFMap,

	// ETC tab
	Light,
	DirectionalLightPosition,
	DirectionalLightColor,
	AmbientLightColor,
	AmbientLightIntensity,
	Exposure_HDR,
	FramesPerSecond_FPS,
	EditorFlags,
	WireFrame,
	DrawNormals,
	DrawMiniAxes,
	DrawBoundingVolumes,
	DrawTerrainHeightMap,
	DrawTerrainMaskingMap,
	DrawTerrainFoliageMap,
	DrawIdentifiers,
	EngineFlags,
	DrawLightingVolumes,
	Draw3DGizmos,
	DrawPickingData,
	DrawGrid,
	DrawShadowMap,

	ClearScene,
	SaveScene,
	LoadScene,
	InsertObject,
	SaveObject,
	_DeleteObject,
	Object_Instance,
	ToDo,
	_3DObject,
	Plus_Inst,
	Minus_Inst,
	ChangeName,
	ChangeObject_InstanceName,
	_2DObject,
	Camera,

	// Test window
	InitTest,
	QuitTest,
	ControlPanel,
	PhysicsEngine,
	StopUpdating,
	StartUpdating,
	UpdateRate,
	ToNextFrame,
	DeltaTime_s,

	// Terrain generator
	TerrainGenerator,
	Width,
	Height,

	// Object generator
	ObjectGenerator,
	_3DPrimitive,
	_3DModelFile,
	_2DPrimitive,
	Square_XY,
	Square_XZ,
	Square_YZ,
	Circle,
	Cube,
	Pyramid,
	Prism,
	Sphere,
	Torus,
	ObjectName,
	RadiusFactor,
	SideCount,
	InnerRadius,
	SegmentCount,
	LoadModelFile,
	IsRigged,
	Square,

	AnimationTexture,
	FoliageModel,
};

enum class EGUIString_MB
{
	SpriteFontCreation,
	NoCreatedDevice,
	CreationFailure,
	SkyDataLoading,
	FailedToCreateSpriteFont,
	FailedToFindSkyDataFile,

	CameraCreation,
	UnusableName,
	NameCollision,
	NameTooLong,
	NameCantBeNullString,

	CameraDeletion,
	FailedToFindTheName,

	CameraGetting,

	Object3DCreation,
	Object3DNameChange,
	Object3DDeletion,
	Object3DGetting,

	Object3DLineCreation,
	Object3DLineGetting,

	Object2DCreation,
	Object2DDeletion,
	Object2DGetting,

	BMFontRendererCreation,

	ModeSetting,
	FirstSetAPlayerCamera,
	FirstSetAPlayerObject,

	LoadAModelFile,
	ModelLoading,
};

enum class EGUIString_DLG
{
	TerrainFile,
	FBX_MESH_OB3D_All,
	AnimationTextureFile,
	FBX,
	PatternFile,
	DDS_HDR,
	DDS,
	SceneFile,
	MESH_OB3D,
};

enum class EGUIString_DLG_Caption
{
	TerrainFile,
	_3DModelFile,
	AnimationTextureFile,
	FoliageModelFile,
	PatternFile,
	EnvironmentMapFile,
	IrradianceMapFile,
	PrefilteredRadianceMapFile,
	IntegratedBRDFMapFile,
	SceneFile,
};

#define GUI_STRING_MENU(id) KGUIString_Menu[(size_t)id][(size_t)m_eLanguage]
#define GUI_STRING_PROPERTY_EDITOR(id) KGUIString_PropertyEditor[(size_t)id][(size_t)m_eLanguage]
#define GUI_STRING_CONTENT(id) KGUIString_Content[(size_t)id][(size_t)m_eLanguage]
#define GUI_STRING_MB(id) KGUIString_MB[(size_t)id][(size_t)m_eLanguage]
#define GUI_STRING_DLG(id) KGUIString_DLG[(size_t)id][(size_t)m_eLanguage]
#define GUI_STRING_DLG_CAPTION(id) KGUIString_DLG_Caption[(size_t)id][(size_t)m_eLanguage]
