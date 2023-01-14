#pragma once

#include <string>

struct SoundComponent {
    std::string assetID;
    int loopCount;

    explicit SoundComponent(std::string assetID = "",
                            int loopCount = 0) :
            assetID(std::move(assetID)),
            loopCount(loopCount) {}

};
