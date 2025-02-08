#ifndef LOGGING_HPP
#define LOGGING_HPP

/**
 * Here we configure logging and expose the initLogging function that can also be called from the test executable
 */

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <iomanip>
#include <ostream>

/**
 * @brief Formats the message in the format TIME - source:line LEVEL <message>
 */
void MyPrefixFormatter(std::ostream &s, const google::LogMessage &m, void * /*data*/)
{
    using std::setw, std::setfill;
    s << setw(2) << m.time().hour() << ':' << setw(2) << m.time().min() << ':' << setw(2) << m.time().sec() << ","
      << setw(3) << static_cast<int>(m.time().usec() / 1000.0) << " - " << m.basename() << ':' << m.line() << " "
      << setfill(' ') << setw(7) << google::GetLogSeverityName(m.severity());
}

/**
 * @brief Installs the above PrefixFormatter, lets gflags parse command line options (like --v=N) and configures the
 * logging
 */
void initLogging(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    FLAGS_alsologtostderr = true;        // Log both to file and stderr
    FLAGS_logbuflevel = -1;              // No buffering, write immediately
    FLAGS_minloglevel = 0;               // Don't suppress any log messages in general
    FLAGS_stderrthreshold = 0;           // Don't suppress any log messages for stderr output
    FLAGS_timestamp_in_logfile_name = 0; // We just want a single logfile at the moment
    FLAGS_colorlogtostderr = 1;          // Looks pretty, why not

    google::InstallPrefixFormatter(&MyPrefixFormatter);
}


#endif /* LOGGING_HPP */
