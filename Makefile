CXX = g++

# Warnings frequently signal eventual errors:
CXXFLAGS=`sdl2-config --cflags` -g -W -Wall -std=c++11 -Weffc++ -Wextra -pedantic -O0 -I `sdl2-config --prefix`/include/

LDFLAGS = `sdl2-config --libs` -lm -lexpat -lSDL2_ttf -lSDL2_image -lSDL2_mixer


OBJS = \
	renderContext.o \
	ioMod.o \
	parseXML.o \
	gameData.o \
	viewport.o \
	world.o \
	spriteSheet.o \
	image.o \
	imageFactory.o \
	frameGenerator.o \
	sprite.o \
	explodingSprite.o \
	multisprite.o \
	twowaymultisprite.o \
	player.o \
	collisionStrategy.o \
	bullet.o \
	bulletPool.o \
	triple.o \
	light.o \
	lights.o \
	chunk.o \
	hud.o \
	vector2f.o \
	clock.o \
	engine.o \
	sound.o \
	main.o
EXEC = run

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(EXEC)
	rm -rf frames/*.bmp
