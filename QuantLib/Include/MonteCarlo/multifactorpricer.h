/*
 * Copyright (C) 2001
 * Ferdinando Ametrano, Luigi Ballabio, Adolfo Benin, Marco Marchioro
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, contact ferdinando@ametrano.net
 *
 * QuantLib license is also available at 
 * http://quantlib.sourceforge.net/LICENSE.TXT
*/

/*! \file multifactorpricer.h

    $Source$
    $Name$
    $Log$
    Revision 1.1  2001/02/02 10:51:31  marmar
    Base class for multi-factor pricers

*/

#ifndef quantlib_montecarlo_multi_factor_pricer_h
#define quantlib_montecarlo_multi_factor_pricer_h

#include "qldefines.h"
#include "multifactormontecarlooption.h"

namespace QuantLib {

    namespace Pricers {
        //! Base class for multi-factor Monte Carlo pricers
        /*! MultiFactorPricer is the base class for the Monte Carlo pricers 
            depending from more than one factor. Eventually it might be linked 
            to the general tree of pricers, in order to have available tools 
            like impliedVolaitlity. Also, it will, eventually, implement the 
            calculation of greeks in montecarlo methods.
            Deriving a class from MultiFactorPricer gives an easy way to write
            a multi-factor Monte Carlo Pricer.
            See PlainBasketOption for an example
        */

        class MultiFactorPricer {
        public:
            MultiFactorPricer() : isInitialized_(false){}
            MultiFactorPricer(long samples, long seed=0);
            ~MultiFactorPricer(){}
            virtual double value() const;
            virtual double errorEstimate() const;
        protected:
            bool isInitialized_;
            long seed_;
            mutable long samples_;
            mutable MonteCarlo::MultiFactorMonteCarloOption montecarloPricer_;
        };

        inline MultiFactorPricer::MultiFactorPricer(long samples, long seed):
                    samples_(samples), seed_(seed), isInitialized_(true){}

        inline double MultiFactorPricer::value() const{
            QL_REQUIRE(isInitialized_,
                "MultiFactorPricer::value has not been initialized");
            return montecarloPricer_.sampleAccumulator(samples_).mean();
        }

        inline double MultiFactorPricer::errorEstimate() const {
            QL_REQUIRE(isInitialized_,
                "MultiFactorPricer::errorEstimate has not been initialized");
            return montecarloPricer_.sampleAccumulator().errorEstimate();
        }

    }

}

#endif
