#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "world.h"
#include "viewport.h"
#include "hud.h"
#include "player.h"
#include "collisionStrategy.h"
#include "bulletPool.h"
#include "lights.h"
#include "sound.h"

class Engine {
public:
  Engine ();
  ~Engine ();
  bool play();
  void switchSprite();
  void toggleHud();
  void toggleNight();
  void toggleGod();

private:
  const RenderContext& rc;
  const IoMod& io;
  Clock& clock;

  SDL_Renderer * const renderer;

  World world;
  World mountains;
  World hills;
  World night;

  Drawable* snow1;
  Drawable* penguin;
  Drawable* fish;
  std::vector<Drawable*> sprites;

  Hud& hud;
  PerPixelCollisionStrategy* collisionStrat;

  bool viewHud;
  bool isNight;
  bool godMode;

  int points;
  int lives;

  Lights light;
  BulletPool* bullets;
  SDLSound sound;

  Viewport& viewport;

  int currentSprite;

  bool makeVideo;

  void draw() const;
  void update(Uint32);

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
};

#endif
