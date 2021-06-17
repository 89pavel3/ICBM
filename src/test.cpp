#include "ICBM.h" 
#include <doctest.h>

TEST_CASE("Collision_1") {
    screenHeight = rand() % 480 + 600;
    screenWidth = rand() % 720;

    // InitWindow(screenWidth, screenHeight, "ICBM");
    // InitAudioDevice();
    // InitGame();
    // UploadGame();
    // SetTargetFPS(60);
    
    for (int i = 0; i < 100; i++)
    {
        Particle particle {Vector2 {rand()%screenWidth, rand()%screenHeight}, Vector2 {rand()%screenWidth, rand()%screenHeight}, Vector2 {rand()%screenWidth, rand()%screenHeight}, Vector2 {screenWidth, screenHeight}, 0, 1};
        bool check = CheckCollisionParticle(particle, false, false, false, false, false);
        REQUIRE(check == true);
    }
}