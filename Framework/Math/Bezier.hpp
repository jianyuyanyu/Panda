#pragma once
#include "Curve.hpp"
#include "Numerical.hpp"
#include <cassert>
#include <map>

namespace Panda
{
    template<typename T>
    class Bezier : public Curve<T>
    {
        private:
            std::map<T, T> m_IncomingControlPoints;
            std::map<T, T> m_OutgoingControlPoints;
            std::vector<T> m_Knots;

        public:
            Bezier() = default;
            Bezier(const std::vector<T> knots, const std::vector<T> incoming_cp, const std::vector<T> outgoint_cp)
            {
                assert(knots.size() == incoming_cp.size() && knots.size() == outgoint_cp.size());

                m_Knots = knots;

                size_t count = knots.size();

                for (size_t i = 0; i < count; ++i)
                {
                    T knot = knots[i];
                    T in_cp = incoming_cp[i];
                    T out_cp = outgoint_cp[i];
                    AddControlPoints(knot, in_cp, out_cp);
                }
            }

            Bezier(const T* knots, const T* incoming_cp, const T* outgoing_cp, const size_t count)
            {
                for (size_t i = 0; i < count; ++i)
                {
                    m_Knots.push_back(knots[i]);
                    AddControlPoints(knots[i], incoming_cp[i], outgoing_cp[i]);
                }
            }

            void AddKnot(const T knot)
            {
                m_Knots.push_back(knot);
            }

            void AddControlPoints(const T knot, const T incoming_cp, const T outgoing_cp)
            {
                assert(incoming_cp <= knot && knot <= outgoing_cp);
                m_IncomingControlPoints.insert({knot, incoming_cp});
                m_OutgoingControlPoints.insert({knot, outgoing_cp});
            }

            T Reverse (const T t) const final
            {
                T t1, t2;

                if (m_Knots.size() < 2)
                    return 0;
                
                if (t <= m_Knots.front())
                    return 0;

                if (t >= m_Knots.back())
                    return 1;
                
                for (size_t i = 0; i < m_Knots.size(); ++i)
                {
                    if (t >= m_Knots[i])
                        continue;

                    t1 = m_Knots[i - 1];
                    t2 = m_Knots[i];
                }

                T c1, c2;
                c1 = m_OutgoingControlPoints.find(t1)->second;
                c2 = m_IncomingControlPoints.find(t2)->second;

                typename NewtonRaphson<T>::nr_f f = [t2, t1, c2, c1, t](T s){
                    return (t2 - 3 * c2 + 3 * c1 - t1) * std::pow(s, 3.0)
                        + 3 * (c2 - 2 * c1 + t1) * std::pow(s, 2.0)
                        + 3 * (c1 - t1) * s
                        + t1 - t;
                };

                typename NewtonRaphson<T>::nr_fprime fprime = [t2, t1, c2, c1](T s)
                {
                    return 3 * (t2 - 3 * c2 + 3 * c1 - t1) * pow(s, 2.0)
                        + 6 * (c2 - 2 * c1 + t1) * s
                        + 3 * (c1 - t1);
                };

                return NewtonRaphson<T>::Solve(0.5f * (t1 + t2), f, fprime);
            }

            T Interpolate(const T s) const final
            {
                if (m_Knots.size() == 0)
                    return 0;
                else if (m_Knots.size() == 1)
                    return m_Knots[0];
                else
                {
                    T t = s * (m_Knots.back() - m_Knots.front());
                    T t1, t2;

                    if (t <= m_Knots.front())
                        return m_Knots.front();
                    
                    if (t >= m_Knots.back())
                        return m_Knots.back();

                    for (size_t i = 1; i < m_Knots.size(); ++i)
                    {
                        if (t >= m_Knots[i])
                            continue;
                        
                        t1 = m_Knots[i - 1];
                        t2 = m_Knots[i];
                    }

                    T c1, c2;
                    c1 = m_OutgoingControlPoints.find(t1)->second;
                    c2 = m_IncomingControlPoints.find(t2)->second;

                    return (t2 - 3 * c2 + 3 * c1 - t1) * std::pow(s, 3.0)
                        + 3 * (c2 - 2 * c1 + t1) * std::pow(s, 2.0)
                        + 3 * (c1 - t1) * s
                        + t1;
                }
            }

            CurveType GetCurveType() const final
            {
                return CurveType::kBezier;
            }
    };
}