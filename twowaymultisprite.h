#ifndef TWOWAYMULTISPRITE__H
#define TWOWAYMULTISPRITE__H
#include <string>
#include <vector>
#include <cmath>
#include "drawable.h"
#include "explodingSprite.h"

class twoWayMultiSprite : public Drawable {
public:
  twoWayMultiSprite(const std::string&);
  twoWayMultiSprite(const twoWayMultiSprite&);
  virtual ~twoWayMultiSprite() {
    if ( explosion ) delete explosion;
  }

  virtual void draw() const;
  virtual void update(Uint32 ticks);

  virtual const Image* getImage() const {
    return images[currentFrame];
  }
  int getScaledWidth()  const {
    return getScale()*images[currentFrame]->getWidth();
  }
  int getScaledHeight()  const {
    return getScale()*images[currentFrame]->getHeight();
  }
  virtual const SDL_Surface* getSurface() const {
    return images[currentFrame]->getSurface();
  }

  virtual void explode();

private:
  std::vector<Image *> images;
  std::vector<Image *> imagesRight;
  std::vector<Image *> imagesLeft;

protected:
  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int worldWidth;
  int worldHeight;

  ExplodingSprite* explosion;

  void advanceFrame(Uint32 ticks);
  twoWayMultiSprite& operator=(const twoWayMultiSprite&);
};
#endif
