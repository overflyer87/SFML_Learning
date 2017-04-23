//In1cludes
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//Define constants at compile time
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAIN_VIEW_WIDTH 800
#define MAIN_VIEW_HEIGHT 600

#define GRAVITY float 981.0f

//Function prototypes
//Collision detection AABB method for two bodies
bool handleCollision(sf::RectangleShape*, sf::RectangleShape*, bool);

//Animate player
sf::IntRect animate(int row, sf::Texture* texture, sf::Vector2u* imageCount,
		float switchTime, float* deltaTime);

//==========
//START MAIN
//==========

int main(int argc, char* argv[]) {
	//Load Music
	sf::SoundBuffer soundBuffer;
	sf::Sound sound;
	soundBuffer.loadFromFile("bongojam.ogg");
	sound.setBuffer(soundBuffer);

	//Create the window and renderer all in one
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
			"My first SFML Game!");

	//Textures
	//Floor
	sf::Texture floorTexture;
	//Players and NPCs
	sf::Texture playerTexture;
	//Other world objects
	sf::Texture obstacleBoxTexture;
	sf::Texture lavaTexture;

	//Shapes
	//Floor
	sf::RectangleShape floorShape;

	//Players and NPCs
	sf::RectangleShape playerShape;

	//Other world objects
	sf::RectangleShape obstacleBoxShape;
	sf::RectangleShape lavaShape;

	//Tie textures to shapes
	//Floor
	floorTexture.loadFromFile("plain_block.png");
	floorShape.setTexture(&floorTexture);

	//Players and NPCs
	playerTexture.loadFromFile("tux_from_linux.png");
	playerShape.setTexture(&playerTexture);

	//Other world objects
	obstacleBoxTexture.loadFromFile("box.png");
	obstacleBoxShape.setTexture(&obstacleBoxTexture);

	lavaTexture.loadFromFile("lava.png");
	lavaShape.setTexture(&lavaTexture);

	//Get the size of textures
	sf::Vector2u playerTextureSize = playerTexture.getSize();

	//Set texture IntRect to select a portion of a sprite if neccessary
	//If you just have a single figure in the texture file you must multiply by 0 as X and Y offset
	//Player and NPCs
	playerShape.setTextureRect(
			sf::IntRect(playerTextureSize.x * 0, playerTextureSize.y * 0,
					playerTextureSize.x, playerTextureSize.y));

	//If you have a sprite with more than 1 column or row deal with animation here
	//Player and NPCs
	sf::Clock clock;
	sf::Vector2u playerImageCount(3, 9);
	float deltaTime = 0.0f;

	//Execute texture options if necessary/wanted
	//Floor
	floorTexture.setSmooth(true);
	//Players and NPCs
	playerTexture.setSmooth(true);
	//Other world objects
	obstacleBoxTexture.setSmooth(true);

	lavaTexture.setSmooth(true);
	lavaTexture.setRepeated(true);

	//Optionally position, rotate your readily created object etc.
	//Floor
	floorShape.setSize(sf::Vector2f(50.0f, 50.0f));
	floorShape.setOrigin(0.0f, 25.0f);
	floorShape.rotate(180);
	//Players and NPCs
	playerShape.setSize(sf::Vector2f(100.0f, 100.0f));
	playerShape.setOrigin(50.0f, 50.0f);
	playerShape.setPosition(50.0f, 330.0f);
	//Other world objects
	obstacleBoxShape.setSize(sf::Vector2f(75.0f, 75.0f));
	obstacleBoxShape.setOrigin(32.5f, 32.5f);
	obstacleBoxShape.setPosition(600.0f, 350.0f);

	lavaShape.setSize(sf::Vector2f(float(WINDOW_WIDTH), 50.0f));

	//If wanted create arrays for repeatedly drawing objects
	//Position will then be set in for loop
	int elementsFloorArray = std::ceil(
			((float) WINDOW_WIDTH / floorShape.getSize().x)) + 1;
	int elementsLavaArray = std::ceil(
			((float) WINDOW_WIDTH / lavaShape.getSize().y)) + 1;
	sf::RectangleShape floorArray[elementsFloorArray] = { floorShape };
	sf::RectangleShape lavaArray[elementsLavaArray] = { lavaShape };
	int floorArrayLength = (sizeof(floorArray) / sizeof(floorArray[0]));
	int lavaArrayLength = (sizeof(lavaArray) / sizeof(lavaArray[0]));

	//Play world music by default
	sound.play();
	sound.setPlayingOffset(sf::seconds(2));
	sound.setLoop(true);
	sound.setVolume(10);

	//=====================
	//!!!Start Game loop!!!
	//=====================

	while (window.isOpen()) {

		deltaTime = clock.restart().asSeconds();

		//Create event for Event loop
		sf::Event event;

		//Start Event loop
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				playerShape.move(-10, 0.0f);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				playerShape.move(10, 0.0f);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				//Pause world music using the 'm' key when sound is playing
				sound.pause();
			}
		}

		//We broke out of event loop because the queue is empty for this frame

		//Animate player
		playerShape.setTextureRect(
				animate(0, &playerTexture, &playerImageCount, 0.3f,
						&deltaTime));

		//Clear the window on each frame, set color to a light sky blue
		window.clear(sf::Color(176, 226, 255, 100));

		//Draw non-repetitive objects
		window.draw(playerShape);
		window.draw(obstacleBoxShape);

		//Draw repetitive objects
		//Draw the floor by iterating through floorShape-array
		for (int i = 0; i < floorArrayLength; i++) {
			if (i == 14) {
				i++;
			} else {
				floorShape.setPosition(((0.0f + 50.0f) * float(i)), 400.0f);
				window.draw(floorShape);
				bool boxAndFloorCollide = handleCollision(&obstacleBoxShape,
						&floorArray[i - 1], false);

				if (!boxAndFloorCollide) {
					std::cout
							<< "Box should fall into lava here! If this worked I would make it move down the Y axis."
							<< std::endl;
				} else {
					std::cout
							<< "Box collides with floor so should just stand on it"
							<< std::endl;
				}
			}
		}
		//Draw the lava under the floor by iterating though lavaShape-array
		for (int i = 0; i < lavaArrayLength; i++) {
			lavaShape.setPosition(0.0f, (410.0f + (50.0f * float(i))));
			window.draw(lavaShape);
		}

		//Check for and handle collision
		handleCollision(&playerShape, &floorShape, false);
		//handleCollisionByStopping(&obstacleBoxShape, &floorShape);
		handleCollision(&playerShape, &obstacleBoxShape, false);

		//Drawing was done to back buffer. Now switch back and to front buffer directly
		//This is a common technology nowadays to avoid tearing and jittering
		window.display();
	}

	//Everything went fine
	return 0;

	//==============
	//EXIT PROGRAMME
	//==============
}

//Function signatures

//Collision detection: AABB method for two bodies
//bool push decides whether or not the collision will be resolved by pushing secondBody away
bool handleCollision(sf::RectangleShape* firstBody,
		sf::RectangleShape* secondBody, bool push) {

	//If the secondBody passed in is NULL return false
	//Make sure the first body is preferably the player and the secondBody is something static like the floor
	if (secondBody == NULL) {
		return false;
	}

	//Get the bodies' positions
	sf::Vector2f posFirstBody = firstBody->getPosition();
	sf::Vector2f posSecondBody = secondBody->getPosition();
	//Get half of the size of each body
	sf::Vector2f posFirstHalfSize = (firstBody->getSize() / 2.0f);
	sf::Vector2f posSecondHalfSize = (secondBody->getSize() / 2.0f);
	//Calculate deltaX and deltaY between the bodies
	float deltaX = posSecondBody.x - posFirstBody.x;
	float deltaY = posSecondBody.y - posFirstBody.y;
	//Calculate intersection by subtracting the sum of the values of the sizes of each body from deltaX/deltaY
	float intersectX = abs(deltaX) - (posSecondHalfSize.x + posFirstHalfSize.x);
	float intersectY = abs(deltaY) - (posSecondHalfSize.y + posFirstHalfSize.y);

	//If one of the intersection results is negative THERE IS A COLLISION
	if (intersectX < 0.0f && intersectY < 0.0f) {
		//If collision on X ordinate is bigger than on Y handle resolution on X level
		if (intersectX > intersectY) {
			//If push is true then resolve collision by pushing the secondBody
			if (push) {
				//If deltaX was positive the firstBody is pushed to the left and the second to the right
				if (deltaX > 0.0f) {
					firstBody->move(intersectX, 0.0f);
					secondBody->move(-intersectX, 0.0f);
					//Else vice versa
				} else {
					firstBody->move(-intersectX, 0.0f);
					secondBody->move(intersectX, 0.0f);
				}
				//Else (if push is false) just push back firstBody which effectively results in stopping it
			} else {
				if (deltaX > 0.0f) {
					firstBody->move(intersectX, 0.0f);
				} else {
					firstBody->move(-intersectX, 0.0f);
				}
			}
			//If collision on Y ordinate is bigger than on X handle resolution on Y level
			//Rest is vice versa described in comments above
		} else {
			if (push) {

				if (deltaX > 0.0f) {
					firstBody->move(0.0f, intersectX);
					secondBody->move(0.0f, -intersectX);
				} else {
					firstBody->move(0.0f, -intersectX);
					secondBody->move(0.0f, intersectX);
				}
			} else {

				if (deltaX > 0.0f) {
					firstBody->move(0.0f, intersectX);
				} else {
					firstBody->move(0.0f, -intersectX);
				}
			}
		}
		//Return true if collision was detected
		return true;
	}
	//Return false if no collision was detected
	return false;
}

//Do player animation
sf::IntRect animate(int row, sf::Texture* texture, sf::Vector2u* imageCount,
		float switchTime, float* deltaTime) {
	float totalTime = 0.0f;
	sf::Vector2u currentImage;
	sf::IntRect uvRect;

	currentImage.x = 0;
	currentImage.y = row;
	totalTime += *deltaTime;

	uvRect.width = texture->getSize().x / float(imageCount->x);
	uvRect.height = texture->getSize().y / float(imageCount->y);

	if (totalTime >= switchTime) {
		std::cout << "totalTime is bigger than switchTime. Increment tux!"
				<< std::endl;
		totalTime -= switchTime;
		currentImage.x++;
		std::cout << "Incremented tux. Current image is: " << currentImage.x
				<< std::endl;

		if (currentImage.x >= imageCount->x) {
			currentImage.x = 0;
		}
	}

	uvRect.left = currentImage.x * uvRect.width;
	uvRect.top = currentImage.y * uvRect.height;

	//START DEBUGGING
	std::cout << "Current Image X : " << currentImage.x << std::endl;
	std::cout << "Current Image Y: " << currentImage.y << std::endl;
	std::cout << "UV Rect Width: " << uvRect.width << std::endl;
	std::cout << "UV Rect Height: " << uvRect.height << std::endl;
	std::cout << "UV Rect Left: " << uvRect.left << std::endl;
	std::cout << "UV Rect Top: " << uvRect.top << std::endl;
	std::cout << "Image Count X: " << imageCount->x << std::endl;
	std::cout << "Image Count Y: " << imageCount->y << std::endl;
	//END DEBUGGING

	return uvRect;
}

