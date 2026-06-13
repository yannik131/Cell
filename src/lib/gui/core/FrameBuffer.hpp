#ifndef D8F4E8FD_F1C9_47D8_B57A_6AC979ABC03A_HPP
#define D8F4E8FD_F1C9_47D8_B57A_6AC979ABC03A_HPP

#include "cell/SimulationRecorder.hpp"

#include <QMutex>

#include <optional>

using Frame = cell::SimulationRecorder::Frame;

class FrameBuffer : public QObject
{
    Q_OBJECT
public:
    void pushFrame(Frame frame);
    std::optional<Frame> takeLatest();

private:
    QMutex mutex_;
    std::optional<Frame> latestFrame_;
};

#endif /* D8F4E8FD_F1C9_47D8_B57A_6AC979ABC03A_HPP */
