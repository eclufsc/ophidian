#ifndef FLUTE_H
#define FLUTE_H

#include "SteinerTree.h"
#include <future>
#include <memory>

namespace ophidian {
namespace interconnection {

class Flute final
{
public:
  static Flute& instance();
  ~Flute();

  template <class T>
  std::unique_ptr<SteinerTree> create(const T& container)
  {
    if (container.size() == 1) {
      return trivialSteinerTree(*std::begin(container));
    } else if (container.size() == 2) {
      typename T::const_iterator it(container.begin());
      typename T::const_iterator it2(it);
      ++it2;
      return singleSegment(*it, *it2);
   }
      std::vector<unsigned> X(container.size());
      std::vector<unsigned> Y(container.size());
      X.resize(0);
      Y.resize(0);

      geometry::Point offset(0.0, 0.0);

      for(const auto & point : container)
      {
          offset.x(std::min(offset.x(), point.x()));
          offset.y(std::min(offset.y(), point.y()));
      }

      offset.x(-offset.x());
      offset.y(-offset.y());

      for(const auto & point : container)
      {
          X.push_back(static_cast<unsigned>(std::round(point.x() + offset.x())));
          Y.push_back(static_cast<unsigned>(std::round(point.y() + offset.y())));
      }

      return callFlute(X, Y, offset);
  }

private:
  std::unique_ptr<SteinerTree> singleSegment(const geometry::Point& p1, const geometry::Point& p2);
  std::unique_ptr<SteinerTree> trivialSteinerTree(const geometry::Point& p);
  std::unique_ptr<SteinerTree> callFlute(const std::vector<unsigned>& X, const std::vector<unsigned>& Y, const geometry::Point &offset);
  Flute();
  Flute(const Flute& o) = delete;
  Flute& operator=(const Flute& o) = delete;
  struct Impl;
  const std::unique_ptr<Impl> this_;
};
}
}

#endif // FLUTE_H
