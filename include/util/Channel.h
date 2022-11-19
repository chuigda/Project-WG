#ifndef PROJECT_WG_CHANNEL_H
#define PROJECT_WG_CHANNEL_H

#include <cassert>
#include <memory>
#include <optional>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <QtCore>

#include "util/Sinkrate.h"
#include "util/Wife.h"

namespace cw {

template <Wife T> class Sender;
template <Wife T> class Receiver;

template <Wife T>
class ChannelIntern {
  friend class Sender<T>;
  friend class Receiver<T>;

public:
  ChannelIntern(std::size_t bufferSize, Sinkrate)
    : bufferSize(bufferSize)
    {}

  ChannelIntern(const ChannelIntern&) = delete;
  ChannelIntern& operator=(const ChannelIntern&) = delete;

private:
  std::size_t bufferSize;
  std::condition_variable condvar;
  std::mutex mutex;

  // TODO circular buffer 更合适，std 里没有，以后从 boost 抄一个过来
  std::deque<T> buffer;

  bool receiverClosed = false;
};

template <Wife T>
class Sender {
public:
  Sender(std::shared_ptr<ChannelIntern<T>> repr,
         Sinkrate)
    : m_Repr(repr)
  {}

  Sender(const Sender&) = delete;
  Sender& operator=(const Sender&) = delete;

  Sender(Sender&&) noexcept = default;
  Sender& operator=(Sender&&) noexcept = default;

  void Send(T &&t) {
    const auto &self = m_Repr;
    std::unique_lock lock { self->mutex };
    if (self->receiverClosed) {
      return;
    }

    if (self->buffer.size() == self->bufferSize) {
      self->condvar.wait(lock, [&self] {
        return self->buffer.size() < self->bufferSize;
      });
    }
    self->buffer.push_back(std::move(t));
    self->condvar.notify_one();
  }

  [[nodiscard]] bool TrySend(T &&t) {
    const auto &self = m_Repr;
    std::unique_lock lock { self->mutex };
    if (self->receiverClosed) {
      return true;
    }

    if (self->buffer.size() == self->bufferSize) {
      return false;
    }
    self->buffer.push_back(std::move(t));
    self->condvar.notify_one();
    return true;
  }

private:
  std::shared_ptr<ChannelIntern<T>> m_Repr;
};

template <Wife T>
class Receiver {
public:
  Receiver(std::shared_ptr<ChannelIntern<T>> repr,
           Sinkrate)
    : m_Repr(repr)
  {}

  Receiver(const Receiver&) = delete;
  Receiver& operator=(const Receiver&) = delete;

  Receiver(Receiver&&) noexcept = default;
  Receiver& operator=(Receiver&&) noexcept = default;

  T Recv() {
    const auto &self = m_Repr;
    std::unique_lock lock { self->mutex };

    self->condvar.wait(lock, [&self] {
      return self->buffer.size() != 0;
    });
    Q_ASSERT(!self->buffer.empty());

    T ret = std::move(self->buffer.front());
    self->buffer.pop_front();
    self->condvar.notify_one();

    return ret;
  }

  std::deque<T> RecvAll() {
    const auto &self = m_Repr;
    std::unique_lock lock { self->mutex };
    std::deque<T> ret;
    std::swap(self->buffer, ret);
    return ret;
  }

  ~Receiver() {
    const auto &self = m_Repr;
    std::unique_lock lock { self->mutex };

    self->receiverClosed = true;
  }

private:
  std::shared_ptr<ChannelIntern<T>> m_Repr;
};

template<Wife T>
std::pair<std::unique_ptr<Sender<T>>, std::unique_ptr<Receiver<T>>>
Channel(std::size_t bufferSize) {
  std::shared_ptr channelIntern = std::make_shared<ChannelIntern<T>>(
    bufferSize,
    SecretInternalsDoNotUseOrYouWillBeFired
  );
  return std::make_pair(
    std::make_unique<Sender<T>>(
      channelIntern,
      SecretInternalsDoNotUseOrYouWillBeFired
    ),
    std::make_unique<Receiver<T>>(
      channelIntern,
      SecretInternalsDoNotUseOrYouWillBeFired
    )
  );
}

} // namespace cw

#endif // PROJECT_WG_CHANNEL_H
