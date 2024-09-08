#include "raymath.h"
#include <math.h>

float Vector2AngleBetween(Vector2 v1, Vector2 v2)
{
    // Step 1: Compute dot product
    float dotProduct = Vector2DotProduct(v1, v2);

    // Step 2: Compute magnitudes
    float length1 = Vector2Length(v1);
    float length2 = Vector2Length(v2);

    // Step 3: Compute cosine of the angle
    float cosineAngle = dotProduct / (length1 * length2);

    // Ensure cosineAngle is in the range [-1, 1] to avoid NaN values due to floating-point
    // precision issues
    if (cosineAngle > 1.0f)
        cosineAngle = 1.0f;
    if (cosineAngle < -1.0f)
        cosineAngle = -1.0f;

    // Step 4: Compute and return the angle in radians
    return acosf(cosineAngle);
}

Vector2 computeCentroid(Vector2 points[], int numPoints)
{
    Vector2 sum = Vector2Zero();
    if (numPoints == 0)
        return sum;   // Handle edge case

    for (int i = 0; i < numPoints; ++i)
    {
        sum = Vector2Add(sum, points[i]);
    }

    return Vector2Scale(sum, 1.0f / numPoints);
}
