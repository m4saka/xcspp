#include "xcspp/helper/experiment_log_stream.hpp"
#include <iostream>

namespace xcspp
{

    ExperimentLogStream::ExperimentLogStream(const std::string & filename, bool useStdoutWhenEmpty)
        : m_os(
            filename.empty()
                ? (useStdoutWhenEmpty ? std::cout : m_ofs)
                : m_ofs)
    {
        m_ofs.open(filename);
    }

    void ExperimentLogStream::write(const std::string & str)
    {
        if (m_os)
        {
            m_os << str << std::flush;
        }
    }

    void ExperimentLogStream::writeLine(const std::string & str)
    {
        if (m_os)
        {
            m_os << str << std::endl;
        }
    }

    void ExperimentLogStream::write(double value)
    {
        if (m_os)
        {
            m_os << value << std::flush;
        }
    }

    void ExperimentLogStream::writeLine(double value)
    {
        if (m_os)
        {
            m_os << value << std::endl;
        }
    }

    SMAExperimentLogStream::SMAExperimentLogStream(const std::string & filename, std::size_t smaWidth, bool useStdoutWhenEmpty)
        : ExperimentLogStream(filename, useStdoutWhenEmpty)
        , m_sma(smaWidth)
        , m_count(0)
    {
    }

    void SMAExperimentLogStream::write(double value)
    {
        if (m_os)
        {
            double smaValue = m_sma(value);
            if (++m_count >= m_sma.order())
            {
                m_os << smaValue << std::flush;
            }
        }
    }

    void SMAExperimentLogStream::writeLine(double value)
    {
        if (m_os)
        {
            double smaValue = m_sma(value);
            if (++m_count >= m_sma.order())
            {
                m_os << smaValue << std::endl;
            }
        }
    }

}
