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
#define TURRETS_AMOUNT              2           // Should not be changed
#define BUILDINGS_AMOUNT            7           // Should not be changed

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

/// Particle
/**
 * Родительский класс для всех снарядов
 */
class Particle {
public:
    Vector2 origin;    ///< Начальная позиция
    Vector2 position;  ///< Текущая позиция
    Vector2 objective; ///< Конечная позиция
    Vector2 speed;     ///< Скорость 

    bool explosive;    ///< Взорвется ли снаряд при колизии
    bool active;       ///< Активность снаряда
    
    /**
     * @brief Construct a new Particle object
     * 
     * @param originP Начальная позиция
     * @param positionP Текущая позиция
     * @param objectiveP Конечная позиция
     * @param speedP Скорость
     * @param explosiveP Взорвется ли снаряд при колизии
     * @param activeP Активность снаряда
     */
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

/// Missile
/*!
 * Дочерний класс Missile (вражеские ракеты) от Particle
 */
/// 
class Missile : public Particle {
public:
    Missile()
    : Particle()
    {
       explosive = 1;
    };
};


/// Interceptor
/*!
 * Дочерний класс Interceptor (дружественные ракеты) от Particle
 */ 
class Interceptor : public Particle {
public:
    Interceptor()
    : Particle()
    {
       explosive = 1;
    };
};

/// Laser
/*!
 * Нереализованный режим огня - лазер
 */
typedef struct Laser {
    Vector2 origin;     ///< Начальная позиция
    Vector2 objective;  ///< Точка задающая луч

    int frame;          ///< Количество кадров, прощедших с активации лазера

    bool active;        ///< Работает ли лазер
} Laser;

/// Airburst
/*!
 * Нереализованный режим огня - разрывной снаряд, разрывается на множество осколков
 */
typedef struct Airburst {
    Vector2 origin;     ///< Начальная позиция
    Vector2 position;   ///< Текущая позиция
    Vector2 objective;  ///< Конечная позиция
    Vector2 speed;      ///< Скорость

    int numberOfShrapnel = 10;  ///< Количество осколков

    bool explosive = 0; ///< Взорвется ли снаряд при колизии
    bool active;        ///< Активность снаряда
} Airburst;

/// Shrapnel
/*!
 * Нереализованный класс нереализовванного режима ведения огня
 */
class Shrapnel : public Particle {
public:
    Shrapnel()
    : Particle()
    {  
    };
};

/// Explosion
/*!
 * Струтктура описывающая взрыв
 */
typedef struct Explosion {
    Vector2 position;           ///< Позиция вызыва
    float radiusMultiplier;     ///< Параметр задающий радиус ??? РУУУУУУУУУУУУУУУУУУРУУУУУУУУУУУУУУУУУУ
    int frame;                  ///< Количество кадров, прощедших с активации взрыва
    bool active;                ///< Активность взрыва
} Explosion;

/// Turret
/*!
 * Струтктура описывающая турели
 */
typedef struct Turret {
    Vector2 position;       ///< Позиция турели
    Vector2 objective;      ///< Позиция, по которой турель ведёт огонь
    bool active;            ///< Жива ли турель
} Turret;

/// Building
/*!
 * Струтктура описывающая мирные здания
 */
typedef struct Building {
    Vector2 position;       ///< Позиция здания
    bool active;            ///< Живо ли здание
} Building; 

/// Textures
/*!
 * Струтктура описывающая текстуры (просто для удобства)
 */
typedef struct Textures{
    Vector2 center;         ///< Центр текстуры
    Vector2 origin;         ///< Позиция текстуры на экране
    int angle;              ///< Угол поворота текстуры
} Textures;

enum FireModes 
{
    INTERCEPTOR,                // single missile
    SWARMING_MISSILES,          // cascade of missiles
    LASERGUN,                   // lasergun
    AIRBURST                    // missile explode with shrapnel
} FireModes;

//------------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------------

static int screenWidth = 1080;      ///< Screen width in pixels
static int screenHeight = 720;      ///< Screen height in pixels

static float inGameTime;            ///< Time since the start of the game
static float groundPositionScale = 0.91;    ///< Height of the ground above the bottom of the screen in ratio

static int framesCounter = 0;       ///< Number of frames since start of the game
static bool gameOver = false;       ///< Game over value
static bool pause = false;          ///< Game pause value
static int score = 0;               ///< Game score value
static int fireMode;                ///< Current fire mode

static Missile missile[MAX_MISSILES];               ///< Array with all missiles
static Interceptor interceptor[MAX_INTERCEPTORS];   ///< Array with all interceptors
static Explosion explosion[MAX_EXPLOSIONS];         ///< Array with all explosions
static Turret turret[TURRETS_AMOUNT];               ///< Array with all turrets
static Building building[BUILDINGS_AMOUNT];         ///< Array with all buildings
static int explosionIndex = 0;

/// Array with names of fire modes
static std::string fireModes [4] {
    "INTERCEPTOR",                  // single missile
    "SWARM",                        // cascade of missiles
    "LASERGUN",                     // lasergun
    "AIRBURST"                      // anti-aircraft explosive projectile
}; 

/// Array of all cooldowns
static float cooldowns [8];
/// Array with cooldowns of fire modes
static const float cooldownsOriginal [8] = { 60, 300, 120, 120 };

//-----------------------------S-------------------------------------------------------------
// Resources Declaration
//------------------------------------------------------------------------------------------

Textures bg;
Textures grass;
Textures turretTop [2];
Textures turretBottom [2];

Texture2D T_bg;
Texture2D T_grass;
Texture2D T_building;
Texture2D T_turretTop;
Texture2D T_turretBottom;

Font font;

Music music;

//------------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------------
/**
 * @brief Initialize new game
 */
static void InitGame(void);

/**
 * @brief Main function, which update all game logic (one frame)
 */
static void UpdateGame(void);

/**
 * @brief Draw all elements (one frame)
 */
static void DrawGame(void);

/**
 * @brief Upload all content: textures, music
 */
static void UploadGame(void);

/**
 * @brief Unload all content
 */
static void UnloadGame(void);


// Additional module functions

/**
 * @brief Check collision particle with bounds, and optionaly with objective, explosions, turrets and buildings
 * 
 * @param particle Particle
 * @param withBounds check collision with bounds
 * @param withObjective check collision with objective
 * @param withExplosions check collision with explosions
 * @param withTurrets check collision with explosions
 * @param withBuildings check collision with buildings
 * 
 * @return particle.active which says that particle still alive or already not
 */
bool CheckCollisionParticle(Particle particle, bool withBounds, bool withObjective, bool withExplosions, bool withTurrets, bool withBuildings);

/**
 * @brief Update all explosions
 */
static void UpdateExplosions();

/**
 * @brief Update all interceptors position and check collision with it 
 */
static void UpdateInterceptors();

/**
 * @brief Update all missiles position and check collision with it 
 */
static void UpdateMissiles();


/**
 * @brief Initiate one of the fire modes
 */
static void CreateOutgoingFire();

/**
 * @brief Creates some missiles
 */
static void CreateIncomingFire();

/**
 * @brief Reduces all cooldowns to zero
 */
static void UpdateCooldown();


static void CreateOutgoingInterceptor();
static void CreateOutgoingSwarmingMissiles();
static void CreateOutgoingLaserBeam();
static void CreateOutgoingAirburst();
static void CreateShrapnel();

static void DrawCooldownBox();

/**
 * @brief Scales the rectangle along the x and y axis
 * 
 * @param rec Original rectangle
 * @param xscale x-axis scale
 * @param yscale y-axis scale
 * 
 * @return Scaled Rectangle
 */
static Rectangle RectangleScale(Rectangle rec, int xscale, int yscale);

/**
 * @brief Draw texture
 * 
 * @param sprite Original texture
 * @param textures Textures type which describe sprite
 * @param angle Angle at which the texture will be rotated
 * @param flipx flip along x-asis
 * @param flipy flip along y-asis
 * 
 * @return Draw sprite
 */
static void DrawSprite(Texture2D sprite, Textures textures, int angle, bool flipx, bool flipy);
