#ifndef PLAYER__H
#define PLAYER__H
#include <string>
#include <vector>
#include <cmath>
#include "drawable.h"
#include "explodingSprite.h"
#include "bulletPool.h"

class Player : public Drawable {
public:
  Player(const std::string&);
  Player(const Player&);
  virtual ~Player() {
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

  void right();
  void left();
  void up();
  void down();
  void stop();

  virtual void explode();

private:
  std::vector<Image *> images;
  std::vector<Image *> imagesRight;
  std::vector<Image *> imagesLeft;
  std::vector<Image *> bobbing;
  std::vector<Image *> bobbingLeft;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int worldWidth;
  int worldHeight;

  ExplodingSprite* explosion;

  Vector2f initialVelocity;

  void advanceFrame(Uint32 ticks);
  Player& operator=(const Player&);
};
#endif
