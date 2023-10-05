#ifndef __I_GUI_SLIDEIMAGE_H_INCLUDED__
#define __I_GUI_SLIDEIMAGE_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
	namespace video
	{
		class ITexture;
	}
	namespace gui
	{
		class IGUISlideImages : public IGUIElement
		{
		public:
			IGUISlideImages(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
				: IGUIElement(EGUIET_SLIDE_IMAGES, environment, parent, id, rectangle) {}
			
			virtual bool addTexture(video::ITexture* image) = 0;

			virtual void SetSlideValue(float SlideValue) = 0;

			virtual float GetSlideValue() = 0;
		};
	}
}



#endif // __I_GUI_SLIDEIMAGE_H_INCLUDED__
