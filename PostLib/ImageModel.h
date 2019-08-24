#pragma once
#include <vector>
#include <string>
#include <FSCore/box.h>
#include <FSCore/FSObjectList.h>
#include <PostLib/GLImageRenderer.h>
#include "GLObject.h"

class C3DImage;

namespace Post {

class CGLImageRenderer;

class CImageSource : public FSObject
{
public:
	CImageSource();
	~CImageSource();

	void SetFileName(const std::string& fileName);
	std::string GetFileName() const;

	bool LoadImageData(const std::string& fileName, int nx, int ny, int nz);

	C3DImage* Get3DImage() { return m_img; }

	void Save(OArchive& ar);
	void Load(IArchive& ar);

	int Width() const;
	int Height() const;
	int Depth() const;

private:
	C3DImage*	m_img;
};

class CImageModel : public CGLObject
{
public:
	CImageModel(CGLModel* mdl);
	~CImageModel();

	bool LoadImageData(const std::string& fileName, int nx, int ny, int nz, const BOX& box);

	int ImageRenderers() const { return (int)m_render.Size(); }
	CGLImageRenderer* GetImageRenderer(int i) { return m_render[i]; }
	size_t RemoveRenderer(CGLImageRenderer* render);

	void AddImageRenderer(CGLImageRenderer* render);

	const BOX& GetBoundingBox() const { return m_box; }

	void SetBoundingBox(BOX b) { m_box = b; }

	bool ShowBox() const;

	void ShowBox(bool b);

	void Render(CGLContext& rc);

	void UpdateData(bool bsave = true) override;

	void Save(OArchive& ar);
	void Load(IArchive& ar);

	CImageSource* GetImageSource() { return m_img; }

private:
	BOX				m_box;						//!< physical dimensions of image
	bool			m_showBox;					//!< show box in Graphics View
	FSObjectList<CGLImageRenderer>	m_render;	//!< image renderers

	CImageSource*	m_img;
};
}
