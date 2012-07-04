#ifndef SOCCER_IMAGE_H
#define SOCCER_IMAGE_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL/gl.h>

#include "common/Color.h"
#include "common/Texture.h"
#include "common/Rectangle.h"

#include "soccer/gui/Widget.h"

namespace Soccer {

class Image : public Widget {
	public:
		Image(const char* filepath, const Common::Rectangle& dim);
		const boost::shared_ptr<Common::Texture> getTexture() const;

	private:
		boost::shared_ptr<Common::Texture> mTexture;
};

}

#endif

