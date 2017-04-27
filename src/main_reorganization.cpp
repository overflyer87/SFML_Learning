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

//Draw rectangle shapes repetitively
void drawRectangleShapesRep(sf::RenderWindow*, sf::RectangleShape*, sf::RectangleShape[], int, char, float);

//Unite loading Function and Drawing text in one function
bool loadFontSetText(sf::Text*, sf::Font*, std::string, std::string, sf::Vector2f*, int, int, int, int, std::string, int);

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

//==========
//START MAIN
//==========

int main(int argc, char* argv[]) {
	//Create the window and renderer all in one
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),	"My first SFML Game!");

	//Create fonts and text
	//Game Over Text
	sf::Font gameOverFont;
	sf::Text gameOver;
	sf::Vector2f gameOverTextPos(100.0f, 200.0f);

	std::cout << loadFontSetText(&gameOver, &gameOverFont, "tahomabd.ttf", "Game Over", &gameOverTextPos, 128, 128, 128, 0, "Bold", 64) << std::endl;

	//Load Music and Sound
	//Background Music
	sf::SoundBuffer soundBuffer;
	sf::Sound sound;
	setSoundBufferLoadSound(&soundBuffer, &sound, "bongojam.ogg");

	//Create world objects
	//Create Player
	sf::RectangleShape playerShape;
	sf::Texture playerTexture;
	playerTexture.loadFromFile("player.png");
	playerShape.setTexture(&playerTexture);
	//Create Box
	sf::RectangleShape boxShape;
	sf::Texture boxTexture;
	boxTexture.loadFromFile("box.png");
	boxShape.setTexture(&boxTexture);
	//Create floor
	sf::RectangleShape floorShape;
	sf::Texture floorTexture;
	floorTexture.loadFromFile("plain_block.png");
	floorShape.setTexture(&floorTexture);
	//Create lava
	sf::RectangleShape lavaShape;
	sf::Texture lavaTexture;
	lavaTexture.loadFromFile("lava.png");
	lavaShape.setTexture(&lavaTexture);


	//Position Objects
	//Position Player
	playerShape.setSize(sf::Vector2f(100.0f, 100.0f));
	playerShape.setOrigin(50.0f, 50.0f);
	playerShape.setPosition(50.0f, 327.0f);

	//Position Box
	boxShape.setSize(sf::Vector2f(70.0f, 70.0f));
	boxShape.setOrigin(35.0f, 35.0f);
	boxShape.setPosition(300.0f, 327.0f);

	//Position floor
	floorShape.setSize(sf::Vector2f(50.0f, 50.0f));
	floorShape.setOrigin(0.0f, 25.0f);
	floorShape.rotate(180);

	//Position lava
	lavaShape.setSize(sf::Vector2f(float(WINDOW_WIDTH), 150.0f));

	//If you have a sprite with more than 1 column or row deal with animation here
	//Player and NPCs
	sf::Clock clock;
	sf::Vector2u playerImageCount(3, 9);
	sf::Vector2u playerCurrentImage;
	float deltaTime = 0.0f;

	//Play world music by default
	sound.play();
	sound.setPlayingOffset(sf::seconds(2));
	sound.setLoop(true);
	sound.setVolume(10);


	//=====================
	//!!!Start Game loop!!!
	//=====================

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

		}
		//We broke out of event loop because the queue is empty for this frame

		//Animate player
		//playerShape.setTextureRect(animate(0, &playerTexture, &playerImageCount, &playerCurrentImage, 0.3f, &deltaTime));

		//Collision detection
		handleCollisionAABBXY(&playerShape, &boxShape, true, 1);

		//Clear the window on each frame, set color to a light sky blue
		window.clear(sf::Color(176, 226, 255, 100));

		//Draw non-repetitive objects
		drawRectangleShape(&window, &playerShape);
		drawRectangleShape(&window, &boxShape);

		//Draw repetitive objects
		sf::RectangleShape floorArray[17] = { floorShape };
		drawRectangleShapesRep(&window, &floorShape, floorArray, 17, 'X', 400.0f);
		//sf::RectangleShape lavaArray[4] = { lavaShape };
		//drawRectangleShapesRep(&window, &lavaShape, lavaArray, 17, 'Y', 0.0f);

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

//Draw rectangle shapes repetitively
void drawRectangleShapesRep(sf::RenderWindow* window, sf::RectangleShape* rectShape, sf::RectangleShape shapeArray[], int size, char axis, float otherAxisPos) {

	if(axis == 'x' || axis == 'X') {
		for(int i = 0; i < size; i++) {
			shapeArray[i].setSize(sf::Vector2f(50.0f, 50.0f));
			shapeArray[i].setOrigin(sf::Vector2f(25.0f, 25.0f));
			shapeArray[i].setPosition(((0.0f + (shapeArray[i].getSize().x)) * float(i)), otherAxisPos);
			window->draw(shapeArray[i]);
		}
	} else if(axis == 'y' || axis == 'Y') {
		for(int i = 0; i < size; i++) {
			shapeArray[i].setSize(sf::Vector2f(50.0f, 50.0f));
			shapeArray[i].setOrigin(sf::Vector2f(25.0f, 25.0f));
			shapeArray[i].setPosition(otherAxisPos, ((0.0f + (shapeArray[i].getSize().y)) * float(i)));
			window->draw(shapeArray[i]);
		}
	} else {
		std::cout << "Only allowed values for char axis are X, x, Y or y!" << std::endl;
	}
}

//Unite loading Function and Drawing text in one function
bool loadFontSetText(sf::Text* text, sf::Font* font, std::string pathToFont, std::string textPrinted, sf::Vector2f* position, int r, int g, int b, int a, std::string style, int size) {

	if(text == nullptr || font == nullptr || pathToFont == "" || pathToFont == " " || textPrinted == "" || textPrinted == " " || r < 0 || g < 0 || b < 0 || a < 0 || size < 0) {
		std::cout << "One of the arguments was in invalid range. Loading font and drawing text aborted!" << std::endl;
		return false;
	}

	std::cout << pathToFont << std::endl;

	//Create font
	font->loadFromFile(pathToFont);

	//Create text
	text->setFont(*font);
	text->setString(textPrinted);
	text->setCharacterSize(size);
	text->setPosition(*position);
	text->setFillColor(sf::Color(r, g, b, a));

	if (style == "Bold") {
		text->setStyle(sf::Text::Bold);
	} else if (style == "Italic") {
		text->setStyle(sf::Text::Italic);
	} else if (style == "StrikeThrough") {
		text->setStyle(sf::Text::StrikeThrough);
	} else if (style == "Underlined") {
		text->setStyle(sf::Text::Underlined);
	} else if (style == "Regular") {
		text->setStyle(sf::Text::Regular);
	} else {
		std::cout << "The style color was not valid. In order to not break function: Setting it to Regular!" << std::endl;
		text->setStyle(sf::Text::Regular);
	}
	std::cout << "Font loaded and Text set properly!" << std::endl;
	return true;
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
					firstBody->move(intersectX, 0.0f);
				} else {
					firstBody->move(-intersectX, 0.0f);
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
						firstBody->move(0.0f, intersectY);
					} else {
						firstBody->move(0.0f, -intersectY);
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
		break;

	default:
		std::cout << "The axis choice must be 1,2,3 for X, Y, XY, respectively. Wrong Choice. Will always return false until fixed!" << std::endl;
		break;
	}
		//Return false if no collision was detected
		return false;
}
