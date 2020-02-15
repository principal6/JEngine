#pragma once

#include "Shape2D.h"

class CImage
{
public:
	CImage(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext);
	~CImage();

public:


private:
	ID3D11Device* 			m_PtrDevice{};
	ID3D11DeviceContext*	m_PtrDeviceContext{};

private:
	CShape2D				m_Shape2D{ m_PtrDevice, m_PtrDeviceContext };
};