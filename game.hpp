#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <memory>

struct Game {
   private:
    SDL_Rect m_rec_texture_new_game{0, 0, 0, 0};
    int m_win_width;  // высота окна
    int m_win_height; // ширина окна
    int m_sdl_init;   // статус загрузки SDL
    int m_img_init;   // статус загрузки подсистемы изображений
    int m_ttf_init;   // статус загрузки подсистемы шрифтов
    std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> m_window;       // окно
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> m_renderer; // рисовальщик
    std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)>
        m_surface_background_image; // поверхность для заднего фона меню
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> m_texture_background_image; // текстура заднего фона меню
    std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> m_ttf_font;                       // шрифт
    bool m_is_running{true};   // флаг остановки главного цикла
    bool m_user_choice{false}; // флаг выбора меню пользователем

    // рисует задний фон
    void draw_background_game() {}

    // получает события системы
    void get_os_event(SDL_Event &event) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                m_is_running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x{}, mouse_y{};
                SDL_GetMouseState(&mouse_x, &mouse_y);
                SDL_Point mouse_pos{mouse_x, mouse_y};
                std::cout << "mouse_x: " << mouse_pos.x << " mouse_y: " << mouse_pos.y << '\n';
                if (SDL_PointInRect(&mouse_pos, &m_rec_texture_new_game)) {
                    m_user_choice = true;
                }
                std::cout << "m_user_choice: " << m_user_choice << '\n';
            }
        }
    }

   public:
    Game(int win_width, int win_height)
        : m_win_height{win_height},
          m_win_width{win_width},
          m_sdl_init{SDL_Init(SDL_INIT_VIDEO)},
          m_img_init{IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)},
          m_ttf_init{TTF_Init()},
          m_window{SDL_CreateWindow("test_window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_win_width,
                                    m_win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
                   SDL_DestroyWindow},
          m_renderer{SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer},
          m_surface_background_image{IMG_Load("../images/start_menu.jpg"), SDL_FreeSurface},
          m_texture_background_image{SDL_CreateTextureFromSurface(m_renderer.get(), m_surface_background_image.get()),
                                     SDL_DestroyTexture},
          m_ttf_font{TTF_OpenFont("../fonts/DejaVuSans.ttf", 25), TTF_CloseFont} {
        if (m_sdl_init != 0) {
            throw std::runtime_error("Error while init video system: " + std::string(SDL_GetError()));
        }
        if (m_img_init == 0) {
            throw std::runtime_error("Error while init image system: " + std::string(SDL_GetError()));
        }
        if (!m_surface_background_image) {
            throw std::runtime_error("Error while load background image: " + std::string(SDL_GetError()));
        }
        if (m_ttf_init) {
            throw std::runtime_error("Error while init ttf system: " + std::string(SDL_GetError()));
        }

        if (!m_window) {
            throw std::runtime_error("Error while create new window: " + std::string(SDL_GetError()));
        }

        if (!m_renderer) {
            throw std::runtime_error("Error while create renderer: " + std::string(SDL_GetError()));
        }
    }
    ~Game() {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    // Показывает начальный экран
    void show_menu() {
        SDL_RenderClear(m_renderer.get());
        SDL_RenderCopy(m_renderer.get(), m_texture_background_image.get(), nullptr, nullptr);

        std::string new_game{"New game"}, records{"Records"}, exit{"Exit"};

        std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_text_new_game{
            TTF_RenderText_Blended(m_ttf_font.get(), new_game.c_str(), {180, 0, 0, 255}), SDL_FreeSurface};
        std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_text_records{
            TTF_RenderText_Blended(m_ttf_font.get(), records.c_str(), {180, 0, 0, 255}), SDL_FreeSurface};
        std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_text_exit{
            TTF_RenderText_Blended(m_ttf_font.get(), exit.c_str(), {180, 0, 0, 255}), SDL_FreeSurface};

        std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_new_game{
            SDL_CreateTextureFromSurface(m_renderer.get(), surface_text_new_game.get()), SDL_DestroyTexture};
        m_rec_texture_new_game = {10, 100, surface_text_new_game->w, surface_text_new_game->h};
        SDL_RenderCopy(m_renderer.get(), texture_new_game.get(), nullptr, &m_rec_texture_new_game);
        std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_records{
            SDL_CreateTextureFromSurface(m_renderer.get(), surface_text_records.get()), SDL_DestroyTexture};
        SDL_Rect rec_texture_records{10, 100 + surface_text_new_game->h, surface_text_records->w,
                                     surface_text_records->h};
        SDL_RenderCopy(m_renderer.get(), texture_records.get(), nullptr, &rec_texture_records);
        std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_exit{
            SDL_CreateTextureFromSurface(m_renderer.get(), surface_text_exit.get()), SDL_DestroyTexture};
        SDL_Rect rec_texture_exit{10, 100 + surface_text_new_game->h + surface_text_records->h, surface_text_exit->w,
                                  surface_text_exit->h};
        SDL_RenderCopy(m_renderer.get(), texture_exit.get(), nullptr, &rec_texture_exit);

        SDL_RenderPresent(m_renderer.get());

        SDL_Event event;
        while (m_is_running && !m_user_choice) {
            get_os_event(event);
            SDL_Delay(500);
        }
    }

    // основной цикл программы
    void run() {
        std::cout << "Start main loop game" << '\n';
        SDL_RenderClear(m_renderer.get());
        std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_background_image_game{
            IMG_Load("../images/background_game.jpg"), SDL_FreeSurface};
        if (!surface_background_image_game) {
            throw std::runtime_error("Error while load background image game: " + std::string(SDL_GetError()));
        }
        std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_background_image{
            SDL_CreateTextureFromSurface(m_renderer.get(), surface_background_image_game.get()), SDL_DestroyTexture};

        std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface_player_image{IMG_Load("../images/player.png"),
                                                                                   SDL_FreeSurface};
        if (!surface_player_image) {
            throw std::runtime_error("Error while load background image game: " + std::string(SDL_GetError()));
        }
        std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> texture_player_image{
            SDL_CreateTextureFromSurface(m_renderer.get(), surface_player_image.get()), SDL_DestroyTexture};
        SDL_Rect player_rect{0, 0, 220, surface_player_image->h / 2};
        SDL_Rect dst_player{100, m_win_height - 250, player_rect.w, player_rect.h};

        int frame = 0;
        int frame_count = 5;
        int cur_frame_time = 0;
        int max_frame_time = 200;
        int last_time = SDL_GetTicks();
        int new_time = 0;
        int dt = 0;

        SDL_Event event;
        while (m_is_running) {
            SDL_RenderClear(m_renderer.get());
            SDL_RenderCopy(m_renderer.get(), texture_background_image.get(), nullptr, nullptr);
            get_os_event(event);
            new_time = SDL_GetTicks();
            dt = new_time - last_time;
            last_time = new_time;
            cur_frame_time += dt;
            if (cur_frame_time >= max_frame_time) {
                cur_frame_time -= max_frame_time;
                frame = (frame + 1) % frame_count;
                player_rect.x = player_rect.w * frame;
            }
            SDL_RenderCopy(m_renderer.get(), texture_player_image.get(), &player_rect, &dst_player);
            SDL_RenderPresent(m_renderer.get());
            SDL_Delay(100);
        }
    }
};