//=============================================================================
//File Name: FilterBase.cpp
//Description: Provides an interface for filter classes
//Author: Tyler Veness
//=============================================================================

#include "FilterBase.hpp"

FilterBase::FilterBase() {
    reset();
}

FilterBase::~FilterBase() {

}

void FilterBase::reset() {
    m_stateEstimate = 0.0;
    m_dt = 0.0;
    m_time.restart();
}

double FilterBase::getEstimate() {
    return m_stateEstimate;
}
