#pragma once

/// \brief Concatenate two tokens.
#define COMMON_CAT_(x, y) x##y

/// \brief Concatenate two tokens (indirect).
#define COMMON_CAT(x, y) COMMON_CAT_(x, y)
