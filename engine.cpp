#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include <ctime>
#include "sprite.h"
#include "multisprite.h"
#include "twowaymultisprite.h"
#include "player.h"
#include "gameData.h"
#include "engine.h"
#include "frameGenerator.h"
#include "lights.h"

Engine::~Engine() {
  std::cout << "deleting bullet pool" << std::endl;
  delete bullets;
  std::cout << "deleting collision strategy" << std::endl;
  delete collisionStrat;
  std::cout << "deleting penguin" << std::endl;
  delete penguin;
  std::cout << "deleting snowball" << std::endl;
  delete snow1;
  std::cout << "deleting fish" << std::endl;
  delete fish;
  std::cout << "Terminating program" << std::endl;
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IoMod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc.getRenderer() ),
  world("clouds", Gamedata::getInstance().getXmlInt("clouds/factor") ),
  mountains("mountains", Gamedata::getInstance().getXmlInt("mountains/factor") ),
  hills("hills", Gamedata::getInstance().getXmlInt("hills/factor")),
  night("night", Gamedata::getInstance().getXmlInt("night/factor")),
  snow1(new twoWayMultiSprite("snowballRoll")),
  penguin(new Player("penguin")),
  fish(new Sprite("fluffy")),
  sprites(std::vector<Drawable*>()),
  hud(Hud::getInstance()),
  collisionStrat(new PerPixelCollisionStrategy()),
  viewHud( true ),
  isNight( false ),
  godMode( false ),
  points ( 0 ),
  lives ( 3 ),
  light(),
  bullets( new BulletPool("snowball") ),
  sound(),
  viewport( Viewport::getInstance() ),
  currentSprite(0),
  makeVideo( false )
{
  penguin->setScale(0.85);
  snow1->setScale(0.4);
  Viewport::getInstance().setObjectToTrack(penguin);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  if(isNight) night.draw();
  else world.draw();
  mountains.draw();
  hills.draw();

  // Set teal color for on-screen text
  SDL_Color textColor;
  if(isNight) textColor = {255, 255, 255, 0};
  else textColor = {0, 140, 140, 0};

  // Create and write string stream to screen
  std::stringstream name;
  name << "Emily Merritt";
  io.writeText(name.str(), 0,
    (Gamedata::getInstance().getXmlInt("view/height")-Gamedata::getInstance().getXmlInt("font/size")-5), textColor);

  for(auto it : sprites) it->draw();
  penguin->draw();
  snow1->draw();
  bullets->draw();

  if(isNight) light.draw();

  if(viewHud) {
    hud.drawHud();
  }
  else {
    std::stringstream hudTemp;
    hudTemp << "Toggle HUD with F1";
    io.writeText(hudTemp.str(),
                Gamedata::getInstance().getXmlInt("view/width")-220,
                10, textColor);
  }

  std::stringstream lifeCount;
  lifeCount << "Lives: " << lives;
  io.writeText(lifeCount.str(), 30, 30, textColor);

  std::stringstream pointVal;
  pointVal << "Points: " << points;
  io.writeText(pointVal.str(), 30, 60, textColor);

  if(godMode) {
    std::stringstream godActive;
    godActive << "God mode active!";
    io.writeText(godActive.str(), 30, 90, textColor);
  }

  if(lives == 0) {
    clock.pause();

    std::stringstream reset;
    reset << "Reset game [R]";

    io.writeText(reset.str(), Gamedata::getInstance().getXmlInt("view/width")/2-60,
                 Gamedata::getInstance().getXmlInt("view/height")/2-40, textColor);
  }

  viewport.draw();
  SDL_RenderPresent(renderer);
}

void Engine::update(Uint32 ticks) {
  if(bullets->collidedWith(snow1)) {
    dynamic_cast<twoWayMultiSprite*>(snow1)->explode();
    snow1->setX(rand()%1920);
  }

  if(!godMode && collisionStrat->execute(*penguin, *snow1)) {
    sound[1];
    dynamic_cast<twoWayMultiSprite*>(snow1)->explode();
    dynamic_cast<Player*>(penguin)->explode();
    srand(time(NULL));
    snow1->setX(rand()%1920);
    lives--;
  }

  for (auto it : sprites) {
    if(collisionStrat->execute(*penguin, *it)){
      points++;
      it->setX(rand()%1920);
      it->setY(0);
    }
  }

  for(auto it : sprites) it->update(ticks);
  penguin->update(ticks);
  snow1->update(ticks);
  bullets->update(ticks);

  if (isNight) {
    night.update();
    light.update();
  }
  else world.update();
  mountains.update();
  hills.update();
  viewport.update(); // always update viewport last

}

// void Engine::switchSprite(){
//   ++currentSprite;
//
//   // Reset currentSprite to beginning if it's reached the end
//   if ( currentSprite > 2 ) {
//     currentSprite = 0;
//   }
//
//   if ( currentSprite == 0 ) {
//     Viewport::getInstance().setObjectToTrack(sprites[0]);
//   }
//   else if ( currentSprite == 1 ) {
//     Viewport::getInstance().setObjectToTrack(sprites[1]);
//   }
//   else {
//     Viewport::getInstance().setObjectToTrack(penguin);
//   }
// }

void Engine::toggleHud() {
  if (viewHud) {
    viewHud = false;
  }
  else if (!viewHud){
    viewHud = true;
  }
}

void Engine::toggleNight() {
  if (isNight) {
    isNight = false;
    hud.toggleNight(isNight);
  }
  else if (!isNight){
    isNight = true;
    hud.toggleNight(isNight);

  }
}

void Engine::toggleGod() {
  if (godMode) {
    godMode = false;
  }
  else if (!godMode){
    godMode = true;
  }
}

bool Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  for(int i = 0; i < 10; i++) {
    sprites.emplace_back(fish);
  }
  for(auto it : sprites) {
    it->setX(rand()%1920);
  }

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        // Exits the game
        if (keystate[SDL_SCANCODE_ESCAPE] || keystate[SDL_SCANCODE_Q]) {
          done = true;
          break;
        }
        // Pause game
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        // View a different sprite
        // if ( keystate[SDL_SCANCODE_T] ) {
        //   switchSprite();
        // }

        // Triggers frame generation
        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
        // Toggle the HUD on/off
        if( keystate[SDL_SCANCODE_F1] ) {
          toggleHud();
        }
        // Toggle Night Mode on/off
        if( keystate[SDL_SCANCODE_F2] ) {
          toggleNight();
          sound.switchBackground(isNight);
        }
        // Toggle God Mode on/off
        if( keystate[SDL_SCANCODE_G] ) {
          toggleGod();
        }
        // Shoot snowballs right
        if( keystate[SDL_SCANCODE_SPACE] && penguin->getVelocityX() > 0 ) {
          bullets->shoot(Vector2f(penguin->getX(),700),
                         Vector2f(1000,penguin->getVelocityY()));
          sound[0];
        }
        // Shoot snowballs left
        if( keystate[SDL_SCANCODE_SPACE] && penguin->getVelocityX() < 0 ) {
          bullets->shoot(Vector2f(penguin->getX(),700),
                         Vector2f(-1000,penguin->getVelocityY()));
          sound[0];
        }
        // Restart entire game
        if( keystate[SDL_SCANCODE_R] ) {
          std::cout << "Resetting game..." << std::endl;
          clock.unpause();
          return true;
        }
      }
    }

    // In this section of the event loop we allow key bounce:
    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame(); // Updates the clock
      draw(); // Draws all images/sprites/menus on screen
      update(ticks);

      // If this flag is on, generate a still frame
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }

    // Make penguin sprite walk left and right
    if ( keystate[SDL_SCANCODE_A] ) {
      // Since it's a Drawable*, we have to cast it
      // as a player to use those methods
      dynamic_cast<Player*>(penguin)->left();
    }
    if ( keystate[SDL_SCANCODE_D] ) {
      dynamic_cast<Player*>(penguin)->right();
    }
  }
  return false;
}
