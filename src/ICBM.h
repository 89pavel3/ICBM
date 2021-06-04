#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <string>
#include <ctime>

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define MAX_MISSILES                100
#define MAX_INTERCEPTORS            30
#define MAX_EXPLOSIONS              100
#define TURRETS_AMOUNT              2           // Not a variable, should not be changed
#define BUILDINGS_AMOUNT            7           // Not a variable, should not be changed

#define TURRET_WIDTH                51
#define TURRET_HEIGHT               80

#define BUILDING_WIDTH              63
#define BUILDING_HEIGHT             90

#define EXPLOSION_RADIUS            40

#define MISSILE_SPEED               1
#define MISSILE_LAUNCH_FRAMES       80
#define INTERCEPTOR_SPEED           10
#define EXPLOSION_INCREASE_TIME     90          // In frames
#define EXPLOSION_TOTAL_TIME        210         // In frames

#define EXPLOSION_COLOR             (Color){ 200, 50, 50, 125 }

//------------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------------
typedef struct Particle {
    Vector2 origin;
    Vector2 position;
    Vector2 objective;
    Vector2 speed;

    bool explosive;
    bool active;
}   Particle;

typedef struct Missile {
    Vector2 origin;
    Vector2 position;
    Vector2 objective;
    Vector2 speed;

    bool explosive = 1;
    bool active;
} Missile;

typedef struct Interceptor {
    Vector2 origin;
    Vector2 position;
    Vector2 objective;
    Vector2 speed;

    int cooldown = 60;

    bool explosive = 1;
    bool active;
} Interceptor;

typedef struct Swarm {
    Vector2 origin;
    Vector2 objective;
    Vector2 speed;
    
    int cooldown = 600;

    bool friendly = 1;    
    bool explosive = 1;
    bool active;
} Swarm;

typedef struct Laser {
    Vector2 origin;
    Vector2 objective;

    int cooldown = 300;
    int duration;
    int frame;

    bool explosive = 0;
    bool active;
} Laser;

typedef struct Airburst {
    Vector2 origin;
    Vector2 position;
    Vector2 objective;
    Vector2 speed;

    int cooldown = 60;
    int numberOfShrapnel = 10;

    bool explosive = 0;
    bool active;
} Airburst;

typedef struct Shrapnel {
    Vector2 origin;
    Vector2 position;
    Vector2 objective;
    Vector2 speed;

    int cooldown = 60;

    bool explosive = 0;
    bool active;
} Shrapnel;



typedef struct Explosion {
    Vector2 position;
    float radiusMultiplier;
    int frame;
    bool active;
} Explosion;

typedef struct Turret {
    Vector2 position;
    Vector2 objective;
    bool active;
} Turret;

typedef struct Building {
    Vector2 position;
    bool active;
} Building;

typedef enum FireModes 
{
    INTERCEPTOR,                // single missile
    SWARMING_MISSILES,          // cascade of missiles
    LASERGUN,                   // lasergun
    AIRBURST                    // missile explode with shrapnel
} FireModes;

typedef struct Textures{
    Vector2 center;
    Vector2 origin;
    int angle;
} Textures;


//------------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------------

static int screenWidth = 1080;
static int screenHeight = 720;

static float inGameTime;
static float groundPositionScale = 0.91;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;
static int score = 0;
static int fireMode;

static Missile missile[MAX_MISSILES] = { 0 };
static Interceptor interceptor[MAX_INTERCEPTORS] = { 0 };
static Explosion explosion[MAX_EXPLOSIONS] = { 0 };
static Turret turret[TURRETS_AMOUNT] = { 0 };
static Building building[BUILDINGS_AMOUNT] = { 0 };
static int explosionIndex = 0;

static std::string fireModes [4] {
    "INTERCEPTOR",                  // single missile
    "SWARM",                        // cascade of missiles
    "LASERGUN",                     // lasergun
    "AIRBURST"                      // anti-aircraft explosive projectile
}; 

//-----------------------------S-------------------------------------------------------------
// Textures Declaration
//------------------------------------------------------------------------------------------

Textures bg;
Textures bgBottom;
Textures grass;
Textures turretTop [2];
Textures turretBottom [2];

Texture2D Tbg;
Texture2D TbgBottom;
Texture2D Tgrass;
Texture2D Tbuilding;
Texture2D TturretTop;
Texture2D TturretBottom;

Font font;

//------------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UploadGame(void);        // Upload game (images, textures, audio)
static void UnloadGame(void);       // Unload game


// Additional module functions
static bool CheckCollisionParticle(Particle particle);

static void UpdateOutgoingFire();
static void UpdateIncomingFire();

static void UpdateOutgoingInterceptor();
static void UpdateOutgoingSwarmingMissiles();
static void UpdateOutgoingLaserBeam();
static void UpdateOutgoingAirburst();
static void UpdateShrapnel();

static Rectangle RectangleScale(Rectangle rec, int xscale, int yscale);
static void DrawSprite(Texture2D sprite, Textures textures, Vector2 pos, int angle, bool flipx, bool flipy);
