/**
 * @file BallModel.cpp
 * Implementation of the BallModel's drawing functions
 */

#include "BallModel.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Modeling/BallPhysics.h"
#include "Tools/Module/Blackboard.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Platform/Time.h"

void BallModel::draw() const
{
  // drawing of the ball model in the field view
  DEBUG_DRAWING("representation:BallModel", "drawingOnField")
  {
    const Vector2f& position(estimate.position);
    const Vector2f& velocity(estimate.velocity);
    CIRCLE("representation:BallModel",
           position.x(), position.y(), 45, 0, // pen width
           Drawings::solidPen, ColorRGBA::red,
           Drawings::solidBrush, ColorRGBA::red);
    ARROW("representation:BallModel", position.x(), position.y(),
          position.x() + velocity.x(), position.y() + velocity.y(), 5, 1, ColorRGBA::red);
    DRAWTEXT("representation:BallModel", position.x() + 100, position.y() + 100, 100, ColorRGBA::red, "validity: " << validity);
  }

  // drawing of the end position
  DEBUG_DRAWING("representation:BallModel:endPosition", "drawingOnField")
  {
    if(Blackboard::getInstance().exists("FieldDimensions"))
    {
      const FieldDimensions& fieldDimensions = static_cast<const FieldDimensions&>(Blackboard::getInstance()["FieldDimensions"]);
      Vector2f position = BallPhysics::getEndPosition(estimate.position, estimate.velocity, fieldDimensions.ballFriction);
      CIRCLE("representation:BallModel:endPosition",
             position.x(),  position.y(), 45, 0, // pen width
             Drawings::solidPen, ColorRGBA::black,
             Drawings::solidBrush, ColorRGBA(168, 25, 99, 220));
    }
  }

  DEBUG_DRAWING3D("representation:BallModel", "robot")
  {
    TRANSLATE3D("representation:BallModel", 0, 0, -230);
    if(Time::getTimeSince(timeWhenLastSeen) < 5000 && Time::getTimeSince(timeWhenDisappeared) < 1000)
    {
      SPHERE3D("representation:BallModel", estimate.position.x(), estimate.position.y(), estimate.radius, estimate.radius, ColorRGBA::orange);
      LINE3D("representation:BallModel", 0, 0, 1.f, estimate.position.x(), estimate.position.y(), 1.f, 5.f, ColorRGBA::orange);
    }
  }
}

void GroundTruthBallModel::draw() const
{
  DEBUG_DRAWING("representation:GroundTruthBallModel", "drawingOnField")
  {
    const Vector2f& position(estimate.position);
    const Vector2f& velocity(estimate.velocity);
    CIRCLE("representation:GroundTruthBallModel",
           position.x(), position.y(), 45, 0, // pen width
           Drawings::solidPen, ColorRGBA(255, 128, 0, 192),
           Drawings::solidBrush, ColorRGBA(255, 128, 0, 192));
    ARROW("representation:GroundTruthBallModel", position.x(), position.y(),
          position.x() + velocity.x(), position.y() + velocity.y(), 5, 1, ColorRGBA(255, 128, 0, 192));
  }
}

BallModelCompressed::BallModelCompressed(const BallModel& ballModel) :
  lastPerception(ballModel.lastPerception.cast<short>()),
  position(ballModel.estimate.position),
  velocity(ballModel.estimate.velocity),
  timeWhenLastSeen(ballModel.timeWhenLastSeen),
  //BEGIN Changed by Dortmund: we need the complete length of an unsigned int for
  //                           representing the timestamp.
  //timeWhenDisappeared((ballModel.timeWhenDisappeared & 0x00ffffff) | ballModel.seenPercentage << 24),
  timeWhenDisappeared(ballModel.timeWhenDisappeared),
  validity(ballModel.validity)
  //END
{}

BallModelCompressed::operator BallModel() const
{
  BallModel ballModel;
  ballModel.lastPerception = lastPerception.cast<float>();
  ballModel.estimate.position = position.cast<float>();
  ballModel.estimate.velocity = velocity.cast<float>();
  ballModel.timeWhenLastSeen = timeWhenLastSeen;
  //BEGIN Changed by Dortmund: we need the complete length of an unsigned int for
  //                           representing the timestamp.
  //ballModel.timeWhenDisappeared = timeWhenDisappeared & 0x00ffffff;
  ballModel.timeWhenDisappeared = timeWhenDisappeared;
  //ballModel.seenPercentage = static_cast<unsigned char>(timeWhenDisappeared >> 24);
  ballModel.seenPercentage = static_cast<unsigned char>(validity * 100.f); // unused by Dortmund
  ballModel.validity = validity;
  // END
  return ballModel;
}

void BallModel3D::draw() const
{
  // drawing of the ball model in the field view
  DEBUG_DRAWING("representation:BallModel3D", "drawingOnField")
  {
    const Vector3f& position(estimate.position);
    const Vector3f& velocity(estimate.velocity);
    CIRCLE("representation:BallModel3D",
           position.x(), position.y(), 45, 0, // pen width
           Drawings::solidPen, ColorRGBA::red,
           Drawings::solidBrush, ColorRGBA::red);
    ARROW("representation:BallModel3D", position.x(), position.y(),
          position.x() + velocity.x(), position.y() + velocity.y(), 5, 1, ColorRGBA::red);
  }

  DEBUG_DRAWING3D("representation:BallModel3D", "robot")
  {
    TRANSLATE3D("representation:BallModel3D", 0, 0, -230);
    if(Time::getTimeSince(timeWhenLastSeen) < 5000 && Time::getTimeSince(timeWhenDisappeared) < 1000)
    {
      SPHERE3D("representation:BallModel3D", estimate.position.x(), estimate.position.y(), estimate.position.z(), estimate.radius, ColorRGBA::orange);
    }
  }
}

