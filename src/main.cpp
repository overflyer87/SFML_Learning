//Includes
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <unistd.h>

//Define constants at compile time
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAIN_VIEW_WIDTH 800.0f
#define MAIN_VIEW_HEIGHT 600.0f

#define GRAVITY float 981.0f

//Function prototypes
//Draw RectangleShapes
void drawRectangleShape(sf::RenderWindow*, sf::RectangleShape*);

//Draw Text
void drawText(sf::RenderWindow*, sf::Text*);

//Unite sound and soundBuffer loading into one function
bool setSoundBufferLoadSound(sf::SoundBuffer*, sf::Sound*, std::string);

//Function that calculates all parameters needed for Collision detection or Touching detection
std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f, sf::Vector2f, float, float, float, float> calculateCollisionParametersAABBXY(sf::RectangleShape*, sf::RectangleShape*);

//Function for touch detection
bool detectTouchingAABBXY(sf::RectangleShape*, sf::RectangleShape*);

//Collision detection AABB method for two bodies
//X and Y axis
//Also takes care of separated X OR Y axis collision
bool handleCollisionAABBXY(sf::RectangleShape*, sf::RectangleShape*, bool, int);

//Do player animation
//First chose the rectangle you want from your sprite
std::tuple<float, float, int> choosePartOfSprite(int row, sf::Texture*, sf::Vector2u, sf::Vector2u);
//Combine with update mechanism to create animation
sf::IntRect createAnimation(int, sf::Texture*, sf::Vector2u, sf::Vector2u, float, float, bool);

//==========
//START MAIN
//==========

int main(int argc, char* argv[]) {
	//Create the window and renderer all in one
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),	"My first SFML Game!");
	sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(MAIN_VIEW_WIDTH, MAIN_VIEW_HEIGHT));

	//Create fonts and text
	//Game Over Text
	sf::Font gameOverFont;
	sf::Text gameOver;
	gameOverFont.loadFromFile("tahomabd.ttf");
	gameOver.setFont(gameOverFont);
	gameOver.setString("GAME OVER");
	gameOver.setPosition(sf::Vector2f(400.0f, 300.0f));
	gameOver.setFillColor(sf::Color::Red);
	sf::Vector2f gameOverTextPos(window.getPosition().x / 2.0f, window.getPosition().y / 2.0f);

	//Load Music and Sound
	//Background Music
	sf::SoundBuffer soundBuffer;
	sf::Sound sound;
	setSoundBufferLoadSound(&soundBuffer, &sound, "bongojam.ogg");

	//Create world objects
	//Create Player
	sf::RectangleShape playerShape;
	sf::Texture playerTexture;
	playerTexture.loadFromFile("tux_from_linux.png");
	playerShape.setTexture(&playerTexture);
	playerTexture.setSmooth(true);
	//Create Box
	sf::RectangleShape boxShape;
	sf::Texture boxTexture;
	boxTexture.loadFromFile("box.png");
	boxShape.setTexture(&boxTexture);
	boxTexture.setSmooth(true);
	//Create floor
	sf::RectangleShape floorShape;
	sf::Texture floorTexture;
	floorTexture.loadFromFile("plain_block.png");
	floorShape.setTexture(&floorTexture);
	floorTexture.setSmooth(true);
	floorShape.rotate(180);
	//Create lava
	sf::RectangleShape lavaShape;
	sf::Texture lavaTexture;
	lavaTexture.loadFromFile("lava.png");
	lavaShape.setTexture(&lavaTexture);
	lavaTexture.setSmooth(true);

	//Position Objects
	//Position Player
	playerShape.setSize(sf::Vector2f(100.0f, 100.0f));
	playerShape.setOrigin(50.0f, 50.0f);
	playerShape.setPosition(50.0f, 327.0f);

	//Position Box
	boxShape.setSize(sf::Vector2f(70.0f, 70.0f));
	boxShape.setOrigin(35.0f, 35.0f);
	boxShape.setPosition(300.0f, 340.0f);

	//Position floor
	floorShape.setSize(sf::Vector2f(50.0f, 50.0f));
	floorShape.setOrigin(0.0f, 25.0f);

	//Position lava
	lavaShape.setSize(sf::Vector2f(float(WINDOW_WIDTH), 150.0f));

	//If wanted create arrays for repeatedly drawing objects
	//Position will then be set in for loop
	int elementsFloorArray = std::ceil(((float) WINDOW_WIDTH / floorShape.getSize().x)) + 1;
	int elementsLavaArray = std::ceil(((float) WINDOW_WIDTH / lavaShape.getSize().y)) + 1;
	sf::RectangleShape dummy;
	sf::RectangleShape floorArray[elementsFloorArray] = { dummy };
	sf::RectangleShape lavaArray[elementsLavaArray] = { lavaShape };
	int floorArrayLength = (sizeof(floorArray) / sizeof(floorArray[0]));
	int lavaArrayLength = (sizeof(lavaArray) / sizeof(lavaArray[0]));

	//If you have a sprite with more than 1 column or row deal with animation here
	//Player and NPCs
	sf::Clock clock;
	sf::Vector2u playerImageCount(3, 9);
	sf::Vector2u playerCurrentImage(0, 0);
	float deltaTime = 0.0f;

	//Set default values before entering game loop

	//Play world music by default
	sound.play();
	sound.setPlayingOffset(sf::seconds(2));
	sound.setLoop(true);
	sound.setVolume(10);
	bool playerIsDead = false;

	//Animate player to face right by default
	playerShape.setTextureRect(createAnimation(0, &playerTexture, playerImageCount, playerCurrentImage, deltaTime, 0.3f, true));


	//=====================
	//!!!Start Game loop!!!
	//=====================

	while (window.isOpen()) {

		deltaTime = clock.restart().asSeconds();
		usleep(deltaTime / 1000000);

		//Create event for Event loop
		sf::Event event;

		//Start Event loop
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				playerShape.setTextureRect(createAnimation(1, &playerTexture, playerImageCount, playerCurrentImage, deltaTime, 0.3f, false));
				playerShape.move(sf::Vector2f(-7.0f, 0.0f));

			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				playerShape.setTextureRect(createAnimation(1, &playerTexture, playerImageCount, playerCurrentImage, deltaTime, 0.3f, true));
				playerShape.move(sf::Vector2f(7.0f, 0.0f));

			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				//Pause world music using the 'm' key when sound is playing
				sound.pause();
			}

		}
		//We broke out of event loop because the queue is empty for this frame

		//Collision detection
		handleCollisionAABBXY(&playerShape, &boxShape, true, 1);

		//Clear the window on each frame, set color to a light sky blue
		window.clear(sf::Color(176, 226, 255, 100));

		//Set view
		window.setView(view);
		view.setCenter(sf::Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));

		//Draw non-repetitive objects
		drawRectangleShape(&window, &playerShape);
		drawRectangleShape(&window, &boxShape);

		//Draw repetitive objects
		//Draw the floor by iterating through floorShape-array

		for (int i = 0; i < floorArrayLength; i++) {

			if (i == 14) {
				i++;
			} else {
				floorArray[i] = floorShape;
				floorShape.setPosition(((0.0f + 50.0f) * float(i)), 400.0f);
				sf::RectangleShape* curFloorTile = &(floorArray[i]);
				drawRectangleShape(&window, &floorShape);
			}

			//If ground was not not drawn make box fall
			if((floorArray[i].getPosition().x == 0) && (floorArray[i].getPosition().y == 0) && (boxShape.getPosition().x > ((floorArray[i + 1].getPosition().x)) + 5.0f)) {
				boxShape.move(0.0f, 0.1f);
			}

			if((floorArray[i].getPosition().x == 0) && (floorArray[i].getPosition().y == 0) && (playerShape.getPosition().x > ((floorArray[i + 1].getPosition().x)) + 5.0f)) {
				playerShape.move(0.0f, 0.05f);
			}
		}


		//Draw the lava under the floor by iterating though lavaShape-array
		for (int i = 0; i < lavaArrayLength; i++) {
			sf::RectangleShape* curLavaTile = &(lavaArray[i]);
			lavaShape.setPosition(0.0f, (410.0f + (50.0f * float(i))));
			drawRectangleShape(&window, &lavaShape);

			/* COMMENTED SINCE VERY BUGGY, COLLISION HAPPENS AT POSITIONS WHERE THERE IS EVEN NO LAVA
			if(detectTouchingAABBXY(&lavaShape, &playerShape)) {
				playerIsDead = true;
				window.clear();
				sound.stop();
				window.draw(gameOver);
				window.display();

				usleep(2000000);
				window.close();
			}*/

		}

		//Drawing was done to back buffer. Now switch back and to front buffer instantly
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

//Function prototypes
//Draw RectangleShapes
void drawRectangleShape(sf::RenderWindow* window, sf::RectangleShape* rectShape) {
	window->draw(*rectShape);
}

//Draw Text
void drawText(sf::RenderWindow* window, sf::Text* text) {
	window->draw(*text);
}

//Unite sound and soundBuffer loading into one function
bool setSoundBufferLoadSound(sf::SoundBuffer* soundBuffer, sf::Sound* sound, std::string pathToSoundFile) {

	if(sound == nullptr || soundBuffer == nullptr || pathToSoundFile == "" || pathToSoundFile == " ")  {
		std::cout << "Sound or sound buffer or path to sound file are not valid!" << std::endl;
		return false;
	}

	soundBuffer->loadFromFile(pathToSoundFile);
	sound->setBuffer(*soundBuffer);

	std::cout << "Sound buffer created and sound loaded properly." << std::endl;

	return true;
}

std::tuple<sf::Vector2f, sf::Vector2f, sf::Vector2f, sf::Vector2f, float, float, float, float> calculateCollisionParametersAABBXY(sf::RectangleShape* firstBody, sf::RectangleShape* secondBody) {
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
		float intersectX = std::abs(deltaX) - (secondBodyHalfSize.x + firstBodyHalfSize.x);
		float intersectY = std::abs(deltaY) - (secondBodyHalfSize.y + firstBodyHalfSize.y);

	return std::make_tuple(posFirstBody, posSecondBody, firstBodyHalfSize, secondBodyHalfSize, deltaX, deltaY, intersectX, intersectY);
}



bool detectTouchingAABBXY(sf::RectangleShape* firstBody, sf::RectangleShape* secondBody) {

//Call calculateCollisionParametersAABBXY

	auto touchCalculatedParameters = calculateCollisionParametersAABBXY(firstBody, secondBody);

	float intersectX = std::get<6>(touchCalculatedParameters);
	float intersectY = std::get<7>(touchCalculatedParameters);

	if((intersectX <= 0 && intersectX > -2)  || (intersectY <= 0 && intersectY > -2)) {
		return true;
	} else {
		return false;
	}
}

//Handle collision according to AABB theorem
//Axis choice: 1 for x coordinate ONLY handling
//Axis choice: 2 for Y coordinate ONLY handling
//Axis choice: 3 for X and Y coordinate handling

bool handleCollisionAABBXY(sf::RectangleShape* firstBody, sf::RectangleShape* secondBody, bool push, int axisChoice) {
	auto touchCalculatedParameters = calculateCollisionParametersAABBXY(firstBody, secondBody);

	float deltaX = std::get<4>(touchCalculatedParameters);
	float deltaY = std::get<5>(touchCalculatedParameters);
	float intersectX = std::get<6>(touchCalculatedParameters);
	float intersectY = std::get<7>(touchCalculatedParameters);

		//If one of the intersection results is negative THERE IS A COLLISION

		//Check if there is a collision on the X axis ONLY if axisChois == 1

	switch(axisChoice) {

	case 1:
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
					firstBody->move(0.0f, 0.0f);
				} else {
					firstBody->move(0.0f, 0.0f);
				}
			}
			return true;
		}
		break;

		//Check if there is a collision on the Y axis ONLY if axisChoice == 2
	case 2:
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
						firstBody->move(0.0f, 0.0f);
					} else {
						firstBody->move(0.0f, 0.0f);
					}
				}
				return true;
			}
			break;


		//Finally check if the objects collide on BOTH X and Y axis

	case 3:
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
						firstBody->move(0.0f, 0.0f);
						secondBody->move(0.0f, 0.0f);
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
						firstBody->move(0.0f, 0.0f);
					} else {
						firstBody->move(0.0f, 0.0f);
					}
				}
			}
				//Return true if collision was detected
			return true;
		}
		break;

	default:
		std::cout << "The axis choice must be 1,2,3 for X, Y, XY, respectively. Wrong Choice. Will always return false until fixed!" << std::endl;
		break;
	}
		//Return false if no collision was detected
		return false;
}

//Do player animation
//First select the rectangle you want from your sprite
std::tuple<float, float, int> choosePartOfSprite(int row, sf::Texture* texture, sf::Vector2u imageCount, sf::Vector2u currentImage) {

	sf::IntRect uvRect;

	currentImage.x = 0;
	currentImage.y = row;

	uvRect.width = texture->getSize().x / float(imageCount.x);
	uvRect.height = texture->getSize().y / float(imageCount.y);

	return std::make_tuple(uvRect.width, uvRect.height, row);
}

//Combine with update mechanism to create animation
sf::IntRect createAnimation(int row, sf::Texture* texture, sf::Vector2u imageCount, sf::Vector2u currentImage, float deltaTime, float switchTime, bool faceRight) {

	float totalTime = 0.0f;
	auto getValuesFromChosenSpriteRect = choosePartOfSprite(row, texture, imageCount, currentImage);

	currentImage.y = std::get<2>(getValuesFromChosenSpriteRect);
	currentImage.x = 0;
	totalTime += deltaTime;

	if(totalTime >= switchTime) {
		totalTime -= switchTime;
		currentImage.x++;

		if(currentImage.x >= imageCount.x) {
			currentImage.x = 0;
		}
	}

	sf::IntRect uvRect;

	uvRect.width = std::get<0>(getValuesFromChosenSpriteRect);
	uvRect.height = std::get<1>(getValuesFromChosenSpriteRect);

	uvRect.top = currentImage.y * uvRect.height;


	if(faceRight) {
		uvRect.left = currentImage.x * uvRect.width;
		uvRect.width = std::abs(uvRect.width);
	} else {
		uvRect.left = (currentImage.x + 1) * std::abs(uvRect.width);
		uvRect.width = -std::abs(uvRect.width);
	}

	return uvRect;
}
