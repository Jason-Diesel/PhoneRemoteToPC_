#pragma once
#include <SFML/Graphics.hpp>

class Button : public sf::Drawable
{
private:
	sf::RectangleShape Background;
	sf::Text text;
public:
	Button(
		sf::Font& font,
		const std::string& string,
		const sf::Vector2f& size,
		const sf::Vector2f& pos,
		const sf::Color& color
	);
	void setUp();
	bool update(sf::RenderWindow& win);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};