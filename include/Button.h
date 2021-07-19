#ifndef BUTTON_H_INCLUDE
#define BUTTON_H_INCLUDE

#include <SFML/Graphics.hpp>

class Button : public sf::Drawable, public sf::Transformable
{
protected:
    sf::Sprite _sp;
    bool _press, _release;

public:
    Button(sf::Texture &t);

    ~Button();

    sf::FloatRect getBounds();

    bool hasClick();

    void update(sf::RenderWindow &target);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

#endif