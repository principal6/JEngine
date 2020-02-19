#pragma once

#include <string>
#include <memory>
#include "CommonTypes.h"
#include "Primitive2D.h"
#include "../Core/BFNTRenderer.h"
#include "../DirectXTK/DirectXTK.h"

class CShader;
class CConstantBuffer;

// Base class for Widgets
class CWidget abstract
{
	friend class CGUIBase;

	struct SCBSpaceData
	{
		DirectX::XMMATRIX Projection{};
		DirectX::XMFLOAT4 Offset{};
	};

public:
	CWidget(const SWidgetCtorData& CtorData);
	CWidget(const CWidget& b) = delete;
	virtual ~CWidget();

protected:
	void _Create(EWidgetType eType, const std::string& Name, const SInt2& Size, CWidget* const Parent);

public:
	void Draw() const;
	bool ShouldDraw() const;

protected:
	virtual void _Draw() const abstract;
	
protected:
	void DrawCaption() const;
	void DrawCaption(const SInt2& SizeOverride, const SInt2& Offset) const;

public:
	void Capture(const SInt2& MousePosition) const;
	void Release() const;
	bool IsCaptured() const;
	const SInt2& GetCapturedMousePosition() const;
	const SInt2& GetCapturedOffset() const;

public:
	EWidgetType GetType() const;
	const std::string& GetName() const;
	std::string GetFullName() const;
	EWidgetState GetState() const;
	const SInt2& GetPosition() const;
	const SInt2& GetOffset() const;
	const SInt2& GetSize() const;
	CWidget* GetParent() const;
	size_t GetChildCount() const;
	CWidget* GetChild(const std::string& Name) const;
	bool HasParent() const;
	bool HasChild() const;
	bool IsChild(const std::string& Name) const;
	bool IsChild(CWidget* const Widget) const;
	bool IsFocused() const;

public:
	void SetOffset(const SInt2& _Offset);
	void SetSelectionSize(const SInt2& SelectionSize);
	void SetCaption(const std::string& UTF8_Caption);
	void SetCaptionAlign(EHorzAlign eHorzAlign);
	void SetCaptionAlign(EVertAlign eVertAlign);
	void SetCaptionAlign(EHorzAlign eHorzAlign, EVertAlign eVertAlign);
	void SetCaptionColor(const SFloat4& CaptionColor);
	const std::string& GetCaption() const;

protected:
	// @important: generates EEventType::Clicked
	void UpdateState(const SInt2& MousePosition, EEventType& eEventType, bool bMouseDown, 
		CWidget* const LastMouseDownWidget, CWidget* const FocusedWidget);
	virtual void _SetActive() abstract;
	virtual void _SetInactive() abstract;
	void SetState(EWidgetState eState);
	virtual void _SetState() abstract;
	void SetStateTexCoordRange(EWidgetState eWidgetState, const SFloat4& TexCoordRange);
	const SInt2& GetNewChildrenOffset() const;

protected:
	bool IsMouseInside(const SInt2& MousePosition);

protected:
	ID3D11Device*							m_PtrDevice{};
	ID3D11DeviceContext*					m_PtrDeviceContext{};

protected:
	CShader*								m_PtrVS{};
	CShader*								m_PtrPS{};
	CConstantBuffer*						m_PtrCBSpace{};
	SCBSpaceData*							m_PtrCBSpaceData{};
	SFloat2*								m_PtrWindowSize{};

protected:
	EWidgetType								m_eType{};
	std::string								m_Name{};
	CPrimitive2D							m_Primitive2D{ m_PtrDevice, m_PtrDeviceContext };

protected:
	std::unique_ptr<CBFNTRenderer>			m_BFNTRenderer{};
	std::string								m_Caption{};
	XMFLOAT4								m_CaptionColor{ 0, 0, 0, 1 };
	EHorzAlign								m_eCaptionHorzAlign{ EHorzAlign::Center };
	EVertAlign								m_eCaptionVertAlign{ EVertAlign::Middle };

protected:
	SInt2									m_Position{}; // this needs to be automatically calculated by CGUIBase
	SInt2									m_Size{};
	SInt2									m_Offset{};
	SInt2									m_SelectionSize{};

protected:
	EWidgetState							m_eState{};
	SFloat4									m_TexCoordRangeNormal{};
	SFloat4									m_TexCoordRangeHover{};
	SFloat4									m_TexCoordRangePressed{};

protected:
	mutable bool							m_bCaptured{ false };
	mutable SInt2							m_CapturedMousePosition{};
	mutable SInt2							m_CapturedOffset{};

protected:
	bool									m_bPrevIsActive{ false };
	bool									m_bIsFocused{ false };

protected:
	CWidget*								m_Parent{};
	std::vector<CWidget*>					m_vChildren{};
	std::unordered_map<std::string, size_t>	m_umapChildNameToIndex{};
	SInt2									m_NewChildrenOffset{};
};

// Button
class CButton final : public CWidget
{
	friend class CGUIBase;

public:
	CButton(const SWidgetCtorData& CtorData);
	~CButton();

protected:
	void Create(const std::string& Name, const SInt2& Size, CWidget* const Parent);
	void CreatePreset(const std::string& Name, const SInt2& Size, EButtonPreset ePreset, CWidget* const Parent);

protected:
	void _Draw() const override;

protected:
	void _SetActive() override;
	void _SetInactive() override;
	void _SetState() override;

private:
	std::unique_ptr<CPrimitive2D>	m_PresetPrimitive{};
};

// Image
class CImage final : public CWidget
{
	friend class CGUIBase;

public:
	CImage(const SWidgetCtorData& CtorData);
	~CImage();

protected:
	void Create(const std::string& Name, const SInt2& Size, const SFloat4& TexCoordRange, CWidget* const Parent);

public:
	void SetSource(ID3D11ShaderResourceView* const SRV);
	void SetSource(ID3D11ShaderResourceView* const SRV, const SFloat4& TexCoordRange);
	ID3D11ShaderResourceView* GetSource() const;

protected:
	void _Draw() const override;

protected:
	void _SetActive() override;
	void _SetInactive() override;
	void _SetState() override;

private:
	ID3D11ShaderResourceView*	m_SRV{};
};

// Image button
class CImageButton final : public CWidget
{
	friend class CGUIBase;

public:
	CImageButton(const SWidgetCtorData& CtorData);
	~CImageButton();

protected:
	void Create(const std::string& Name, const SInt2& Size, CWidget* const Parent);

protected:
	void _Draw() const override;

protected:
	void _SetActive() override;
	void _SetInactive() override;
	void _SetState() override;
};

// Window
class CWindow final : public CWidget
{
	friend class CGUIBase;

public:
	CWindow(const SWidgetCtorData& CtorData);
	~CWindow();

protected:
	void CreateImage(const std::string& Name, const SInt2& Size, const SFloat4& TexCoordRange, CWidget* const Parent);
	void CreatePrimitive(const std::string& Name, const SInt2& Size, float Roundness, CWidget* const Parent);

public:
	void Open();
	void Close();
	bool IsOpen();

protected:
	void SetTitleBar(const SInt2& TitleBarLeftTop, const SInt2& TitleBarSize);

public:
	bool IsMouseOnTitleBar(const SInt2& MousePosition) const;

protected:
	void _Draw() const override;

protected:
	void _SetActive() override;
	void _SetInactive() override;
	void _SetState() override;

private:
	SInt2							m_TitleBarLeftTop{};
	SInt2							m_TitleBarSize{};
	bool							m_bIsOpen{ false };

private:
	std::unique_ptr<CPrimitive2D>	m_TitleBarPrimitive{};
};

// Text
class CText final : public CWidget
{
	friend class CGUIBase;

public:
	CText(const SWidgetCtorData& CtorData);
	~CText();

protected:
	void Create(const std::string& Name, const SInt2& Size, CWidget* const Parent);

protected:
	void _Draw() const override;

public:
	void SetBackgroundColor(const SFloat4& Color);

protected:
	void _SetActive() override;
	void _SetInactive() override;
	void _SetState() override;
};

// TextEdit
class CTextEdit final : public CWidget
{
	friend class CGUIBase;

public:
	enum class EDirection
	{
		Left,
		Right,
		Home,
		End,
	};

public:
	CTextEdit(const SWidgetCtorData& CtorData);
	~CTextEdit();

private:
	void Create(const std::string& Name, const SInt2& Size, CWidget* const Parent);

protected:
	void _Draw() const override;

private:
	bool InsertChar(const std::string& UTF8_Char);
	void InsertString(const std::string& UTF8_String);
	void MoveCaret(EDirection eDirection, bool bShouldDeselect = true);
	void MoveCaret(const SInt2& MousePosition);
	void Select(EDirection eDirection);
	void Select(const SInt2& MousePosition);
	void Deselect();
	void DeletePreChar();
	void DeletePostChar();
	void DeleteSelection();
	bool HasSelection() const;
	uint32_t GetSelectionLength() const;
	uint32_t GetSelectionByteLength() const;
	std::string GetSelection() const;

private:
	bool CopyToClipboard();
	bool CutToClipboard();
	bool PasteFromClipboard();

private:
	uint32_t _GetCaretAtFromMousePosition(const SInt2& MousePosition) const;

private:
	void _SetActive() override;
	void _SetInactive() override;
	void _SetState() override;

private:
	static constexpr int32_t		KSpaceVert{ 2 };
	static constexpr int32_t		KSpaceHorz{ 3 };
	static constexpr uint32_t		KMaxStringLength{ 1000 };

private:
	std::unique_ptr<CPrimitive2D>	m_CaretPrimitive{};
	uint32_t*						m_PtrCaretBlinkTime{};
	mutable uint32_t				m_CaretTimer{};
	mutable uint32_t				m_PrevTimePoint_ms{};
	uint32_t						m_CaretAt{};
	int32_t							m_CaretOffsetX{};
	int32_t							m_StringOffsetX{};

private:
	std::unique_ptr<CPrimitive2D>	m_SelectionPrimitive{};
	uint32_t						m_SelectionStart{};
	uint32_t						m_SelectionEnd{};
	int32_t							m_SelectionWidth{};
};
