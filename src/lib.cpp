#include "ICBM.hpp"
#include <raylib.h>

//------------------------------------------------------------------------------------------
// Game Module Functions Definition
//------------------------------------------------------------------------------------------

// Initialize game variables (start new game)
void InitGame() {
    inGameTime = 0;
    fireMode = 0;
    font = GetFontDefault();

    // Initialize missiles
    for (int i = 0; i < MAX_MISSILES; i++) {
        missile[i].origin = (Vector2){0, 0};
        missile[i].speed = (Vector2){0, 0};
        missile[i].position = (Vector2){0, 0};

        missile[i].active = false;
    }

    // Initialize interceptors
    for (int i = 0; i < MAX_INTERCEPTORS; i++) {
        interceptor[i].origin = (Vector2){0, 0};
        interceptor[i].speed = (Vector2){0, 0};
        interceptor[i].position = (Vector2){0, 0};

        interceptor[i].active = false;
    }

    // Initialize explosions
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        explosion[i].position = (Vector2){0, 0};
        explosion[i].frame = 0;
        explosion[i].active = false;
    }

    // Initialize buildings and turrets
    int spacing = screenWidth / (TURRETS_AMOUNT + BUILDINGS_AMOUNT + 1);

    // Placing
    turret[0].position =
        (Vector2){(float)1 * spacing,
                  screenHeight * groundPositionScale - TURRET_HEIGHT / 2};
    for (int i = 0; i < BUILDINGS_AMOUNT; i++)
        building[i].position =
            (Vector2){((float)i + 2) * spacing,
                      screenHeight * groundPositionScale - BUILDING_HEIGHT / 2};
    turret[1].position =
        (Vector2){(float)9 * spacing,
                  screenHeight * groundPositionScale - TURRET_HEIGHT / 2};

    // Activation
    for (int i = 0; i < TURRETS_AMOUNT; i++)
        turret[i].active = true;
    for (int i = 0; i < BUILDINGS_AMOUNT; i++)
        building[i].active = true;

    // Initialize game variables
    score = 0;

    // Start music
    PlayMusicStream(music);
}

// Update game logic (one frame)
void UpdateGame() {
    if (!gameOver) {
        UpdateMusicStream(music);

        if (IsKeyPressed('P')) {
            pause = !pause;
            if (pause)
                PauseMusicStream(music);
            else
                ResumeMusicStream(music);
        }
        if (!pause) {
            framesCounter++;

            // Update fire mode
            float mouseWheel = GetMouseWheelMove();
            if (mouseWheel > 0) {
                fireMode = (fireMode + 1) % 4;
            }
            if (mouseWheel < 0) {
                fireMode = (fireMode + 5) % 4;
            }

            // Interceptors update
            UpdateInterceptors();

            // Missiles update
            UpdateMissiles();

            // Explosions update
            UpdateExplosions();

            // Fire logic
            UpdateCooldown();
            CreateOutgoingFire();
            CreateIncomingFire();

            // Game over logic
            int counter = 0;

            for (int i = 0; i < TURRETS_AMOUNT; i++) {
                if (!turret[i].active)
                    counter++;
                if (counter == TURRETS_AMOUNT)
                    gameOver = true;
            }

            counter = 0;
            for (int i = 0; i < BUILDINGS_AMOUNT; i++) {
                if (!building[i].active)
                    counter++;
                if (counter == BUILDINGS_AMOUNT)
                    gameOver = true;
            }

            inGameTime += GetFrameTime();
        }
    } else {
        StopMusicStream(music);

        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame() {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawTextureV(T_bg, bg.origin, WHITE);

    if (!gameOver) {
        // Draw missiles
        for (int i = 0; i < MAX_MISSILES; i++) {
            if (missile[i].active) {
                DrawLineEx(missile[i].origin, missile[i].position, 0.5f, RED);

                if (framesCounter % 16 < 8)
                    DrawCircle(missile[i].position.x, missile[i].position.y, 3,
                               YELLOW);
            }
        }

        // Draw interceptors
        for (int i = 0; i < MAX_INTERCEPTORS; i++) {
            if (interceptor[i].active) {
                DrawLine(interceptor[i].origin.x, interceptor[i].origin.y,
                         interceptor[i].position.x, interceptor[i].position.y,
                         GREEN);

                if (framesCounter % 16 < 8)
                    DrawCircle(interceptor[i].position.x,
                               interceptor[i].position.y, 3, BLUE);
            }
        }

        // Draw explosions
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (explosion[i].active)
                DrawCircle(explosion[i].position.x, explosion[i].position.y,
                           EXPLOSION_RADIUS * explosion[i].radiusMultiplier,
                           EXPLOSION_COLOR);
        }

        // Draw turrets
        for (int i = 0; i < TURRETS_AMOUNT; i++) {
            Vector2 mousePosition = GetMousePosition();
            turretTop[i].angle =
                atan2(mousePosition.y - turretTop[i].center.y,
                      mousePosition.x - turretTop[i].center.x) *
                180 / M_PI;
            if (turret[i].active)
                DrawSprite(T_turretTop, turretTop[i], turretTop[i].angle, 0, i);
            if (turret[i].active)
                DrawTextureV(T_turretBottom, turretBottom[i].origin, WHITE);
        }

        // Draw buildings
        for (int i = 0; i < BUILDINGS_AMOUNT; i++) {
            Rectangle T_buildingRec{(float)(i % 5) * T_building.width / 5, 0,
                                    (float)T_building.width / 5,
                                    (float)T_building.height};
            if (building[i].active)
                DrawTextureRec(
                    T_building, T_buildingRec,
                    Vector2{building[i].position.x - BUILDING_WIDTH / 2,
                            building[i].position.y - BUILDING_HEIGHT / 2},
                    WHITE);
        }

        // Draw grass
        DrawTextureEx(T_grass, grass.origin, 0, 1, WHITE);

        // Draw bottom of background
        DrawTextureRec(T_bg,
                       Rectangle{0, screenHeight * groundPositionScale,
                                 (float)screenWidth,
                                 screenHeight * (1 - groundPositionScale)},
                       Vector2{0, screenHeight * groundPositionScale}, WHITE);

        // Draw cooldown box
        DrawCooldownBox();

        // Draw score
        DrawText(TextFormat("SCORE %4i", score), 20, 20, 40, LIGHTGRAY);

        // Draw fire mode frame
        DrawTextEx(font, TextFormat("%s", fireModes[fireMode].c_str()),
                   Vector2{20, 80}, 25, 1, BLACK);

        // Time and Fps
        DrawText(TextFormat("%.2f", inGameTime), screenWidth - 40,
                 screenHeight - 40, 20, LIME);
        DrawFPS(screenWidth - 80, screenHeight - 20);

        // Pause
        if (pause)
            DrawText("GAME PAUSED",
                     screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2,
                     screenHeight / 2 - 40, 40, YELLOW);
    } else
        DrawText("PRESS [ENTER] TO PLAY AGAIN",
                 GetScreenWidth() / 2 -
                     MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                 GetScreenHeight() / 2 - 50, 20, WHITE);

    EndDrawing();
}

//------------------------------------------------------------------------------------------
// Module for fire logic
//------------------------------------------------------------------------------------------

// Update intercceptors position and check collisions with it
void UpdateInterceptors() {
    for (int i = 0; i < MAX_INTERCEPTORS; i++) {
        if (interceptor[i].active) {
            // Update position
            interceptor[i].position.x += interceptor[i].speed.x;
            interceptor[i].position.y += interceptor[i].speed.y;

            interceptor[i].active = CheckCollisionParticle(
                interceptor[i], true, true, false, false, false);
        }
    }
}

// Update missiles position and check collisions with it
void UpdateMissiles() {
    for (int i = 0; i < MAX_MISSILES; i++) {
        if (missile[i].active) {
            // Update position
            missile[i].position.x += missile[i].speed.x;
            missile[i].position.y += missile[i].speed.y;

            missile[i].active = CheckCollisionParticle(missile[i], true, true,
                                                       true, true, true);
        }
    }
}

// Initiate one of the fire modes
void CreateOutgoingFire() {
    switch (fireMode) {
    case INTERCEPTOR:
        CreateOutgoingInterceptor();
        break;
    case SWARMING_MISSILES:
        CreateOutgoingSwarmingMissiles();
        break;
    case LASERGUN:
        CreateOutgoingLaserBeam();
        break;
    case AIRBURST:
        CreateOutgoingAirburst();
        break;
    }
}

// Initiate incoming missiles
void CreateIncomingFire() {
    int missileIndex = 0;

    // Launch missile
    if (framesCounter % MISSILE_LAUNCH_FRAMES == 0) {
        float module;
        float sideX;
        float sideY;

        // Activate the missile
        missile[missileIndex].active = true;

        // Assign start position
        missile[missileIndex].origin =
            (Vector2){(float)GetRandomValue(20, screenWidth - 20), -10};
        missile[missileIndex].position = missile[missileIndex].origin;
        missile[missileIndex].objective =
            (Vector2){(float)GetRandomValue(20, screenWidth - 20),
                      (float)screenHeight + 10};

        // Calculate speed
        module = hypot(
            missile[missileIndex].objective.x - missile[missileIndex].origin.x,
            missile[missileIndex].objective.y - missile[missileIndex].origin.y);

        sideX = (missile[missileIndex].objective.x -
                 missile[missileIndex].origin.x) *
                MISSILE_SPEED / module;
        sideY = (missile[missileIndex].objective.y -
                 missile[missileIndex].origin.y) *
                MISSILE_SPEED / module;

        missile[missileIndex].speed = (Vector2){sideX, sideY};

        // Update
        missileIndex++;
        if (missileIndex == MAX_MISSILES)
            missileIndex = 0;
    }
}

// Update all cooldowns
void UpdateCooldown() {
    for (int i = 0; i < 8; ++i)
        if (cooldowns[i] > 0)
            cooldowns[i] -= 1;
}

// Check collision particle with buildings, turrets and explosions
bool CheckCollisionParticle(Particle particle, bool withBounds,
                            bool withObjective, bool withExplosion,
                            bool withTurret, bool withBuilding) {
    // Collision and particle out of bounds
    if (withBounds)
        if (particle.position.y > groundPositionScale * screenHeight) {
            // particle disappears
            particle.active = false;

            // Explosion appears
            if (particle.explosive) {
                explosion[explosionIndex].position = particle.position;
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex++;
                if (explosionIndex == MAX_EXPLOSIONS)
                    explosionIndex = 0;
            }
        }
    
    // CHeck collision with turrets
    if (withTurret)
        for (int j = 0; j < TURRETS_AMOUNT; j++) {
            if (turret[j].active) {
                if (CheckCollisionPointRec(
                        particle.position,
                        (Rectangle){turret[j].position.x - TURRET_WIDTH / 2,
                                    turret[j].position.y -
                                        TURRET_HEIGHT / 2,
                                    TURRET_WIDTH, TURRET_HEIGHT})) {
                    // particle disappears
                    particle.active = false;

                    // Explosion and destroy building
                    turret[j].active = false;

                    if (particle.explosive) {
                        explosion[explosionIndex].position =
                            particle.position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS)
                            explosionIndex = 0;
                    }

                    break;
                }
            }
        }

    // CHeck collision with buildings
    if (withBuilding)
        for (int j = 0; j < BUILDINGS_AMOUNT; j++) {
            if (building[j].active) {
                if (CheckCollisionPointRec(
                        particle.position,
                        (Rectangle){
                            building[j].position.x - BUILDING_WIDTH / 2,
                            building[j].position.y - BUILDING_HEIGHT / 2,
                            BUILDING_WIDTH, BUILDING_HEIGHT})) {
                    // particle disappears
                    particle.active = false;

                    // Explosion and destroy building
                    building[j].active = false;
                    if (particle.explosive) {
                        explosion[explosionIndex].position =
                            particle.position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS)
                            explosionIndex = 0;
                    }

                    break;
                }
            }
        }

    // CHeck collision with explosions
    if (withExplosion)
        for (int j = 0; j < MAX_EXPLOSIONS; j++) {
            if (explosion[j].active) {
                if (CheckCollisionPointCircle(
                        particle.position, explosion[j].position,
                        EXPLOSION_RADIUS * explosion[j].radiusMultiplier)) {
                    // particle disappears and we earn 1 points
                    particle.active = false;
                    score += 1;
                    if (particle.explosive) {
                        explosion[explosionIndex].position =
                            particle.position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS)
                            explosionIndex = 0;
                    }

                    break;
                }
            }
        }

    // Check collision with objective
    if (withObjective) {
        // Distance to objective
        float distance = hypot(particle.position.x - particle.objective.x,
                            particle.position.y - particle.objective.y);

        if (distance < hypot(particle.speed.x, particle.speed.y)) {
            // Interceptor disappears
            particle.active = false;

            // Explosion
            explosion[explosionIndex].position = particle.position;
            explosion[explosionIndex].active = true;
            explosion[explosionIndex].frame = 0;
            explosionIndex++;
            if (explosionIndex == MAX_EXPLOSIONS)
                explosionIndex = 0;
        }
    }
    return particle.active;
}

//------------------------------------------------------------------------------------------
// Additional fire modules for diff types
//------------------------------------------------------------------------------------------
void CreateOutgoingInterceptor() {
    int interceptorNumber = 0;
    int turretShooting = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        turretShooting = 0;
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        turretShooting = 1;

    if (turretShooting > -1 && turret[turretShooting].active &&
        cooldowns[turretShooting * 1] == 0) {
        float module;
        float sideX;
        float sideY;

        // Activate the interceptor
        interceptor[interceptorNumber].active = true;

        // Assign start position
        interceptor[interceptorNumber].origin =
            (Vector2){turret[turretShooting].position.x,
                      turret[turretShooting].position.y - TURRET_HEIGHT / 2 +
                          T_turretTop.height / 2};
        interceptor[interceptorNumber].position =
            interceptor[interceptorNumber].origin;
        interceptor[interceptorNumber].objective = GetMousePosition();

        // Calculate speed
        module = hypot(interceptor[interceptorNumber].objective.x -
                           interceptor[interceptorNumber].origin.x,
                       interceptor[interceptorNumber].objective.y -
                           interceptor[interceptorNumber].origin.y);

        sideX = (interceptor[interceptorNumber].objective.x -
                 interceptor[interceptorNumber].origin.x) *
                INTERCEPTOR_SPEED / module;
        sideY = (interceptor[interceptorNumber].objective.y -
                 interceptor[interceptorNumber].origin.y) *
                INTERCEPTOR_SPEED / module;

        interceptor[interceptorNumber].speed = (Vector2){sideX, sideY};

        // Update
        cooldowns[turretShooting * 1] = INTERCEPTOR_COOLDOWN;
        interceptorNumber++;
        if (interceptorNumber == MAX_INTERCEPTORS)
            interceptorNumber = 0;
    }
}

void CreateOutgoingSwarmingMissiles() {
    int interceptorNumber = 0;
    int turretShooting = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        turretShooting = 0;
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        turretShooting = 1;

    if (turretShooting > -1 && turret[turretShooting].active &&
        cooldowns[turretShooting] == 0) {

        cooldowns[turretShooting * 1] = INTERCEPTOR_COOLDOWN;
    }
}

void CreateOutgoingLaserBeam() {
    int interceptorNumber = 0;
    int turretShooting = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        turretShooting = 0;
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        turretShooting = 1;

    if (turretShooting > -1 && turret[turretShooting].active &&
        cooldowns[turretShooting] == 0) {

        cooldowns[turretShooting * 3] = INTERCEPTOR_COOLDOWN;
    }
}

void CreateOutgoingAirburst() {
    int interceptorNumber = 0;
    int turretShooting = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        turretShooting = 0;
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        turretShooting = 1;

    if (turretShooting > -1 && turret[turretShooting].active &&
        cooldowns[turretShooting] == 0) {

        cooldowns[turretShooting * 4] = INTERCEPTOR_COOLDOWN;
    }
}

void CreateShrapnel() {}

//------------------------------------------------------------------------------------------
// Additional modules
//------------------------------------------------------------------------------------------

// Update explosions
void UpdateExplosions() {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosion[i].active) {
            explosion[i].frame++;

            if (explosion[i].frame <= EXPLOSION_INCREASE_TIME)
                explosion[i].radiusMultiplier =
                    explosion[i].frame / (float)EXPLOSION_INCREASE_TIME;
            else if (explosion[i].frame <= EXPLOSION_TOTAL_TIME)
                explosion[i].radiusMultiplier =
                    1 - (explosion[i].frame - (float)EXPLOSION_INCREASE_TIME) /
                            (float)EXPLOSION_TOTAL_TIME;
            else {
                explosion[i].frame = 0;
                explosion[i].active = false;
            }

            // Check collision with turrets
            for (int j = 0; j < TURRETS_AMOUNT; j++) {
                if (turret[j].active) {
                    if (CheckCollisionCircleRec(
                            explosion[i].position,
                            EXPLOSION_RADIUS * explosion[i].radiusMultiplier,
                            Rectangle{turret[j].position.x - TURRET_WIDTH / 2,
                                      turret[j].position.y - TURRET_WIDTH / 2,
                                      TURRET_WIDTH, TURRET_HEIGHT})) {
                        turret[j].active = false;
                    }
                }
            }

            // Check collision with buildings
            for (int j = 0; j < BUILDINGS_AMOUNT; j++) {
                if (building[j].active) {
                    if (CheckCollisionCircleRec(
                            explosion[i].position,
                            EXPLOSION_RADIUS * explosion[i].radiusMultiplier,
                            Rectangle{
                                building[j].position.x - BUILDING_WIDTH / 2,
                                building[j].position.y - BUILDING_HEIGHT / 2,
                                BUILDING_WIDTH, BUILDING_HEIGHT})) {
                        building[j].active = false;
                    }
                }
            }
        }
    }
}

// Flip rectangle horizontally or vertically
Rectangle RectangleScale(Rectangle rec, int xscale, int yscale) {
    return (Rectangle){rec.x, rec.y, rec.width * xscale, rec.height * yscale};
}

// Draw flipped (or not) texture
void DrawSprite(Texture2D sprite, Textures textures, int angle, bool flipx,
                bool flipy) {
    Rectangle flippedRectangle =
        RectangleScale({0, 0, (float)sprite.width, (float)sprite.height},
                       flipx ? -1 : 1, flipy ? -1 : 1);
    DrawTexturePro(sprite, flippedRectangle,
                   Rectangle{textures.center.x, textures.center.y,
                             (float)sprite.width, (float)sprite.height},
                   Vector2{(float)sprite.width / 2, (float)sprite.height / 2},
                   angle, WHITE);
}

// Draw cooldown boxes
void DrawCooldownBox() {
    float thickness = 3;
    float boxWidth = 20;

    Rectangle cooldownBoxLeft{0, (float)screenHeight / 3, boxWidth,
                              (float)screenHeight / 3};
    Rectangle cooldownBoxRight{screenWidth - boxWidth, (float)screenHeight / 3,
                               boxWidth, (float)screenHeight / 3};

    DrawRectangleLinesEx(cooldownBoxLeft, thickness, BLACK);
    DrawRectangleLinesEx(cooldownBoxRight, thickness, BLACK);

    if (cooldowns[2 * fireMode] > 0) {
        Rectangle cooldownBoxLeftMoving{
            thickness,
            2 * screenHeight / 3 + thickness -
                (screenHeight / 3) *
                    (cooldowns[fireMode * 2] / cooldownsOriginal[fireMode]),
            boxWidth - 2 * thickness,
            (screenHeight / 3) *
                    (cooldowns[fireMode * 2] / cooldownsOriginal[fireMode]) -
                2 * thickness};
        DrawRectangleGradientEx(cooldownBoxLeftMoving, BLUE, RED, RED, BLUE);
    } else
        DrawRectangle(thickness, screenHeight / 3 + thickness,
                      boxWidth - 2 * thickness,
                      screenHeight / 3 - 2 * thickness, GREEN);

    if (cooldowns[2 * fireMode + 1] > 0) {
        Rectangle cooldownBoxRightMoving{
            screenWidth - boxWidth + thickness,
            2 * screenHeight / 3 + thickness -
                (screenHeight / 3) *
                    (cooldowns[fireMode * 2 + 1] / cooldownsOriginal[fireMode]),
            boxWidth - 2 * thickness,
            (screenHeight / 3) * (cooldowns[fireMode * 2 + 1] /
                                  cooldownsOriginal[fireMode]) -
                2 * thickness};
        DrawRectangleGradientEx(cooldownBoxRightMoving, BLUE, RED, RED, BLUE);
    } else
        DrawRectangle(screenWidth - boxWidth + thickness,
                      screenHeight / 3 + thickness, boxWidth - 2 * thickness,
                      screenHeight / 3 - 2 * thickness, GREEN);
}

//------------------------------------------------------------------------------------------
// Modules that upload and unload all images, textures, music
//------------------------------------------------------------------------------------------

// Upload game
void UploadGame(void) {
    // Upload and set game icon
    Image icon = LoadImage("../resources/icon.png");
    SetWindowIcon(icon);

    // Upload background
    T_bg = LoadTexture("../resources/bg.png");
    bg.origin = Vector2{0, 0};
    bg.center =
        Vector2{T_bg.width / 2 + bg.origin.x, T_bg.height / 2 + bg.origin.y};

    // Upload grass
    T_grass = LoadTexture("../resources/grass.png");
    grass.origin =
        Vector2{0, screenHeight * groundPositionScale - T_grass.height};
    grass.center = Vector2{T_grass.width / 2 + grass.origin.x,
                           T_grass.height / 2 + grass.origin.y};

    // Upload building textures
    T_building = LoadTexture("../resources/homes.png");

    // Upload turret textures
    T_turretTop = LoadTexture("../resources/turretTop.png");

    for (int i = 0; i < 2; i++) {
        turretTop[i].origin =
            Vector2{turret[i].position.x - T_turretTop.width / 2,
                    turret[i].position.y - TURRET_HEIGHT / 2};
        turretTop[i].center =
            Vector2{T_turretTop.width / 2 + turretTop[i].origin.x,
                    T_turretTop.height / 2 + turretTop[0].origin.y};
    }

    T_turretBottom = LoadTexture("../resources/turretBottom.png");

    for (int i = 0; i < 2; i++) {
        turretBottom[i].origin = Vector2{
            turret[i].position.x - TURRET_WIDTH / 2,
            turret[i].position.y + TURRET_HEIGHT / 2 - T_turretBottom.height};
        turretBottom[i].center =
            Vector2{T_turretBottom.width / 2 + turretBottom[i].origin.x,
                    T_turretBottom.height / 2 + turretBottom[0].origin.y};
    }

    // Upload music
    Music music = LoadMusicStream("../resources/music.mp3");
}

// Unload game variables
void UnloadGame(void) {
    // UnloadImage();
    UnloadTexture(T_bg);
    UnloadTexture(T_turretTop);
    UnloadTexture(T_turretBottom);
    UnloadTexture(T_building);

    // Unload music
    UnloadMusicStream(music);
}
