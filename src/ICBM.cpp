#include "ICBM.h"

//------------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------------
int main(void)
{   
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "ICBM");

    InitGame();
    
    UploadGame();

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateGame();               // Game logic
        DrawGame();                 // Game render
        //----------------------------------------------------------------------------------
    }
    
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------------
// Game Module Functions Definition
//------------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{   
    inGameTime = 0;
    fireMode = 0;
    font = GetFontDefault();

    // Initialize missiles
    for (int i = 0; i < MAX_MISSILES; i++)
    {
        missile[i].origin = (Vector2){ 0, 0 };
        missile[i].speed = (Vector2){ 0, 0 };
        missile[i].position = (Vector2){ 0, 0 };

        missile[i].active = false;
    }
    
    // Initialize interceptors
    for (int i = 0; i < MAX_INTERCEPTORS; i++)
    {
        interceptor[i].origin = (Vector2){ 0, 0 };
        interceptor[i].speed = (Vector2){ 0, 0 };
        interceptor[i].position = (Vector2){ 0, 0 };

        interceptor[i].active = false;
    }
    
    // Initialize explosions
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        explosion[i].position = (Vector2){ 0, 0 };
        explosion[i].frame = 0;
        explosion[i].active = false;
    }
    
    // Initialize buildings and turrets
    int spacing = screenWidth/(TURRETS_AMOUNT + BUILDINGS_AMOUNT + 1);

    // Placing
    turret[0].position = (Vector2){ (float) 1 * spacing, screenHeight * groundPositionScale - TURRET_HEIGHT/2 };
    for (int i = 0; i < BUILDINGS_AMOUNT; i++)  building[i].position = (Vector2){ ((float) i + 2) * spacing, screenHeight * groundPositionScale - BUILDING_HEIGHT/2 };
    turret[1].position = (Vector2){ (float) 9 * spacing, screenHeight * groundPositionScale - TURRET_HEIGHT/2 };

    // Activation
    for (int i = 0; i < TURRETS_AMOUNT; i++) turret[i].active = true;
    for (int i = 0; i < BUILDINGS_AMOUNT; i++) building[i].active = true;

    // Initialize game variables
    score = 0;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            framesCounter++;

            static float distance;

            // Update fire mode
            float mouseWheel = GetMouseWheelMove(); 
            if(mouseWheel > 0){
                fireMode = (fireMode + 1) % 4;
            }
            if (mouseWheel < 0){
                fireMode = (fireMode + 5) % 4;
            }

            // Interceptors update
            for (int i = 0; i < MAX_INTERCEPTORS; i++)
            {
                if (interceptor[i].active)
                {
                    // Update position
                    interceptor[i].position.x += interceptor[i].speed.x;
                    interceptor[i].position.y += interceptor[i].speed.y;

                    // Distance to objective
                    distance = hypot(interceptor[i].position.x - interceptor[i].objective.x,
                                     interceptor[i].position.y - interceptor[i].objective.y);

                    if (distance < INTERCEPTOR_SPEED)
                    {
                        // Interceptor dissapears
                        interceptor[i].active = false;

                        // Explosion
                        explosion[explosionIndex].position = interceptor[i].position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;

                        break;
                    }
                }
            }

            // Missiles update
            for (int i = 0; i < MAX_MISSILES; i++)
            {
                if (missile[i].active)
                {
                    // Update position
                    missile[i].position.x += missile[i].speed.x;
                    missile[i].position.y += missile[i].speed.y;
                    
                    missile[i].active = CheckCollisionParticle(Particle {missile[i].origin, missile[i].position, missile[i].objective, missile[i].speed, missile[i].explosive, missile[i].active});
                    
                }
                
            }

            // Explosions update
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosion[i].active)
                {
                    explosion[i].frame++;

                    if (explosion[i].frame <= EXPLOSION_INCREASE_TIME) explosion[i].radiusMultiplier = explosion[i].frame/(float)EXPLOSION_INCREASE_TIME;
                    else if (explosion[i].frame <= EXPLOSION_TOTAL_TIME) explosion[i].radiusMultiplier = 1 - (explosion[i].frame - (float)EXPLOSION_INCREASE_TIME)/(float)EXPLOSION_TOTAL_TIME;
                    else
                    {
                        explosion[i].frame = 0;
                        explosion[i].active = false;
                    }

                    // Check collicion with turrets
                    for (int j = 0; j < TURRETS_AMOUNT; j++){
                        if (turret[j].active){
                            if (CheckCollisionCircleRec(explosion[i].position, EXPLOSION_RADIUS * explosion[i].radiusMultiplier, Rectangle { turret[j].position.x - TURRET_WIDTH/2, turret[j].position.y -TURRET_WIDTH/2, TURRET_WIDTH, TURRET_HEIGHT })){
                                turret[j].active = false;
                            }
                        }
                    }

                    // Check collision with buildings
                    for (int j = 0; j < BUILDINGS_AMOUNT; j++){
                        if (building[j].active){
                            if (CheckCollisionCircleRec(explosion[i].position, EXPLOSION_RADIUS * explosion[i].radiusMultiplier, Rectangle { building[j].position.x - BUILDING_WIDTH/2, building[j].position.y - BUILDING_HEIGHT/2, BUILDING_WIDTH, BUILDING_HEIGHT })){
                                building[j].active = false;
                            }
                        }
                    }
                }
            }


            // Fire logic
            UpdateOutgoingFire();
            UpdateIncomingFire();

            // Game over logic
            int checker = 0;

            for (int i = 0; i < TURRETS_AMOUNT; i++)
            {
                if (!turret[i].active) checker++;
                if (checker == TURRETS_AMOUNT) gameOver = true;
            }

            checker = 0;
            for (int i = 0; i < BUILDINGS_AMOUNT; i++)
            {
                if (!building[i].active) checker++;
                if (checker == BUILDINGS_AMOUNT) gameOver = true;
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}
    
// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawTextureV(Tbg, bg.origin, WHITE);

        if (!gameOver)
        {
            // Draw missiles
            for (int i = 0; i < MAX_MISSILES; i++)
            {
                if (missile[i].active)
                {
                    DrawLineEx(missile[i].origin, missile[i].position, 0.5f, RED);

                    if (framesCounter % 16 < 8) DrawCircle(missile[i].position.x, missile[i].position.y, 3, YELLOW);
                }
            }
            
            // Draw interceptors
            for (int i = 0; i < MAX_INTERCEPTORS; i++)
            {
                if (interceptor[i].active)
                {
                    DrawLine(interceptor[i].origin.x, interceptor[i].origin.y, interceptor[i].position.x, interceptor[i].position.y, GREEN);

                    if (framesCounter % 16 < 8) DrawCircle(interceptor[i].position.x, interceptor[i].position.y, 3, BLUE);
                }
            }
            
            // Draw explosions
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosion[i].active) DrawCircle(explosion[i].position.x, explosion[i].position.y, EXPLOSION_RADIUS*explosion[i].radiusMultiplier, EXPLOSION_COLOR);
            }

            // Draw turrets
            for (int i = 0; i < TURRETS_AMOUNT; i++)
            {   
                Vector2 mousePosition = GetMousePosition();
                turretTop[i].angle = atan2(mousePosition.y - turretTop[i].center.y, mousePosition.x - turretTop[i].center.x) * 180 / M_PI;
                if (turret[i].active) DrawSprite(TturretTop, turretTop[i],  turretTop[i].origin, turretTop[i].angle, 0, i);
                if (turret[i].active) DrawTextureV(TturretBottom, turretBottom[i].origin, WHITE);
                // if (turret[i].active) DrawRectangle(turret[i].position.x - TURRET_WIDTH/2, turret[i].position.y - TURRET_HEIGHT/2, TURRET_WIDTH, TURRET_HEIGHT, {255, 255, 255, 60});
            }
            
            // Draw buildings
            for (int i = 0; i < BUILDINGS_AMOUNT; i++)
            {
                Rectangle TbuildingRec { (float) (i % 5) * Tbuilding.width/5, 0, (float) Tbuilding.width/5, (float) Tbuilding.height };
                if (building[i].active) DrawTextureRec(Tbuilding, TbuildingRec, Vector2 {building[i].position.x - BUILDING_WIDTH/2, building[i].position.y - BUILDING_HEIGHT/2}, WHITE);
                // if (building[i].active) DrawRectangle(building[i].position.x - BUILDING_WIDTH/2, building[i].position.y - BUILDING_HEIGHT/2, BUILDING_WIDTH, BUILDING_HEIGHT, {255, 255, 255, 60});
            }

            // Draw grass
            DrawTextureEx(Tgrass, grass.origin, 0, 1, WHITE);

            // Draw bottom of background
            DrawTextureEx(TbgBottom, bgBottom.origin, 0, 1, WHITE);

            // Draw score
            DrawText(TextFormat("SCORE %4i", score), 20, 20, 40, LIGHTGRAY);
            
            // Draw fire mode frame
            Rectangle fireModeRec {20, 80, 150, 30};
            DrawTextEx(font, TextFormat("%s", fireModes[fireMode].c_str()), Vector2{20, 80}, 25, 1, BLACK);
            
            // Time and Fps
            DrawText(TextFormat("%.2f", inGameTime), screenWidth - 40, screenHeight - 40, 20, LIME);
            DrawFPS(screenWidth - 80, screenHeight - 20);

            // Pause
            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, YELLOW);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, WHITE);

    EndDrawing();

    inGameTime = inGameTime + GetFrameTime();
}


//------------------------------------------------------------------------------------------
// Module for fire logic
//------------------------------------------------------------------------------------------
static void UpdateOutgoingFire()
{
    switch (fireMode)
    {
        case INTERCEPTOR:   
            UpdateOutgoingInterceptor();
            break;
        case SWARMING_MISSILES:  
            UpdateOutgoingSwarmingMissiles();
            break;
        case LASERGUN:
            UpdateOutgoingLaserBeam();
            break;
        case AIRBURST:
            UpdateOutgoingAirburst();
            break;
    }
}

static void UpdateIncomingFire()
{
    static int missileIndex = 0;

    // Launch missile
    if (framesCounter % MISSILE_LAUNCH_FRAMES == 0)
    {
        float module;
        float sideX;
        float sideY;

        // Activate the missile
        missile[missileIndex].active = true;

        // Assign start position
        missile[missileIndex].origin = (Vector2){ (float) GetRandomValue(20, screenWidth - 20), -10 };
        missile[missileIndex].position = missile[missileIndex].origin;
        missile[missileIndex].objective = (Vector2){ (float)  GetRandomValue(20, screenWidth - 20), (float)  screenHeight + 10 };

        // Calculate speed
        module = hypot( missile[missileIndex].objective.x - missile[missileIndex].origin.x,
                        missile[missileIndex].objective.y - missile[missileIndex].origin.y);

        sideX = (missile[missileIndex].objective.x - missile[missileIndex].origin.x)*MISSILE_SPEED/module;
        sideY = (missile[missileIndex].objective.y - missile[missileIndex].origin.y)*MISSILE_SPEED/module;

        missile[missileIndex].speed = (Vector2){ sideX, sideY };

        // Update
        missileIndex++;
        if (missileIndex == MAX_MISSILES) missileIndex = 0;
    }
}


//------------------------------------------------------------------------------------------
// Additional fire modules for diff types
//------------------------------------------------------------------------------------------
static void UpdateOutgoingInterceptor()
{
    static int interceptorNumber = 0;
    int turretShooting = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) turretShooting = 0;
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) turretShooting = 1;

    if (turretShooting > -1 && turret[turretShooting].active)
    {
        float module;
        float sideX;
        float sideY;

        // Activate the interceptor
        interceptor[interceptorNumber].active = true;

        // Assign start position
        interceptor[interceptorNumber].origin = (Vector2){ turret[turretShooting].position.x, turret[turretShooting].position.y - TURRET_HEIGHT/2 + TturretTop.height/2};
        interceptor[interceptorNumber].position = interceptor[interceptorNumber].origin;
        interceptor[interceptorNumber].objective = GetMousePosition();

        // Calculate speed
        module = hypot( interceptor[interceptorNumber].objective.x - interceptor[interceptorNumber].origin.x,
                        interceptor[interceptorNumber].objective.y - interceptor[interceptorNumber].origin.y);

        sideX = (interceptor[interceptorNumber].objective.x - interceptor[interceptorNumber].origin.x)*INTERCEPTOR_SPEED/module;
        sideY = (interceptor[interceptorNumber].objective.y - interceptor[interceptorNumber].origin.y)*INTERCEPTOR_SPEED/module;

        interceptor[interceptorNumber].speed = (Vector2){ sideX, sideY };

        // Update
        interceptorNumber++;
        if (interceptorNumber == MAX_INTERCEPTORS) interceptorNumber = 0;
    }
}

static void UpdateOutgoingSwarmingMissiles()
{
    
}

static void UpdateOutgoingLaserBeam()
{

}

static void UpdateOutgoingAirburst()
{

}

static void UpdateShrapnel()
{

}

//------------------------------------------------------------------------------------------
// Additional modules
//------------------------------------------------------------------------------------------

// Flip rectangle horizontaly or verticaly
Rectangle RectangleScale(Rectangle rec, int xscale, int yscale)
{ 
    return (Rectangle){rec.x, rec.y, rec.width * xscale, rec.height * yscale};
}

// Draw flipped (or not) texture
void DrawSprite(Texture2D sprite, Textures textures, Vector2 pos, int angle, bool flipx, bool flipy) 
{
    Rectangle flippedRectangle = RectangleScale({ 0, 0, (float)sprite.width, (float)sprite.height }, flipx==true?-1:1, flipy==true?-1:1);
    DrawTexturePro(sprite, flippedRectangle, Rectangle { textures.center.x, textures.center.y, (float)sprite.width, (float) sprite.height }, Vector2{ (float)sprite.width/2, (float)sprite.height/2}, angle, WHITE);                
}

// Check collision particle with buildings, turrets and explosions
bool CheckCollisionParticle(Particle particle){
    // Collision and particle out of bounds
    if (particle.position.y > groundPositionScale * screenHeight) {
        // particle dissapears
        particle.active = false;

        // Explosion appears
        if (particle.explosive){
            explosion[explosionIndex].position = particle.position;
            explosion[explosionIndex].active = true;
            explosion[explosionIndex].frame = 0;
            explosionIndex++;
            if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;
        }
    }
    else
    {
        // CHeck collision with turrets
        for (int j = 0; j < TURRETS_AMOUNT; j++)
        {
            if (turret[j].active)
            {
                if (CheckCollisionPointRec(particle.position,  (Rectangle){ turret[j].position.x - TURRET_WIDTH/2, turret[j].position.y - TURRET_HEIGHT/2,
                                                                            TURRET_WIDTH, TURRET_HEIGHT }))
                {
                    // particle dissapears
                    particle.active = false;

                    // Explosion and destroy building
                    turret[j].active = false;

                    if (particle.explosive)
                    {   
                        explosion[explosionIndex].position = particle.position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;
                    }

                    break;
                }
            }
        }

        // CHeck collision with buildings
        for (int j = 0; j < BUILDINGS_AMOUNT; j++)
        {
            if (building[j].active)
            {
                if (CheckCollisionPointRec(particle.position,  (Rectangle){ building[j].position.x - BUILDING_WIDTH/2, building[j].position.y - BUILDING_HEIGHT/2, BUILDING_WIDTH, BUILDING_HEIGHT }))
                {
                    // particle dissapears
                    particle.active = false;

                    // Explosion and destroy building
                    building[j].active = false;
                    if (particle.explosive)
                    {
                        explosion[explosionIndex].position = particle.position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;
                    }

                    break;
                }
            }
        }

        // CHeck collision with explosions
        for (int j = 0; j < MAX_EXPLOSIONS; j++)
        {
            if (explosion[j].active)
            {
                if (CheckCollisionPointCircle(particle.position, explosion[j].position, EXPLOSION_RADIUS*explosion[j].radiusMultiplier))
                {
                    // particle dissapears and we earn 1 points
                    particle.active = false;
                    score += 1;
                    if (particle.explosive)
                    {
                        explosion[explosionIndex].position = particle.position;
                        explosion[explosionIndex].active = true;
                        explosion[explosionIndex].frame = 0;
                        explosionIndex++;
                        if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;
                    }

                    break;
                }
            }
        }
    }
    return particle.active;
}

//------------------------------------------------------------------------------------------
// Modules that upload and unload all images, textures, music
//------------------------------------------------------------------------------------------

// Upload game 
void UploadGame(void){
    // Upload and set game icon
    Image icon = LoadImage("../resources/icon.png");
    SetWindowIcon(icon);
    
    // Upload background
    Tbg = LoadTexture("../resources/bg.png");
    bg.origin = Vector2 { 0, 0 };
    bg.center = Vector2 { Tbg.width/2 + bg.origin.x, Tbg.height/2 + bg.origin.y };

    TbgBottom = LoadTexture("../resources/bg_bottom.png");
    bgBottom.origin = Vector2 { 0, screenHeight * groundPositionScale };
    bgBottom.center = Vector2 { TbgBottom.width/2 + bgBottom.origin.x, TbgBottom.height/2 + bgBottom.origin.y };

    // Upload grass
    Tgrass = LoadTexture("../resources/grass_blue.png");
    grass.origin = Vector2 { 0, screenHeight * groundPositionScale - Tgrass.height };
    grass.center = Vector2 { Tgrass.width/2 + grass.origin.x, Tgrass.height/2 + grass.origin.y };

    // Upload building textures
    Tbuilding = LoadTexture("../resources/homes.png");

    // Upload turret textures
    TturretTop = LoadTexture("../resources/turretTop.png");
    
    for (int i = 0; i < 2; i++){
        turretTop[i].origin = Vector2 { turret[i].position.x - TturretTop.width/2, turret[i].position.y - TURRET_HEIGHT/2 };
        turretTop[i].center = Vector2 { TturretTop.width/2 + turretTop[i].origin.x, TturretTop.height/2 + turretTop[0].origin.y };
    }

    TturretBottom = LoadTexture("../resources/turretBottom.png");
    
    for (int i = 0; i < 2; i++){
        turretBottom[i].origin = Vector2 { turret[i].position.x - TURRET_WIDTH/2, turret[i].position.y + TURRET_HEIGHT/2 - TturretBottom.height };
        turretBottom[i].center = Vector2 { TturretBottom.width/2 + turretBottom[i].origin.x, TturretBottom.height/2 + turretBottom[0].origin.y };
    }

    // Upload music


}

// Unload game variables
void UnloadGame(void)
{
    // UnloadImage();
    UnloadTexture(Tbg);
    UnloadTexture(TbgBottom);
    UnloadTexture(TturretTop);
    UnloadTexture(TturretBottom);
    UnloadTexture(Tbuilding);    
}
