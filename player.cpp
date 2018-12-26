#include "player.h"
#include "gameData.h"
#include "imageFactory.h"

void Player::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

// Player constructor from XML string
Player::Player( const std::string& name) :
  // Create Drawable base object from XML data
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"),
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  // Set image factory vectors to sprite images
  images( ImageFactory::getInstance().getImages(name+"Bobbing") ),
  imagesRight( ImageFactory::getInstance().getImages(name+"Right") ),
	imagesLeft( ImageFactory::getInstance().getImages(name+"Left")),
  bobbing( ImageFactory::getInstance().getImages(name+"Bobbing") ),
	bobbingLeft( ImageFactory::getInstance().getImages(name+"BobbingLeft") ),
  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"Bobbing/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"Bobbing/frameInterval")),
  timeSinceLastFrame( 0 ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
	explosion(nullptr),
  initialVelocity(getVelocity())
{ }

// Player initialization list
Player::Player(const Player& s) :
  Drawable(s),
  images(s.images),
  imagesRight(s.imagesRight),
	imagesLeft(s.imagesLeft),
  bobbing(s.bobbing),
	bobbingLeft(s.bobbingLeft),
  currentFrame(s.currentFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
	explosion(nullptr),
  initialVelocity( s.initialVelocity )
  { }

// Player copy assign overload
Player& Player::operator=(const Player& s) {
  Drawable::operator=(s);
  images = (s.images);
	imagesRight = (s.imagesRight);
	imagesLeft = (s.imagesLeft);
	bobbing = (s.bobbing);
	bobbingLeft = (s.bobbingLeft);
  currentFrame = (s.currentFrame);
  numberOfFrames = ( s.numberOfFrames );
  frameInterval = ( s.frameInterval );
  timeSinceLastFrame = ( s.timeSinceLastFrame );
  worldWidth = ( s.worldWidth );
  worldHeight = ( s.worldHeight );
	explosion = ( nullptr );
  initialVelocity = ( s.initialVelocity );
  return *this;
}

// Draw player to the window
void Player::draw() const {
	if ( explosion ) {
		explosion->draw();
	}
  else images[currentFrame]->draw(getX(), getY(), getScale());
}

// Stop player from moving and set to bobbing animation
void Player::stop() {
	if(getVelocityX() < 0) {
		setVelocityX( 0.93*getVelocityX() );
		setVelocityY(0);
		images = bobbingLeft;
	}
	else {
		setVelocityX( 0.93*getVelocityX() );
		setVelocityY(0);
		images = bobbing;
	}
}

// Move player right
void Player::right() {
  if ( getX() < worldWidth-getScaledWidth()) {
    setVelocityX(initialVelocity[0]);
    images = imagesRight;
  }
}

// Move player left
void Player::left()  {
  if ( getX() > 0) {
    setVelocityX(-initialVelocity[0]);
		images = imagesLeft;
  }
}

// Move player up
void Player::up()    {
	if ( getY() > 0) {
    setVelocityY( -initialVelocity[1] );
  }
}

// Move player down
void Player::down()  {
  if ( getY() < worldHeight-getScaledHeight()) {
    setVelocityY( initialVelocity[1] );
  }
}

void Player::explode() {
	if ( !explosion ) {
		Sprite sprite(getName(),getPosition(), getVelocity(), images[0]);
    sprite.setScale( getScale() );
		explosion = new ExplodingSprite(sprite);
  }
}

// Update player based on location and velocity
void Player::update(Uint32 ticks) {
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
    setVelocityY( fabs( getVelocityY() ) );
  }
  if ( getY() > worldHeight-getScaledHeight()) {
    setVelocityY( -fabs( getVelocityY() ) );
  }
  if ( getX() < 0) {
    setVelocityX( fabs( getVelocityX() ) );
  }
  if ( getX() > worldWidth-getScaledWidth()) {
    setVelocityX( -fabs( getVelocityX() ) );
  }

  stop();
}
