#include "Button.h"

Button::Button(sf::Texture &t)
{
    text = sf::Text();
    _press = false;
    _release = false;
    sp = sf::Sprite(t);
}

Button::~Button()
{
    //
}

sf::FloatRect Button::getBounds()
{
    return getTransform().transformRect(sp.getLocalBounds());
}

bool Button::hasClick()
{
    bool click = _press == false && _release == true;
    _release = false;
    return click;
}

void Button::update(sf::RenderWindow &target)
{
    sf::Vector2f mousePos(
        sf::Mouse::getPosition(target).x,
        sf::Mouse::getPosition(target).y);

    if (getBounds().contains(mousePos) &&
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        _press = true;
        _release = false;
    }
    else
    {
        _release = _press ? true : false;
        _press = false;
    }
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(sp, states.transform * getTransform());
    target.draw(text, states.transform * getTransform() * text.getTransform());
}
