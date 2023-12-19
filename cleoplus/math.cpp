#include "externs.h"
#include <addons/SimplexPerlinNoise.h>
#include <algorithm>

CLEO_Fn(QUAT_SLERP)
{
    CQuaternion *from = (CQuaternion *)cleo->ReadParam(handle)->i;
    CQuaternion *to = (CQuaternion *)cleo->ReadParam(handle)->i;
    float t = cleo->ReadParam(handle)->f;
    CQuaternion *result = (CQuaternion *)cleo->ReadParam(handle)->i;
    result->Slerp(from, to, t);
}
CLEO_Fn(SET_MATRIX_ROTATION_FROM_QUAT)
{
    CMatrix *matrix = (CMatrix *)cleo->ReadParam(handle)->i;
    CQuaternion *quat = (CQuaternion *)cleo->ReadParam(handle)->i;
    matrix->SetRotate(*quat);
}
CLEO_Fn(SET_QUAT_FROM_MATRIX)
{
    RwMatrix *matrix = (RwMatrix *)cleo->ReadParam(handle)->i;
    CQuaternion *quat = (CQuaternion *)cleo->ReadParam(handle)->i;
    QuaternionSet(quat, *matrix);
}
CLEO_Fn(ROTATE_QUAT_ON_AXIS)
{
    CVector axis;

    CQuaternion *quat = (CQuaternion *)cleo->ReadParam(handle)->i;
    axis.x = cleo->ReadParam(handle)->f;
    axis.y = cleo->ReadParam(handle)->f;
    axis.z = cleo->ReadParam(handle)->f;
    float angle = cleo->ReadParam(handle)->f;
    unsigned int combineOp = cleo->ReadParam(handle)->i;

    RtQuatRotate(quat, &axis, angle, (RwOpCombineType)combineOp);
}
CLEO_Fn(GET_NORMALISED_QUAT)
{
    CQuaternion *dst, *src;
    CQuaternion quat;

    src = (CQuaternion *)cleo->ReadParam(handle)->i;
    dst = (CQuaternion *)cleo->ReadParam(handle)->i;

    quat = *src;
    quat.Normalize();
    *dst = quat;
}
CLEO_Fn(MULTIPLY_QUATS)
{
    CQuaternion *result, *quat1, *quat2;

    quat1 = (CQuaternion *)cleo->ReadParam(handle)->i;
    quat2 = (CQuaternion *)cleo->ReadParam(handle)->i;
    result = (CQuaternion *)cleo->ReadParam(handle)->i;

    QuaternionMult(result, quat1, quat2);
}
CLEO_Fn(INITIALISE_QUAT)
{
    CQuaternionF *quat = (CQuaternionF *)cleo->ReadParam(handle)->i;
    quat->imag.x = cleo->ReadParam(handle)->f;
    quat->imag.y = cleo->ReadParam(handle)->f;
    quat->imag.z = cleo->ReadParam(handle)->f;
    quat->real =   cleo->ReadParam(handle)->f;
}
CLEO_Fn(GET_QUAT_ELEMENTS)
{
    CQuaternionF *quat = (CQuaternionF *)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->f = quat->imag.x;
    cleo->GetPointerToScriptVar(handle)->f = quat->imag.y;
    cleo->GetPointerToScriptVar(handle)->f = quat->imag.z;
    cleo->GetPointerToScriptVar(handle)->f = quat->real;
}
CLEO_Fn(ROTATE_MATRIX_ON_AXIS)
{
    RwMatrix *matrix; RwV3d axis; float angle; unsigned int combineOp;

    matrix = (RwMatrix *)cleo->ReadParam(handle)->i;

    axis.x = cleo->ReadParam(handle)->f;
    axis.y = cleo->ReadParam(handle)->f;
    axis.z = cleo->ReadParam(handle)->f;
    angle = cleo->ReadParam(handle)->f;
    combineOp = cleo->ReadParam(handle)->i;
    RwMatrixRotate(matrix, &axis, angle, (RwOpCombineType)combineOp);
}
CLEO_Fn(GET_MATRIX_X_ANGLE)
{
    RwMatrix *matrix = (RwMatrix *)cleo->ReadParam(handle)->i;
    float x = matrix->right.x;
    float y = matrix->right.y;
    float z = matrix->right.z;
    float angle = GetATanOfXY(z, sqrtf(x * x + y * y)) * 57.295776f - 90.0f;
    while (angle < 0.0) angle += 360.0;
    cleo->GetPointerToScriptVar(handle)->f = angle;
}
CLEO_Fn(GET_MATRIX_Y_ANGLE)
{
    RwMatrix *matrix = (RwMatrix *)cleo->ReadParam(handle)->i;
    float x = matrix->up.x;
    float y = matrix->up.y;
    float z = matrix->up.z;
    float angle = GetATanOfXY(z, sqrtf(x * x + y * y)) * 57.295776f - 90.0f;
    while (angle < 0.0) angle += 360.0;
    cleo->GetPointerToScriptVar(handle)->f = angle;
}
CLEO_Fn(GET_MATRIX_Z_ANGLE)
{
    RwMatrix *matrix = (RwMatrix *)cleo->ReadParam(handle)->i;
    float angle = GetATanOfXY(matrix->right.x, matrix->right.y) * 57.295776f - 90.0f;
    while (angle < 0.0) angle += 360.0;
    cleo->GetPointerToScriptVar(handle)->f = angle;
}
#pragma clang diagnostic ignored "-Wunsequenced" // warning: unsequenced modification and access to 'k'
inline float EaseBounceOut(float k)
{
    if (k < (1.0f / 2.75f)) return 7.5625f * k * k;
    else if (k < (2.0f / 2.75f)) return 7.5625f * (k -= (1.5f / 2.75f))*k + 0.75f;
    else if (k < (2.5f / 2.75f)) return 7.5625f * (k -= (2.25f / 2.75f))*k + 0.9375f;
    else return 7.5625f * (k -= (2.625f / 2.75f)) * k + 0.984375f;
}
CLEO_Fn(EASE)
{
    float k = cleo->ReadParam(handle)->f;
    int mode = cleo->ReadParam(handle)->i;
    int way = cleo->ReadParam(handle)->i;

    float result = 0.0f;

    switch (mode)
    {
        case 1: // Quadratic
        {
            switch (way)
            {
                case 0: //In
                {
                    result = k * k;
                    break;
                }
                case 1: //Out
                {
                    result = k * (2.0f - k);
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = 0.5f*k*k;
                    else result = -0.5f*((k -= 1.0f)*(k - 2.0f) - 1.0f);
                    break;
                }
            }
            break;
        };
        case 2: // Cubic
        {
            switch (way)
            {
                case 0: //In
                {
                    result = k * k*k;
                    break;
                }
                case 1: //Out
                {
                    result = 1.0f + ((k -= 1.0f)*k*k);
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = 0.5f*k*k*k;
                    else result = 0.5f*((k -= 2.0f)*k*k + 2.0f);
                    break;
                }
            }
            break;
        }
        case 3: // Quartic
        {
            switch (way)
            {
                case 0: //In
                {
                    result = k * k*k*k;
                    break;
                }
                case 1: //Out
                {
                    result = 1.0f - ((k -= 1.0f)*k*k*k);
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = 0.5f*k*k*k*k;
                    else result = -0.5f*((k -= 2.0f)*k*k*k - 2.0f);
                    break;
                }
            }
            break;
        }
        case 4: // Quintic
        {
            switch (way)
            {
                case 0: //In
                {
                    result = k * k*k*k*k;
                    break;
                }
                case 1: //Out
                {
                    result = 1.0f - ((k -= 1.0f)*k*k*k*k);
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = 0.5f*k*k*k*k*k;
                    result = 0.5f*((k -= 2.0f)*k*k*k*k + 2.0f);
                    break;
                }
            }
            break;
        }
        case 5: // Sinusoidal
        {
            switch (way)
            {
                case 0: //In
                {
                    result = 1.0f - cosf(k*PI / 2.0f);
                    break;
                }
                case 1: //Out
                {
                    result = sinf(k*PI / 2.0f);
                    break;
                }
                case 2: //InOut
                {
                    result = 0.5f*(1.0f - cosf(PI*k));
                    break;
                }
            }
            break;
        }
        case 6: // Exponential
        {
            switch (way)
            {
                case 0: //In
                {
                    result = k == 0.0f ? 0.0f : powf(1024.0f, k - 1.0f);
                    break;
                }
                case 1: //Out
                {
                    result = k == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f*k);
                    break;
                }
                case 2: //InOut
                {
                    if (k == 0.0f) result = 0.0f;
                    else if (k == 1.0f) result = 1.0f;
                    else if ((k *= 2.0f) < 1.0f) result = 0.5f*powf(1024.0f, k - 1.0f);
                    else result = 0.5f*(-powf(2.0f, -10.0f*(k - 1.0f)) + 2.0f);
                    break;
                }
            }
            break;
        }
        case 7: // Circular
        {
            switch (way)
            {
                case 0: //In
                {
                    result = 1.0f - sqrtf(1.0f - k * k);
                    break;
                }
                case 1: //Out
                {
                    result = sqrtf(1.0f - ((k -= 1.0f)*k));
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = -0.5f*(sqrtf(1.0f - k * k) - 1);
                    else result = 0.5f*(sqrtf(1.0f - (k -= 2.0f)*k) + 1.0f);
                    break;
                }
            }
            break;
        }
        case 8: // Elastic
        {
            switch (way)
            {
                case 0: //In
                {
                    if (k == 0) result = 0;
                    if (k == 1) result = 1;
                    result = -powf(2.0f, 10.0f*(k -= 1.0f))*sinf((k - 0.1f)*(2.0f*PI) / 0.4f);
                    break;
                }
                case 1: //Out
                {
                    if (k == 0) result = 0;
                    if (k == 1) result = 1;
                    result = powf(2.0f, -10.0f*k)*sinf((k - 0.1f)*(2.0f*PI) / 0.4f) + 1.0f;
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = -0.5f*powf(2.0f, 10.0f*(k -= 1.0f))*sinf((k - 0.1f)*(2.0f*PI) / 0.4f);
                    else result = powf(2.0f, -10.0f*(k -= 1.0f))*sinf((k - 0.1f)*(2.0f*PI) / 0.4f)*0.5f + 1.0f;
                    break;
                }
            }
            break;
        }
        case 9: // Back
        {
            static float s = 1.70158f;
            static float s2 = 2.5949095f;

            switch (way)
            {
                case 0: //In
                {
                    result = k * k*((s + 1.0f)*k - s);
                    break;
                }
                case 1: //Out
                {
                    result = (k -= 1.0f)*k*((s + 1.0f)*k + s) + 1.0f;
                    break;
                }
                case 2: //InOut
                {
                    if ((k *= 2.0f) < 1.0f) result = 0.5f*(k*k*((s2 + 1.0f)*k - s2));
                    else result = 0.5f*((k -= 2.0f)*k*((s2 + 1.0f)*k + s2) + 2.0f);
                    break;
                }
            }
            break;
        }
        case 10: // Bounce
        {
            switch (way)
            {
                case 0: //In
                {
                    result = 1.0f - EaseBounceOut(1.0f - k);
                    break;
                }
                case 1: //Out
                {
                    EaseBounceOut(k);
                    break;
                }
                case 2: //InOut
                {
                    if (k < 0.5f) result = (1.0f - EaseBounceOut(1.0f - k * 2.0f))*0.5f;
                    else result = EaseBounceOut(k * 2.0f - 1.0f)*0.5f + 0.5f;
                    break;
                }
            }
            break;
        }
        default: // Linear
        {
            result = k;
            break;
        }
    }
    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(GET_ANGLE_FROM_TWO_COORDS)
{
    float cx = cleo->ReadParam(handle)->f;
    float cy = cleo->ReadParam(handle)->f;
    float ex = cleo->ReadParam(handle)->f;
    float ey = cleo->ReadParam(handle)->f;

    float dx, dy, q1, q2, r;
    dy = ey;
    dy -= cy;
    dx = ex;
    dx -= cx;
    //to magnitude
    q1 = dx;
    q1 *= dx;
    q2 = dy;
    q2 *= dy;
    q1 += q2;
    r = sqrtf(q1);
    dx /= r;
    dy /= r;

    float result = GetATanOfXY(dx, dy) * 57.295776f - 90.0f;
    while (result < 0.0) result += 360.0;
    if (isnan(result)) result = 0.0f;
    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(PERLIN_NOISE)
{
    float result = SimplexNoise::noise(cleo->ReadParam(handle)->f);
    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(PERLIN_NOISE_FRACTAL)
{
    float f = cleo->ReadParam(handle)->f;
    int octaves = cleo->ReadParam(handle)->i;
    float frequency = cleo->ReadParam(handle)->f;
    float amplitude = cleo->ReadParam(handle)->f;
    float lacunarity = cleo->ReadParam(handle)->f;
    float persistence = cleo->ReadParam(handle)->f;

    SimplexNoise *simplexNoise = new SimplexNoise(frequency, amplitude, lacunarity, persistence);
    float result = simplexNoise->fractal(octaves, f);
    delete simplexNoise;

    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(GET_COORD_FROM_ANGLED_DISTANCE)
{
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float angle = cleo->ReadParam(handle)->f;
    float dist = cleo->ReadParam(handle)->f;

    angle *= 0.0174533f;
    angle *= -1.0f;
    float eq1 = sin(angle);
    float eq2 = cos(angle);

    eq1 *= dist;
    eq2 *= dist;

    x += eq1;
    y += eq2;

    cleo->GetPointerToScriptVar(handle)->f = x;
    cleo->GetPointerToScriptVar(handle)->f = y;
}
CLEO_Fn(PERLIN_NOISE_FRACTAL_2D)
{
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    int octaves = cleo->ReadParam(handle)->i;
    float frequency = cleo->ReadParam(handle)->f;
    float amplitude = cleo->ReadParam(handle)->f;
    float lacunarity = cleo->ReadParam(handle)->f;
    float persistence = cleo->ReadParam(handle)->f;

    SimplexNoise *simplexNoise = new SimplexNoise(frequency, amplitude, lacunarity, persistence);
    float result = simplexNoise->fractal(octaves, x, y);
    delete simplexNoise;

    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(PERLIN_NOISE_FRACTAL_3D)
{
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    int octaves = cleo->ReadParam(handle)->i;
    float frequency = cleo->ReadParam(handle)->f;
    float amplitude = cleo->ReadParam(handle)->f;
    float lacunarity = cleo->ReadParam(handle)->f;
    float persistence = cleo->ReadParam(handle)->f;

    SimplexNoise *simplexNoise = new SimplexNoise(frequency, amplitude, lacunarity, persistence);
    float result = simplexNoise->fractal(octaves, x, y, z);
    delete simplexNoise;

    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(CLAMP_FLOAT)
{
    float f = cleo->ReadParam(handle)->f;
    float min = cleo->ReadParam(handle)->f;
    float max = cleo->ReadParam(handle)->f;
    float result = 0.0f;
    if (!isnan(f)) result = std::clamp(f, min, max);
    cleo->GetPointerToScriptVar(handle)->f = result;
}
CLEO_Fn(CLAMP_INT)
{
    int i =   cleo->ReadParam(handle)->i;
    int min = cleo->ReadParam(handle)->i;
    int max = cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = std::clamp(i, min, max);
}
CLEO_Fn(CONVERT_DIRECTION_TO_QUAT)
{
    CVector dir;
    CQuaternionF *quaternion = (CQuaternionF *)cleo->ReadParam(handle)->i;

    dir.x = cleo->ReadParam(handle)->f;
    dir.y = cleo->ReadParam(handle)->f;
    dir.z = cleo->ReadParam(handle)->f;

    CVector forward = dir;
    VectorNormalise(&forward);

    // Fixes a bug, I don't know...
    if (forward.x == 0.0f) forward.x = 0.001f;
    if (forward.y == 0.0f) forward.y = 0.001f;
    if (forward.z == 0.0f) forward.z = 0.001f;

    CVector up = { 0.0f, 0.0, 1.0f };

    up = up - (forward * DotProduct(up, forward));
    VectorNormalise(&up);

    ///////////////////////

    CVector vector = forward;
    VectorNormalise(&vector);

    CVector vector2;
    vector2.Cross(up, vector);

    CVector vector3;
    vector3.Cross(vector, vector2);

    float m00 = vector2.x;
    float m01 = vector2.y;
    float m02 = vector2.z;
    float m10 = vector3.x;
    float m11 = vector3.y;
    float m12 = vector3.z;
    float m20 = vector.x;
    float m21 = vector.y;
    float m22 = vector.z;


    float num8 = (m00 + m11) + m22;

    if (num8 > 0.0f)
    {
        float num = (float)sqrtf(num8 + 1.0f);
        quaternion->real = num * 0.5f;
        num = 0.5f / num;
        quaternion->imag.x = (m12 - m21) * num;
        quaternion->imag.y = (m20 - m02) * num;
        quaternion->imag.z = (m01 - m10) * num;
    }
    if ((m00 >= m11) && (m00 >= m22))
    {
        float num7 = (float)sqrtf(((1.0f + m00) - m11) - m22);
        float num4 = 0.5f / num7;
        quaternion->imag.x = 0.5f * num7;
        quaternion->imag.y = (m01 + m10) * num4;
        quaternion->imag.z = (m02 + m20) * num4;
        quaternion->real = (m12 - m21) * num4;
    }
    if (m11 > m22)
    {
        float num6 = (float)sqrtf(((1.0f + m11) - m00) - m22);
        float num3 = 0.5f / num6;
        quaternion->imag.x = (m10 + m01) * num3;
        quaternion->imag.y = 0.5f * num6;
        quaternion->imag.z = (m21 + m12) * num3;
        quaternion->real = (m20 - m02) * num3;
    }
    else {
        float num5 = (float)sqrtf(((1.0f + m22) - m00) - m11);
        float num2 = 0.5f / num5;
        quaternion->imag.x = (m20 + m02) * num2;
        quaternion->imag.y = (m21 + m12) * num2;
        quaternion->imag.z = 0.5f * num5;
        quaternion->real = (m01 - m10) * num2;
    }
    ((CQuaternion*)quaternion)->Normalize();
}
CLEO_Fn(LERP)
{
    float a = cleo->ReadParam(handle)->f;
    float b = cleo->ReadParam(handle)->f;
    float t = cleo->ReadParam(handle)->f;
    cleo->GetPointerToScriptVar(handle)->f = a * (1 - t) + b * t;
}
CLEO_Fn(SET_MATRIX_LOOK_DIRECTION)
{
    CMatrix *matrix = (CMatrix *)cleo->ReadParam(handle)->i;
    CVector origin;
    CVector direction;
    origin.x = cleo->ReadParam(handle)->i;
    origin.y = cleo->ReadParam(handle)->i;
    origin.z = cleo->ReadParam(handle)->i;
    direction.x = cleo->ReadParam(handle)->i;
    direction.y = cleo->ReadParam(handle)->i;
    direction.z = cleo->ReadParam(handle)->i;

    CreateMatFromVec(NULL, matrix, &origin, &direction);
}