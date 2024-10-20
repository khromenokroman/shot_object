
#include <iostream>
#include <memory>

#include "game.hpp"

static constexpr int START_WINDOW_HEIGHT = 800;
static constexpr int START_WINDOW_WIDTH = 1200;

int main() {
    std::unique_ptr<Game> game;
    try {
        game = std::make_unique<Game>(START_WINDOW_WIDTH, START_WINDOW_HEIGHT);
    } catch (std::exception const &ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    game->run();

    return 0;
}
