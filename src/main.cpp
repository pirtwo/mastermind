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
    const float BOARD_X = 40;
    const float BOARD_Y = 100;
    const float SLOT_PADDING_X = 5;
    const float SLOT_PADDING_Y = 5;
    const float PEG_PADDING_X = 12;
    const float PEG_PADDING_Y = 12;

    sf::RenderWindow window(sf::VideoMode(500, 750), "Mastermind");
    window.setFramerateLimit(60);

    //======= load assets =======//
    sf::Font font;
    sf::Texture pinTexture;
    sf::Texture pegTexture;
    sf::Texture slotTexture;
    sf::Texture buttonTexture;
    if (!font.loadFromFile("./assets/OpenSans-Light.ttf") ||
        !pinTexture.loadFromFile("./assets/pin.png") ||
        !pegTexture.loadFromFile("./assets/peg.png") ||
        !slotTexture.loadFromFile("./assets/slot.png") ||
        !buttonTexture.loadFromFile("./assets/button.png"))
    {
        return EXIT_FAILURE;
    }

    //========= initialize ===========//
    srand(time(NULL));

    sf::Sprite pinSprite(pinTexture);
    //pinSprite.setScale(0.2, 0.2);
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
        black{sf::Color(1, 1, 1, 255)},
        white{sf::Color::White},
        yellow{sf::Color::Yellow},
        orange{sf::Color(255, 175, 0, 255)},
        blackFlag{sf::Color::Red},
        whiteFlag{sf::Color(125, 125, 125, 255)};
    std::array<Peg, COLOR_NUM> pegs = {red, blue, green, brown, black, white, yellow, orange};

    std::vector<Button> pegBtns;
    Button clearBtn(buttonTexture);
    clearBtn.text.setFont(font);
    clearBtn.text.setString("DEL");
    clearBtn.text.setCharacterSize(17);
    clearBtn.text.setFillColor(sf::Color::Red);
    clearBtn.text.setPosition(5, 5);
    clearBtn.setPosition(340, 20);
    Button checkBtn(buttonTexture);
    checkBtn.text.setFont(font);
    checkBtn.text.setString("SET");
    checkBtn.text.setCharacterSize(17);
    checkBtn.text.setFillColor(sf::Color::Red);
    checkBtn.text.setPosition(5, 5);
    checkBtn.setPosition(400, 20);
    for (size_t i = 0; i < pegs.size(); i++)
    {
        Button btn(pegTexture);
        btn.sp.setColor(pegs[i].color);
        btn.setScale(0.2, 0.2);
        btn.setPosition(i * (btn.getBounds().width + 5) + 50, 30);
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
        int blackFlagNum = 0;
        int whiteFlagNum = 0;

        /* check current guess against code */
        for (auto &&peg : pegs)
        {
            int inCode = 0;
            int inGuess = 0;
            int inplace = 0;
            int misplaced = 0;

            for (int i = 0; i < CODE_LEN; i++)
            {
                if (peg.color == code[i].color)
                    inCode++;
                if (peg.color == board[indexY][i].color)
                    inGuess++;
                if (peg.color == code[i].color &&
                    peg.color == board[indexY][i].color)
                    inplace++;
            }

            if (inCode > 0)
                misplaced = inCode - inGuess >= 0
                                ? inGuess - inplace
                                : inCode - inplace;

            blackFlagNum += inplace;
            whiteFlagNum += misplaced;
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
        {
            std::cout << "clear\r\n";

            if (indexX > 0)
            {
                indexX--;
                board[indexY][indexX] = empty;
            }
        }

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
                slotSprite.setColor(
                    indexX == j && indexY == i
                        ? sf::Color::Yellow
                        : sf::Color::White);
                slotSprite.setPosition(
                    BOARD_X + j * (slotSprite.getGlobalBounds().width + SLOT_PADDING_X),
                    BOARD_Y + i * (slotSprite.getGlobalBounds().height + SLOT_PADDING_X));
                window.draw(slotSprite);
            }

        /* draw pegs */
        for (int i = 0; i < GUESS_NUM; i++)
            for (int j = 0; j < CODE_LEN; j++)
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

        /* draw pins */
        for (int i = 0; i < GUESS_NUM; i++)
            for (int j = CODE_LEN; j < CODE_LEN * 2; j++)
            {
                auto pin = board[i][j];
                if (pin.color == empty.color)
                    continue;
                pinSprite.setColor(pin.color);
                pinSprite.setPosition(
                    BOARD_X + j * (pinSprite.getGlobalBounds().width + SLOT_PADDING_X) +
                        (slotSprite.getGlobalBounds().width + SLOT_PADDING_X) * CODE_LEN / 2,
                    BOARD_Y + i * (slotSprite.getGlobalBounds().height + SLOT_PADDING_Y) + PEG_PADDING_Y);
                window.draw(pinSprite);
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