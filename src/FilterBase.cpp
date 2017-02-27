// Copyright (c) Tyler Veness 2014-2017. All Rights Reserved.

#include "FilterBase.hpp"

using namespace std::chrono_literals;

FilterBase::FilterBase() { reset(); }

void FilterBase::reset() {
    m_stateEstimate = 0.0;
    m_dt = 0.0s;
    m_lastTime = std::chrono::system_clock::now();
}

double FilterBase::getEstimate() { return m_stateEstimate; }
