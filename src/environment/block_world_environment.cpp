#include "xcspp/environment/block_world_environment.hpp"
#include <fstream>
#include <stdexcept>

namespace xcspp
{
    namespace
    {
        constexpr int XDiff(int idx)
        {
            constexpr int xDiffs[] = { 0, +1, +1, +1,  0, -1, -1, -1 };
            return xDiffs[idx];
        }

        constexpr int YDiff(int idx)
        {
            constexpr int yDiffs[] = { -1, -1,  0, +1, +1, +1,  0, -1 };
            return yDiffs[idx];
        }

        std::vector<int> CharToBits(char block, bool threeBitMode)
        {
            if (threeBitMode)
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
    }

    void BlockWorldEnvironment::setToRandomEmptyPosition()
    {
        const auto randomPosition = m_random.chooseFrom(m_emptyPositions);
        m_currentX = randomPosition.first;
        m_currentY = randomPosition.second;
        m_initialX = m_currentX;
        m_initialY = m_currentY;

        if (!isEmpty(m_currentX, m_currentY))
        {
            throw std::domain_error("Failed to set to empty position in BlockWorldEnvironment::setToRandomEmptyPosition()");
        }
    }

    BlockWorldEnvironment::BlockWorldEnvironment(const std::string & mapFilename, std::size_t maxStep, bool threeBitMode, bool allowsDiagonalAction)
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

    char BlockWorldEnvironment::getBlockChar(int x, int y) const
    {
        x = (x + m_worldWidth) % m_worldWidth;
        y = (y + m_worldHeight) % m_worldHeight;
        return m_worldMap[y][x];
    }

    bool BlockWorldEnvironment::isEmpty(int x, int y) const
    {
        switch (getBlockChar(x, y))
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

    bool BlockWorldEnvironment::isFood(int x, int y) const
    {
        switch (getBlockChar(x, y))
        {
        case 'F':
        case 'G':
            return true;
        default:
            return false;
        }
    }

    bool BlockWorldEnvironment::isObstacle(int x, int y) const
    {
        switch (getBlockChar(x, y))
        {
        case 'T':
        case 'O':
        case 'Q':
            return true;
        default:
            return false;
        }
    }

    std::vector<int> BlockWorldEnvironment::situation(int x, int y) const
    {
        std::vector<int> situation;
        for (int i = 0; i < kDirectionValueCount; ++i)
        {
            const auto block = getBlockChar(x + XDiff(i), y + YDiff(i));
            for (const auto & bit : CharToBits(block, m_threeBitMode))
            {
                situation.push_back(bit);
            }
        }
        return situation;
    }

    double BlockWorldEnvironment::executeAction(int action)
    {
        if (action < 0 || kDirectionValueCount <= action)
        {
            throw std::invalid_argument("BlockWorldEnvironment::executeAction() received an invalid action out of the range 0-7.");
        }

        m_lastInitialX = m_initialX;
        m_lastInitialY = m_initialY;

        // The coordinates after performing the action
        const int x = (m_currentX + XDiff(action) + m_worldWidth) % m_worldWidth;
        const int y = (m_currentY + YDiff(action) + m_worldHeight) % m_worldHeight;

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

    std::string BlockWorldEnvironment::toString() const
    {
        std::string str;
        for (int y = 0; y < m_worldHeight; ++y)
        {
            for (int x = 0; x < m_worldWidth; ++x)
            {
                str += (x == m_currentX && y == m_currentY) ? '*' : getBlockChar(x, y);
            }
            str += '\n';
        }
        return str;
    }

}
