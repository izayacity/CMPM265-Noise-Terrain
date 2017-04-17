#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <noise/noise.h>
#include "../include/noiseutils.h"
#include "../include/TileMap.h"
#include <cassert>

using namespace noise;
const int terrain_width = 16;
const int terrain_height = 8;
const int TERRAIN_SIZE = 128;
static int terrain[TERRAIN_SIZE];    // array to store the result of perlin noise

int mapping (float value) {
	if (value < -0.25) {
		return 23;
	} else if (value >= -0.25 && value < 0) {
		return 15;
	} else if (value >= 0 && value < 0.25) {
		return 6;
	} else if (value >= 0.25 && value < 0.5) {
		return 9;
	} else if (value >= 0.5 && value < 0.75) {
		return 12;
	} else if (value >= 0.75) {
		return 5;
	}
}

void perlinNoise () {
	module::Perlin myModule;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;

	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (terrain_width, terrain_height);
	heightMapBuilder.SetBounds (2.0, 6.0, 1.0, 5.0);
	heightMapBuilder.Build ();

	int k = 0;
	for (int i = 0; i < terrain_width; i++) {
		for (int j = 0; j < terrain_height; j++) {
			float raw = heightMap.GetValue (i, j);
			terrain[k] = mapping (raw);
			std::cout << terrain[k] << " "; k++;
		}
		std::cout << std::endl;
	}
}

int main () {
	// create the window
	sf::RenderWindow window (sf::VideoMode (768, 384), "Tilemap");

	// create the tilemap from the level definition
	TileMap map;
	perlinNoise ();
	assert (map.load ("static/image/tileset.gif", sf::Vector2u (48, 48), terrain, 16, 8));

	// run the main loop
	while (window.isOpen ()) {
		// handle events
		sf::Event event;
		while (window.pollEvent (event)) {
			if (event.type == sf::Event::Closed)
				window.close ();
		}

		// draw the map
		window.clear ();
		window.draw (map);
		window.display ();
	}

	return 0;
}
