#include <iostream>
#include <map>
#include <list>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>

class Entity {
    int salud;
    int x, y;
    std::string name;
    int level;

public:
    Entity(const std::string& name, int salud, int x, int y, int level);
    void heal(int amount);
    void damage(int amount);
    void move(int dx, int dy);
    void levelUp();
    void mostrarestado() const;

    int getsalud() const { return salud; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getNivel() const { return level; }
};


Entity::Entity(const std::string& nombre, int salud, int x, int y, int level)
    : name(nombre), salud(salud), x(x), y(y), level(level) {}

void Entity::heal(int amount) {
    salud += amount;
    std::cout << "Healed by " << amount << ". New salud: " << salud << std::endl;
}

void Entity::damage(int amount) {
    salud -= amount;
    std::cout << "Damaged by " << amount << ". New salud: " << salud << std::endl;
}

void Entity::move(int dx, int dy) {
    x += dx;
    y += dy;
    std::cout << "Movido a (" << x << "," << y << ")" << std::endl;
}

void Entity::levelUp() {
    level++;
    std::cout << "Level up! Now level " << level << std::endl;
}

void Entity::mostrarestado() const {
    std::cout << "Entidad: " << name
              << "Salud: " << salud
              << "Pos: (" << x << "," << y << ")"
              << "Nivel: " << level << std::endl;
}

using Command = std::function<void(const std::list<std::string>&)>;

class CommandCenter {
    Entity& entity;
    std::map<std::string, Command> commands;
    std::list<std::string> history;

public:
    CommandCenter(Entity& ent);

    void registerCommand(const std::string& name, Command cmd);
    void unregisterCommand(const std::string& name);
    void execute(const std::string& name, const std::list<std::string>& args);
    void showHistory() const;

    void registerMacro(const std::string& name,
                       const std::list<std::pair<std::string, std::list<std::string>>>& steps);
    void executeMacro(const std::string& name);
};

CommandCenter::CommandCenter(Entity& ent) : entity(ent) {}

void CommandCenter::registerCommand(const std::string& name, Command cmd) {
    commands[name] = cmd;
    std::cout << "Comando '" << name << "' registrado correctamente!\n";
}

void CommandCenter::unregisterCommand(const std::string& name) {
    auto it = commands.find(name);
    if (it != commands.end()) {
        commands.erase(it);
        std::cout << "Comando '" << name << "' removido.\n";
    } else {
        std::cout << "Comando '" << name << "' no encontrado.\n";
    }
}

void CommandCenter::execute(const std::string& name, const std::list<std::string>& args) {
    auto it = commands.find(name);
    if (it != commands.end()) {
        std::ostringstream before;
        before << "Antes " << name << ": salud=" << entity.getsalud()
               << ", pos=(" << entity.getX() << "," << entity.getY() << ")";

        try {
            it->second(args);
        } catch (const std::exception& e) {
            std::cout << "Error executing command: " << e.what() << std::endl;
            return;
        }

        std::ostringstream after;
        after << "Despues " << name << ": salud=" << entity.getsalud()
              << ", pos=(" << entity.getX() << "," << entity.getY() << ")";

        history.push_back(before.str());
        history.push_back(after.str());
    } else {
        std::cout << "Error: Comando '" << name << "' no fue encontrado.\n";
    }
}

void CommandCenter::showHistory() const {
    std::cout << "\nHISTORIAL DE COMANDO" << std::endl;
    for (const auto& entry : history) {
        std::cout << entry << std::endl;
    }
}

void CommandCenter::registerMacro(
    const std::string& name,
    const std::list<std::pair<std::string, std::list<std::string>>>& steps)
{
    commands[name] = [this, steps, name](const std::list<std::string>&) {
        std::cout << "\n--- Macro: " << name << " ---" << std::endl;
        for (auto it = steps.begin(); it != steps.end(); ++it) {
            const auto& cmdName = it->first;
            const auto& args = it->second;
            auto cmdIt = commands.find(cmdName);
            if (cmdIt == commands.end()) {
                std::cout << "Macro error: comando '" << cmdName << "' no fue encontrado\n";
                return;
            }
            execute(cmdName, args);
        }
        std::cout << "--- End of Macro ---\n" << std::endl;
    };
    std::cout << "Macro '" << name << "' registered.\n";
}

void CommandCenter::executeMacro(const std::string& name) {
    execute(name, {});
}

class MoveCommand {
    Entity& e;
    int executionCount = 0;
public:
    MoveCommand(Entity& ent) : e(ent) {}
    void operator()(const std::list<std::string>& args) {
        if (args.size() != 2) throw std::runtime_error("move needs dx dy");
        int dx = std::stoi(args.front());
        int dy = std::stoi(*std::next(args.begin()));
        e.move(dx, dy);
        executionCount++;
        std::cout << "Move executed " << executionCount << " times\n";
    }
};

// Función libre
void cmd_heal(Entity& e, const std::list<std::string>& args) {
    if (args.size() != 1) throw std::runtime_error("se necesita un arg");
    int amount = std::stoi(args.front());
    e.heal(amount);
}

int main() {
    std::cout << "Creando entidad------" << std::endl;
    Entity hero("Heroe", 100, 0, 0, 1);
    hero.mostrarestado();

    CommandCenter center(hero);

    // comando como función libre
    center.registerCommand("heal", [&center, &hero](const std::list<std::string>& args) {
        cmd_heal(hero, args);
    });

    // comando como lambda
    center.registerCommand("damage", [&hero](const std::list<std::string>& args) {
        if (args.size() != 1) throw std::runtime_error("damage needs 1 arg");
        int dmg = std::stoi(args.front());
        hero.damage(dmg);
    });

    // comando como functor
    center.registerCommand("move", MoveCommand(hero));


    center.registerCommand("levelup", [&hero](const std::list<std::string>&) {
        hero.levelUp();
    });

    center.registerCommand("status", [&hero](const std::list<std::string>&) {
        hero.mostrarestado();
    });


    std::cout << "\nEJECUTANDO COMANDOS------------" << std::endl;
    center.execute("heal", {"30"});
    center.execute("damage", {"20"});
    center.execute("move", {"5", "3"});
    center.execute("levelup", {});
    center.execute("status", {});

    std::cout << "\nTESTEO DE COMANDO INVALIDO" << std::endl;
    center.execute("invalid", {});

    std::cout << "\nREGISTRO DE MACROS-----------" << std::endl;

    // Macro 1: Heal y status
    center.registerMacro("quick_heal", {
        {"heal", {"50"}},
        {"status", {}}
    });

    // Macro 2: Movimiento y daño
    center.registerMacro("attack_and_move", {
        {"damage", {"15"}},
        {"move", {"2", "1"}},
        {"status", {}}
    });

    // Macro 3: Recuperación completa
    center.registerMacro("full_recovery", {
        {"heal", {"100"}},
        {"levelup", {}},
        {"status", {}}
    });

    // Ejecutar macros
    std::cout << "\nEJECUCION DE MACROS" << std::endl;
    center.executeMacro("quick_heal");
    center.executeMacro("attack_and_move");
    center.executeMacro("full_recovery");
    center.showHistory();

    std::cout << "\nTESTEO PARA REMOVER LOS COMANDOS" << std::endl;
    center.unregisterCommand("levelup");
    std::cout << "\nESTADO FINAL DEL SISTEMA -------------\n";
    hero.mostrarestado();
    return 0;
}