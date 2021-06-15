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

#define INTERCEPTOR_COOLDOWN        60
#define SWARM_COOLDOWN              300
#define LASERGUN_COOLDOWN           120
#define AIRBURST_COOLDOWN           120

#define SWARM_DURATION              60

//------------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------------
class Particle {
public:
    Vector2 origin;
    Vector2 position;
    Vector2 objective;
    Vector2 speed;

    bool explosive;
    bool active;
    
    Particle(Vector2 originP = Vector2 {0, 0}, Vector2 positionP = Vector2 {0, 0}, Vector2 objectiveP = Vector2 {0, 0}, Vector2 speedP = Vector2 {0, 0}, bool explosiveP = 0, bool activeP =0)
    {
        this->origin = originP;
        this->position = positionP;
        this->objective = objectiveP;
        this->speed = speedP;

        this->explosive = explosiveP;
        this->active = activeP;
    };

    ~Particle(){}
};

class Missile : public Particle {
public:
    Missile()
    : Particle()
    {
       explosive = 1;
    };
};

class Interceptor : public Particle {
public:
    Interceptor()
    : Particle()
    {
       explosive = 1;
    };
};


typedef struct Laser {
    Vector2 origin;
    Vector2 objective;

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

    int numberOfShrapnel = 10;

    bool explosive = 0;
    bool active;
} Airburst;

class Shrapnel : public Particle {
public:
    Shrapnel()
    : Particle()
    {  
    };
};


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

enum FireModes 
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

static Missile missile[MAX_MISSILES];
static Interceptor interceptor[MAX_INTERCEPTORS];
static Explosion explosion[MAX_EXPLOSIONS];
static Turret turret[TURRETS_AMOUNT];
static Building building[BUILDINGS_AMOUNT];
static int explosionIndex = 0;

static std::string fireModes [4] {
    "INTERCEPTOR",                  // single missile
    "SWARM",                        // cascade of missiles
    "LASERGUN",                     // lasergun
    "AIRBURST"                      // anti-aircraft explosive projectile
}; 

static float cooldowns [8];
static const float cooldownsOriginal [8] = { 60, 300, 120, 120 };

//-----------------------------S-------------------------------------------------------------
// Resources Declaration
//------------------------------------------------------------------------------------------

Textures bg;
Textures bgBottom;
Textures grass;
Textures turretTop [2];
Textures turretBottom [2];

Texture2D T_bg;
Texture2D T_bgBottom;
Texture2D T_grass;
Texture2D T_building;
Texture2D T_turretTop;
Texture2D T_turretBottom;

Font font;

Music music;

//------------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UploadGame(void);        // Upload game (images, textures, audio)
static void UnloadGame(void);       // Unload game


// Additional module functions
bool CheckCollisionParticle(Particle particle, bool withObjective, bool withExplosion, bool withTurret, bool withBuilding);
static void UpdateExplosions();

static void UpdateOutgoingFire();
static void UpdateIncomingFire();
static void UpdateCooldown();

static void UpdateOutgoingInterceptor();
static void UpdateOutgoingSwarmingMissiles();
static void UpdateOutgoingLaserBeam();
static void UpdateOutgoingAirburst();
static void UpdateShrapnel();

static void DrawCooldownBox();

static Rectangle RectangleScale(Rectangle rec, int xscale, int yscale);
static void DrawSprite(Texture2D sprite, Textures textures, int angle, bool flipx, bool flipy);
