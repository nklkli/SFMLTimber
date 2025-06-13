module;
#include "pch.h"
export module Timber;
//using namespace sf;
import std;
using std::chrono::system_clock;


auto getElapsedSeconds() {
	static auto start = system_clock::now();
	auto millis = duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
	auto seconds = millis / 1000.0f;
	return seconds;
}

float sinus(float frequency, float amplitude)
{
	const auto seconds = getElapsedSeconds();
	return sinf(frequency * std::numbers::pi_v<float> *2 * seconds) * amplitude;
}

float rand_clamp(int min, int max)
{
	return  static_cast<float>((rand() % abs(max - min)) + min);
}

void handleCount(
	const sf::RenderWindow& window,
	const sf::Time& elapsed,
	int cloudNumber,
	bool& cloudActive,
	float& cloudSpeed,
	sf::Sprite& spriteCloud
)
{
	// Manage the clouds
		// Cloud 1
	if (!cloudActive)
	{
		// How fast is the cloud
		cloudSpeed = rand_clamp(50, 100);

		// How high is the cloud
		float height = rand_clamp(0, 200);
		float x = rand_clamp(-1000, -200);
		spriteCloud.setPosition({ x, height });
		cloudActive = true;
	}
	else
	{
		spriteCloud.setPosition({
			spriteCloud.getPosition().x + (cloudSpeed * elapsed.asSeconds()),
			spriteCloud.getPosition().y
			});

		// Has the cloud reached the right hand edge of the screen?
		if (spriteCloud.getPosition().x > window.getSize().x + 350)
		{
			// Set it up ready to be a whole new cloud next frame
			cloudActive = false;
		}
	}
}


void updateBranches(int seed);
constexpr int NUM_BRANCHES = 6;
std::unique_ptr<sf::Sprite> branches[NUM_BRANCHES];
// Where is the player/branch?
// Left or Right
enum class Side{LEFT,RIGHT,NONE};
Side branchPositions[NUM_BRANCHES];


export
int main()
{
	std::srand(static_cast<unsigned int>(time(nullptr)));
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	sf::RenderWindow window(
		//sf::VideoMode(desktop.size, desktop.bitsPerPixel)
		sf::VideoMode({ 1400,800 })
		, "SFML works!"
		, sf::Style::Default
		//, sf::State::Fullscreen
	);


	window.setVerticalSyncEnabled(true);

	sf::Texture textureBackground{ "assets/graphics/background.png" };
	sf::Sprite spriteBackground{ textureBackground };

	sf::Texture textureTree{ "assets/graphics/tree.png" };
	sf::Sprite spriteTree{ textureTree };
	spriteTree.setPosition({ 810,0 });

	sf::Texture textureBee{ "assets/graphics/bee.png" };
	sf::Sprite spriteBee{ textureBee };
	float beeY = 100.0f;
	spriteBee.setPosition({ 0,beeY });
	// Is the bee currently moving?
	bool beeActive = false;
	// How fast can the bee fly
	float beeSpeed = 0.0f;

	// make 3 cloud sprites from 1 texture
	sf::Texture textureCloud{ "assets/graphics/cloud.png" };
	// 3 New sprites with the same texture
	sf::Sprite spriteCloud1{ textureCloud };
	sf::Sprite spriteCloud2{ textureCloud };
	sf::Sprite spriteCloud3{ textureCloud };
	// Position the clouds on the left of the screen
	// at different heights
	spriteCloud1.setPosition({ 0, 0 });
	spriteCloud2.setPosition({ 0, 100 });
	spriteCloud3.setPosition({ 0, 200 });
	// Are the clouds currently on screen?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	// How fast is each cloud?
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	/*sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);*/

	sf::Clock clock{};
	bool paused = true;
	int score = 0;

	sf::RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setFillColor(sf::Color::Red);
	timeBar.setOrigin({ 200,40 });
	timeBar.setPosition({ window.getSize().x / 2.f, window.getSize().y - 50.f });
	sf::Time gameTimeTotoal;
	float timeRemaining = 6;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	sf::Font font{ "assets/fonts/KOMIKAP_.ttf" };
	sf::Text messageText{ font, "Press ENTER to start!", 75 };
	messageText.setFillColor(sf::Color::White);
	messageText.setOrigin(messageText.getLocalBounds().getCenter());
	messageText.setPosition({
		window.getSize().x / 2.0f,
		window.getSize().y / 2.0f,
		});

	sf::Text scoreText{ font, "Score = 0", 100 };
	scoreText.setFillColor(sf::Color::White);

	scoreText.setPosition({ 20,20 });

	sf::Texture textureBranch{"asssets/graphics/branch.png"};
	for (int i = 0; i < NUM_BRANCHES; ++i)
	{
		branches[i].reset(new sf::Sprite{ textureBranch });
		branches[i]->setPosition({-2000, -2000});
		branches[i]->setOrigin({220,20});
	}
	


	while (window.isOpen())
	{



		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape))
		{
			window.close();
		}

		// Start the game		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Enter))
		{
			paused = false;
			score = 0;
			timeRemaining = 6;
			clock.start();
		}


		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			/*if (auto kp = event->getIf<sf::Event::KeyPressed>())
			{
				if (kp->scancode == sf::Keyboard::Scancode::Escape)
					window.close();
			}*/

		}

		/*
		**********************************************
		Update the scene
		********************************************
		*/
		if (!paused) {
			// Measure time
			sf::Time dt = clock.restart();
			timeRemaining -= dt.asSeconds();
			timeBar.setSize({
				timeBarWidthPerSecond * timeRemaining,
				timeBarHeight
				});

			if (timeRemaining <= 0) {
				clock.reset();
				paused = true;
				messageText.setString("Out of time!");
				messageText.setOrigin(messageText.getLocalBounds().getCenter());
				messageText.setPosition({
					window.getSize().x / 2.f,
					window.getSize().y / 2.f
					});
			}

			if (!beeActive)
			{
				// How fast is the bee
				//srand(static_cast<int>(time(nullptr)));
				beeSpeed = (rand() % 50) + 100.0f;

				// How high is the bee
				//srand(static_cast<int>(time(nullptr)) * 10);
				float height = (rand() % 500) + 50.f;
				spriteBee.setPosition(
					{
					static_cast<float>(window.getSize().x),
					height }
					);
				beeActive = true;
			}
			else// Move the bee
			{
				spriteBee.setPosition(
					{
						spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
						 beeY + sinus(0.1f, 100)
					}
				);
				// Has the bee reached the left-hand edge of the screen?
				if (spriteBee.getPosition().x < -100)
				{
					// Set it up ready to be a whole new bee next frame
					beeActive = false;
				}
			}

			// Manage the clouds
			// Cloud 1
			handleCount(window, dt, 0, cloud1Active, cloud1Speed, spriteCloud1);
			handleCount(window, dt, 1, cloud2Active, cloud2Speed, spriteCloud2);
			handleCount(window, dt, 2, cloud3Active, cloud3Speed, spriteCloud3);

			

			auto newScore = std::format("{}{}", "Score = ", score);
			scoreText.setString(newScore);

			/**************************************
			 * UPDATE THE BRANCH SPRITES
			 **************************************/
			for (int i = 0; i < NUM_BRANCHES; ++i)
			{
				float height = i * 150.f;
				if (branchPositions[i]==Side::LEFT)
				{
					//move the sprite to the left side
					branches[i]->setPosition({ 610,height });
					branches[i]->setRotation(sf::degrees(180));
				}else if (branchPositions[i]==Side::RIGHT)
				{
					// move the branch to right side
					branches[i]->setPosition({1330, height});
					branches[i]->setRotation(sf::degrees(0));
				}else
				{
					//hide the branch
					branches[i]->setPosition({-3000, height});
				}
			}
		}

		/* *************************************************
							DRAW THE SCENE
		****************************************************
		*/
		window.clear();
		window.draw(spriteBackground);
		// Draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		for (int i = 0; i < NUM_BRANCHES; ++i)
		{
			window.draw(*branches[i].get());
		}

		// Draw the tree
		window.draw(spriteTree);
		// Draw the insect
		window.draw(spriteBee);
		//window.draw(shape);

		window.draw(scoreText);
		window.draw(timeBar);

		if (paused) {
			window.draw(messageText);
		}
		window.display();
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
