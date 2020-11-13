#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <cstddef> // std::size_t

#include "ienvironment.hpp"
#include "xcspp/util/random.hpp"

namespace xcspp
{

    class BlockWorldEnvironment : public IEnvironment
    {
    private:
        int m_worldWidth;
        int m_worldHeight;
        std::vector<std::string> m_worldMap;
        std::vector<std::pair<int, int>> m_emptyPositions;

        // Position of the last random initialization
        int m_initialX;
        int m_initialY;

        // Current position
        int m_currentX;
        int m_currentY;

        // Current position before random initialization
        int m_lastX;
        int m_lastY;

        // Position of the last random initialization before the current step
        int m_lastInitialX;
        int m_lastInitialY;

        const std::size_t m_maxStep;
        std::size_t m_lastStep;
        std::size_t m_currentStep;

        bool m_isEndOfProblem;

        Random m_random;

        const bool m_allowsDiagonalAction;
        const bool m_threeBitMode;

        enum Direction : int
        {
            kUp = 0,
            kUpRight,
            kRight,
            kDownRight,
            kDown,
            kDownLeft,
            kLeft,
            kUpLeft,
            kDirectionValueCount,
        };

        void setToRandomEmptyPosition();

    public:
        // Constructor
        BlockWorldEnvironment(const std::string & mapFilename, std::size_t maxStep, bool threeBitMode, bool allowsDiagonalAction);

        // Destructor
        virtual ~BlockWorldEnvironment() = default;

        int worldWidth() const
        {
            return m_worldWidth;
        }

        int worldHeight() const
        {
            return m_worldHeight;
        }

        int initialX() const
        {
            return m_initialX;
        }

        int initialY() const
        {
            return m_initialY;
        }

        int currentX() const
        {
            return m_currentX;
        }

        int currentY() const
        {
            return m_currentY;
        }

        std::size_t currentStep() const
        {
            return m_currentStep;
        }

        int lastX() const
        {
            return m_lastX;
        }

        int lastY() const
        {
            return m_lastY;
        }

        std::size_t lastStep() const
        {
            return m_lastStep;
        }

        int lastInitialX() const
        {
            return m_lastInitialX;
        }

        int lastInitialY() const
        {
            return m_lastInitialY;
        }

        char getBlockChar(int x, int y) const;

        bool isEmpty(int x, int y) const;

        bool isFood(int x, int y) const;

        bool isObstacle(int x, int y) const;

        std::vector<int> situation(int x, int y) const;

        virtual std::vector<int> situation() const override
        {
            return situation(m_currentX, m_currentY);
        }

        virtual double executeAction(int action) override;

        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        virtual std::unordered_set<int> availableActions() const override
        {
            if (m_allowsDiagonalAction)
            {
                return { kUp, kUpRight, kRight, kDownRight, kDown, kDownLeft, kLeft, kUpLeft };
            }
            else
            {
                return { kUp, kRight, kDown, kLeft };
            }
        }

        std::string toString() const;

        friend std::ostream & operator<< (std::ostream & os, const BlockWorldEnvironment & obj)
        {
            return os << obj.toString();
        }
    };

}
