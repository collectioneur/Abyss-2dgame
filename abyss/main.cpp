#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <string>

#include "monsterspawner.hpp"
#include "bullet.hpp"
#include "button.hpp"
#include "character.hpp"
#include "gamestate.hpp"
#include "healthBar.hpp"
#include "logoAnimation.hpp"
#include "menu.hpp"
#include "monster.hpp"
#include "endscreen.hpp"
#include "helpPanel.hpp"
#include "statistics.hpp"
#include "characters.hpp"
#include "characterInfo.hpp"
#include "pause.hpp"

using namespace sf;
using namespace std;

int main() {
    RenderWindow window(VideoMode(1920, 1080), "Abyss");
    Font font;
    if (!font.loadFromFile("./Fonts/NightinTokyo.ttf")) {
        cerr << "Error loading font" << endl;
        return -1;
    }

    GameState gameState = GameState::MENU;
    Menu menu(font, gameState);
    EndScreen endscreen(font, gameState);
    Statistics Statistics(font, gameState);
    Statistics.loadStats();
    Texture logoTexture, logoText, menuTexture, gameTexture, characterTexture, bulletTexture, monsterTexture_1, monsterTexture_2, monsterTexture_3, monsterTexture_4, gameLogoTexture, gunWindowTexture, katanaWindowTexture, characterTextureFlipped, monsterTexture_2Flipped;
    if (!logoTexture.loadFromFile("Assets/logo.png") ||
        !logoText.loadFromFile("Assets/textlogo.png") ||
        !menuTexture.loadFromFile("Assets/menulogo.WEBP") ||
        !gameTexture.loadFromFile("Assets/gamebackground.png") ||
        !characterTexture.loadFromFile("Assets/character3.png") ||
        !bulletTexture.loadFromFile("Assets/bullet2.png") ||
        !monsterTexture_1.loadFromFile("Assets/monsterbasic.png") || !gameLogoTexture.loadFromFile("Assets/gamelogo.png") ||
        !monsterTexture_2.loadFromFile("Assets/monster_2.png") || !monsterTexture_3.loadFromFile("Assets/monster_3.png") || !monsterTexture_4.loadFromFile("Assets/monsterbasic.png") || !gunWindowTexture.loadFromFile("Assets/gunPanel.png") || !katanaWindowTexture.loadFromFile("Assets/katanaPanel.png") || !characterTextureFlipped.loadFromFile("Assets/flippedCharacter.png") || !monsterTexture_2Flipped.loadFromFile("Assets/monster_2Flipped.png")) {
        cerr << "Error loading image" << endl;
        return -1;
    }
    vector<Monster> monsters;
    vector<characterInfo> allCharacters;
    allCharacters.emplace_back(characterTextureFlipped , font, "Asuna", "In her childhood,\nAsuna lost her family due to demons\nthat invaded her village.\nFor twelve years now,\nshe has been storming the gates of hell\nto reach her offenders\nand is annihilating the demonic horde.");
    allCharacters.emplace_back(monsterTexture_1 , font, "Nosferatu", "A fat and slow demon,\npossessing immense strength and health.\nIf not killed,\nit will pursue you for\nthe rest of your life.");
    allCharacters.emplace_back(monsterTexture_2Flipped , font, "Void", "An extremely fast creature\nthat destroys everything in its path.\nIt sees nothing in front of itself,\nbut this does not prevent it from\ninjuring the character upon contact\nand leaving two offspring after its death.");
    allCharacters.emplace_back(monsterTexture_3 , font, "Sol interfectorem", "Sun killer, rising from the ground every few seconds\nand shooting bullets over long distances.\nIt can be visible and invisible,\nand appear at any moment in any place!");
    characters Characters(font, gameState);
    pause Pause(font, gameState);
    Character character(characterTexture, window.getSize(), bulletTexture);
    int characterAtThisMoment = 0;
    Vector2f helpPanelPos = Vector2f(50.f, 50.f);
    helpPanel helpPanel(gunWindowTexture, font, katanaWindowTexture, helpPanelPos);
    LogoAnimation logoAnimation(logoTexture, logoText);
    FloatRect bounds = logoAnimation.getLocalBounds();
    float xLogo = (window.getSize().x - bounds.width) / 2;
    float yLogo = (window.getSize().y - bounds.height) / 2;
    logoAnimation.setPosition(xLogo, yLogo, 800);

    Sprite menuSprite(menuTexture);
    float yScale = (window.getSize().y / menuSprite.getLocalBounds().height);
    float xScale = (window.getSize().x / menuSprite.getLocalBounds().width);
    menuSprite.setScale(xScale, yScale);

    Sprite gameBackground(gameTexture);
    yScale = (window.getSize().y / gameBackground.getLocalBounds().height);
    xScale = (window.getSize().x / gameBackground.getLocalBounds().width);
    gameBackground.setScale(xScale, yScale);
    
    
    Clock spawnClock;
    Time decreasingTime = seconds(0.f);
    Time spawnInterval = seconds(2.f);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(2, 3);

    Clock clock;
    FloatRect playArea(100.f, 70.f, window.getSize().x - 200.f, window.getSize().y - 180.f);
    FloatRect spawnArea1(600.f, 71.f, 100.f, 0.f), spawnArea2(window.getSize().x-101.f, 250.f, 0.f, 100.f);
    int monsterType = 0;
    int monsterAmount = 3;
    int monster3Amount = 0, monster2Amount = 0, monster1Amount = 0;
    MonsterSpawner spawner_1(monsterTexture_1, spawnArea1, spawnArea2, bulletTexture);
    MonsterSpawner spawner_2(monsterTexture_2, spawnArea1, spawnArea2, bulletTexture);
    MonsterSpawner spawner_3(monsterTexture_3, playArea, playArea, bulletTexture);
    MonsterSpawner spawner_4(monsterTexture_4, spawnArea1, spawnArea2, bulletTexture);
    int swapWeapons = 0;
    Clock limitKatana;
    float katanaCooldown = 0.5;
    Clock timeSpentGame;
    bool gameStart = 0;
    float timeSpent = 0.0;
    int demonsKilled = 0;
    //statistics
    int demonsKilledByType[6];
    for(int i = 0; i<6; i++) {
        demonsKilledByType[i] = 0;
    }
    bool stopStatistics = 0;
    int bulletsLeft = 1000;
    int katanasLeft = 30;
    helpPanel.weaponsLeft(bulletsLeft, katanasLeft);
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed && gameState == GameState::GAME) {
                character.setDestination(window.mapPixelToCoords(Mouse::getPosition(window)));
            } else if (event.type == Event::KeyPressed && gameState == GameState::GAME) {
                if (event.key.code == Keyboard::Space && !swapWeapons && bulletsLeft) {
                    bulletsLeft--;
                    character.shoot(window.mapPixelToCoords(Mouse::getPosition(window)));
                } else if (event.key.code == Keyboard::Space && swapWeapons && katanasLeft) {
                    if (limitKatana.getElapsedTime() >= seconds(katanaCooldown)) {
                        katanasLeft--;
                        character.attackWithKatana(window.mapPixelToCoords(Mouse::getPosition(window)), monsters);
                        limitKatana.restart();
                    }
                } else if (event.key.code == Keyboard::M) {
                    helpPanel.change();
                    swapWeapons++;
                    swapWeapons %= 2;
                }
                else if (event.key.code == Keyboard::Escape) {
                    gameState = GameState::PAUSE;
                }
            }
        }
        if (gameState == GameState::GAME) {
            if(!gameStart) {
                gameStart = 1;
                timeSpentGame.restart();
            }
            helpPanel.weaponsLeft(bulletsLeft, katanasLeft);
            Time deltaTime = clock.restart();
            character.faceTowards(window.mapPixelToCoords(Mouse::getPosition(window)));
            character.update(deltaTime, playArea);
            character.updateBullets(deltaTime, playArea);
            Time elapsed = spawnClock.getElapsedTime();
            if (elapsed >= spawnInterval - decreasingTime && monsters.size() < 10) {
                monsterType = rand()%monsterAmount+1;
                if(monsterType == 1 && monster1Amount - character.amountKilledDemonType(1) < 4) {
                    monster1Amount++;
                    spawner_1.spawnMonster(monsters, monsterType);
                }
                else if(monsterType == 2 && monster2Amount - character.amountKilledDemonType(2) < 3) {
                    monster2Amount++;
                    spawner_2.spawnMonster(monsters, monsterType);
                }
                else if(monsterType == 3 && monster3Amount - character.amountKilledDemonType(3) < 3){
                    monster3Amount++;
                    spawner_3.spawnMonster(monsters, monsterType);
                }
                if (decreasingTime < seconds(0.5)) decreasingTime += seconds(0.02);
                spawnClock.restart();
                spawnInterval = seconds(dis(gen)) - decreasingTime;
            }
            if(character.checkKilledMonster_2()) {
                monsters.emplace_back(monsterTexture_2, character.getPosKilledMonster_2(), 5, bulletTexture);
                auto monsterIt = monsters.end()-1;
                monsterIt -> setDirectionForBabyMonster_1(character.getDirKilledMonster_2());
                monsters.emplace_back(monsterTexture_2, character.getPosKilledMonster_2(), 5, bulletTexture);
                monsterIt++;
                monsterIt -> setDirectionForBabyMonster_1(-character.getDirKilledMonster_2());
                character.resetKilledMonster_2();
            }
            for (auto& monster : monsters) {
                monster.update(deltaTime, character, monsters, playArea);
            }
            character.checkCollisionWithMonsters(monsters);
            if (character.getHealth() <= 0) gameState = GameState::END_GAME;
            character.updateInvulnerability(deltaTime);
            character.checkBulletMonsterCollisions(monsters);
        }
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        if(gameState == GameState::MENU) {
            menu.update(mousePos, event);
        }
        if(gameState == GameState::END_GAME) {
            endscreen.update(mousePos, event);
        }
        if(gameState == GameState::STATISTICS) {
            Statistics.update(mousePos, event);
        }
        if(gameState == GameState::CHARACTERS) {
            characterAtThisMoment = Characters.getcharacterNum();
            Characters.update(mousePos, event);
            allCharacters[Characters.getcharacterNum()].update(mousePos, event);
        }
        if(gameState == GameState::PAUSE) {
            Pause.update(mousePos, event);
        }
        Time elapsed = clock.restart();
        if (!logoAnimation.isFinished()) {
            logoAnimation.update(elapsed);
        }

        window.clear();
        if (!logoAnimation.isFinished()) {
            logoAnimation.draw(window);
        } else {
            if (gameState == GameState::MENU) {
                Characters.reset();
                window.draw(menuSprite);
                menu.draw(window, gameLogoTexture);
            } else if (gameState == GameState::GAME) {
                stopStatistics = 0;
                window.draw(gameBackground);
                if (swapWeapons) window.draw(character.getAttackArea(mousePos));
                character.draw(window);
                character.drawBullets(window);
                for (auto& monster : monsters) {
                    monster.draw(window);
                }
                helpPanel.draw(window);
            } else if (gameState == GameState::END_GAME) {
                for (auto it = monsters.begin(); it != monsters.end();) {
                    monsters.erase(it);
                }
                monster1Amount = 0;
                monster2Amount = 0;
                monster3Amount = 0;
                katanasLeft = 30;
                bulletsLeft = 1000;
                swapWeapons = 0;
                decreasingTime = seconds(0.f);
                if(gameStart){
                    timeSpent = timeSpentGame.getElapsedTime().asSeconds();
                    gameStart = 0;
                    demonsKilled = character.howManyKilled();
                    timeSpentGame.restart();
                }
                for(int i = 0; i<6; i++) {
                    demonsKilledByType[i] = character.amountKilledDemonType(i);
                }
                if(!stopStatistics) {
                    Statistics.setStats(timeSpent, demonsKilled, demonsKilledByType);
                    Statistics.saveStats();
                    stopStatistics = 1;
                }
                endscreen.statistics(demonsKilled, timeSpent);
                
                window.draw(menuSprite);
                character.reset(window.getSize());
                helpPanel.reset();
                if(Pause.showButtonEnd() == 1) {
                    Pause.draw(window);
                    gameState = GameState::GAME;
                    Pause.returnButtonEndToZero();
                }
                else if(Pause.showButtonEnd() == 2) {
                    Pause.draw(window);
                    gameState = GameState::MENU;
                    Pause.returnButtonEndToZero();
                }
                else endscreen.draw(window);
            }
            else if(gameState == GameState::STATISTICS) {
                window.draw(menuSprite);
                Statistics.draw(window);
                Statistics.saveStats();
            }
            else if(gameState == GameState::CHARACTERS) {
                window.draw(menuSprite);
                Characters.draw(window);
                allCharacters[Characters.getcharacterNum()].draw(window);
                if(characterAtThisMoment != Characters.getcharacterNum()) {
                    allCharacters[characterAtThisMoment].reset();
                }
            }
            else if(gameState == GameState::PAUSE) {
                window.draw(menuSprite);
                Pause.draw(window);
            }
        }
        window.display();
    }

    return 0;
}
