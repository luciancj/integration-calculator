#ifndef CLI_CONTENT_INTEGRATION_STRINGS_H
#define CLI_CONTENT_INTEGRATION_STRINGS_H

#include "../utils/console.h"

namespace cli::content {

using cli::utils::TextBlock;

extern const TextBlock printIntegrationBanner;
extern const TextBlock bannerRightRiemannSum;
extern const TextBlock bannerLeftRiemannSum;
extern const TextBlock bannerMidpointRiemannSum;
extern const TextBlock bannerTrapezoidalRule;
extern const TextBlock bannerSimpsonsRule;
extern const TextBlock printGoodbyeBanner;

} // namespace cli::content

#endif
