#include <stdexcept>

#include "common/Texture.h"

#include "soccer/gui/Image.h"

namespace Soccer {

using namespace Common;

Image::Image(const char* filepath, const Common::Rectangle& dim)
	: Widget(dim)
{
	mTexture = boost::shared_ptr<Texture>(new Texture(filepath, 0, 0));
}

const boost::shared_ptr<Texture> Image::getTexture() const
{
	return mTexture;
}


}
