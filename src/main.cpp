#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <noise/noise.h>
#include "../include/noiseutils.h"
#include "../include/TileMap.h"
#include <cassert>

using namespace noise;
const int terrain_width = 48;
const int terrain_height = 48;
const int TERRAIN_SIZE = 2304;
static int terrain[TERRAIN_SIZE];    // array to store the result of perlin noise
module::Perlin myModule;
utils::NoiseMap heightMap;
utils::NoiseMapBuilderPlane heightMapBuilder;

enum Geo { Water = 28, Sand = 19, Coco = 16, Tree = 54, Grass = 240, Mountain = 62};

int mapping (float value) {
	if (value < -0.25) {
		return Geo::Water;
	} else if (value >= -0.25 && value < 0) {
		return Geo::Sand;
	} else if (value >= 0 && value < 0.25) {
		return Geo::Coco;
	} else if (value >= 0.25 && value < 0.5) {
		return Geo::Tree;
	} else if (value >= 0.5 && value < 0.75) {
		return Geo::Grass;
	} else if (value >= 0.75) {
		return Geo::Mountain;
	}
}

bool threeSand (int i) {
	if ((i + 1) % terrain_width == 0 && (terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i - 1] == 19 * 3))
		return true;
	else if (i % terrain_width == 0 && (terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i + 1] == 19 * 3))
		return true;
	else if (terrain[i - terrain_width] == Geo::Sand && terrain[i + terrain_width] == Geo::Sand && terrain[i - 1] == Geo::Sand)
		return true;
	else if (terrain[i - terrain_width] == Geo::Sand && terrain[i + terrain_width] == Geo::Sand && terrain[i + 1] == Geo::Sand)
		return true;
	else if (terrain[i - terrain_width] == Geo::Sand && terrain[i + 1] == Geo::Sand && terrain[i - 1] == Geo::Sand)
		return true;
	else if (terrain[i + terrain_width] == Geo::Sand && terrain[i + 1] == Geo::Sand && terrain[i - 1] == Geo::Sand)
		return true;
	return false;
}

bool threeWater (int i) {
	if ((i + 1) % terrain_width == 0 && (terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i - 1] == 28 * 3))
		return true;
	else if (i % terrain_width == 0 && (terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i + 1] == 28 * 3))
		return true;
	else if (terrain[i - terrain_width] == Geo::Water && terrain[i + terrain_width] == Geo::Water && terrain[i - 1] == Geo::Water)
		return true;
	else if (terrain[i - terrain_width] == Geo::Water && terrain[i + terrain_width] == Geo::Water && terrain[i + 1] == Geo::Water)
		return true;
	else if (terrain[i - terrain_width] == Geo::Water && terrain[i + 1] == Geo::Water && terrain[i - 1] == Geo::Water)
		return true;
	else if (terrain[i + terrain_width] == Geo::Water && terrain[i + 1] == Geo::Water && terrain[i - 1] == Geo::Water)
		return true;
	return false;
}

void postProcess () {
	if (terrain[0] == Geo::Water && terrain[1] != Geo::Water && terrain[terrain_width] != Geo::Water) {
		terrain[0] = Geo::Sand;
	} else if (terrain[0] != Geo::Water && terrain[1] == Geo::Water && terrain[terrain_width] == Geo::Water) {
		terrain[0] = Geo::Water;
	}

	if (terrain[terrain_width - 1] == Geo::Water && terrain[terrain_width - 2] != Geo::Water && terrain[terrain_width * 2 - 1] != Geo::Water) {
		terrain[terrain_width - 1] = Geo::Sand;
	} else if (terrain[terrain_width - 1] != Geo::Water && terrain[terrain_width - 2] == Geo::Water && terrain[terrain_width * 2 - 1] == Geo::Water) {
		terrain[terrain_width - 1] = Geo::Water;
	}
	
	for (int i = 1; i < terrain_width - 1; i++) {
		if (terrain[i] != Geo::Water && terrain[i - 1] == Geo::Water &&
			terrain[i + 1] == Geo::Water && terrain[i + terrain_width] == Geo::Water) {
			terrain[i] = Geo::Water;
		} else if (terrain[i] == Geo::Water && terrain[i - 1] != Geo::Water &&
			terrain[i + 1] != Geo::Water && terrain[i + terrain_width] != Geo::Water) {		
			terrain[i] = Geo::Sand;
		}
	}

	if (terrain[TERRAIN_SIZE - terrain_width] == Geo::Water && terrain[TERRAIN_SIZE - terrain_width + 1]
		!= Geo::Water && terrain[TERRAIN_SIZE - terrain_width * 2] != Geo::Water) {
		terrain[TERRAIN_SIZE - terrain_width] = Geo::Sand;
	} else if (terrain[TERRAIN_SIZE - terrain_width] != Geo::Water && terrain[TERRAIN_SIZE - terrain_width + 1]
		== Geo::Water && terrain[TERRAIN_SIZE - terrain_width * 2] == Geo::Water) {
		terrain[TERRAIN_SIZE - terrain_width] = Geo::Water;
	}

	if (terrain[TERRAIN_SIZE - 1] == Geo::Water && terrain[TERRAIN_SIZE - 2] != Geo::Water
		&& terrain[TERRAIN_SIZE - terrain_width - 1] != Geo::Water) {
		terrain[TERRAIN_SIZE - 1] = Geo::Sand;
	} else if (terrain[TERRAIN_SIZE - 1] != Geo::Water && terrain[TERRAIN_SIZE - 2] == Geo::Water
		&& terrain[TERRAIN_SIZE - terrain_width - 1] == Geo::Water) {
		terrain[TERRAIN_SIZE - 1] = Geo::Water;
	}

	for (int i = TERRAIN_SIZE - terrain_width + 1; i < TERRAIN_SIZE - 1; i++) {
		if (terrain[i] != Geo::Water && terrain[i - 1] == Geo::Water &&
			terrain[i + 1] == Geo::Water && terrain[i + terrain_width] == Geo::Water) {
			terrain[i] = Geo::Water;
		} else if (terrain[i] == Geo::Water && terrain[i - 1] != Geo::Water &&
			terrain[i + 1] != Geo::Water && terrain[i + terrain_width] != Geo::Water) {
			terrain[i] = Geo::Sand;
		}
	}

	for (int i = terrain_width; i < TERRAIN_SIZE - terrain_width; i++) {
		if (threeSand (i))
			terrain[i] = Geo::Sand;
		else if (threeWater (i))
			terrain[i] = Geo::Water;
		else if ((i + 1) % terrain_width != 0 && i % terrain_width != 0) {
			int value = terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i - 1] + terrain[i + 1];
			if (value == 19 * 2 + 28 * 2 || value == 19 + 16 + 28 * 2 || value == 16 * 2 + 28 * 2) {
				if (terrain[i + terrain_width] == terrain[i + 1] && terrain[i + terrain_width] == Geo::Water) {
					terrain[i] = 12;
				} else if (terrain[i - terrain_width] == terrain[i - 1] && terrain[i - terrain_width] == Geo::Water) {
					terrain[i] = 50;
				} else if (terrain[i + terrain_width] == terrain[i - 1] && terrain[i + terrain_width] == Geo::Water) {
					terrain[i] = 14;
				} else if (terrain[i - terrain_width] == terrain[i + 1] && terrain[i - terrain_width] == Geo::Water) {
					terrain[i] = 48;
				}
			}
		} else if (i % terrain_width == 0) {
			int value = terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i + 1];
			if (value == 19 + 28 * 2 || value == 16 + 28 * 2) {
				if (terrain[i + terrain_width] == terrain[i + 1] && terrain[i + terrain_width] == Geo::Water) {
					terrain[i] = 12;
				} else if (terrain[i - terrain_width] == terrain[i + 1] && terrain[i - terrain_width] == Geo::Water) {
					terrain[i] = 48;
				}
			}
		} else if ((i + 1) % terrain_width == 0) {
			int value = terrain[i - terrain_width] + terrain[i + terrain_width] + terrain[i - 1];
			if (value == 19 + 28 * 2 || value == 16 + 28 * 2) {
				if (terrain[i - terrain_width] == terrain[i - 1] && terrain[i - terrain_width] == Geo::Water) {
					terrain[i] = 50;
				} else if (terrain[i + terrain_width] == terrain[i - 1] && terrain[i + terrain_width] == Geo::Water) {
					terrain[i] = 14;
				}
			}
		}
	}
}

void perlinNoise () {
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (terrain_width, terrain_height);
	heightMapBuilder.SetBounds (0.0, 4.0, 1.0, 5.0);
	heightMapBuilder.Build ();

	int k = 0;
	for (int i = 0; i < terrain_width; i++) {
		for (int j = 0; j < terrain_height; j++) {
			//float raw = heightMap.GetValue ((i + (int)elapsed.asSeconds()) % terrain_width, (j + (int)elapsed.asSeconds ()) % terrain_height);
			float raw = heightMap.GetValue (i, j);
			terrain[k] = mapping (raw);
			//std::cout << terrain[k] << " ";
			k++;
		}
		//std::cout << std::endl;
	}
	postProcess ();
	postProcess ();
	//for (double i = 0; i < terrain_width; i++) {
	//	for (double j = 0; j < terrain_height; j++) {
	//		double raw = myModule.GetValue (i, j, elapsed.asMilliseconds () % 48 + 0.f);
	//		terrain[k] = mapping (raw);
	//		//std::cout << terrain[k] << " "; k++;
	//		std::cout << raw << " ";
	//		k++;
	//	}
	//	std::cout << std::endl;
	//}
}

int main () {
	const int gameWidth = 768;
	const int gameHeight = 768;

	// create the window
	sf::RenderWindow window (sf::VideoMode (768, 768), "Tilemap");

	// load fonts
	sf::Font fontHNL;
	assert (fontHNL.loadFromFile ("static/font/HelveticaNeue Light.ttf"));

	// draw the background of text field
	sf::RectangleShape textField;
	textField.setFillColor (sf::Color::Black);
	textField.setSize (sf::Vector2f (gameWidth + 0.f, 30.f));
	textField.setPosition (0, gameHeight - 30);

	// display the text
	sf::Text seedText;
	seedText.setFont (fontHNL);
	seedText.setCharacterSize (20);
	seedText.setFillColor (sf::Color::Green);
	std::string seedStr = std::to_string (myModule.GetSeed ());
	seedText.setString ("Set Perlin noise seed as: " + seedStr);
	seedText.setPosition (20, gameHeight - 30);

	// create the tilemap from the level definition
	TileMap map;
	perlinNoise ();
	assert (map.load ("static/image/tileset.gif", sf::Vector2u (16, 16), terrain,
		terrain_width, terrain_height));
	
	// run the main loop
	while (window.isOpen ()) {
		// handle events
		sf::Event event;
		while (window.pollEvent (event)) {
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				window.close ();
			} 
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Up)) {
				myModule.SetSeed (myModule.GetSeed () + 1);
				perlinNoise ();
				assert (map.load ("static/image/tileset.gif", sf::Vector2u (16, 16), terrain,
					terrain_width, terrain_height));
				seedStr = std::to_string (myModule.GetSeed ());
				seedText.setString ("Seed (Use UP/DOWN key to change): " + seedStr);
			} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Down)) {
				myModule.SetSeed (myModule.GetSeed () - 1);
				perlinNoise ();
				assert (map.load ("static/image/tileset.gif", sf::Vector2u (16, 16), terrain,
					terrain_width, terrain_height));
				seedStr = std::to_string (myModule.GetSeed ());
				seedText.setString ("Seed (Use UP/DOWN key to change): " + seedStr);
			}
		}

		// draw the map
		window.clear ();
		window.draw (map);
		window.draw (textField);
		window.draw (seedText);
		window.display ();
	}
	return 0;
}