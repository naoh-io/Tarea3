#include <iostream>
#include <map>
#include <list>
#include <functional>

class Entity {
    int health;
    int x, y;
    std::string name;
    int level;

public:
    Entity(const std::string& name, int health, int x, int y, int level);
    void heal(int amount);
    void damage(int amount);
    void move(int dx, int dy);
    void levelUp();
    void mostrarestado() const;
    int getHealth() const;
    int getX() const;
    int getY() const;
    int getLevel() const;
};

Entity::Entity(const std::string& name, int health, int x, int y, int level)
    : name(name), health(health), x(x), y(y), level(level) {}

void Entity::heal(int amount) {
    health += amount;
}

void Entity::damage(int amount) {
    health -= amount;
}

void Entity::move(int dx, int dy) {
    x += dx;
    y += dy;
}

void Entity::levelUp() {
    level++;
}

void Entity::mostrarestado() const {
    std::cout << "Entity: " << name
              << "Salud: " << health
              << "Pos: (" << x << "," << y << ")"
              << "Nivel: " << level << std::endl;
}

int main() {

}