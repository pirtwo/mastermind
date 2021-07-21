#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "Button.h"

struct Peg
{
    sf::Color color;
};

int randInt(int min, int max)
{
    return min + rand() % (max - min);
}

int main()
{
    const int CODE_LEN = 4;
    const int COLOR_NUM = 8;
    const int GUESS_NUM = 10;
    const float BOARD_X = 50;
    const float BOARD_Y = 100;
    const float SLOT_PADDING_X = 5;
    const float SLOT_PADDING_Y = 5;
    const float PEG_PADDING_X = 10;
    const float PEG_PADDING_Y = 10;

    sf::RenderWindow window(sf::VideoMode(500, 700), "Mastermind");
    window.setFramerateLimit(60);

    //======= load assets =======//
    sf::Font font;
    sf::Texture pegTexture;
    sf::Texture slotTexture;
    if (!font.loadFromFile("./assets/OpenSans-Light.ttf") ||
        !pegTexture.loadFromFile("./assets/peg.png") ||
        !slotTexture.loadFromFile("./assets/slot.png"))
    {
        return EXIT_FAILURE;
    }

    //========= initialize ===========//
    srand(time(NULL));

    sf::Sprite pegSprite(pegTexture);
    pegSprite.setScale(0.2, 0.2);
    sf::Sprite slotSprite(slotTexture);
    slotSprite.setScale(0.2, 0.2);

    Peg
        empty{sf::Color(0, 0, 0, 255)},
        red{sf::Color::Red},
        blue{sf::Color::Blue},
        green{sf::Color::Green},
        brown{sf::Color(150, 125, 70, 255)},
        black{sf::Color::Black},
        white{sf::Color::White},
        yellow{sf::Color::Yellow},
        orange{sf::Color(255, 175, 0, 255)},
        blackFlag{sf::Color(1, 1, 1, 255)},
        whiteFlag{sf::Color(255, 255, 255, 255)};
    std::array<Peg, COLOR_NUM> pegs = {red, blue, green, brown, black, white, yellow, orange};

    std::vector<Button> pegBtns;
    Button clearBtn(slotTexture);
    clearBtn.text.setFont(font);
    clearBtn.text.setString("CLEAR");
    clearBtn.text.setCharacterSize(17);
    clearBtn.text.setFillColor(sf::Color::Red);
    clearBtn.text.setScale(2, 2);
    clearBtn.text.setPosition(10, 10);
    clearBtn.setScale(0.2, 0.2);
    clearBtn.setPosition(200, 200);
    Button checkBtn(slotTexture);
    checkBtn.text.setFont(font);
    checkBtn.text.setString("CHECK");
    checkBtn.text.setCharacterSize(17);
    checkBtn.text.setFillColor(sf::Color::Red);
    checkBtn.text.setScale(2, 2);
    checkBtn.text.setPosition(10, 10);
    checkBtn.setScale(0.2, 0.2);
    checkBtn.setPosition(300, 200);
    for (size_t i = 0; i < pegs.size(); i++)
    {
        Button btn(pegTexture);
        btn.sp.setColor(pegs[i].color);
        btn.setScale(0.2, 0.2);
        btn.setPosition(i * btn.getBounds().width + 10, 20);
        pegBtns.push_back(btn);
    }

    int indexX = 0, indexY = 0;
    std::array<Peg, CODE_LEN> code;
    std::array<std::array<Peg, CODE_LEN * 2>, GUESS_NUM> board;

    auto genCode = [&]()
    {
        for (size_t i = 0; i < code.size(); i++)
            code[i] = pegs[randInt(0, pegs.size())];
    };

    auto genFeedback = [&]()
    {
        std::vector<Peg> visited;
        int blackFlagNum = 0;
        int whiteFlagNum = 0;

        /* check current guess against code */
        for (int i = 0; i < CODE_LEN; i++)
        {
            if (board[indexY][i].color == code[i].color)
                blackFlagNum++;

            // TODO: count white flags
        }

        /* fill feedback */
        int index = CODE_LEN;
        while (blackFlagNum > 0 || whiteFlagNum > 0)
        {
            if (blackFlagNum > 0)
            {
                board[indexY][index] = blackFlag;
                blackFlagNum--;
            }
            else
            {
                board[indexY][index] = whiteFlag;
                whiteFlagNum--;
            }
            index++;
        }
    };

    auto checkWin = [&]() -> bool
    {
        for (int i = 0; i < CODE_LEN; i++)
        {
            if (code[i].color != board[indexY][i].color)
                return false;
        }
        return true;
    };

    auto checkLose = [&]() -> bool
    {
        return indexY >= GUESS_NUM;
    };

    auto newGame = [&]()
    {
        code = {empty};
        board = {empty};
        indexX = 0;
        indexY = 0;
        genCode();
    };

    newGame();

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            switch (e.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
            }
        }

        //========= update =========//
        checkBtn.update(window);
        if (checkBtn.hasClick())
        {
            std::cout << "check\r\n";

            if (indexX == CODE_LEN)
            {
                genFeedback();

                if (checkWin())
                {
                    std::cout << "Win!!!\r\n";
                }
                else if (checkLose())
                {
                    std::cout << "Lose!!!\r\n";
                }

                indexX = 0;
                indexY++;
            }
        }

        clearBtn.update(window);
        if (clearBtn.hasClick())
            std::cout << "clear\r\n";

        for (int i = 0; i < COLOR_NUM; i++)
        {
            pegBtns[i].update(window);
            if (pegBtns[i].hasClick())
            {
                std::cout << "click " << i << "\r\n";

                if (indexX >= 0 && indexX < CODE_LEN)
                {
                    board[indexY][indexX] = pegs[i];
                    indexX++;
                }
            }
        }

        //========= draw ==========//
        window.clear(sf::Color::White);

        /* draw slots */
        for (int i = 0; i < GUESS_NUM; i++)
            for (int j = 0; j < CODE_LEN; j++)
            {
                slotSprite.setPosition(
                    BOARD_X + j * (slotSprite.getGlobalBounds().width + SLOT_PADDING_X),
                    BOARD_Y + i * (slotSprite.getGlobalBounds().height + SLOT_PADDING_X));
                window.draw(slotSprite);
            }

        /* draw pegs */
        for (int i = 0; i < GUESS_NUM; i++)
            for (int j = 0; j < CODE_LEN * 2; j++)
            {
                auto peg = board[i][j];
                if (peg.color == empty.color)
                    continue;
                pegSprite.setColor(peg.color);
                pegSprite.setPosition(
                    BOARD_X + j * (slotSprite.getGlobalBounds().width + SLOT_PADDING_X) + PEG_PADDING_X,
                    BOARD_Y + i * (slotSprite.getGlobalBounds().height + SLOT_PADDING_Y) + PEG_PADDING_Y);
                window.draw(pegSprite);
            }

        /* draw result */
        for (int i = 0; i < CODE_LEN; i++)
        {
            auto peg = code[i];
            pegSprite.setColor(peg.color);
            pegSprite.setPosition(i * pegSprite.getGlobalBounds().width, 600);
            window.draw(pegSprite);
        }

        /* draw buttons */
        for (auto &&i : pegBtns)
            window.draw(i);
        window.draw(checkBtn);
        window.draw(clearBtn);

        window.display();
    }

    return EXIT_SUCCESS;
}