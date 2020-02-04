#pragma once

static const char* KGUIString_Menu[][2]
{
	{ "Terrain",								u8"지형"								},
	{ "Create",									u8"만들기"								},
	{ "Load",									u8"불러오기"							},
	{ "Save",									u8"내보내기"							},
	{ "Window",									u8"창"									},
	{ "Property editor",						u8"속성 편집기"							},
	{ "Scene editor",							u8"장면 편집기"							},
	{ "Quit",									u8"종료"								},
};

static const char* KGUIString_PropertyEditor[][2]
{
	{ "Object",									u8"오브젝트"							},
	{ "Terrain",								u8"지형"								},
	{ "Scene",									u8"장면"								},
	{ "IBL",									u8"IBL"									},
	{ "ETC",									u8"기타"								},

	{ "Physics Properties",						u8"물리 속성"							},
	{ "Bounding Volume",						u8"바운딩 볼륨"							},
	{ "Tessellation",							u8"테셀레이션"							},
	{ "Material",								u8"재질"								},
	{ "Animation",								u8"애니메이션"							},
	{ "Object Intelligence",					u8"오브젝트 지능"						},
	{ "Instance Intelligence",					u8"인스턴스 지능"						},
};

static const char* KGUIString_Content[][2]
{
	// Object tab
	{ "<Please select an object>",				u8"<오브젝트를 선택하세요>"				},
	{ "Selected object",						u8"선택된 오브젝트:"					},
	{ "Vertex count",							u8"정점 개수"							},
	{ "Triangle count",							u8"삼각형 개수"							},

	{ "<Please select an instance>",			u8"<인스턴스를 선택하세요>"				},
	{ "Selected instance",						u8"선택된 인스턴스"						},

	{ "Tranlsation",							u8"위치"								},
	{ "Rotation",								u8"회전"								},
	{ "Rotation Pitch",							u8"회전 Pitch"							},
	{ "Rotation Yaw",							u8"회전 Yaw"							},
	{ "Scaling",								u8"크기"								},

	{ "Role of the object",						u8"오브젝트 역할:"						},

	{ "None",									u8"없음"								},
	{ "Player",									u8"플레이어"							},
	{ "Enviroment",								u8"환경"								},
	{ "Monster",								u8"몬스터"								},

	{ "Velocity",								u8"속도"								},

	{ "BoundingSphere",							u8"바운딩 구"							},
	{ "AxisAlignedBoundingBox",					u8"바운딩 AABB"							},

	{ "Outer bounding sphere",					u8"외부 바운딩 구"						},
	{ "Center",									u8"중심"								},
	{ "Radius bias",							u8"반지름 편향치"						},
	{ "Radius (auto)",							u8"반지름 (자동)"						},
	{ "Radius",									u8"반지름"								},
	{ "Inner bounding volumes",					u8"내부 바운딩 볼륨"					},

	{ "Insert",									u8"추가"								},
	{ "Delete",									u8"제거"								},
	{ "Edit",									u8"편집"								},
	{ "Save",									u8"저장"								},
	{ "Load",									u8"열기"								},
	{ "Confirm",								u8"결정"								},
	{ "Cancel",									u8"취소"								},
	{ "Close",									u8"닫기"								},
	{ "Bounding volume type",					u8"바운딩 볼륨 유형"					},
	{ "X half size",							u8"X 절반 크기"							},
	{ "Y half size",							u8"Y 절반 크기"							},
	{ "Z half size",							u8"Z 절반 크기"							},

	{ "Use tessellation",						u8"테셀레이션 사용 여부"				},
	{ "Edge tessellation factor",				u8"테셀레이션 변 계수"					},
	{ "Inside tessellation factor",				u8"테셀레이션 내부 계수"				},
	{ "Displacement factor",					u8"변위 계수"							},

	{ "Ignore scene material",					u8"장면 재질 무시하기"					},
	{ "Change material name",					u8"재질 이름 변경"						},
	{ "Base color",								u8"베이스 색상"							},
	{ "Diffuse color",							u8"디퓨즈 색상"							},
	{ "Ambient color",							u8"앰비언트 색상"						},
	{ "Specular color",							u8"스페큘러 색상"						},
	{ "Specular exponent",						u8"스페큘러 지수"						},
	{ "Specular intensity",						u8"스페큘러 세기"						},
	{ "Roughness",								u8"거칠기"								},
	{ "Metalness",								u8"금속성"								},
	{ "Texture",								u8"텍스처"								},
	{ "Clear all the textures",					u8"모든 텍스처 해제"					},
	{ "Normal",									u8"법선"								},
	{ "Opacity",								u8"불투명도"							},
	{ "Ambient occlusion",						u8"앰비언트 오클루젼"					},
	{ "Displacement",							u8"변위"								},

	{ "Is GPU skinned",							u8"GPU 스키닝 여부"						},
	{ "Yes",									u8"예"									},
	{ "No",										u8"아니오"								},
	{ "Animation count",						u8"애니메이션 개수"						},
	{ "Animation list",							u8"애니메이션 목록"						},
	{ "Instance animation ID",					u8"인스턴스 애니메이션 ID"				},
	{ "Object animation ID",					u8"오브젝트 애니메이션 ID"				},
	{ "Insert animation",						u8"애니메이션 추가"						},
	{ "Animation name",							u8"애니메이션 이름"						},
	{ "File name",								u8"파일 이름"							},
	{ "Edit animation",							u8"애니메이션 수정"						},
	{ "Registered Type",						u8"등록된 유형"							},
	{ "Ticks per second",						u8"초당 Tick 수"						},
	{ "Behavior start tick",					u8"행동 개시 Tick"						},

	{ "Pattern",								u8"패턴"								},
	{ "Set pattern",							u8"패턴 지정"							},
	{ "Free pattern",							u8"패턴 해제"							},
	{ "Pattern list",							u8"패턴 목록"							},
	{ "No pattern found",						u8"패턴이 존재하지 않습니다"			},

	{ "Current camera",							u8"현재 화면 카메라"					},
	{ "Go back to Editor Camera",				u8"에디터 카메라로 돌아가기"			},
	{ "Selected camera",						u8"선택된 카메라"						},
	{ "Camera type",							u8"카메라 종류"							},
	{ "First person",							u8"1인칭"								},
	{ "Third person",							u8"3인칭"								},
	{ "Free look",								u8"자유 시점"							},
	{ "Zoom distance",							u8"줌 거리"								},
	{ "Movement speed",							u8"이동 속력"							},
	{ "Be player camera",						u8"플레이어 카메라로"					},
	{ "Be current camera",						u8"현재 화면 카메라로"					},

	{ "Selected light",							u8"선택된 광원"							},
	{ "Light type",								u8"광원 종류"							},
	{ "Point light",							u8"점 광원"								},
	{ "Spot light",								u8"스포트 라이트"						},
	{ "Color",									u8"색상"								},
	{ "Orientation",							u8"방향"								},
	{ "Range",									u8"범위"								},
	{ "Theta",									u8"각도(Theta)"							},

	// Terrain tab
	{ "Width * Height",							u8"가로 * 세로"							},
	{ "Masking detail",							u8"마스킹 수준"							},
	{ "Terrain tessellation factor",			u8"지형 테셀레이션 계수"				},
	{ "Draw water",								u8"물 그리기"							},
	{ "Water Y position",						u8"물 Y 위치"							},
	{ "Water tessellation factor",				u8"물 테셀레이션 계수"					},

	{ "<Set-height mode>",						u8"<높이 지정 모드>"					},
	{ "<Change-height mode>",					u8"<높이 변경 모드>"					},
	{ "<Average-height mode>",					u8"<높이 평균 모드>"					},
	{ "<Masking mode>",							u8"<마스킹 모드>"						},
	{ "<Foliage placing mode>",					u8"<초목 배치 모드>"					},

	{ "Height prime",							u8"지정할 높이"							},
	{ "Masking layer",							u8"마스킹 레이어"						},
	{ "Masking ratio",							u8"마스킹 배합율"						},
	{ "Masking attenuation",					u8"마스킹 감쇠"							},

	{ "Generate foliage cluster",				u8"초목 클러스터 생성"					},
	{ "Foliage placing detail",					u8"초목 배치 수준"						},
	{ "Foliage density",						u8"초목 밀도"							},
	{ "Wind velocity",							u8"바람 속도"							},
	{ "Wind radius",							u8"바람 반지름"							},
	{ "Selection radius",						u8"선택 반지름"							},
	{ "Selection position",						u8"선택 위치"							},
	{ "Foliage generator",						u8"초목 생성기"							},

	{ "Insert material",						u8"재질 추가"							},
	{ "<Please create or load terrain>",		u8"<지형을 생성하거나 불러오세요>"		},
	{ "Load texture from file",					u8"파일에서 텍스처 불러오기"			},
	{ "Free texture",							u8"텍스처 해제하기"						},
	{ "New name",								u8"새 이름"								},
	{ "Use default",							u8"기본값 사용"							},

	// Scene tab
	{ "World floor height",						u8"월드 바닥 높이"						},
	{ "Scene material",							u8"장면 재질"							},
	{ "Intelligence pattern",					u8"지능 패턴"							},
	{ "Content",								u8"내용"								},
	{ "Pattern file viewer",					u8"패턴 파일 뷰어"						},

	// IBL tab
	{ "Environment map",						u8"환경 맵"								},
	{ "Current map",							u8"현재 맵"								},
	{ "Irradiance map",							u8"조도 맵"								},
	{ "Generate map",							u8"맵 생성하기"							},
	{ "Prefiltered radiance map",				u8"사전필터링한 휘도 맵"				},
	{ "Integrated BRDF map",					u8"적분한 BRDF 맵"						},

	// ETC tab
	{ "Light",									u8"광원"								},
	{ "Directional light position",				u8"디렉셔널 라이트 위치"				},
	{ "Directional light color",				u8"디렉셔널 라이트 색상"				},
	{ "Ambient light color",					u8"앰비언트 라이트 색상"				},
	{ "Ambient light intensity",				u8"앰비언트 라이트 세기"				},
	{ "Exposure (HDR)",							u8"노출 (HDR)"							},
	{ "Frames per second (FPS)",				u8"초당 프레임 (FPS)"					},
	{ "Editor flags",							u8"에디터 플래그"						},
	{ "Wire frame",								u8"와이어 프레임"						},
	{ "Draw normals",							u8"법선 표시"							},
	{ "Draw mini-axes",							u8"소형 좌표축 표시"					},
	{ "Draw bounding volumes",					u8"바운딩 볼륨 표시"					},
	{ "Draw terrain height map",				u8"지형 높이맵 표시"					},
	{ "Draw terrain masking map",				u8"지형 마스킹맵 표시"					},
	{ "Draw terrain foliage map",				u8"지형 초목맵 표시"					},
	{ "Draw identifiers",						u8"아이디(ID) 표시"						},
	{ "Engine flags",							u8"엔진 플래그"							},
	{ "Draw lighting volumes",					u8"광원 볼륨 표시"						},
	{ "Draw 3D Gizmos",							u8"3D 기즈모 표시"						},
	{ "Draw picking data",						u8"피킹 데이터 표시"					},
	{ "Draw grid",								u8"그리드 표시"							},
	{ "Draw shadow map",						u8"그림자 맵 표시"						},

	// Scene editor
	{ "Clear scene",							u8"장면 비우기"							},
	{ "Save scene",								u8"장면 저장"							},
	{ "Load scene",								u8"장면 열기"							},
	{ "Insert object",							u8"오브젝트 추가"						},
	{ "Save object",							u8"오브젝트 저장"						},
	{ "Delete object",							u8"오브젝트 제거"						},
	{ "Object & Instance",						u8"오브젝트 & 인스턴스"					},
	{ "To do",									u8"할일"								},
	{ "3D Object",								u8"3D 오브젝트"							},
	{ "+Inst",									u8"+인스"								},
	{ "-Inst",									u8"-인스"								},
	{ "Change name",							u8"이름 변경"							},
	{ "Change object/instance name",			u8"오브젝트/인스턴스 이름 변경"			},
	{ "2D Object",								u8"2D 오브젝트"							},
	{ "Camera",									u8"카메라"								},

	// Test window
	{ "Init Test",								u8"테스트 시작"							},
	{ "Quit Test",								u8"테스트 종료"							},
	{ "Control panel",							u8"제어판"								},
	{ "Physics engine",							u8"물리 엔진"							},
	{ "Stop updating",							u8"갱신 중지"							},
	{ "Start updating",							u8"갱신 재개"							},
	{ "Update speed",							u8"갱신 속력"							},
	{ "To next frame",							u8"다음 프레임으로"						},
	{ "Delta time (s)",							u8"델타 타임 (s)"						},

	// Terrain generator
	{ "Terrain generator",						u8"지형 생성기"							},
	{ "Width",									u8"가로"								},
	{ "Height",									u8"세로"								},

	// Object generator
	{ "Object generator",						u8"오브젝트 생성기"						},
	{ "3D primitive",							u8"3D 도형"								},
	{ "3D model file",							u8"3D 모델 파일"						},
	{ "2D primitive",							u8"2D 도형"								},
	{ "Square (XY)",							u8"정사각형 (XY)"						},
	{ "Square (XZ)",							u8"정사각형 (XZ)"						},
	{ "Square (YZ)",							u8"정사각형 (YZ)"						},
	{ "Circle",									u8"원"									},
	{ "Cube",									u8"정육면체"							},
	{ "Pyramid",								u8"각뿔"								},
	{ "Prism",									u8"각기둥"								},
	{ "Sphere",									u8"구"									},
	{ "Torus",									u8"도넛"								},
	{ "Object name",							u8"오브젝트 이름"						},
	{ "Radius factor",							u8"반지름 인수"							},
	{ "Side count",								u8"옆면 수"								},
	{ "Inner radius",							u8"띠 반지름"							},
	{ "Segment count",							u8"세그먼트 수"							},
	{ "Load model file",						u8"모델 파일 불러오기"					},
	{ "Is rigged",								u8"리깅 여부"							},
	{ "Square",									u8"정사각형"							},

	{ "Animation texture",						u8"애니메이션 텍스처"					},
	{ "Foliage model",							u8"초목 모델"							},
};

static const char* KGUIString_MB[][2]
{
	{ "SpriteFont creation",						"스프라이트 폰트 생성"						},
	{ "No created device",							"디바이스가 생성되지 않았습니다"			},
	{ "Creation failure",							"생성 실패"									},
	{ "SkyData loading",							"스카이 데이터 로딩"						},
	{ "Failed to create sprite font",				"스프라이트 폰트 생성 실패"					},
	{ "Failed to find sky data file",				"스카이 데이터 파일을 찾을 수 없습니다"		},

	{ "Camera creation",							"카메라 생성"								},
	{ "Unusable name",								"고를 수 없는 이름입니다"					},
	{ "Name collision",								"중복된 이름입니다"							},
	{ "Name too long",								"이름이 너무 깁니다"						},
	{ "Name can't be null string",					"이름은 공백일 수 없습니다"					},

	{ "Camera deletion",							"카메라 삭제"								},
	{ "Failed to find the name",					"존재하지 않는 이름입니다"					},

	{ "Camera getting",								"카메라 얻어오기"							},

	{ "Object3D creation",							"오브젝트3D 생성"							},
	{ "Object3D name change",						"오브젝트3D 이름 변경"						},
	{ "Object3D deletion",							"오브젝트3D 삭제"							},
	{ "Object3D getting",							"오브젝트3D 얻어오기"						},

	{ "Object3DLine creation",						"오브젝트3D라인 생성"						},
	{ "Object3DLine getting",						"오브젝트3D라인 얻어오기"					},

	{ "Object2D creation",							"오브젝트2D 생성"							},
	{ "Object2D deletion",							"오브젝트2D 삭제"							},
	{ "Object2D getting",							"오브젝트2D 얻어오기"						},

	{ "BMFontRendeer creation",						"BMFont렌더러 생성"							},

	{ "Mode setting",								"모드 지정"									},
	{ "First set a player camera",					"먼저 플레이어 카메라를 지정해 주세요"		},
	{ "First set a player object",					"먼저 플레이어 오브젝트를 지정해 주세요"	},

	{ "Load a model file",							"모델 파일을 불러오세요"					},
	{ "Model loading",								"모델 불러오기"								},
};

static const char* KGUIString_DLG[][2]
{
	{	"Terrain file(*.terr)\0*.terr\0",
		"지형 파일(*.terr)\0*.terr\0"	},
	{	"FBX file\0*.fbx\0MESH file\0*.mesh\0OB3D file\0*.ob3d\0All files\0*.*\0",
		"FBX 파일\0*.fbx\0MESH 파일\0*.mesh\0OB3D 파일\0*.ob3d\0모든 파일\0*.*\0"	},
	{	"Animation texture file(*.dds)\0*.dds\0",
		"애니메이션 텍스처 파일(*.dds)\0*.dds\0"	},
	{	"FBX file\0*.fbx\0",
		"FBX 파일\0*.fbx\0"	},
	{	"Pattern file(*.ptrn)\0*.ptrn\0",
		"패턴 파일(*.ptrn)\0*.ptrn\0"	},
	{	"DDS file\0*.dds\0HDR file\0*.hdr\0",
		"DDS 파일\0*.dds\0HDR 파일\0*.hdr\0"	},
	{	"DDS file\0*.dds\0",
		"DDS 파일\0*.dds\0"	},
	{	"Scene file(*.scene)\0*.scene\0",
		"장면 파일(*.scene)\0*.scene\0"	},
	{	"MESH file\0*.mesh\0OB3D file\0*.ob3d\0",
		"MESH 파일\0*.mesh\0OB3D 파일\0*.ob3d\0"	},
};

static const char* KGUIString_DLG_Caption[][2]
{
	{	"Terrain file",							"지형 파일"						},
	{	"3D model file",						"3D 모델 파일"					},
	{	"Animation texture file",				"애니메이션 텍스처 파일"		},
	{	"Foliage model file",					"초목 모델 파일"				},
	{	"Pattern file",							"패턴 파일"						},
	{	"Environment map file",					"환경 맵 파일"					},
	{	"Irradiance map file",					"조도 맵 파일"					},
	{	"Prefiltered radiance map file",		"사전필터링한 휘도 맵 파일"		},
	{	"Integrated BRDF map file",				"적분한 BRDF 맵 파일"			},
	{	"Scene file",							"장면 파일"						},
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
