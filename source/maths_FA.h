#ifndef MATHS_FA__H
#define MATHS_FA__H

#include <limits.h>
#include <float.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define EPSILON 0.0000001f
#define EPSILON_DOUBLE 0.0000000000001f
#define PI 3.1415926535f

//NOTE(Stanisz13): VECTORS
typedef struct
{
    union
    {
        struct
        {
            float x, y;
        };
        float mem[2];
    };
} FVec2;

typedef struct
{
    union
    {
        struct
        {
            double x, y;
        };
        double mem[2];
    };
} DVec2;

typedef struct
{
    union
    {
        struct
        {
            unsigned x, y;
        };
        unsigned mem[2];
    };
} UVec2;

typedef struct
{
    union
    {
        struct
        {
            int x, y;
        };
        int mem[2];
    };
} IVec2;

typedef struct
{
    union
    {
        struct
        {
            float x, y, z;
        };
        float mem[3];
    };
} FVec3;

typedef struct
{
    union
    {
        struct
        {
            double x, y, z;
        };
        double mem[3];
    };
} DVec3;

typedef struct
{
    union
    {
        struct
        {
            unsigned x, y, z;
        };
        unsigned mem[3];
    };
} UVec3;

typedef struct
{
    union
    {
        struct
        {
            int x, y, z;
        };
        int mem[3];
    };
} IVec3;

typedef struct
{
    union
    {
        struct
        {
            float x, y, z, w;
        };
        float mem[4];
    };
} FVec4;

typedef struct
{
    union
    {
        struct
        {
            double x, y, z, w;
        };
        double mem[4];
    };
} DVec4;

typedef struct
{
    union
    {
        struct
        {
            unsigned x, y, z, w;
        };
        unsigned mem[4];
    };
} UVec4;

typedef struct
{
    union
    {
        struct
        {
            int x, y, z, w;
        };
        int mem[4];
    };
} IVec4;

//NOTE(Stanisz13): MATRICES
//NOTE(Stanisz13): FLOAT
typedef struct
{
    union
    {
        struct
        {
            FVec2 col1, col2;
        };
        float mem[4];
    };
} FMat2;

typedef struct
{
    union
    {
        struct
        {
            FVec3 col1, col2, col3;
        };
        float mem[9];
    };
} FMat3;

typedef struct
{
    union
    {
        struct
        {
            FVec4 col1, col2, col3, col4;
        };
        float mem[16];
    };
} FMat4;

//NOTE(Stanisz13): RANDOM
typedef struct
{
    unsigned seed;
    
} RandomSeries;

//NOTE(Stanisz13): GLOBALS
RandomSeries randomSeries_FA;


//NOTE(Stanisz13): FUNCTIONS
//NOTE(Stanisz13): NUMERIC
float absFloat(float v);
int absInt(int v);
double absDouble(double v);

float minFloat(float v, float w);
float maxFloat(float v, float w);
double minDouble(double v, double w);
double maxDouble(double v, double w);
int maxInt(int v, int w);
int minInt(int v, int w);
unsigned minUnsigned(unsigned v, unsigned w);
unsigned maxUnsigned(unsigned v, unsigned w);

float degreesToRadians(float deg);
float radiansToDegrees(float rad);

//NOTE(Stanisz13): RANDOM
void setRandomSeed(unsigned s);
void setRandomSeedTime();

unsigned getRandomUnsignedOnInterval(unsigned a, unsigned b);
float getRandomFloatOnInterval(float a, float b);
double getRandomDoubleOnInterval(double a, double b);
int getRandomIntOnInterval(int a, int b);

FVec2 getRandomFVec2OnInterval(float a, float b);
DVec2 getRandomDVec2OnInterval(double a, double b);
UVec2 getRandomUVec2OnInterval(unsigned a, unsigned b);
IVec2 getRandomIVec2OnInterval(int a, int b);
FVec3 getRandomFVec3OnInterval(float a, float b);
DVec3 getRandomDVec3OnInterval(double a, double b);
UVec3 getRandomUVec3OnInterval(unsigned a, unsigned b);
IVec3 getRandomIVec3OnInterval(int a, int b);
FVec4 getRandomFVec4OnInterval(float a, float b);
DVec4 getRandomDVec4OnInterval(double a, double b);
UVec4 getRandomUVec4OnInterval(unsigned a, unsigned b);
IVec4 getRandomIVec4OnInterval(int a, int b);

//NOTE(Stanisz13): VECTORS
FVec2 scaleFVec2(FVec2 v, float s);
DVec2 scaleDVec2(DVec2 v, double s);
UVec2 scaleUVec2(UVec2 v, unsigned s);
IVec2 scaleIVec2(IVec2 v, int s);
FVec3 scaleFVec3(FVec3 v, float s);
DVec3 scaleDVec3(DVec3 v, double s);
UVec3 scaleUVec3(UVec3 v, unsigned s);
IVec3 scaleIVec3(IVec3 v, int s);
FVec4 scaleFVec4(FVec4 v, float s);
DVec4 scaleDVec4(DVec4 v, double s);
UVec4 scaleUVec4(UVec4 v, unsigned s);
IVec4 scaleIVec4(IVec4 v, int s);

FVec2 addFVec2(FVec2 v, FVec2 w);
DVec2 addDVec2(DVec2 v, DVec2 w);
UVec2 addUVec2(UVec2 v, UVec2 w);
IVec2 addIVec2(IVec2 v, IVec2 w);
FVec3 addFVec3(FVec3 v, FVec3 w);
DVec3 addDVec3(DVec3 v, DVec3 w);
UVec3 addUVec3(UVec3 v, UVec3 w);
IVec3 addIVec3(IVec3 v, IVec3 w);
FVec4 addFVec4(FVec4 v, FVec4 w);
DVec4 addDVec4(DVec4 v, DVec4 w);
UVec4 addUVec4(UVec4 v, UVec4 w);
IVec4 addIVec4(IVec4 v, IVec4 w);

FVec2 hadamardFVec2(FVec2 v, FVec2 w);
DVec2 hadamardDVec2(DVec2 v, DVec2 w);
UVec2 hadamardUVec2(UVec2 v, UVec2 w);
IVec2 hadamardIVec2(IVec2 v, IVec2 w);
FVec3 hadamardFVec3(FVec3 v, FVec3 w);
DVec3 hadamardDVec3(DVec3 v, DVec3 w);
UVec3 hadamardUVec3(UVec3 v, UVec3 w);
IVec3 hadamardIVec3(IVec3 v, IVec3 w);
FVec4 hadamardFVec4(FVec4 v, FVec4 w);
DVec4 hadamardDVec4(DVec4 v, DVec4 w);
UVec4 hadamardUVec4(UVec4 v, UVec4 w);
IVec4 hadamardIVec4(IVec4 v, IVec4 w);

float dotProductFVec2(FVec2 v, FVec2 w);
double dotProductDVec2(DVec2 v, DVec2 w);
unsigned dotProductUVec2(UVec2 v, UVec2 w);
int dotProductIVec2(IVec2 v, IVec2 w);
float dotProductFVec3(FVec3 v, FVec3 w);
double dotProductDVec3(DVec3 v, DVec3 w);
unsigned dotProductUVec3(UVec3 v, UVec3 w);
int dotProductIVec3(IVec3 v, IVec3 w);
float dotProductFVec4(FVec4 v, FVec4 w);
double dotProductDVec4(DVec4 v, DVec4 w);
unsigned dotProductUVec4(UVec4 v, UVec4 w);
int dotProductIVec4(IVec4 v, IVec4 w);

float lengthSquaredFVec2(FVec2 v);
double lengthSquaredDVec2(DVec2 v);
unsigned lengthSquaredUVec2(UVec2 v);
unsigned lengthSquaredIVec2(IVec2 v);
float lengthSquaredFVec3(FVec3 v);
double lengthSquaredDVec3(DVec3 v);
unsigned lengthSquaredUVec3(UVec3 v);
unsigned lengthSquaredIVec3(IVec3 v);
float lengthSquaredFVec4(FVec4 v);
double lengthSquaredDVec4(DVec4 v);
unsigned lengthSquaredUVec4(UVec4 v);
unsigned lengthSquaredIVec4(IVec4 v);

float lengthFVec2(FVec2 v);
double lengthDVec2(DVec2 v);
float lengthUVec2(UVec2 v);
float lengthIVec2(IVec2 v);
float lengthFVec3(FVec3 v);
double lengthDVec3(DVec3 v);
float lengthUVec3(UVec3 v);
float lengthIVec3(IVec3 v);
float lengthFVec4(FVec4 v);
double lengthDVec4(DVec4 v);
float lengthUVec4(UVec4 v);
float lengthIVec4(IVec4 v);

//NOTE(Stanisz13): MATRICES

float* accessFMat2(FMat2* m, unsigned x, unsigned y);
float* accessFMat3(FMat3* m, unsigned x, unsigned y);
float* accessFMat4(FMat4* m, unsigned x, unsigned y);

FMat2 setFMat2ByElements(const float* data);
FMat3 setFMat3ByElements(const float* data);
FMat4 setFMat4ByElements(const float* data);

FMat2 setFMat2ByVectors(FVec2 a, FVec2 b);
FMat3 setFMat3ByVectors(FVec3 a, FVec3 b, FVec3 c);
FMat4 setFMat4ByVectors(FVec4 a, FVec4 b, FVec4 c, FVec4 d);

FMat2 identityFMat2();
FMat3 identityFMat3();
FMat4 identityFMat4();

FMat2 addFMat2(FMat2 a, FMat2 b);
FMat3 addFMat3(FMat3 a, FMat3 b);
FMat4 addFMat4(FMat4 a, FMat4 b);

FMat2 scaleFMat2(FMat2 a, float b);
FMat3 scaleFMat3(FMat3 a, float b);
FMat4 scaleFMat4(FMat4 a, float b);

FMat2 mulFMat2(FMat2 a, FMat2 b);
FMat3 mulFMat3(FMat3 a, FMat3 b);
FMat4 mulFMat4(FMat4 a, FMat4 b);

FVec2 mulFMat2ByFVec2(FMat2 a, FVec2 b);
FVec3 mulFMat3ByFVec3(FMat3 a, FVec3 b);
FVec4 mulFMat4ByFVec4(FMat4 a, FVec4 b);

FMat4 projectionFMat4(float near, float far,
                      float aRatio, float FOVradians);

#endif
