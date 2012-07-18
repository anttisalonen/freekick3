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

void Image::draw(int screenWidth, int screenHeight)
{
	const Rectangle& r = this->getRectangle();
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->getTexture()->getTexture());
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(r.x, screenHeight - r.y, 0.5f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(r.x + r.w, screenHeight - r.y, 0.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(r.x + r.w, screenHeight - r.y - r.h, 0.5f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(r.x, screenHeight - r.y - r.h, 0.5f);
	glEnd();
}

}
