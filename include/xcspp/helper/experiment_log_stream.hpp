#pragma once
#include <iosfwd> // std::ostream
#include <fstream>
#include <string>
#include <cstddef>
#include "simple_moving_average.hpp"

namespace xcspp
{

    class ExperimentLogStream
    {
    private:
        std::ofstream m_ofs;

    protected:
        std::ostream & m_os;

    public:
        explicit ExperimentLogStream(const std::string & filename = "", bool useStdoutWhenEmpty = true);

        virtual ~ExperimentLogStream() = default;

        void write(const std::string & str);

        void writeLine(const std::string & str);

        virtual void write(double value);

        virtual void writeLine(double value);
    };

    class SMAExperimentLogStream : public ExperimentLogStream
    {
    private:
        SimpleMovingAverage<double> m_sma;
        std::size_t m_count;

    public:
        explicit SMAExperimentLogStream(const std::string & filename = "", std::size_t smaWidth = 1, bool useStdoutWhenEmpty = true);

        virtual void write(double value) override;

        virtual void writeLine(double value) override;
    };

}
