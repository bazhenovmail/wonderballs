#include "ContinuousCollisions.h"
#include "Vector.h"
#include <assert.h>
#include <math.h>

namespace BallPhysics
{

ContinuousCollisionCirclePointOccured continuousCollisionCirclePoint(
    const Vector& pointOrigin,
    const Vector& circleOrigin,
    float radius,
    const Vector& pointVelocity,
    const Vector& circleVelocity )
{
    Vector dv = pointVelocity - circleVelocity;
    Vector cp = pointOrigin - circleOrigin;

    ContinuousCollisionCirclePointOccured result;

    if ( dotProduct( cp, dv ) < 0 )
    {
        float rSquared = radius * radius;

        result.a = dv.lengthSquared();
        result.b = 2 * dotProduct( cp, dv );
        float c = cp.lengthSquared() - rSquared;
        result.d = result.b * result.b - 4 * result.a * c;

        result.occured = result.d >= 0;
    }
    else
        result.occured = false;

    return result;
}

ContinuousCollisionCirclePointOccured continuousCollisionCircleCircle(
    const Vector& circle1Origin,
    const Vector& circle2Origin,
    float radius1,
    float radius2,
    const Vector& circle1Velocity,
    const Vector& circle2Velocity )
{
    Vector dv = circle1Velocity - circle2Velocity;
    Vector cp = circle1Origin - circle2Origin;
    float radius = radius1 + radius2;

    ContinuousCollisionCirclePointOccured result;

    if ( dotProduct( cp, dv ) < 0 )
    {
        float rSquared = radius * radius;

        result.a = dv.lengthSquared();
        result.b = 2 * dotProduct( cp, dv );
        float c = cp.lengthSquared() - rSquared;
        result.d = result.b * result.b - 4 * result.a * c;

        result.occured = result.d >= 0;
    }
    else
        result.occured = false;

    return result;
}

float continuousCollisionCirclePointTime( const ContinuousCollisionCirclePointOccured& occuredResult )
{
    assert( occuredResult.occured );
    return ( -occuredResult.b + sqrt( occuredResult.d ) * ( ( occuredResult.a > 0 ) * -2 + 1 ) ) / 2 / occuredResult.a; //slow operation!
}

ContinuousCollisionCircleLineOccured continuousCollisionCircleLine(
    const Vector& linePoint1Origin,
    const Vector& linePoint2Origin,
    const Vector& circleOrigin,
    float radius,
    const Vector& lineVelocity,
    const Vector& circleVelocity )
{
    ContinuousCollisionCircleLineOccured result;
    result.dr = linePoint2Origin - linePoint1Origin;
    if ( result.dr.x == 0.0f && result.dr.y == 0.0f )
    {
        result.occured = false;
        return result;
    }

    Vector surfacePerpClockwise{ -result.dr.y, result.dr.x };
    Vector velocityDiff = circleVelocity - lineVelocity;
    if ( dotProduct( velocityDiff, surfacePerpClockwise ) >= 0.0f || dotProduct( ( linePoint1Origin - circleOrigin ), surfacePerpClockwise ) > 0 )
    {
        result.occured = false;
        return result;
    }

    result.denominator = result.dr.y * ( lineVelocity.x - circleVelocity.x ) + result.dr.x * ( circleVelocity.y - lineVelocity.y );
    result.occured = result.denominator != 0.0f;
    return result;
}

ContinuousCollisionCircleLineTime continuousCollisionCircleLineTime(
    const ContinuousCollisionCircleLineOccured& occured,
    const Vector& circleOrigin,
    const Vector& linePoint1Origin,
    float radius )
{
    assert( occured.occured );
    ContinuousCollisionCircleLineTime result;
    result.drSquared = dotProduct( occured.dr, occured.dr );
    float plusMinus = radius * pow( result.drSquared, 0.5f );
    Vector point1ToCircle = circleOrigin - linePoint1Origin;
    float restNumerator = occured.dr.y * point1ToCircle.x - occured.dr.x * point1ToCircle.y;
    result.dt = ( restNumerator + plusMinus * ( ( occured.denominator > 0 ) * -2 + 1 ) ) / occured.denominator;
    return result;
}

Vector continuousCollisionCircleLinePoint(
    const Vector& linePoint1Origin,
    const Vector& linePoint2Origin,
    const Vector& circleOrigin,
    const Vector& lineVelocity,
    const Vector& circleVelocity,
    const ContinuousCollisionCircleLineTime& lineTime,
    Vector& r1,
    Vector& r2 )
{

    r1 = linePoint1Origin + lineVelocity * lineTime.dt;
    r2 = linePoint2Origin + lineVelocity * lineTime.dt;
    Vector c = circleOrigin + circleVelocity * lineTime.dt;

    return Vector(
        r1.x + ( r2.x - r1.x )
        * ( ( c.x - r1.x ) * ( r2.x - r1.x ) + ( c.y - r1.y ) * ( r2.y - r1.y ) )
        / ( lineTime.drSquared ),
        r1.y + ( r2.y - r1.y )
        * ( ( c.x - r1.x ) * ( r2.x - r1.x ) + ( c.y - r1.y ) * ( r2.y - r1.y ) )
        / ( lineTime.drSquared ) );
}

bool continuousCollisionCircleLinePointOnTrack(
    const Vector& r1,
    const Vector& r2,
    const Vector& collisionPoint )
{
    float minX, maxX;
    if ( r1.x > r2.x )
    {
        maxX = r1.x;
        minX = r2.x;
    }
    else
    {
        maxX = r2.x;
        minX = r1.x;
    }
    float minY, maxY;
    if ( r1.y > r2.y )
    {
        maxY = r1.y;
        minY = r2.y;
    }
    else
    {
        maxY = r2.y;
        minY = r1.y;
    }

    return
        collisionPoint.x >= minX
        && collisionPoint.x <= maxX
        && collisionPoint.y >= minY
        && collisionPoint.y <= maxY;
}

} //namespace