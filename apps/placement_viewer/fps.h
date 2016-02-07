#ifndef FPS_HPP
#define FPS_HPP

#include <SFML/Graphics.hpp>

class FPS
{
public:
    /// @brief Constructor with initialization.
    ///
    FPS() : mFrame(0), mFps(0) {}

    /// @brief Update the frame count.
    ///
    void update();

    /// @brief Get the current FPS count.
    /// @return FPS count.
    const unsigned int getFPS() const { return mFps; }

private:
    unsigned int mFrame;
    unsigned int mFps;
    sf::Clock mClock;
};

#endif // FPS_HPP

