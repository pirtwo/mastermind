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

    Peg
        empty{sf::Color(0, 0, 0, 255)},
        red{sf::Color::Red},
        blue{sf::Color::Blue},
        green{sf::Color::Green},
        brown{sf::Color(0, 0, 0, 255)},
        black{sf::Color::Black},
        white{sf::Color::White},
        yellow{sf::Color::Yellow},
        orange{sf::Color(0, 0, 0, 255)},
        flagBlack{sf::Color(0, 0, 0, 255)},
        flagWhite{sf::Color(0, 0, 0, 255)};

    std::array<Peg, 8> pegs = {
        red, blue, green, brown, black, white, yellow, orange};

    sf::Sprite peg(pegTexture);
    peg.setScale(0.2, 0.2);
    sf::Sprite slot(slotTexture);
    slot.setScale(0.2, 0.2);

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
    checkBtn.setPosition(200, 200);

    for (int i = 0; i < pegs.size(); i++)
    {
        Button btn(pegTexture);
        btn.sp.setColor(pegs[i].color);
        btn.setScale(0.2, 0.2);
        btn.setPosition(i * btn.getBounds().width, 20);
        pegBtns.push_back(btn);
    }

    srand(time(NULL));

    std::array<sf::Color, CODE_LEN> code = {empty};
    std::array<std::array<sf::Color, CODE_LEN * 2>, GUESS_NUM> board = {empty};
    int currRowIdx = 0;

    auto genCode = [&]()
    {
        for (int i = 0; i < code.size(); i++)
            code[i] = pegs[randInt(0, pegs.size())];
    };

    auto genFeedback = [&]()
    {
        int blackFlag = 0;
        int whiteFlag = 0;
        auto currRow = board[currRowIdx];

        /* check current guess */
        for (int i = 0; i < CODE_LEN; i++)
        {
            int guess = currRow[i];

            if (guess == code[i])
            {
                blackFlag++;
            }
            else if (std::any_of(code.begin(), code.end(), [&](int n)
                                 { return guess == n; }))
            {
                whiteFlag++;
            }
        }

        /* fill feedback */
        int index = CODE_LEN;
        while (blackFlag > 0 || whiteFlag > 0)
        {
            if (blackFlag > 0)
            {
                currRow[index] = Feedback::FLAG_BLACK;
                blackFlag--;
            }
            else
            {
                currRow[index] = Feedback::FLAG_WHITE;
                whiteFlag--;
            }
            index++;
        }
    };

    auto checkWin = [&]() -> bool
    {
        for (int i = 0; i < CODE_LEN; i++)
        {
            if (code[i] != board[currRowIdx][i])
                return false;
        }
        return true;
    };

    auto checkLose = [&]() -> bool
    {
        return currRowIdx >= GUESS_NUM;
    };

    auto newGame = [&]()
    {
        code = {0};
        board = {0};
        currRowIdx = 0;
    };

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
            std::cout << "check\r\n";

        clearBtn.update(window);
        if (clearBtn.hasClick())
            std::cout << "clear\r\n";

        for (auto &&btn : pegBtns)
        {
            btn.update(window);
            if (btn.hasClick())
            {
                //
            }
        }

        //========= draw ==========//
        window.clear();

        for (int i = 0; i < GUESS_NUM; i++)
            for (int j = 0; j < CODE_LEN; j++)
            {
                slot.setPosition(
                    BOARD_X + j * (slot.getGlobalBounds().width + SLOT_PADDING_X),
                    BOARD_Y + i * (slot.getGlobalBounds().height + SLOT_PADDING_X));
                window.draw(slot);
            }

        for (int i = 0; i < GUESS_NUM; i++)
            for (int j = 0; j < CODE_LEN; j++)
            {
                int guess = board[i][j];

                switch (guess)
                {
                case 0:
                    peg.setColor(sf::Color::Red);
                    break;
                case Peg::BLUE:
                    peg.setColor(sf::Color::Blue);
                    break;
                case Peg::GREEN:
                    peg.setColor(sf::Color::Green);
                    break;
                case Peg::BROWN:
                    peg.setColor(sf::Color(0, 0, 0, 255));
                    break;
                case Peg::BLACK:
                    peg.setColor(sf::Color::Black);
                    break;
                case Peg::WHITE:
                    peg.setColor(sf::Color::White);
                    break;
                case Peg::YELLOW:
                    peg.setColor(sf::Color::Yellow);
                    break;
                case Peg::ORANGE:
                    peg.setColor(sf::Color(0, 0, 0, 255));
                    break;
                default:
                    break;
                }

                peg.setPosition(
                    BOARD_X + j * (slot.getGlobalBounds().width + SLOT_PADDING_X) + PEG_PADDING_X,
                    BOARD_Y + i * (slot.getGlobalBounds().height + SLOT_PADDING_Y) + PEG_PADDING_Y);
                window.draw(peg);
            }

        for (auto &&i : pegBtns)
        {
            window.draw(i);
        }

        window.draw(checkBtn);
        window.draw(clearBtn);

        window.display();
    }

    return EXIT_SUCCESS;
}