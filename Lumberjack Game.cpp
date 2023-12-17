#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

char generateRandomDirection() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int randomValue = rand() % 2;

    return (randomValue == 0) ? 'L' : 'R';
}

int main() {

    SoundBuffer chopBuffer;
    if (!chopBuffer.loadFromFile("chop.ogg")) {
        cout << "Could not load chop.ogg" << endl;
        exit(5);
    }
    Sound chopSound;
    chopSound.setBuffer(chopBuffer);

    RenderWindow window(VideoMode(600, 800), "Lumberjack");
    World world(Vector2f(0, 0));
    int score = 0;
    Clock gameClock;

    PhysicsSprite& lumberjackRight = *new PhysicsSprite();
    Texture lumberjackRightTex;
    LoadTex(lumberjackRightTex, "images/lumberjack_R.png");
    lumberjackRight.setTexture(lumberjackRightTex);
    Vector2f szRight = lumberjackRight.getSize();
    lumberjackRight.setCenter(Vector2f(390, 800 - (szRight.y / 2) - 35));
    world.AddPhysicsBody(lumberjackRight);

    PhysicsSprite& lumberjackLeft = *new PhysicsSprite();
    Texture lumberjackLeftTex;
    LoadTex(lumberjackLeftTex, "images/lumberjack_L.png");
    lumberjackLeft.setTexture(lumberjackLeftTex);
    Vector2f szLeft = lumberjackLeft.getSize();
    lumberjackLeft.setCenter(Vector2f(210, 800 - (szLeft.y / 2) - 35));
    world.AddPhysicsBody(lumberjackLeft);

    PhysicsSprite trunk;
    Texture trunkTex;
    LoadTex(trunkTex, "images/trunk.png");
    trunk.setTexture(trunkTex);
    trunk.setCenter(Vector2f(300, 400));

    bool drawingBranchL = true;
    bool drawingBranchR = true;
    bool drawingNewBranchL = true;
    bool drawingNewBranchR = true;

    PhysicsSprite branchRight;
    Texture branchRTex;
    LoadTex(branchRTex, "images/branch_R.png");
    branchRight.setTexture(branchRTex);
    branchRight.setCenter(Vector2f(390, 230));
    world.AddPhysicsBody(branchRight);

    PhysicsSprite branchLeft;
    Texture branchLTex;
    LoadTex(branchLTex, "images/branch_L.png");
    branchLeft.setTexture(branchLTex);
    branchLeft.setCenter(Vector2f(210, 356));
    world.AddPhysicsBody(branchLeft);

    PhysicsRectangle bottom;
    bottom.setSize(Vector2f(600, 30));
    bottom.setCenter(Vector2f(300, 680));
    bottom.setStatic(true);
    world.AddPhysicsBody(bottom);

    /*
    // Collision Test Ball
    PhysicsCircle ball;
    ball.setCenter(Vector2f(580, 720));
    ball.setRadius(20);
    ball.setVelocity(Vector2f(-0.3, 0.0));
    world.AddPhysicsBody(ball);
    */

    Font fnt;
    if (!fnt.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    float branchLowerAmount = 32.0f; // Starting Two Branches Lower Amount
    bool rightKeyPressedPrev = false;
    bool leftKeyPressedPrev = false;

    branchLeft.onCollision = [&drawingBranchL, &branchLeft, &world](PhysicsBodyCollisionResult result) {
        if (drawingBranchL) {
            drawingBranchL = false;
            world.RemovePhysicsBody(branchLeft);
        }
        };

    branchRight.onCollision = [&drawingBranchR, &branchRight, &world](PhysicsBodyCollisionResult result) {
        if (drawingBranchR) {
            drawingBranchR = false;
            world.RemovePhysicsBody(branchRight);
        }
        };

    bool handleCollisions = false;
    int frameCounter = 0;
    const int framesWait = 2;

    lumberjackLeft.onCollision = [&branchLeft, &handleCollisions, &window](PhysicsBodyCollisionResult result) {
        if (handleCollisions) {
            cout << "Game Over :(" << endl;
            cout << "You were hit by a branch" << endl;
            window.close();
        }
        };

    lumberjackRight.onCollision = [&branchRight, &handleCollisions, &window](PhysicsBodyCollisionResult result) {
        if (handleCollisions) {
            cout << "Game Over :(" << endl;
            cout << "You were hit by a branch" << endl;
            window.close();
        }
        };

    vector<PhysicsSprite*> branches;
    vector<PhysicsSprite*> branchesToRemove;

    int keyPressCounter = 0;
    const int keyPressThreshold = 3; // New Branch Every # Key Presses

    // Game Loop
    while (window.isOpen()) {
        Time currentTime(clock.getElapsedTime());
        Time deltaTime(currentTime - lastTime);
        int deltaTimeMS(deltaTime.asMilliseconds());
        if (deltaTimeMS > 0) {
            world.UpdatePhysics(deltaTimeMS);
            lastTime = currentTime;
        }

        PhysicsSprite* currentLumberjack = &lumberjackRight;
        static bool displayLeftLumberjack = false;

        bool rightKeyPressed = Keyboard::isKeyPressed(Keyboard::Right);
        bool leftKeyPressed = Keyboard::isKeyPressed(Keyboard::Left);

        if (!handleCollisions) {
            frameCounter++;

            if (frameCounter >= framesWait) {
                handleCollisions = true;
            }
        }

        for (auto& branch : branches) {
            float branchMoveAmount = 39.0f; // New Branches Lower Amount

            if (rightKeyPressed && !rightKeyPressedPrev) {
                branch->move(0, branchMoveAmount);
                branch->setCenter(branch->getCenter() + Vector2f(0, branchMoveAmount));
            }

            if (leftKeyPressed && !leftKeyPressedPrev) {
                branch->move(0, branchMoveAmount);
                branch->setCenter(branch->getCenter() + Vector2f(0, branchMoveAmount));
            }
        }



        if (rightKeyPressed && !rightKeyPressedPrev) {
            keyPressCounter++;
            if (keyPressCounter >= keyPressThreshold) {
                PhysicsSprite* newBranch = new PhysicsSprite();

                if (generateRandomDirection() == 'R') {
                    newBranch->setTexture(branchRTex);
                    newBranch->setCenter(Vector2f(390, 0));
                }
                else {
                    newBranch->setTexture(branchLTex);
                    newBranch->setCenter(Vector2f(210, 0));
                }

                world.AddPhysicsBody(*newBranch);
                branches.push_back(newBranch);

                keyPressCounter = 0;
            }
        }

        if (leftKeyPressed && !leftKeyPressedPrev) {
            keyPressCounter++;
            if (keyPressCounter >= keyPressThreshold) {
                PhysicsSprite* newBranch = new PhysicsSprite();

                if (generateRandomDirection() == 'R') {
                    newBranch->setTexture(branchRTex);
                    newBranch->setCenter(Vector2f(390, 0));
                }
                else {
                    newBranch->setTexture(branchLTex);
                    newBranch->setCenter(Vector2f(210, 0));
                }

                world.AddPhysicsBody(*newBranch);
                branches.push_back(newBranch);

                keyPressCounter = 0;
            }
        }

        for (auto& newBranch : branches) {
            newBranch->onCollision = [&world, &branchesToRemove, &newBranch, &window](PhysicsBodyCollisionResult result) {
                branchesToRemove.push_back(newBranch);
                };
        }

        for (auto& branchToRemove : branchesToRemove) {
            auto it = find(branches.begin(), branches.end(), branchToRemove);
            if (it != branches.end()) {
                world.RemovePhysicsBody(*branchToRemove);
                delete branchToRemove;
                branches.erase(it);
            }
        }
        branchesToRemove.clear();

        if (rightKeyPressed && !rightKeyPressedPrev) {
            score++;
            chopSound.play();
            displayLeftLumberjack = false;
            world.RemovePhysicsBody(lumberjackLeft);
            branchRight.move(0, branchLowerAmount);
            branchLeft.move(0, branchLowerAmount);
            branchRight.setCenter(branchRight.getCenter() + Vector2f(0, branchLowerAmount));
            branchLeft.setCenter(branchLeft.getCenter() + Vector2f(0, branchLowerAmount));
        }

        if (leftKeyPressed && !leftKeyPressedPrev) {
            score++;
            chopSound.play();
            displayLeftLumberjack = true;
            world.RemovePhysicsBody(lumberjackRight);
            branchRight.move(0, branchLowerAmount);
            branchLeft.move(0, branchLowerAmount);
            branchRight.setCenter(branchRight.getCenter() + Vector2f(0, branchLowerAmount));
            branchLeft.setCenter(branchLeft.getCenter() + Vector2f(0, branchLowerAmount));
        }

        rightKeyPressedPrev = rightKeyPressed;
        leftKeyPressedPrev = leftKeyPressed;

        window.clear(Color(182, 255, 248));

        if (displayLeftLumberjack) {
            currentLumberjack = &lumberjackLeft;
            window.draw(*currentLumberjack);
        }
        else {
            window.draw(*currentLumberjack);
        }

        if (drawingBranchL) {
            window.draw(branchLeft);
        }
        if (drawingBranchR) {
            window.draw(branchRight);
        }

        if (drawingNewBranchL) {
            for (auto& newBranch : branches) {
                if (newBranch->getTexture() == &branchLTex) {
                    window.draw(*newBranch);
                }
            }
        }

        if (drawingNewBranchR) {
            for (auto& newBranch : branches) {
                if (newBranch->getTexture() == &branchRTex) {
                    window.draw(*newBranch);
                }
            }
        }

        Time elapsedTime = gameClock.getElapsedTime();

        // Ending Score
        if (score >= 30) {
            cout << "Game Over!" << endl;
            cout << "You chopped the tree in " << elapsedTime.asSeconds() << " seconds" << endl;
            window.close();
        }

        Text scoreText;
        scoreText.setFont(fnt);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::Black);
        scoreText.setPosition(10, 10);
        scoreText.setString("Score: " + to_string(score));
        scoreText.setString("Score: " + to_string(score) + "\nTime: " + to_string(static_cast<int>(elapsedTime.asSeconds())) + " seconds");


        //window.draw(ball);
        //window.draw(bottom);
        window.draw(trunk);
        window.draw(scoreText);
        window.display();
    }

    return 0;
}