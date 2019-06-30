#include "SceneObjectTrack.hpp"
#include <cassert>
#include "Bezier.hpp"
#include "Linear.hpp"

namespace Panda
{
   void SceneObjectTrack::Update(const float timePoint)
    {
        if (m_pTransform)
        {
            auto timeCurveType = m_Time->GetCurveType();
            auto valueCurveType = m_Value->GetCurveType();

            float proportion = 0.0f;
            size_t index = 0;
            switch(timeCurveType)
            {
                case CurveType::kLinear:
                    proportion = std::dynamic_pointer_cast<Linear<float, float>>(m_Time)->Reverse(timePoint, index);
                    break;
                case CurveType::kBezier:
                    proportion = std::dynamic_pointer_cast<Bezier<float, float>>(m_Time)->Reverse(timePoint, index);
                    break;
                default:
                    assert(0);
            }

            switch(valueCurveType)
            {
                case CurveType::kLinear:
                {
                    switch(m_kTrackType)
                    {
                        case SceneObjectTrackType::kScalar:
                        {
                            auto new_val = std::dynamic_pointer_cast<Linear<float, float>>(m_Value)->Interpolate(proportion, index);
                            UpdateTransform(new_val);
                            break;
                        }
                        case SceneObjectTrackType::kVector3:
                        {
                            auto new_val = std::dynamic_pointer_cast<Linear<Vector3Df, Vector3Df>>(m_Value)->Interpolate(proportion, index);
                            UpdateTransform(new_val);
                            break;
                        }
                        case SceneObjectTrackType::kQuaternion:
                        {
                            auto new_val = std::dynamic_pointer_cast<Linear<Vector4Df, float>>(m_Value)->Interpolate(proportion, index);
                            UpdateTransform(new_val);
                            break;
                        }
                        case SceneObjectTrackType::kMatrix:
                        {
                            auto new_val = std::dynamic_pointer_cast<Linear<Matrix4f, float>>(m_Value)->Interpolate(proportion, index);
                            UpdateTransform(new_val);
                            break;
                        }
                    }
                    break;                    
                }

                case CurveType::kBezier:
                {
                    switch(m_kTrackType)
                    {
					case SceneObjectTrackType::kScalar:
					{
						auto new_val = std::dynamic_pointer_cast<Bezier<float, float>>(m_Value)->Interpolate(proportion, index);
						UpdateTransform(new_val);
						break;
					}
					case SceneObjectTrackType::kVector3:
					{
						auto new_val = std::dynamic_pointer_cast<Bezier<Vector3Df, Vector3Df>>(m_Value)->Interpolate(proportion, index);
						UpdateTransform(new_val);
						break;
					}
					case SceneObjectTrackType::kQuaternion:
					{
						auto new_val = std::dynamic_pointer_cast<Bezier<Vector4Df, float>>(m_Value)->Interpolate(proportion, index);
						UpdateTransform(new_val);
						break;
					}
					case SceneObjectTrackType::kMatrix:
					{
						auto new_val = std::dynamic_pointer_cast<Bezier<Matrix4f, float>>(m_Value)->Interpolate(proportion, index);
						UpdateTransform(new_val);
						break;
					}
                    }
                    break;
                }

                default:
                    assert(0);
            }
        }
    }

    template <typename U>
    inline void SceneObjectTrack::UpdateTransform(const U new_val)
    {
        m_pTransform->Update(new_val);
    }
}