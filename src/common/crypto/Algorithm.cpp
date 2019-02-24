/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
 * Copyright 2018      SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2019 XMRig       <https://github.com/girmx>, <support@girmx.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "common/crypto/Algorithm.h"


#ifdef _MSC_VER
#   define strncasecmp _strnicmp
#   define strcasecmp  _stricmp
#endif


#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


struct AlgoData
{
    const char *name;
    const char *shortName;
    girmx::Algo algo;
    girmx::Variant variant;
};


static AlgoData const algorithms[] = {
    { "cryptonight",           "cn",           girmx::CRYPTONIGHT,       girmx::VARIANT_AUTO },
    { "cryptonight/0",         "cn/0",         girmx::CRYPTONIGHT,       girmx::VARIANT_0    },
    { "cryptonight/1",         "cn/1",         girmx::CRYPTONIGHT,       girmx::VARIANT_1    },
    { "cryptonight/xtl",       "cn/xtl",       girmx::CRYPTONIGHT,       girmx::VARIANT_XTL  },
    { "cryptonight/msr",       "cn/msr",       girmx::CRYPTONIGHT,       girmx::VARIANT_MSR  },
    { "cryptonight/xao",       "cn/xao",       girmx::CRYPTONIGHT,       girmx::VARIANT_XAO  },
    { "cryptonight/rto",       "cn/rto",       girmx::CRYPTONIGHT,       girmx::VARIANT_RTO  },
    { "cryptonight/2",         "cn/2",         girmx::CRYPTONIGHT,       girmx::VARIANT_2    },
    { "cryptonight/half",      "cn/half",      girmx::CRYPTONIGHT,       girmx::VARIANT_HALF },
    { "cryptonight/xtlv9",     "cn/xtlv9",     girmx::CRYPTONIGHT,       girmx::VARIANT_HALF },

#   ifndef XMRIG_NO_AEON
    { "cryptonight-lite",      "cn-lite",      girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_AUTO },
    { "cryptonight-light",     "cn-light",     girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_AUTO },
    { "cryptonight-lite/0",    "cn-lite/0",    girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_0    },
    { "cryptonight-lite/1",    "cn-lite/1",    girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_1    },
#   endif

#   ifndef XMRIG_NO_SUMO
    { "cryptonight-heavy",      "cn-heavy",      girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_AUTO },
    { "cryptonight-heavy/0",    "cn-heavy/0",    girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_0    },
    { "cryptonight-heavy/xhv",  "cn-heavy/xhv",  girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_XHV  },
    { "cryptonight-heavy/tube", "cn-heavy/tube", girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_TUBE },
#   endif
};


#ifdef XMRIG_PROXY_PROJECT
static AlgoData const xmrStakAlgorithms[] = {
    { "cryptonight-monerov7",    nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_1    },
    { "cryptonight_v7",          nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_1    },
    { "cryptonight-monerov8",    nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_2    },
    { "cryptonight_v8",          nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_2    },
    { "cryptonight_v7_stellite", nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_XTL  },
    { "cryptonight_lite",        nullptr, girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_0    },
    { "cryptonight-aeonv7",      nullptr, girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_1    },
    { "cryptonight_lite_v7",     nullptr, girmx::CRYPTONIGHT_LITE,  girmx::VARIANT_1    },
    { "cryptonight_heavy",       nullptr, girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_0    },
    { "cryptonight_haven",       nullptr, girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_XHV  },
    { "cryptonight_masari",      nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_MSR  },
    { "cryptonight_masari",      nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_MSR  },
    { "cryptonight-bittube2",    nullptr, girmx::CRYPTONIGHT_HEAVY, girmx::VARIANT_TUBE }, // bittube-miner
    { "cryptonight_alloy",       nullptr, girmx::CRYPTONIGHT,       girmx::VARIANT_XAO  }, // xmr-stak-alloy
};
#endif


static const char *variants[] = {
    "0",
    "1",
    "tube",
    "xtl",
    "msr",
    "xhv",
    "xao",
    "rto",
    "2",
    "half"
};


static_assert(girmx::VARIANT_MAX == ARRAY_SIZE(variants), "variants size mismatch");


bool girmx::Algorithm::isValid() const
{
    if (m_algo == INVALID_ALGO) {
        return false;
    }

    for (size_t i = 0; i < ARRAY_SIZE(algorithms); i++) {
        if (algorithms[i].algo == m_algo && algorithms[i].variant == m_variant) {
            return true;
        }
    }

    return false;
}


const char *girmx::Algorithm::variantName() const
{
    if (m_variant == VARIANT_AUTO) {
        return "auto";
    }

    return variants[m_variant];
}


void girmx::Algorithm::parseAlgorithm(const char *algo)
{
    m_algo    = INVALID_ALGO;
    m_variant = VARIANT_AUTO;

    assert(algo != nullptr);
    if (algo == nullptr || strlen(algo) < 1) {
        return;
    }

    if (*algo == '!') {
        m_flags |= Forced;

        return parseAlgorithm(algo + 1);
    }

    for (size_t i = 0; i < ARRAY_SIZE(algorithms); i++) {
        if ((strcasecmp(algo, algorithms[i].name) == 0) || (strcasecmp(algo, algorithms[i].shortName) == 0)) {
            m_algo    = algorithms[i].algo;
            m_variant = algorithms[i].variant;
            break;
        }
    }

    if (m_algo == INVALID_ALGO) {
        assert(false);
    }
}


void girmx::Algorithm::parseVariant(const char *variant)
{
    m_variant = VARIANT_AUTO;

    if (variant == nullptr || strlen(variant) < 1) {
        return;
    }

    if (*variant == '!') {
        m_flags |= Forced;

        return parseVariant(variant + 1);
    }

    for (size_t i = 0; i < ARRAY_SIZE(variants); i++) {
        if (strcasecmp(variant, variants[i]) == 0) {
            m_variant = static_cast<Variant>(i);
            return;
        }
    }

    if (strcasecmp(variant, "xtlv9") == 0) {
        m_variant = VARIANT_HALF;
    }
}


void girmx::Algorithm::parseVariant(int variant)
{
    assert(variant >= -1 && variant <= 2);

    switch (variant) {
    case -1:
    case 0:
    case 1:
        m_variant = static_cast<Variant>(variant);
        break;

    case 2:
        m_variant = VARIANT_2;
        break;

    default:
        break;
    }
}


void girmx::Algorithm::setAlgo(Algo algo)
{
    m_algo = algo;
}


#ifdef XMRIG_PROXY_PROJECT
void girmx::Algorithm::parseXmrStakAlgorithm(const char *algo)
{
    m_algo    = INVALID_ALGO;
    m_variant = VARIANT_AUTO;

    assert(algo != nullptr);
    if (algo == nullptr) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(xmrStakAlgorithms); i++) {
        if (strcasecmp(algo, xmrStakAlgorithms[i].name) == 0) {
            m_algo    = xmrStakAlgorithms[i].algo;
            m_variant = xmrStakAlgorithms[i].variant;
            break;
        }
    }

    if (m_algo == INVALID_ALGO) {
        assert(false);
    }
}
#endif


const char *girmx::Algorithm::name(bool shortName) const
{
    for (size_t i = 0; i < ARRAY_SIZE(algorithms); i++) {
        if (algorithms[i].algo == m_algo && algorithms[i].variant == m_variant) {
            return shortName ? algorithms[i].shortName : algorithms[i].name;
        }
    }

    return "invalid";
}
