#include <SFML/Graphics.hpp>
#include "../include/TileMap.h"
#include <cassert>

int main () {
	// create the window
	sf::RenderWindow window (sf::VideoMode (512, 256), "Tilemap");

	// define the level with an array of tile indices
	const int level[] =
	{
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
		0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
		0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
		2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
	};

	// create the tilemap from the level definition
	TileMap map;
	assert (map.load ("static/image/tileset.png", sf::Vector2u (32, 32), level, 16, 8));

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