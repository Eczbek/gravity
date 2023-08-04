#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <xieite/xieite.hpp>

// Here be dragons

struct Entity {
	double mass;
	xieite::geometry::Point position;
	xieite::geometry::Point velocity;
	xieite::graphics::Color color;

	constexpr Entity(const double mass = 1, const xieite::geometry::Point position = xieite::geometry::Point(), const xieite::geometry::Point velocity = xieite::geometry::Point(), const xieite::graphics::Color color = xieite::graphics::Color()) noexcept
	: mass(mass), position(position), velocity(velocity), color(color) {}
};

int main() {
	xieite::system::BufferPosition screenSize = xieite::system::terminal.getScreenSize();
	screenSize.column /= 2;

	std::mt19937 rng = std::mt19937(std::random_device {}());
	auto distW = std::uniform_real_distribution<double>(0, 1);
	auto distX = std::uniform_real_distribution<double>(-10, 10);
	auto distY = std::uniform_real_distribution<double>(-1, 1);
	auto distZ = std::uniform_int_distribution<int>(0, 255);

	std::vector<Entity> entities;
	for (int i = 0; i < 3; ++i) {
		entities.push_back(Entity(distW(rng), xieite::geometry::Point(distX(rng), distX(rng)), xieite::geometry::Point(distY(rng), distY(rng)), xieite::graphics::Color(distZ(rng), distZ(rng), distZ(rng))));
	}

	while (true) {
		for (Entity& entity1 : entities) {
			for (auto i = entities.begin(); i != entities.end(); ++i) {
				Entity& entity2 = *i;
				if (&entity1 == &entity2) {
					continue;
				}
				const double distanceX = (entity2.position.x - entity1.position.x);
				const double distanceY = (entity2.position.y - entity1.position.y);
				entity1.velocity.x += entity1.mass * entity2.mass / distanceX * std::abs(distanceX);
				entity1.velocity.y += entity1.mass * entity2.mass / distanceY * std::abs(distanceY);
			}
		}

		double centerX = 0;
		double centerY = 0;
		for (Entity& entity : entities) {
			centerX += entity.position.x += entity.velocity.x;
			centerY += entity.position.y += entity.velocity.y;
		}
		centerX /= entities.size();
		centerY /= entities.size();

		xieite::system::terminal.clearScreen();

		for (Entity& entity : entities) {
			const double positionX = entity.position.x - centerX + screenSize.column / 2;
			const double positionY = entity.position.y - centerY + screenSize.row / 2;
			if ((positionX < 0) || (positionX >= screenSize.column) || (positionY < 0) || (positionY >= screenSize.row)) {
				continue;
			}
			xieite::system::terminal.setCursorPosition(xieite::system::BufferPosition(positionY, positionX * 2));
			xieite::system::terminal.setBackgroundColor(entity.color);
			std::cout << "  ";
			xieite::system::terminal.resetBackgroundColor();
		}

		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
