#ifndef __C_GUI_SLIDEIMAGES_H_INCLUDED__
#define __C_GUI_SLIDEIMAGES_H_INCLUDED__


#include "IGUISlideImages.h"


namespace irr
{
namespace gui
{

	class CGUISlideImages : public IGUISlideImages
	{
	public:

		//! constructor
		CGUISlideImages(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		virtual ~CGUISlideImages();

		virtual void draw();

		virtual void OnPostRender(u32 timeMs);

		virtual bool addTexture(video::ITexture* image);

		virtual void SetSlideValue(float SlideValue);
		virtual float GetSlideValue() {return m_SlideValue;}

	private:
		core::array<video::ITexture*> m_Textures;

		video::SColor Color;

		float	m_SlideValue;

	};


}	// end namespace gui
}	// end namespace irr

#endif // __C_GUI_SLIDEIMAGES_H_INCLUDED__