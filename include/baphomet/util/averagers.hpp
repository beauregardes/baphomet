#pragma once

#include <queue>

namespace baphomet {

class Averager {
public:
  virtual void update(double v) = 0;
  double value() const;

protected:
  double value_ {0.0};
};

class CMA : public Averager {
public:
  CMA() = default;

  void update(double v);

private:
  unsigned int sample_count_ {0};
};

class EMA : public Averager  {
public:
  double alpha {0.0};

  EMA(double alpha);

  void update(double v);
};

class SMA : public Averager {
public:
  std::size_t sample_count {0};

  SMA(std::size_t sample_count);

  void update(double v);

private:
  std::queue<double> samples_ {};
};

} // namespace baphomet
