#include "twowaymultisprite.h"
#include "gameData.h"
#include "imageFactory.h"

void twoWayMultiSprite::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

twoWayMultiSprite::twoWayMultiSprite( const std::string& name) :
  Drawable(name+"Right",
           Vector2f(Gamedata::getInstance().getXmlInt(name+"Right"+"/startLoc/x"),
                    Gamedata::getInstance().getXmlInt(name+"Right"+"/startLoc/y")),
           Vector2f(Gamedata::getInstance().getXmlInt(name+"Right"+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"Right"+"/speedY"))
           ),
  images( ImageFactory::getInstance().getImages(name+"Right") ),
  imagesRight( ImageFactory::getInstance().getImages(name+"Right") ),
  imagesLeft( ImageFactory::getInstance().getImages(name+"Left") ),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"Right"+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"Right"+"/frameInterval")),
  timeSinceLastFrame( 0 ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
	explosion(nullptr)
{ }

twoWayMultiSprite::twoWayMultiSprite(const twoWayMultiSprite& s) :
  Drawable(s),
  images(s.images),
  imagesRight(s.imagesRight),
  imagesLeft(s.imagesLeft),
  currentFrame(s.currentFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
	explosion( s.explosion )
  { }

twoWayMultiSprite& twoWayMultiSprite::operator=(const twoWayMultiSprite& s) {
  Drawable::operator=(s);
  images = (s.images);
	imagesRight = (s.imagesRight);
	imagesLeft = (s.imagesLeft);
  currentFrame = (s.currentFrame);
  numberOfFrames = ( s.numberOfFrames );
  frameInterval = ( s.frameInterval );
  timeSinceLastFrame = ( s.timeSinceLastFrame );
  worldWidth = ( s.worldWidth );
  worldHeight = ( s.worldHeight );
	explosion = ( s.explosion );
  return *this;
}

void twoWayMultiSprite::draw() const {
	if ( explosion ) explosion->draw();
	else images[currentFrame]->draw(getX(), getY(), getScale());}

void twoWayMultiSprite::update(Uint32 ticks) {
	if ( explosion ) {
    explosion->update(ticks);
    if ( explosion->chunkCount() == 0 ) {
      delete explosion;
      explosion = nullptr;
    }
    return;
  }

  advanceFrame(ticks);

  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if ( getY() < 0) {
    setVelocityY( std::abs( getVelocityY() ) );
  }
  if ( getY() > worldHeight-getScaledHeight()) {
    setVelocityY( -std::abs( getVelocityY() ) );
  }

  if ( getX() < 0) {
    setVelocityX( std::abs( getVelocityX() ) );
    images = imagesRight;
  }
  if ( getX() > worldWidth-getScaledWidth()) {
    setVelocityX( -std::abs( getVelocityX() ) );
    images = imagesLeft;
  }

}

void twoWayMultiSprite::explode() {
	if ( !explosion ) {
		Sprite sprite(getName(),getPosition(), getVelocity(), images[currentFrame]);
    sprite.setScale( getScale() );
		explosion = new ExplodingSprite( sprite );
  }
}
