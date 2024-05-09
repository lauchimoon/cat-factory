#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

// Bundled assets go here
//#include "assets/build/bg.h"
//#include "assets/build/base-cat.h"
#include "assets/build/frame.h"
#include "assets/build/export-button.h"
//#include "assets/build/backgrounds.h"
#include "assets/build/backgrounds-small.h"
//#include "assets/build/fur.h"
//#include "assets/build/furs-selector.h"
//#include "assets/build/hats.h"
//#include "assets/build/hats-small.h"
//#include "assets/build/facewear.h"
//#include "assets/build/face-small.h"
//#include "assets/build/body.h"
//#include "assets/build/body-small.h"
//#include "assets/build/arrow-selectors.h"
#include "assets/build/font.h"

#define NFURS 7
#define NHATS 12
#define NFACEWEAR 10
#define NBODYWEAR 8
#define NBACKGROUNDS 7
#define NSELECTORS 6

#define WINDOW_TITLE "Cat Factory"

#define RANDOMIZATION_VALUE(x) (GetRandomValue(0, x - 1))
#define BG_COLOR1 (Color){ 61, 113, 173, 255 }
#define BG_COLOR2 WHITE

enum {
    WINDOW_WIDTH = 1060,
    WINDOW_HEIGHT = 760,
    FPS = 60,

    CAT_OFFSET_X = 40,
    CAT_OFFSET_Y = 120,
};

typedef struct Configuration {
    int bg, fur, hat, face, body;
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
Texture tx_hats_preview;
Texture tx_facewear;
Texture tx_face_preview;
Texture tx_bodywear;
Texture tx_body_preview;

Texture tx_arrow_selectors;
Texture tx_bg;

Font font;

Rectangle frame_src;
Rectangle frame_pos;

// Utility functions
bool rectangle_pressed(Rectangle box);
bool rectangle_hovered(Rectangle box);
void load_resources(void);
void unload_resources(void);
void initialize_bg_buttons(Rectangle *buttons, size_t buttons_size);
void initialize_fur_buttons(Rectangle *buttons, size_t buttons_size);
void initialize_selector_buttons(Rectangle *buttons, size_t buttons_size);
void randomize(Configuration *cfg);

Texture load_texture_from_memory(const char *file_type, const unsigned char *file_data, int data_size);

int main()
{
#ifndef CAT_DEBUG
    SetTraceLogLevel(LOG_NONE);
#endif
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    load_resources();

    // Configurables
    Configuration config = { 0 };

    // Variables
    RenderTexture rt_cat_export = LoadRenderTexture(tx_bg_frame.width, tx_bg_frame.height);
    int frame_counter = 0; // To make export text appear and disappear

    // Constants
    frame_src = (Rectangle){ 0, 0, rt_cat_export.texture.width, -rt_cat_export.texture.height };
    frame_pos = (Rectangle){ WINDOW_WIDTH - rt_cat_export.texture.width, 0, rt_cat_export.texture.width, rt_cat_export.texture.height };
    Rectangle randomize_button_bounds = { WINDOW_WIDTH - rt_cat_export.texture.width, WINDOW_HEIGHT - 95, tx_export_button.width/2 - 20, tx_export_button.height - 30 };
    Rectangle export_button_bounds = { WINDOW_WIDTH - 265, WINDOW_HEIGHT - 95, tx_export_button.width/2 - 30, tx_export_button.height - 30 };

    Rectangle buttons_backgrounds[NBACKGROUNDS] = { 0 };
    Rectangle buttons_furs[NFURS] = { 0 };
    Rectangle buttons_selectors[6] = { 0 };

    initialize_bg_buttons(buttons_backgrounds, NBACKGROUNDS);
    initialize_fur_buttons(buttons_furs, NFURS);
    initialize_selector_buttons(buttons_selectors, NSELECTORS);

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
        config.hat -= rectangle_pressed(buttons_selectors[0]);
        config.hat += rectangle_pressed(buttons_selectors[1]);
        if (config.hat >= NHATS) config.hat = 0;
        if (config.hat < 0) config.hat = NHATS - 1;

        // Change facewear
        config.face -= rectangle_pressed(buttons_selectors[2]);
        config.face += rectangle_pressed(buttons_selectors[3]);
        if (config.face >= NFACEWEAR) config.face = 0;
        if (config.face < 0) config.face = NFACEWEAR - 1;

        // Change bodywear
        config.body -= rectangle_pressed(buttons_selectors[4]);
        config.body += rectangle_pressed(buttons_selectors[5]);
        if (config.body >= NBODYWEAR) config.body = 0;
        if (config.body < 0) config.body = NBODYWEAR - 1;

        // Scramble the configuration!
        if (rectangle_pressed(randomize_button_bounds)) randomize(&config);

        // Export cat image
        if (rectangle_pressed(export_button_bounds)) {
            ++frame_counter;
            Image cat_image = LoadImageFromTexture(rt_cat_export.texture);
            ImageFlipVertical(&cat_image);
            ExportImage(cat_image, "cat.png");
            UnloadImage(cat_image);
        }

        if (frame_counter > 0) {
            ++frame_counter;
            if (frame_counter >= 60) {
                frame_counter = 0;
            }
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

            // Draw cat's bodywear
            DrawTextureRec(
                    tx_bodywear,
                    (Rectangle){ config.body*tx_bodywear.width/NBODYWEAR, 0, tx_bodywear.width/NBODYWEAR, tx_facewear.height },
                    (Vector2){ rt_cat_export.texture.width/2 - tx_base_cat.width/2 - 5, rt_cat_export.texture.height - 3.5*(float)CAT_OFFSET_Y },
                    WHITE
            );

            // Draw cat's facewear
            DrawTextureRec(
                    tx_facewear,
                    (Rectangle){ config.face*tx_facewear.width/NFACEWEAR, 0, tx_facewear.width/NFACEWEAR, tx_facewear.height },
                    (Vector2){ rt_cat_export.texture.width/2 - tx_base_cat.width/2 - 5, rt_cat_export.texture.height - 3.5*(float)CAT_OFFSET_Y },
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

        ClearBackground(BLACK);
        DrawTexture(tx_bg, 0, 0, WHITE);
        DrawTexturePro(rt_cat_export.texture, frame_src, frame_pos, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Backgrounds preview
        DrawTexture(tx_backgrounds_preview, frame_pos.x, frame_pos.height + 20, WHITE);

        // Fur preview
        DrawTextEx(font, "Pelaje", (Vector2){ 195, 5 }, 30, 2.0, BLACK);
        DrawTexture(tx_fur_preview, 25, 40, WHITE);
        DrawRectangleLinesEx(buttons_furs[config.fur], 4, (!config.fur)? WHITE : BLACK); // indicates which fur is selected

        // Hat preview
        DrawTextEx(font, "Gorros", (Vector2){ 185, 210 }, 30, 2.0, BLACK);
        DrawTextureRec(tx_hats_preview, (Rectangle){ config.hat*tx_hats_preview.width/NHATS, 0, tx_hats_preview.width/NHATS, tx_hats_preview.height },
                (Vector2){ 175, 250 }, WHITE);
        DrawTexture(tx_arrow_selectors, 138, 250, WHITE);

        // Facewear preview
        DrawTextEx(font, "Accesorios para la cara", (Vector2){ 50, 390 }, 30, 2.0, BLACK);
        DrawTextureRec(tx_face_preview, (Rectangle){ config.face*tx_face_preview.width/NFACEWEAR, 0, tx_face_preview.width/NFACEWEAR, tx_face_preview.height },
                (Vector2){ 175, 430 }, WHITE);
        DrawTexture(tx_arrow_selectors, 138, 430, WHITE);

        // Bodywear preview
        DrawTextEx(font, "Accesorios para el cuerpo", (Vector2){ 35, 570 }, 30, 2.0, BLACK);
        DrawTextureRec(tx_body_preview, (Rectangle){ config.body*tx_body_preview.width/NBODYWEAR, 0, tx_body_preview.width/NBODYWEAR, tx_body_preview.height },
                (Vector2){ 175, 610 }, WHITE);
        DrawTexture(tx_arrow_selectors, 138, 610, WHITE);

        // Export button
        DrawTexture(tx_export_button, WINDOW_WIDTH - rt_cat_export.texture.width, WINDOW_HEIGHT - 110, WHITE);
        DrawTextEx(font, "Exportado!", (Vector2){ WINDOW_WIDTH - 370, WINDOW_HEIGHT - 140 }, 30, 2.0, (frame_counter > 0)? WHITE : BLANK);

        EndDrawing();
    }

    UnloadRenderTexture(rt_cat_export);
    unload_resources();
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

void load_resources(void)
{
    //tx_base_cat = load_texture_from_memory(".png", base_cat_png, base_cat_png_len);
    tx_bg_frame = load_texture_from_memory(".png", frame_png, frame_png_len);
    tx_export_button = load_texture_from_memory(".png", export_button_png, export_button_png_len);
    //tx_backgrounds = load_texture_from_memory(".png", backgrounds_png, backgrounds_png_len);
    tx_backgrounds_preview = load_texture_from_memory(".png", backgrounds_small_png, backgrounds_small_png_len);
    //tx_fur = load_texture_from_memory(".png", fur_png, fur_png_len);
    //tx_fur_preview = load_texture_from_memory(".png", furs_selector_png, furs_selector_png_len);
    //tx_hats = load_texture_from_memory(".png", hats_png, hats_png_len);
    //tx_hats_preview = load_texture_from_memory(".png", hats_small_png, hats_small_png_len);
    //tx_facewear = load_texture_from_memory(".png", facewear_png, facewear_png_len);
    //tx_face_preview = load_texture_from_memory(".png", face_small_png, face_small_png_len);
    //tx_bodywear = load_texture_from_memory(".png", body_png, body_png_len);
    //tx_body_preview = load_texture_from_memory(".png", body_small_png, body_small_png_len);

    //tx_arrow_selectors = load_texture_from_memory(".png", arrow_selectors_png, arrow_selectors_png_len);
    //tx_bg = load_texture_from_memory(".png", bg_png, bg_png_len);

    font = LoadFontFromMemory(".ttf", font_ttf, font_ttf_len, 30, NULL, 0);
}

void unload_resources(void)
{
    UnloadTexture(tx_bg_frame);
    UnloadTexture(tx_backgrounds);
    UnloadTexture(tx_backgrounds_preview);
    UnloadTexture(tx_export_button);
    UnloadTexture(tx_base_cat);
    UnloadTexture(tx_hats);
    UnloadTexture(tx_hats_preview);
    UnloadTexture(tx_facewear);
    UnloadTexture(tx_face_preview);
    UnloadTexture(tx_bodywear);
    UnloadTexture(tx_body_preview);

    UnloadTexture(tx_arrow_selectors);
    UnloadTexture(tx_bg);

    UnloadFont(font);
}

void initialize_bg_buttons(Rectangle *buttons, size_t buttons_size)
{
    for (int i = 0; i < buttons_size; ++i) {
        buttons[i].y = frame_pos.height + 20;
        buttons[i].width = 79;
        buttons[i].x = frame_pos.x + i*buttons[i].width;
        buttons[i].height = tx_backgrounds_preview.height;
    }   
}

void initialize_fur_buttons(Rectangle *buttons, size_t buttons_size)
{
    for (int i = 0; i < buttons_size; ++i) {
        buttons[i].y = 40;
        buttons[i].width = 64;
        buttons[i].x = 25 + i*buttons[i].width;
        buttons[i].height = tx_fur_preview.height;
    }
}

void initialize_selector_buttons(Rectangle *buttons, size_t buttons_size)
{
    buttons[0].y = 250;
    buttons[1].y = 250;
    buttons[2].y = 430;
    buttons[3].y = 430;
    buttons[4].y = 610;
    buttons[5].y = 610;

    for (int i = 0; i < buttons_size; ++i) {
        buttons[i].x = 138 + 163*(i%2);
        buttons[i].width = 37;
        buttons[i].height = 128;
    }
}

void randomize(Configuration *cfg)
{
    cfg->bg = RANDOMIZATION_VALUE(NBACKGROUNDS);
    cfg->fur = RANDOMIZATION_VALUE(NFURS);
    cfg->hat = RANDOMIZATION_VALUE(NHATS);
    cfg->face = RANDOMIZATION_VALUE(NFACEWEAR);
    cfg->body = RANDOMIZATION_VALUE(NBODYWEAR);
}

Texture load_texture_from_memory(const char *file_type, const unsigned char *file_data, int data_size)
{
    Image tmp = LoadImageFromMemory(file_type, file_data, data_size);
    Texture ret = LoadTextureFromImage(tmp);
    UnloadImage(tmp);

    return ret;
}

// TODO LIST
// [x] draw face accessories
// [x] draw body accessories
// [x] add missing accessories' ui
// [x] make clear which option (fur, background...) is chosen
// [x] translate to spanish
// [x] better font
// [x] add background
// [x] add randomization button
// [x] enhance export button
// [x] embed resources into application
// [x] display text when exporting
