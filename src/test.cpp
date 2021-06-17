#include "ICBM.hpp" 
#include <doctest.h>

TEST_CASE("Window ready"){
    for(int i = 1; i < 10; i++){
        screenHeight = rand() % 480 + 600;
        screenWidth = rand() % 600 + 120;
        
        InitWindow(screenWidth, screenHeight, "Hoey Weey");
        REQUIRE(IsWindowReady() == true);
        CloseWindow();
    }
}

TEST_CASE("Unreal collision with Particle") {
    for (int i = 0; i < 100; i++)
    {
        Particle particle {Vector2 {rand()%screenWidth, rand()%screenHeight}, Vector2 {rand()%screenWidth, rand()%screenHeight}, Vector2 {rand()%screenWidth, rand()%screenHeight}, Vector2 {screenWidth, screenHeight}, 0, 1};
        bool check = CheckCollisionParticle(particle, false, false, false, false, false);
        REQUIRE(check == true);
    }
}

TEST_CASE("Rectangle FLIP"){
    for(int i = 1; i < 100; i++){
        int x = rand() % screenWidth;
        int y = rand() % screenHeight;
        int width = rand() % screenWidth - screenWidth;
        int height = rand() % screenHeight - screenHeight;
        int flipx = rand() % 2 * 2 - 1;
        int flipy = rand() % 2 * 2 - 1;

        Rectangle rectangle = {x, y, width, height};
        Rectangle scaledRectangle = RectangleScale(rectangle, flipx, flipy);
        REQUIRE(scaledRectangle.width * flipx == rectangle.width);
        REQUIRE(scaledRectangle.height * flipy == rectangle.height);
    }
}