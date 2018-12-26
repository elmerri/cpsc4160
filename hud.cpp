#include <SDL_image.h>
#include <sstream>
#include "hud.h"
#include "gameData.h"
#include "renderContext.h"

Hud& Hud::getInstance() {
  static Hud instance;
  return instance;
}

Hud::~Hud() {
  TTF_CloseFont(font);
  TTF_Quit();
}

Hud::Hud() :
  init(TTF_Init()),
  x(Gamedata::getInstance().getXmlInt("hud/x")),
  y(Gamedata::getInstance().getXmlInt("hud/y")),
  w(Gamedata::getInstance().getXmlInt("hud/w")),
  h(Gamedata::getInstance().getXmlInt("hud/h")),
  viewport(Viewport::getInstance()),
  clock(Clock::getInstance()),
  renderer( RenderContext::getInstance().getRenderer() ),
  font(TTF_OpenFont(Gamedata::getInstance().getXmlStr("font/file").c_str(),
                    Gamedata::getInstance().getXmlInt("font/size"))),
  textColor({0, 140, 140, 0}),
  nightOn(false)
{
  if ( init == -1 ) {
    throw std::string("error: Couldn't init font");
  }
  if (font == NULL) {
    throw std::string("error: font not found");
  }
  textColor.r = Gamedata::getInstance().getXmlInt("hud/r");
  textColor.g = Gamedata::getInstance().getXmlInt("hud/g");
  textColor.b = Gamedata::getInstance().getXmlInt("hud/b");
  textColor.a = Gamedata::getInstance().getXmlInt("hud/a");
}


void Hud::writeText(const std::string& msg, SDL_Color c, int x, int y) const {
  int textWidth;
  int textHeight;

  SDL_Surface* surface = TTF_RenderText_Solid(font, msg.c_str(), c);

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  textWidth = surface->w;
  textHeight = surface->h;
  SDL_FreeSurface(surface);
  SDL_Rect dst = {x, y, textWidth, textHeight};

  SDL_RenderCopy(renderer, texture, NULL, &dst);
  SDL_DestroyTexture(texture);
}


void Hud::drawHud() {
    int distx = x+5;
    int disty = y+5;

    if(nightOn) textColor = {255, 255, 255, 0};
    else textColor = {0, 140, 140, 0};

    std::stringstream strm;
    strm << "FPS: " << clock.getFps();
    writeText(strm.str(), textColor,distx, disty);

    std::stringstream strm2;
    if ( clock.getSeconds() < 60 ) {
      strm2 << "Time: " << clock.getSeconds() << "s";
    }
    else {
      int mins = clock.getSeconds()/60;
      int secs = clock.getSeconds()%60;

      strm2 << "Time: " << mins << "m " << secs << "s";
    }
    writeText(strm2.str(), textColor,distx, disty+25);

    std::stringstream hud;
    hud << "Toggle HUD: F1";
    writeText(hud.str(), textColor,distx, disty+70);

    std::stringstream night;
    night << "Toggle Night: F2";
    writeText(night.str(), textColor,distx, disty+100);

    std::stringstream reset;
    reset << "Reset: R";
    writeText(reset.str(), textColor,distx, disty+47);

    std::stringstream direct;
    direct << "Left and Right: A & D";
    writeText(direct.str(), textColor,distx, disty+160);

    std::stringstream god;
    god << "God Mode: G";
    writeText(god.str(), textColor,distx, disty+190);

    std::stringstream shoot;
    shoot << "Shoot: Space";
    writeText(shoot.str(), textColor, distx, disty+130);

    std::stringstream info1;
    info1 << "  Collect as many";
    writeText(info1.str(), textColor, distx, disty+260);

    std::stringstream info2;
    info2 << "  fish as you can";
    writeText(info2.str(), textColor, distx, disty+290);

    std::stringstream info3;
    info3 << "  with 3 lives!";
    writeText(info3.str(), textColor, distx, disty+320);

    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255/3 );

    SDL_RenderFillRect( renderer, &r );

    SDL_SetRenderDrawColor( renderer, 0, 255, 255, 255 );
    SDL_RenderDrawRect( renderer, &r );
}

void Hud::toggleNight(bool night) {
  nightOn = night;
}
