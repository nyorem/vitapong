#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <psp2/kernel/processmgr.h>
#include "vita2dpp.h"

#define EXIT_COMBO SCE_CTRL_START
#define SCORE_WIN 10

struct Paddle : Rectangle {
    Paddle (Vec2f const& p0, Vec2f const& dims0) : Rectangle(p0, dims0) {
    }

    bool player = true;
    int score = 0;
};

struct Ball : Circle {
    Ball (Vec2f p0, Vec2f v0, float r) : Circle(p0, r), v0(v0), v(v0) {
    }

    void move (float dt) {
        Circle::move(dt * v);
    }

    Vec2f speed () const {
        return v;
    }

    Vec2f& speed () {
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
            float maxBounceAngle = M_PI / 6;
            float bounceAngle = normalizedInterY * maxBounceAngle;
            float n0 = v0.length();

            // Update speed
            v.x = (paddle.player ? n0 : -n0) * cos(bounceAngle);
            v.y = n0 * sin(bounceAngle);

            return true;
        }

        return false;
    }

    Vec2f v0, v;
};

enum class GameState {
    Playing,
    Paused,
    GameOver,
};

enum class GameMode {
    OnePlayer,
    TwoPlayers,
};

// TODO
struct Game {
    Game () {
    }

    void update (float dt) {
    }

    void run () {
    }

    GameState state;
    GameMode mode;
};

int main (void) {
    // Vita2D initialization
    // Intialization
    vita2d_init();
    vita2d_set_clear_color(BLACK);

    // Load PGF font
    vita2d_pgf* pgf = vita2d_load_default_pgf();

    InputState input;

    // FPS counting
    SceUInt64 cur_micros = 0, dt_micros = 0, last_micros = 0;
    uint32_t frames = 0;
    float fps = 0.0f;
    float dt = 0.0f;

    // Ball
    // TODO: random initial velocity
    Ball ball(Vec2f(SCREEN_W / 2, SCREEN_H / 2),
              300.0f * Vec2f(1, -1),
              10);

    // Paddles
    float square_w = 20, square_h = 120;
    float paddle_speed = 5;

    Paddle player(Vec2f(10, SCREEN_H / 2 - square_h / 2),
                  Vec2f(square_w, square_h));
    player.player = true;

    Paddle cpu(Vec2f(SCREEN_W - 10 - square_w, SCREEN_H / 2 - square_h / 2),
               Vec2f(square_w, square_h));
    cpu.player = false;

    while (1) {
        // Prevent the screen from turning off
        sceKernelPowerTick(0);

        // Input
        input.update();

        // Exit with Start
        if (input.isButtonPressed(EXIT_COMBO))
            break;

        // Player moves with the arrows
        if (input.isButtonPressed(SCE_CTRL_UP)) {
            player.moveY(-paddle_speed);
        } else if (input.isButtonPressed(SCE_CTRL_DOWN)) {
            player.moveY(+paddle_speed);
        }

        // Player moves with the left analog stick
        if (abs(input.ly) > 50) {
            player.moveY(paddle_speed * input.ly / 50.0f);
        }

        // CPU moves with the right analog stick
        if (abs(input.ry) > 50) {
            cpu.moveY(paddle_speed * input.ry / 50.0f);
        }

        // Move ball
        ball.move(dt);

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
        // TODO: sleep before putting the ball back in the center
        if (ball.y() < 0.0f) {
            ball.y() = 0.0f;
            ball.speed().y = -ball.speed().y;
        } else if (ball.y() + 2 * ball.radius() > SCREEN_H) {
            ball.y() = SCREEN_H - 2 * ball.radius();
            ball.speed().y = -ball.speed().y;
        } else if (ball.x() < 0.0f) {
            ball.x() = SCREEN_W / 2;
            ball.y() = SCREEN_H / 2;
            cpu.score++;
        } else if (ball.x() + 2 * ball.radius() > SCREEN_W) {
            ball.x() = SCREEN_W / 2;
            ball.y() = SCREEN_H / 2;
            player.score++;
        }

        // Ball with the paddles
        // TODO
        if (ball.collide(player)) {
            // TODO: sound
        } else if (ball.collide(cpu)) {
            // TODO: sound
        }

        // Render
        vita2d_start_drawing();
            vita2d_clear_screen();

            ball.render(WHITE);
            player.render(WHITE);
            cpu.render(WHITE);

            vita2d_pgf_draw_textf(pgf, SCREEN_W - 160, 30, GREEN, 1.0f, "FPS: %.2f", fps);
        vita2d_end_drawing();

        // Calculate FPS
        cur_micros = sceKernelGetProcessTimeWide();

        if (cur_micros >= (last_micros + 1000000)) {
            dt_micros = cur_micros - last_micros;
            last_micros = cur_micros;
            fps = (frames/(double) dt_micros)*1000000.0f;
            frames = 0;
            dt = 1.0f / fps;
        }

        frames++;

        vita2d_swap_buffers();
    }
    vita2d_fini();

    // Cleanup
    vita2d_free_pgf(pgf);

    sceKernelExitProcess(0);

    return 0;
}
