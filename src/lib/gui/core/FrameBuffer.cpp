#include "core/FrameBuffer.hpp"

void FrameBuffer::pushFrame(Frame frame)
{
    QMutexLocker lock(&mutex_);
    latestFrame_ = std::move(frame);
}

std::optional<Frame> FrameBuffer::takeLatest()
{
    QMutexLocker lock(&mutex_);
    if (!latestFrame_)
        return std::nullopt;

    auto frame = std::move(latestFrame_);
    latestFrame_.reset();

    return frame;
}
