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
      std::vector<unsigned> X;
      std::vector<unsigned> Y;
      enum PointComponent
      {
        kX = 0,
        kY
      };
      auto extractComponent = [](const T& container, std::vector<unsigned>& result,
           const PointComponent component) -> const std::vector<unsigned>& {
        result.reserve(container.size());
        double minElement = std::numeric_limits<double>::max();
        std::for_each(container.begin(), container.end(), [&minElement, component](auto & el){
            const double * point = reinterpret_cast<const double*>(&el);
            minElement = std::min(minElement, point[component]);
        });
        double offset = 0.0;
        if (minElement < 0.0) {
          offset = -minElement;
        }
        std::for_each(container.begin(), container.end(), [&result, offset, component](auto & el){
            const double * point = reinterpret_cast<const double*>(&el);
            result.push_back(point[component] + offset);
        });
        return result;
      };
      auto futureX = std::async(std::launch::async, extractComponent, container, std::ref(X), PointComponent::kX);
      auto futureY = std::async(std::launch::async, extractComponent, container, std::ref(Y), PointComponent::kY);
      auto tree = callFlute(futureX.get(), futureY.get());
      return std::move(tree);
  }

private:
  std::unique_ptr<SteinerTree> singleSegment(const geometry::Point& p1,
                                             const geometry::Point& p2);
  std::unique_ptr<SteinerTree> trivialSteinerTree(const geometry::Point& p);
  std::unique_ptr<SteinerTree> callFlute(const std::vector<unsigned>& X,
                                         const std::vector<unsigned>& Y);
  Flute();
  Flute(const Flute& o) = delete;
  Flute& operator=(const Flute& o) = delete;
  struct Impl;
  const std::unique_ptr<Impl> this_;
};
}
}

#endif // FLUTE_H
