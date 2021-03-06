#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#define M_PI 3.14159265358979323846
#include <cmath>
#include <vector>

#include "vita2dpp.h"
#include "vita_audio.h"

enum {
    TEXT_TOP    = 0,
    TEXT_CENTER = 1,
    TEXT_BOTTOM = 2,
    TEXT_LEFT   = 3,
    TEXT_RIGHT  = 4,
};

int vita2d_pgf_draw_aligned_text (vita2d_pgf* font, int x, int y,
                                  unsigned int color, float scale,
                                  int horiz, int vert,
                                  const char* text) {
    int width = -1, height = -1;
    vita2d_pgf_text_dimensions(font, scale, text, &width, &height);

    // left (horizontal) and top (vertical) do not need any processing
    if (horiz == TEXT_CENTER) {
        x -= 0.5 * width;
    } else if (horiz == TEXT_RIGHT) {
        x -= width;
    }

    if (vert == TEXT_CENTER) {
        y -= 0.5 * height;
    } else if (vert == TEXT_BOTTOM) {
        y -= height;
    }

    return vita2d_pgf_draw_text(font, x, y, color, scale, text);
}

// Constants
#define PADDLE_W (20.0f)
#define PADDLE_H (120.0f)
#define PADDLE_SPEED (7.5f)
#define BALL_SPEED (10.0f)
#define SCORE_WIN (10)
#define EXIT_COMBO (SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER)

struct Paddle : Rectangle {
    Paddle () : Rectangle() {
    }

    Paddle (glm::vec2 const& p0, glm::vec2 const& dims0) : Rectangle(p0, dims0) {
    }

    void clear () {
        score = 0;
    }

    bool player = true;
    int score = 0;
};

struct Ball : Circle {
    Ball () : Circle() {
    }

    Ball (glm::vec2 const& p0, float r) : Circle(p0, r) {
        setRandomSpeed();
    }

    void init (glm::vec2 const& p0, float r) {
        this->p = p0;
        this->r = r;
        setRandomSpeed();
    }

    void clear () {
        x() = SCREEN_W / 2;
        y() = SCREEN_H / 2;

        setRandomSpeed();
    }

    void setRandomSpeed () {
        float theta = 0.0f;
        if (ri(0, 1) == 0) {
            theta = rf(-M_PI / 4, M_PI / 4);
        } else {
            theta = rf(3 * M_PI / 4, 5 * M_PI / 4);
        }
        v0 = BALL_SPEED * glm::vec2(cos(theta), sin(theta));
        v = v0;
    }

    void move () {
        Circle::move(v);
    }

    glm::vec2 speed () const {
        return v;
    }

    glm::vec2& speed () {
        return v;
    }

    bool collide (Paddle const& paddle) {
        if (intersects(paddle)) {
            // Y coordinate of the intersection point (between -1 and 1)
            float interY = y() + radius() / 2,
                  relativeInterY = interY - paddle.top(),
                  normalizedInterY = relativeInterY / paddle.height();
            normalizedInterY = 2 * normalizedInterY - 1;

            // Angle
            float bounceAngle = normalizedInterY * maxBounceAngle;
            float n0 = glm::length(v0);

            // Update speed
            v.x = (paddle.player ? n0 : -n0) * cos(bounceAngle);
            v.y = n0 * sin(bounceAngle);

            return true;
        }

        return false;
    }

    glm::vec2 v0, v;
    float maxBounceAngle = M_PI / 6;
};

enum class GameState {
    Menu,
    Play,
    Pause,
    GameOver,
};

enum class GameMode {
    OnePlayer,
    TwoPlayers,
};

struct Menu {
    Menu () {
    }

    void init (const char* title) {
        this->title = title;
    }

    void up () {
        current = (current - 1 + choices.size()) % choices.size();
    }

    void down () {
        current = (current + 1) % choices.size();
    }

    void add (const char* name) {
        choices.emplace_back(name);
    }

    void render (int x  = SCREEN_W / 2, int y = SCREEN_H / 2,
                 int horiz = TEXT_CENTER, int vert = TEXT_CENTER) const {
        vita2d_pgf_draw_aligned_text(pgf, x, y, WHITE, 2.0f, horiz, vert, title);
        y += 50;

        for (unsigned int i = 0; i < choices.size(); ++i) {
            int c = WHITE;
            if (current == i) {
                c = RED;
            }

            vita2d_pgf_draw_aligned_text(pgf, x, y, c, 1.0f, horiz, vert, choices[i].name);
            y += 20;
        }
    }

    vita2d_pgf* pgf;

    struct Choice {
        Choice (const char* name) : name(name) {
        }

        const char* name;
    };

    unsigned int current = 0;
    const char* title;
    std::vector<Choice> choices;
};

struct Game {
    Game () {
        srand(time(nullptr));

        // vita2d initialization
        vita2d_init();
        vita2d_set_clear_color(BLACK);

        // Load PGF font
        pgf = vita2d_load_default_pgf();

        // Objects
        restart();

        // Menu
        menu.init("Pong");
        menu.pgf = pgf;
        menu.add("One Player");
        menu.add("Two Players");
        menu.add("Quit");

        // Audio
        // Library
        vitaWavInit();

        // Sounds
        beep = vitaWavLoad("app0:data/beep.wav");
        boop = vitaWavLoad("app0:data/boop.wav");
    }

    void restart () {
        state = GameState::Menu;
        exit = false;
        debug = false;

        // Ball
        ball.init(glm::vec2(SCREEN_W / 2, SCREEN_H / 2),
                  10);

        // Paddles
        player.init(glm::vec2(10, SCREEN_H / 2 - PADDLE_H / 2),
                    glm::vec2(PADDLE_W, PADDLE_H));
        player.player = true;
        player.clear();

        cpu.init(glm::vec2(SCREEN_W - 10 - PADDLE_W, SCREEN_H / 2 - PADDLE_H / 2),
                 glm::vec2(PADDLE_W, PADDLE_H));
        cpu.player = false;
        cpu.clear();
    }

    void handleInput () {
        // Exit
        if (input.isButtonPressed(EXIT_COMBO)) {
            exit = true;
        }

        // Debug
        if (input.isButtonPressedOnce(SCE_CTRL_SELECT)) {
            debug = !debug;
        }

        switch (state) {
            case GameState::Menu:
                if (input.isButtonPressedOnce(SCE_CTRL_UP)) {
                    menu.up();
                } else if (input.isButtonPressedOnce(SCE_CTRL_DOWN)) {
                    menu.down();
                }

                if (input.isButtonPressed(SCE_CTRL_CROSS)) {
                    switch (menu.current) {
                        case 0:
                            state = GameState::Play;
                            mode = GameMode::OnePlayer;
                            break;

                        case 1:
                            state = GameState::Play;
                            mode = GameMode::TwoPlayers;
                            break;

                        case 2:
                            exit = true;
                            break;

                        default:
                            break;
                    }
                }
                break;

            case GameState::Play:
                // Pause
                if (input.isButtonPressedOnce(SCE_CTRL_START)) {
                    state = GameState::Pause;
                }

                // Player moves with the left analog stick or Up / Down arrows
                if (abs(input.ly) > 50) {
                    player.moveY(PADDLE_SPEED * input.ly / 50.0f);
                }

                if (input.isButtonPressed(SCE_CTRL_UP)) {
                    player.moveY(-PADDLE_SPEED);
                } else if (input.isButtonPressed(SCE_CTRL_DOWN)) {
                    player.moveY(PADDLE_SPEED);
                }

                switch (mode) {
                    case GameMode::OnePlayer:
                        // TODO
                        break;

                    case GameMode::TwoPlayers:
                        // CPU (or Player 2) moves with the right analog stick or Triangle / Cross
                        if (abs(input.ry) > 50) {
                            cpu.moveY(PADDLE_SPEED * input.ry / 50.0f);
                        }

                        if (input.isButtonPressed(SCE_CTRL_TRIANGLE)) {
                            cpu.moveY(-PADDLE_SPEED);
                        } else if (input.isButtonPressed(SCE_CTRL_CROSS)) {
                            cpu.moveY(PADDLE_SPEED);
                        }
                        break;
                }
                break;

            case GameState::Pause:
                // Resume
                if (input.isButtonPressedOnce(SCE_CTRL_START)) {
                    state = GameState::Play;
                }

                // Go back to menu
                if (input.isButtonPressedOnce(SCE_CTRL_CIRCLE)) {
                    restart();
                }
                break;

            case GameState::GameOver:
                if (input.isButtonPressedOnce(SCE_CTRL_START)) {
                    restart();
                }
                break;
        }
    }

    void update () {
        handleInput();

        if (state != GameState::Play)
            return;

        // Move ball
        ball.move();

        // Check collisions
        // Player with screen boundaries
        if (player.y() < 0.0f) {
            player.y() = 0.0f;
        } else if (player.y() + player.height() > SCREEN_H) {
            player.y() = SCREEN_H - player.height();
        }

        // CPU with screen boundaries
        if (cpu.y() < 0.0f) {
            cpu.y() = 0.0f;
        } else if (cpu.y() + cpu.height() > SCREEN_H) {
            cpu.y() = SCREEN_H - cpu.height();
        }

        // Ball with screen boundaries
        if (ball.y() < 0.0f) {
            ball.y() = 0.0f;
            ball.speed().y = -ball.speed().y;
        } else if (ball.y() + 2 * ball.radius() > SCREEN_H) {
            ball.y() = SCREEN_H - 2 * ball.radius();
            ball.speed().y = -ball.speed().y;
        } else if (ball.x() < 0.0f) {
            cpu.score++;
            ball.clear();
            sleep(1);
        } else if (ball.x() + 2 * ball.radius() > SCREEN_W) {
            player.score++;
            ball.clear();
            sleep(1);
        }

        // Ball with the paddles
        if (ball.collide(player)) {
            vitaWavPlay(beep);
        } else if (ball.collide(cpu)) {
            vitaWavPlay(boop);
        }

        if (player.score >= SCORE_WIN) {
            state = GameState::GameOver;
        } else if (cpu.score >= SCORE_WIN) {
            state = GameState::GameOver;
        }
    }

    void render () const {
        switch (state) {
            case GameState::Menu:
                menu.render(SCREEN_W / 2, SCREEN_H / 2 - 100);
                break;

            case GameState::Play:
                ball.render(WHITE);
                player.render(WHITE);
                cpu.render(WHITE);

                if (debug) {
                    vita2d_pgf_draw_textf(pgf, SCREEN_W - 160, 30, GREEN, 1.0f, "FPS: %.2f", fps);
                }

                vita2d_pgf_draw_textf(pgf, SCREEN_W / 2 + 20, 30, WHITE, 2.0f, "%d", cpu.score);
                vita2d_pgf_draw_textf(pgf, SCREEN_W / 2 - 20, 30, WHITE, 2.0f, "%d", player.score);
                break;

            case GameState::Pause:
                vita2d_pgf_draw_aligned_text(pgf, SCREEN_W / 2, SCREEN_H / 2,
                                             WHITE, 1.0f,
                                             TEXT_CENTER, TEXT_CENTER,
                                             "Press Start to resume");
                vita2d_pgf_draw_aligned_text(pgf, SCREEN_W / 2, SCREEN_H / 2 + 20,
                                             WHITE, 1.0f,
                                             TEXT_CENTER, TEXT_CENTER,
                                             "Press Circle to return to Menu");
                break;

            case GameState::GameOver:
                vita2d_pgf_draw_text(pgf,
                                      SCREEN_W / 2 - 100, SCREEN_H / 2, WHITE, 1.0f, "Press Start to restart");
                break;
        }
    }

    void run () {
        while (! exit) {
            // Update
            input.update();
            update();
            input.endUpdate();

            // Render
            vita2d_start_drawing();
                vita2d_clear_screen();
                render();
            vita2d_end_drawing();

            // Calculate FPS
            cur_micros = sceKernelGetProcessTimeWide();

            if (cur_micros >= (last_micros + 1000000)) {
                dt_micros = cur_micros - last_micros;
                last_micros = cur_micros;
                fps = (frames/(double) dt_micros) * 1000000.0f;
                frames = 0;
            }

            frames++;

            vita2d_wait_rendering_done();
            vita2d_swap_buffers();
        }
        vita2d_fini();

        // Cleanup
        vita2d_free_pgf(pgf);

        vitaWavShutdown();
    }

    // FPS counting
    SceUInt64 cur_micros = 0, dt_micros = 0, last_micros = 0;
    uint32_t frames = 0;
    float fps = 0.0f;

    InputState input;
    bool exit = false;

    // Objects
    Paddle player, cpu;
    Ball ball;
    Menu menu;

    // Sounds
    vitaWav *beep = nullptr, *boop = nullptr;

    GameState state = GameState::Menu;
    GameMode mode;

    bool debug = false;
    vita2d_pgf* pgf;
};

int main (void) {
    Game().run();

    sceKernelExitProcess(0);

    return 0;
}

