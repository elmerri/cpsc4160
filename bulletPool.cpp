#include <iostream>
#include <sstream>
#include <cmath>
#include "ioMod.h"
#include "gameData.h"
#include "bulletPool.h"

CollisionStrategy* getStrategy(const string& name) {
  std::string sName = Gamedata::getInstance().getXmlStr(name+"/strategy");
  if ( sName == "midpoint" ) return new MidPointCollisionStrategy;
  if ( sName == "rectangular" ) return new RectangularCollisionStrategy;
  if ( sName == "perpixel" ) return new PerPixelCollisionStrategy;
  throw std::string("No strategy in getStrategy");
}

BulletPool::~BulletPool() {
  delete strategy;
}

BulletPool::BulletPool(const std::string& n) :
  name(n),
  strategy( getStrategy(name) ),
  frameInterval(Gamedata::getInstance().getXmlInt(name+"/interval")),
  timeSinceLastFrame( 0 ),
  bulletList(),
  freeList()
{ }

BulletPool::BulletPool(const BulletPool& b) :
  name(b.name),
  strategy(b.strategy),
  frameInterval(b.frameInterval),
  timeSinceLastFrame( b.timeSinceLastFrame ),
  bulletList(b.bulletList),
  freeList(b.freeList)
{ }

bool BulletPool::collidedWith(const Drawable* obj) const {
  auto ptr = bulletList.begin();
  while (ptr != bulletList.end()) {
    if ( strategy->execute(*ptr, *obj) ) {
      freeList.emplace_back(*ptr);
      ptr = bulletList.erase(ptr);
      return true;
    }
    ++ptr;
  }
  return false;
}

void BulletPool::shoot(const Vector2f& position, const Vector2f& velocity) {
	if (timeSinceLastFrame > frameInterval) {
    // If no bullets in pool, make one:
    if ( freeList.empty() ) {
      Bullet b(name);
      b.setPosition( position );
      b.setVelocity( velocity );
      bulletList.emplace_back( b );
    }
    else {
      Bullet b = freeList.front();
      freeList.pop_front();
      b.reset();
      b.setVelocity(velocity);
      b.setPosition(position);
      bulletList.emplace_back( b );
    }
		timeSinceLastFrame = 0;
	}
}

void BulletPool::draw() const {
  // SDL_Color poolColor = {0, 140, 140, 0};
  //
  // std::stringstream stream;
  // stream << "Active bullets: " << bulletList.size();
  // IoMod::getInstance().writeText(stream.str(), 30, 60, poolColor);
  // stream.clear();
  // stream.str("");
  // stream << "Bullet pool: " << freeList.size();
  // IoMod::getInstance().writeText(stream.str(), 30, 90, poolColor);
  for ( Bullet bullet : bulletList ) {
    bullet.draw();
  }
}

void BulletPool::update(Uint32 ticks) {
	timeSinceLastFrame += ticks;
  std::list<Bullet>::iterator ptr = bulletList.begin();
  while (ptr != bulletList.end()) {
    ptr->update(ticks);
    if (ptr->goneTooFar()) {  // Check to see if we should free a Bullet
      freeList.emplace_back(*ptr);
      ptr = bulletList.erase(ptr);
    }
    else ++ptr;
  }
}
