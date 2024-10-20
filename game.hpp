#pragma once
#include <SDL2/SDL.h>

#include <memory>

struct Game {
   private:
    int m_win_width;
    int m_win_height;
    int m_init;
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> m_window;
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer;
    bool is_running{true};

    // рисует задний фон
    void draw_background() {
        SDL_SetRenderDrawColor(m_renderer.get(), 190, 189, 191, 255);
        SDL_RenderClear(m_renderer.get());
        SDL_RenderPresent(m_renderer.get());
    }

    // получает события системы
    void get_os_event(SDL_Event &event) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }
    }

   public:
    Game(int win_width, int win_height)
        : m_win_height{win_height},
          m_win_width{win_width},
          m_init{SDL_Init(SDL_INIT_VIDEO)},
          m_window{SDL_CreateWindow("test_window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_win_width,
                                    m_win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
                   SDL_DestroyWindow},
          m_renderer{SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer} {
        if (m_init != 0) {
            throw std::runtime_error("Error while init video system: " + std::string(SDL_GetError()));
        }

        if (!m_window) {
            throw std::runtime_error("Error while create new window: " + std::string(SDL_GetError()));
        }

        if (!m_renderer) {
            throw std::runtime_error("Error while create renderer: " + std::string(SDL_GetError()));
        }
    }
    ~Game() { SDL_Quit(); }

    // основной цикл программы
    void run() {
        SDL_Event event;
        while (is_running) {
            draw_background();
            get_os_event(event);
        }
    }
};