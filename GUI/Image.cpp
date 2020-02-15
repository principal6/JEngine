#include "Image.h"
#include "../Core/SharedHeader.h"

CImage::CImage(ID3D11Device* const Device, ID3D11DeviceContext* const DeviceContext) :
	m_PtrDevice{ Device }, m_PtrDeviceContext{ DeviceContext }
{
	assert(m_PtrDevice);
	assert(m_PtrDeviceContext);
}

CImage::~CImage()
{
}
