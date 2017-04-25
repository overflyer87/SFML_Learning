//In1cludes
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unistd.h>

//Define constants at compile time
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAIN_VIEW_WIDTH 800.0f
#define MAIN_VIEW_HEIGHT 600.0f

#define GRAVITY float 981.0f

//Function prototypes
//Collision detection AABB method for two bodies
//X and Y achsis
bool handleCollisionXY(sf::RectangleShape*, sf::RectangleShape*, bool);
//X achsis only
bool handleCollisionX(sf::RectangleShape*, sf::RectangleShape*, bool);
//Y achsis only
bool handleCollisionY(sf::RectangleShape*, sf::RectangleShape*, bool);
//Touch detection (either X OR Y / Intersection == 0)
bool detectTouching(sf::RectangleShape*, sf::RectangleShape*);

//Animate player
sf::IntRect animate(int row, sf::Texture* texture, sf::Vector2u* imageCount, sf::Vector2u*, float switchTime, float* deltaTime);

//Handle view resizing
void resizeView(const sf::RenderWindow*, sf::View*);

//==========
//START MAIN
//==========

int main(int argc, char* argv[]) {
	//Create the window and renderer all in one
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),	"My first SFML Game!");

	//Create fonts
	sf::Font gameOverFont;
	gameOverFont.loadFromFile("tahoma.ttf");

	//Create text pieces
	//Game Over
	sf::Text gameOver;
	gameOver.setFont(gameOverFont);
	gameOver.setString("YOU DID DIED YOU TWAT!");
	gameOver.setCharacterSize(32);
	gameOver.setColor(sf::Color::Red);
	gameOver.setStyle(sf::Text::Bold);
	gameOver.setPosition(510.0f, 4950.0f);

	//Create a view
	sf::View view(sf::Vector2f(0.0f, 0.0f),	sf::Vector2f(MAIN_VIEW_WIDTH, MAIN_VIEW_HEIGHT));

	//Load Music
	sf::SoundBuffer soundBuffer;
	sf::Sound sound;
	soundBuffer.loadFromFile("bongojam.ogg");
	sound.setBuffer(soundBuffer);

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

	//If you have a sprite with more than 1 column or row deal with animation here
	//Player and NPCs
	sf::Clock clock;
	sf::Vector2u playerImageCount(3, 9);
	sf::Vector2u playerCurrentImage;
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
	playerShape.setPosition(50.0f, 327.0f);
	//Other world objects
	obstacleBoxShape.setSize(sf::Vector2f(75.0f, 75.0f));
	obstacleBoxShape.setOrigin(32.5f, 32.5f);
	obstacleBoxShape.setPosition(600.0f, 337.5f);

	lavaShape.setSize(sf::Vector2f(float(WINDOW_WIDTH), 150.0f));

	//If wanted create arrays for repeatedly drawing objects
	//Position will then be set in for loop
	int elementsFloorArray = std::ceil(
			((float) WINDOW_WIDTH / floorShape.getSize().x)) + 1;
	int elementsLavaArray = std::ceil(
			((float) WINDOW_WIDTH / lavaShape.getSize().y)) + 1;
	sf::RectangleShape dummy;
	sf::RectangleShape floorArray[elementsFloorArray] = { dummy };
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

	bool isDead = false;

	while (window.isOpen()) {

		deltaTime = clock.getElapsedTime().asSeconds();

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

			if (sf::Event::Resized) {
				resizeView(&window, &view);
			}
		}

		//We broke out of event loop because the queue is empty for this frame

		//Animate player
		playerShape.setTextureRect(animate(0, &playerTexture, &playerImageCount, &playerCurrentImage, 0.05f, &deltaTime));

		view.setCenter(playerShape.getPosition());

		//Collision detection
		handleCollisionXY(&playerShape, &obstacleBoxShape, true);
		detectTouching(&playerShape, &floorShape);

		//Clear the window on each frame, set color to a light sky blue
		window.clear(sf::Color(176, 226, 255, 100));
		window.setView(view);

		//Draw non-repetitive objects
		window.draw(playerShape);
		window.draw(obstacleBoxShape);

		//Draw repetitive objects
		//Draw the floor by iterating through floorShape-array



		for (int i = 0; i < floorArrayLength; i++) {

			if (i == 14) {
				i++;
			} else {
				floorArray[i] = floorShape;
				floorShape.setPosition(((0.0f + 50.0f) * float(i)), 400.0f);
				sf::RectangleShape* curFloorTile = &(floorArray[i]);
				window.draw(*curFloorTile);
			}

			if((floorArray[i].getPosition().x == 0) && (floorArray[i].getPosition().y == 0) && (obstacleBoxShape.getPosition().x > floorArray[i + 1].getPosition().x)) {
					 obstacleBoxShape.move(0.0f, 0.5f);
			}

			if((floorArray[i].getPosition().x == 0) && (floorArray[i].getPosition().y == 0) && (playerShape.getPosition().x > floorArray[i + 1].getPosition().x)) {
					playerShape.move(0.0f, 0.5f);

				if(playerShape.getPosition().y >= 5000.0f && playerShape.getPosition().y <= 5200.0f) {
					playerShape.move(0.0f, -0.5f);
					isDead = true;
					sound.stop();

					window.clear();
					window.draw(gameOver);
					window.display();

					usleep(2000000);
					window.close();
				}
			}
		}


		//Draw the lava under the floor by iterating though lavaShape-array
		for (int i = 0; i < lavaArrayLength; i++) {
			lavaShape.setPosition(0.0f, (410.0f + (50.0f * float(i))));
			window.draw(lavaShape);
		}

		//Drawing was done to back buffer. Now switch back and to front buffer ijnstantly
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
//X and Y achsis
//bool push decides whether or not the collision will be resolved by pushing secondBody away
bool handleCollisionXY(sf::RectangleShape* firstBody,
		sf::RectangleShape* secondBody, bool push) {

	//Get the bodies' positions
	sf::Vector2f posFirstBody = firstBody->getPosition();
	sf::Vector2f posSecondBody = secondBody->getPosition();
	//Get half of the size of each body
	sf::Vector2f firstBodyHalfSize = (firstBody->getSize() / 2.0f);
	sf::Vector2f secondBodyHalfSize = (secondBody->getSize() / 2.0f);
	//Calculate deltaX and deltaY between the bodies
	float deltaX = posSecondBody.x - posFirstBody.x;
	float deltaY = posSecondBody.y - posFirstBody.y;
	//Calculate intersection by subtracting the sum of the values of the sizes of each body from deltaX/deltaY
	float intersectX = abs(deltaX)	- (secondBodyHalfSize.x + firstBodyHalfSize.x);
	float intersectY = abs(deltaY)	- (secondBodyHalfSize.y + firstBodyHalfSize.y);

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
			} else if (intersectX <= intersectY) {
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

				if (deltaY > 0.0f) {
					firstBody->move(0.0f, intersectX);
					secondBody->move(0.0f, -intersectX);
				} else {
					firstBody->move(0.0f, -intersectX);
					secondBody->move(0.0f, intersectX);
				}
			} else {

				if (deltaY > 0.0f) {
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

//Collision dectection X achsis only
bool handleCollisionX(sf::RectangleShape* firstBody,
		sf::RectangleShape* secondBody, bool push) {
	//Get the bodies' positions
	sf::Vector2f posFirstBody = firstBody->getPosition();
	sf::Vector2f posSecondBody = secondBody->getPosition();
	//Get half of the size of each body
	sf::Vector2f firstBodyHalfSize = (firstBody->getSize() / 2.0f);
	sf::Vector2f secondBodyHalfSize = (secondBody->getSize() / 2.0f);
	//Calculate deltaX between the bodies
	float deltaX = posSecondBody.x - posFirstBody.x;

	//Calculate intersection by subtracting the sum of the values of the sizes of each body from deltaX
	float intersectX = abs(deltaX) - (secondBodyHalfSize.x + firstBodyHalfSize.x);

	if (intersectX < 0.0f) {
		if (push) {
			if (deltaX > 0.0f) {
				firstBody->move(intersectX, 0.0f);
				secondBody->move(-intersectX, 0.0f);
			} else {
				firstBody->move(-intersectX, 0.0f);
				secondBody->move(intersectX, 0.0f);
			}

		} else {
			if (deltaX > 0.0f) {
				firstBody->move(intersectX, 0.0f);
			} else {
				firstBody->move(-intersectX, 0.0f);
			}
		}
		return true;
	}

	return false;
}

//Collision detection Y achsis only
bool handleCollisionY(sf::RectangleShape* firstBody,
		sf::RectangleShape* secondBody, bool push) {
	//Get the bodies' positions
	sf::Vector2f posFirstBody = firstBody->getPosition();
	sf::Vector2f posSecondBody = secondBody->getPosition();
	//Get half of the size of each body
	sf::Vector2f firstBodyHalfSize = (firstBody->getSize() / 2.0f);
	sf::Vector2f secondBodyHalfSize = (secondBody->getSize() / 2.0f);
	//Calculate deltaY between the bodies
	float deltaY = posSecondBody.y - posFirstBody.y;

	//Calculate intersection by subtracting the sum of the values of the sizes of each body from deltaY
	float intersectY = abs(deltaY)
			- (secondBodyHalfSize.y + firstBodyHalfSize.y);

	if (intersectY < 0.0f) {
		if (push) {
			if (deltaY > 0.0f) {
				firstBody->move(0.0f, intersectY);
				secondBody->move(0.0f, -intersectY);
			} else {
				firstBody->move(0.0f, -intersectY);
				secondBody->move(0.0f, intersectY);
			}

		} else {
			if (deltaY > 0.0f) {
				firstBody->move(0.0f, intersectY);
			} else {
				firstBody->move(0.0f, -intersectY);
			}
		}
		return true;
	}

	return false;
}

bool detectTouching(sf::RectangleShape* firstBody, sf::RectangleShape* secondBody) {

	//Get the bodies' positions
	sf::Vector2f posFirstBody = firstBody->getPosition();
	sf::Vector2f posSecondBody = secondBody->getPosition();
	//Get half of the size of each body
	sf::Vector2f firstBodyHalfSize = (firstBody->getSize() / 2.0f);
	sf::Vector2f secondBodyHalfSize = (secondBody->getSize() / 2.0f);
	//Calculate deltaX and deltaY between the bodies
	float deltaX = posSecondBody.x - posFirstBody.x;
	float deltaY = posSecondBody.y - posFirstBody.y;
	//Calculate intersection by subtracting the sum of the values of the sizes of each body from deltaX/deltaY
	float intersectX = abs(deltaX) - (secondBodyHalfSize.x + firstBodyHalfSize.x);
	float intersectY = abs(deltaY) - (secondBodyHalfSize.y + firstBodyHalfSize.y);

	if((intersectY <= 0 && intersectY > -2)  || (intersectX <= 0 && intersectY > -2)) {
		return true;
	} else {
		return false;
	}
}

//Do player animation
sf::IntRect animate(int row, sf::Texture* texture, sf::Vector2u* imageCount, sf::Vector2u* currentImage, float switchTime, float* deltaTime) {
	float totalTime = 0.1f;
	sf::IntRect uvRect;

	currentImage->x = 0;
	currentImage->y = row;
	totalTime += *deltaTime;

	uvRect.width = texture->getSize().x / float(imageCount->x);
	uvRect.height = texture->getSize().y / float(imageCount->y);

	if (totalTime >= switchTime) {

		totalTime -= switchTime;
		currentImage->x++;

		if (currentImage->x >= imageCount->x) {
			currentImage->x = 0;
		}
	}

	uvRect.left = currentImage->x * uvRect.width;
	uvRect.top = currentImage->y * uvRect.height;

	return uvRect;
}

//Handle view resizing
void resizeView(const sf::RenderWindow* window, sf::View* view) {
	float aspectRatio = float(window->getSize().x) / float(window->getSize().y);
	view->setSize(MAIN_VIEW_HEIGHT * aspectRatio, MAIN_VIEW_HEIGHT);
}
