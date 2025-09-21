#include "Button.h"
#include <iostream>

Button::Button(
	sf::Font& font,
	const std::string& string, 
	const sf::Vector2f& size,
	const sf::Vector2f& pos,
	const sf::Color& color
)
{
	Background.setFillColor(color);
	Background.setSize(size);
	Background.setOrigin(size.x / 2, size.y / 2);
	Background.setPosition(pos);

	text.setFont(font);
	text.setString(string);
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(pos);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(Background);
	target.draw(text);
}

void Button::setUp()
{
	text.setOrigin(
		text.getGlobalBounds().width / 2, 
		text.getGlobalBounds().height / 2
	);
	text.setPosition(Background.getPosition());
}

bool Button::update(sf::RenderWindow& win)
{
	const sf::Vector2i mousePos = sf::Mouse::getPosition(win);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
		(
			mousePos.x > Background.getPosition().x - (Background.getSize().x / 2) &&
			mousePos.x < Background.getPosition().x + (Background.getSize().x / 2) &&
			mousePos.y > Background.getPosition().y - (Background.getSize().y / 2) &&
			mousePos.y < Background.getPosition().y + (Background.getSize().y / 2)
		)
	) 
	{
		std::cout << "hi" << std::endl;
		return true;
	}
	return false;
}
