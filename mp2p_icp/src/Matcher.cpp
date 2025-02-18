/* -------------------------------------------------------------------------
 *  A repertory of multi primitive-to-primitive (MP2P) ICP algorithms in C++
 * Copyright (C) 2018-2021 Jose Luis Blanco, University of Almeria
 * See LICENSE for license information.
 * ------------------------------------------------------------------------- */
/**
 * @file   Matcher.cpp
 * @brief  Pointcloud matching generic base class
 * @author Jose Luis Blanco Claraco
 * @date   June 22, 2020
 */

#include <mp2p_icp/Matcher.h>
#include <mrpt/core/exceptions.h>

IMPLEMENTS_VIRTUAL_MRPT_OBJECT(Matcher, mrpt::rtti::CObject, mp2p_icp)

using namespace mp2p_icp;

void Matcher::initialize(const mrpt::containers::yaml& params)
{
    runFromIteration = params.getOrDefault<uint32_t>("runFromIteration", 0);
    runUpToIteration = params.getOrDefault<uint32_t>("runUpToIteration", 0);
}

void Matcher::match(
    const metric_map_t& pcGlobal, const metric_map_t& pcLocal,
    const mrpt::poses::CPose3D& localPose, const MatchContext& mc,
    MatchState& ms, Pairings& out) const
{
    if (mc.icpIteration < runFromIteration) return;
    if (runUpToIteration > 0 && mc.icpIteration > runUpToIteration) return;
    impl_match(pcGlobal, pcLocal, localPose, mc, ms, out);
}

Pairings mp2p_icp::run_matchers(
    const matcher_list_t& matchers, const metric_map_t& pcGlobal,
    const metric_map_t& pcLocal, const mrpt::poses::CPose3D& local_wrt_global,
    const MatchContext&                   mc,
    const mrpt::optional_ref<MatchState>& userProvidedMS)
{
    Pairings pairings;

    MatchState* ms = nullptr;

    std::optional<MatchState> localMS;

    if (userProvidedMS.has_value())
    {
        // Use user-provided memory storage:
        ms = &userProvidedMS.value().get();
    }
    else
    {
        // Reserve here:
        localMS.emplace(pcGlobal, pcLocal);
        ms = &localMS.value();
    }

    for (const auto& matcher : matchers)
    {
        ASSERT_(matcher);
        Pairings pc;
        matcher->match(pcGlobal, pcLocal, local_wrt_global, mc, *ms, pc);
        pairings.push_back(pc);
    }
    return pairings;
}
