#pragma once
#include <vector>
#include <unordered_set>
#include <fstream>
#include <stdexcept>
#include <cstddef> // std::size_t

#include "ienvironment.hpp"
#include "xcspp/random.hpp"

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

        const bool m_allowsDiagonalAction;
        const bool m_threeBitMode;

        static constexpr int xDiff(int idx)
        {
            constexpr int xDiffs[] = { 0, +1, +1, +1,  0, -1, -1, -1 };
            return xDiffs[idx];
        }

        static constexpr int yDiff(int idx)
        {
            constexpr int yDiffs[] = { -1, -1,  0, +1, +1, +1,  0, -1 };
            return yDiffs[idx];
        }

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

        std::vector<int> charToBits(unsigned char block) const
        {
            if (m_threeBitMode)
            {
                switch(block)
                {
                case 'T':
                case 'O':
                    return { 0, 1, 0 }; // O-obstacle
                case 'Q':
                    return { 0, 1, 1 }; // Q-obstacle
                case 'F':
                    return { 1, 1, 0 }; // Food-F
                case 'G':
                    return { 1, 1, 1 }; // Food-G
                default:
                    return { 0, 0, 0 }; // Empty
                }
            }
            else
            {
                switch(block)
                {
                case 'T':
                case 'O':
                case 'Q':
                    return { 0, 1 }; // Tree (Obstacle)
                case 'F':
                case 'G':
                    return { 1, 1 }; // Food
                default:
                    return { 0, 0 }; // Empty
                }
            }
        }

        void setToRandomEmptyPosition()
        {
            auto randomPosition = m_random.chooseFrom(m_emptyPositions);
            m_currentX = randomPosition.first;
            m_currentY = randomPosition.second;
            m_initialX = m_currentX;
            m_initialY = m_currentY;

            if (!isEmpty(m_currentX, m_currentY))
            {
                throw std::domain_error("Failed to set to empty position in BlockWorldEnvironment::setToRandomEmptyPosition()");
            }
        }

    public:
        // Constructor
        BlockWorldEnvironment(const std::string & mapFilename, std::size_t maxStep, bool threeBitMode, bool allowsDiagonalAction)
            : m_worldWidth(0)
            , m_worldHeight(0)
            , m_maxStep(maxStep)
            , m_lastStep(0)
            , m_currentStep(0)
            , m_isEndOfProblem(false)
            , m_allowsDiagonalAction(allowsDiagonalAction)
            , m_threeBitMode(threeBitMode)
        {
            std::ifstream ifs(mapFilename);
            std::string line;
            while (std::getline(ifs, line))
            {
                if (m_worldWidth == 0)
                {
                    // If the world width is not yet discovered, set it to the string length
                    m_worldWidth = static_cast<int>(line.length());
                }
                else
                {
                    // Make sure all lines in the text file have the same length
                    if (m_worldWidth != static_cast<int>(line.length()))
                    {
                        throw std::runtime_error("In BlockWorldEnvironment, all lines in the text file must have the same length.");
                    }
                }

                m_worldMap.push_back(line);
                ++m_worldHeight;
            }

            // Store empty positions for random initialization
            for (int y = 0; y < m_worldHeight; ++y)
            {
                for (int x = 0; x < m_worldWidth; ++x)
                {
                    if (isEmpty(x, y))
                    {
                        m_emptyPositions.emplace_back(x, y);
                    }
                }
            }

            setToRandomEmptyPosition();
            m_lastX = m_currentX;
            m_lastY = m_currentY;
            m_lastInitialX = m_initialX;
            m_lastInitialY = m_initialY;
        }

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

        unsigned char getBlock(int x, int y) const
        {
            x = (x + m_worldWidth) % m_worldWidth;
            y = (y + m_worldHeight) % m_worldHeight;
            return m_worldMap[y][x];
        }

        bool isEmpty(int x, int y) const
        {
            switch (getBlock(x, y))
            {
            case 'T':
            case 'O':
            case 'Q':
            case 'F':
            case 'G':
                return false;
            default:
                return true;
            }
        }

        bool isFood(int x, int y) const
        {
            switch (getBlock(x, y))
            {
            case 'F':
            case 'G':
                return true;
            default:
                return false;
            }
        }

        bool isObstacle(int x, int y) const
        {
            switch (getBlock(x, y))
            {
            case 'T':
            case 'O':
            case 'Q':
                return true;
            default:
                return false;
            }
        }

        std::vector<int> situation(int x, int y) const
        {
            std::vector<int> situation;
            for (int i = 0; i < kDirectionValueCount; ++i)
            {
                const auto block = getBlock(x + xDiff(i), y + yDiff(i));
                for (const auto & bit : charToBits(block))
                {
                    situation.push_back(bit);
                }
            }
            return situation;
        }

        virtual std::vector<int> situation() const override
        {
            return situation(m_currentX, m_currentY);
        }

        virtual double executeAction(int action) override
        {
            if (action < 0 || kDirectionValueCount <= action)
            {
                throw std::invalid_argument("BlockWorldEnvironment::executeAction() received an invalid action out of the range 0-7.");
            }

            m_lastInitialX = m_initialX;
            m_lastInitialY = m_initialY;

            // The coordinates after performing the action
            const int x = (m_currentX + xDiff(action) + m_worldWidth) % m_worldWidth;
            const int y = (m_currentY + yDiff(action) + m_worldHeight) % m_worldHeight;

            // Determine the reward and move the position
            double reward;
            if (isFood(x, y))
            {
                m_lastX = x;
                m_lastY = y;
                setToRandomEmptyPosition();
                m_isEndOfProblem = true;
                m_lastStep = m_currentStep + 1;
                m_currentStep = 0;
                reward = 1000.0;
            }
            else if (isEmpty(x, y))
            {
                m_currentX = x;
                m_currentY = y;
                m_lastX = m_currentX;
                m_lastY = m_currentY;
                m_isEndOfProblem = false;
                reward = 0.0;
            }
            else
            {
                m_lastX = m_currentX;
                m_lastY = m_currentY;
                m_isEndOfProblem = false;
                reward = 0.0;
            }

            if (!m_isEndOfProblem)
            {
                m_lastStep = ++m_currentStep;
                if (m_currentStep >= m_maxStep)
                {
                    // Teletransport when the step count reaches the maximum step size
                    setToRandomEmptyPosition();
                    m_isEndOfProblem = true;
                    m_currentStep = 0;
                }
            }

            return reward;
        }

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

        std::string toString() const
        {
            std::string str;
            for (int y = 0; y < m_worldHeight; ++y)
            {
                for (int x = 0; x < m_worldWidth; ++x)
                {
                    str += (x == m_currentX && y == m_currentY) ? '*' : getBlock(x, y);
                }
                str += '\n';
            }
            return str;
        }

        friend std::ostream & operator<< (std::ostream & os, const BlockWorldEnvironment & obj)
        {
            return os << obj.toString();
        }
    };

}
