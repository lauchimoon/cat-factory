#include "raylib.h"
#include <stddef.h>

#define NFURS 7
#define NHATS 12
#define NBACKGROUNDS 7

#define WINDOW_TITLE "Cat Factory"

#define RANDOMIZATION_VALUE(x) (GetRandomValue(0, x - 1))

enum {
    WINDOW_WIDTH = 1060,
    WINDOW_HEIGHT = 740,
    FPS = 60,

    CAT_OFFSET_X = 40,
    CAT_OFFSET_Y = 120,
};

typedef struct Configuration {
    int bg, fur, hat;
} Configuration;

// Global constants
// Yes I suck at programming

Texture tx_base_cat;
Texture tx_bg_frame;
Texture tx_export_button;
Texture tx_backgrounds;
Texture tx_backgrounds_preview;
Texture tx_fur;
Texture tx_fur_preview;
Texture tx_hats;

Rectangle frame_src;
Rectangle frame_pos;

// Utility functions
bool rectangle_pressed(Rectangle box);
bool rectangle_hovered(Rectangle box);
void load_textures(void);
void unload_textures(void);
void initialize_bg_buttons(Rectangle *buttons, size_t buttons_size);
void initialize_fur_buttons(Rectangle *buttons, size_t buttons_size);
void randomize(Configuration *cfg);

int main()
{
#ifndef CAT_DEBUG
    SetTraceLogLevel(LOG_NONE);
#endif
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    load_textures();

    // Configurables
    Configuration config = { 0 };

    // Variables
    RenderTexture rt_cat_export = LoadRenderTexture(tx_bg_frame.width, tx_bg_frame.height);
    Rectangle export_button_bounds = { WINDOW_WIDTH - rt_cat_export.texture.width, WINDOW_HEIGHT - 110,
                                       tx_export_button.width, tx_export_button.height };

    // Constants
    frame_src = (Rectangle){ 0, 0, rt_cat_export.texture.width, -rt_cat_export.texture.height };
    frame_pos = (Rectangle){ WINDOW_WIDTH - rt_cat_export.texture.width, 0, rt_cat_export.texture.width, rt_cat_export.texture.height };

    Rectangle buttons_backgrounds[NBACKGROUNDS] = { 0 };
    Rectangle buttons_furs[NFURS] = { 0 };

    initialize_bg_buttons(buttons_backgrounds, NBACKGROUNDS);
    initialize_fur_buttons(buttons_furs, NFURS);

    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        // Change background
        for (int i = 0; i < NBACKGROUNDS; ++i) {
            if (rectangle_pressed(buttons_backgrounds[i])) {
                config.bg = i;
            }
        }

        // Change fur
        for (int i = 0; i < NFURS; ++i) {
            if (rectangle_pressed(buttons_furs[i])) {
                config.fur = i;
            }
        }

        // Change hat
        config.hat -= IsKeyPressed(KEY_LEFT);
        config.hat += IsKeyPressed(KEY_RIGHT);

        // Scramble the configuration!
        if (IsKeyPressed(KEY_R)) randomize(&config);

        // Export cat image
        if (rectangle_pressed(export_button_bounds)) {
            Image cat_image = LoadImageFromTexture(rt_cat_export.texture);
            ImageFlipVertical(&cat_image);
            ExportImage(cat_image, "cat.png");
            UnloadImage(cat_image);
        }

        BeginDrawing();
        BeginTextureMode(rt_cat_export);
            ClearBackground(BLANK);

            // Draw background
            DrawTextureRec(
                    tx_backgrounds,
                    (Rectangle){ config.bg*tx_backgrounds.width/NBACKGROUNDS, 0, tx_backgrounds.width/NBACKGROUNDS - 20, tx_backgrounds.height - 10 },
                    (Vector2){ 10, 5 },
                    WHITE
            );

            DrawTexture(tx_bg_frame, 0, 0, WHITE);

            // Draw fur type
            DrawTextureRec(
                    tx_fur,
                    (Rectangle){ config.fur*tx_fur.width/NFURS, 0, tx_fur.width/NFURS, tx_fur.height },
                    (Vector2){ rt_cat_export.texture.width/2 - tx_base_cat.width/2 - 5, rt_cat_export.texture.height - 3.5*(float)CAT_OFFSET_Y },
                    WHITE
            );

            // Draw the cat
            DrawTexture(
                    tx_base_cat,
                    rt_cat_export.texture.width/2 - tx_base_cat.width/2 - 5,
                    rt_cat_export.texture.height - 3.5*(float)CAT_OFFSET_Y,
                    WHITE
            );

            // Draw cat's hat
            DrawTextureRec(
                    tx_hats,
                    (Rectangle){ config.hat*tx_hats.width/NHATS, 0, tx_hats.width/NHATS, tx_hats.height },
                    (Vector2){ rt_cat_export.texture.width/2 - tx_base_cat.width/2 - 5, rt_cat_export.texture.height - 3.5*(float)CAT_OFFSET_Y },
                    WHITE
            );
        EndTextureMode();

        ClearBackground(RAYWHITE);
        DrawTexturePro(rt_cat_export.texture, frame_src, frame_pos, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawTexture(tx_backgrounds_preview, frame_pos.x + frame_pos.width/7, frame_pos.height + 20, WHITE);
        DrawTexture(tx_fur_preview, 25, 40, WHITE);
        DrawTexture(tx_export_button, export_button_bounds.x, export_button_bounds.y, rectangle_hovered(export_button_bounds)? GREEN : BLACK);

        for (int i = 0; i < NBACKGROUNDS; ++i) {
            DrawRectangleRec(buttons_backgrounds[i], Fade(RED, i/10.0f));
            DrawRectangleRec(buttons_furs[i], Fade(RED, i/10.0f));
        }
        EndDrawing();
    }

    UnloadRenderTexture(rt_cat_export);
    unload_textures();
    CloseWindow();
    return 0;
}

bool rectangle_pressed(Rectangle box)
{
    return (CheckCollisionPointRec(GetMousePosition(), box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

bool rectangle_hovered(Rectangle box)
{
    return CheckCollisionPointRec(GetMousePosition(), box);
}

void load_textures(void)
{
    tx_base_cat = LoadTexture("./assets/base-cat.png");
    tx_bg_frame = LoadTexture("./assets/frame.png");
    tx_export_button = LoadTexture("./assets/export-button.png");
    tx_backgrounds = LoadTexture("./assets/backgrounds.png");
    tx_backgrounds_preview = LoadTexture("./assets/backgrounds-small.png");
    tx_fur = LoadTexture("./assets/fur.png");
    tx_fur_preview = LoadTexture("./assets/furs-selector.png");
    tx_hats = LoadTexture("./assets/hats.png");
}

void unload_textures(void)
{
    UnloadTexture(tx_bg_frame);
    UnloadTexture(tx_backgrounds);
    UnloadTexture(tx_backgrounds_preview);
    UnloadTexture(tx_export_button);
    UnloadTexture(tx_base_cat);
    UnloadTexture(tx_hats);
}

void initialize_bg_buttons(Rectangle *buttons, size_t buttons_size)
{
    for (int i = 0; i < buttons_size; ++i) {
        buttons[i].y = frame_pos.height + 20;
        buttons[i].width = tx_backgrounds_preview.width/NBACKGROUNDS;
        buttons[i].x = frame_pos.x + i*buttons[i].width + frame_pos.width/7;
        buttons[i].height = tx_backgrounds_preview.height;
    }   
}

void initialize_fur_buttons(Rectangle *buttons, size_t buttons_size)
{
    for (int i = 0; i < buttons_size; ++i) {
        buttons[i].y = 40;
        buttons[i].width = tx_fur_preview.width/NFURS;
        buttons[i].x = 25 + i*buttons[i].width + 2.5;
        buttons[i].height = tx_fur_preview.height;
    }
}

void randomize(Configuration *cfg)
{
    cfg->bg = RANDOMIZATION_VALUE(NBACKGROUNDS);
    cfg->fur = RANDOMIZATION_VALUE(NFURS);
    cfg->hat = RANDOMIZATION_VALUE(NHATS);
}

// TODO LIST
// [-] draw face accesories
// [-] draw body accesories
// [-] add missing accesories' ui
// [-] make clear which option (fur, background...) is chosen
// [-] add background
// [-] add randomization button
// [-] enhance export button
// [-] add starting screen?
// [-] embed resources into application
