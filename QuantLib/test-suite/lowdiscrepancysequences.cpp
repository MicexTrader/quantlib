
/*
 Copyright (C) 2003 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

// $Id$

#include "lowdiscrepancysequences.hpp"
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <fstream>

using namespace QuantLib;
using namespace QuantLib::Math;
using namespace QuantLib::RandomNumbers;
using namespace QuantLib::Math;

CppUnit::Test* LDSTest::suite() {
    CppUnit::TestSuite* tests =
        new CppUnit::TestSuite("Low discrepancy sequences' tests");

    tests->addTest(new CppUnit::TestCaller<LDSTest>
        ("Testing " + IntegerFormatter::toString(PPMT_MAX_DIM) +
         " primitive polynomials modulo two",
                    &LDSTest::testPolynomialsModuloTwo));
    tests->addTest(new CppUnit::TestCaller<LDSTest>
                   ("Testing Sobol sequences up to dimension " +
                    IntegerFormatter::toString(PPMT_MAX_DIM),
                    &LDSTest::testSobol));
    tests->addTest(new CppUnit::TestCaller<LDSTest>
                   ("Testing Halton sequences",
                    &LDSTest::testHalton));
    tests->addTest(new CppUnit::TestCaller<LDSTest>
                   ("Testing Halton and Sobol discrepancy",
                    &LDSTest::testDiscrepancy));
    return tests;
}


void LDSTest::testPolynomialsModuloTwo() {

    static const unsigned long jj[] = {
                 1,       1,       2,       2,       6,       6,      18,
                16,      48,      60,     176,     144,     630,     756,
              1800,    2048,    7710,    7776,   27594,   24000,   84672,
            120032,  356960,  276480, 1296000, 1719900, 4202496
    };

    unsigned long i=0,j=0,n=0;
    long polynomial=0;
    while (n<PPMT_MAX_DIM || polynomial!=-1) {
        if (polynomial==-1) {
            ++i; // Increase degree index
            j=0; // Reset index of polynomial in degree.
        }
        polynomial = PrimitivePolynomials[i][j];
        if (polynomial==-1) {
            --n;
            if (j!=jj[i]) {
                CPPUNIT_FAIL("Only " + IntegerFormatter::toString(j)
                    + " polynomials in degree "
                    + IntegerFormatter::toString(i+1) + " instead of "
                    + IntegerFormatter::toString(jj[i]));
            }
        }
        ++j; // Increase index of polynomial in degree i+1
        ++n; // Increase overall polynomial counter
    }

}


void LDSTest::testSobol() {

    Array point;

    // testing max dimensionality
    Size dimensionality = PPMT_MAX_DIM;
    unsigned long seed = 123456;
    SobolRsg rsg(dimensionality, seed);
    Size points = 100, i;
    for (i=0; i<points; i++) {
        point = rsg.nextSequence().value;
        if (point.size()!=dimensionality) {
            CPPUNIT_FAIL("Sobol sequence generator returns "
                         " a sequence of wrong dimensionality: " +
                         IntegerFormatter::toString(point.size())
                         + " instead of  " +
                         IntegerFormatter::toString(dimensionality));
        }
    }

    // testing homogeneity properties
    dimensionality = 33;
    seed = 123456;
    rsg = SobolRsg(dimensionality, seed);
    SequenceStatistics<> stat(dimensionality);
    std::vector<double> mean, stdev, variance, skewness, kurtosis;
    Size k = 0;
    for (int j=1; j<5; j++) { // five cycle
//        std::cout << j << ", " ;
        points = Size(QL_POW(2.0, j))-1; // base 2
        for (; k<points; k++) {
            point = rsg.nextSequence().value;
            stat.add(point);
        }
        mean = stat.mean();
        for (i=0; i<dimensionality; i++) {
            if (mean[i] != 0.5) {
                CPPUNIT_FAIL(IntegerFormatter::toOrdinal(i+1) +
                             " dimension mean (" +
                             DoubleFormatter::toString(mean[i]) +
                             ") at the end of the " +
                             IntegerFormatter::toOrdinal(j+1) +
                             " cycle in Sobol sequence is not " +
                             DoubleFormatter::toString(0.5));
            }
        }
    }


    // testing first dimension (van der Corput sequence)
    const double vanderCorputSequenceModuloTwo[] = {
        // first cycle (zero excluded)
        0.50000,
        // second cycle
        0.75000, 0.25000,
        // third cycle
        0.37500, 0.87500, 0.62500, 0.12500,
        // fourth cycle
        0.18750, 0.68750, 0.93750, 0.43750, 0.31250, 0.81250, 0.56250, 0.06250,
        // fifth cycle
        0.09375, 0.59375, 0.84375, 0.34375, 0.46875, 0.96875, 0.71875, 0.21875,
        0.15625, 0.65625, 0.90625, 0.40625, 0.28125, 0.78125, 0.53125, 0.03125
    };

    dimensionality = 1;
    rsg = SobolRsg(dimensionality);
    points = Size(QL_POW(2.0, 5))-1; // five cycles
    for (i=0; i<points; i++) {
        point = rsg .nextSequence().value;
        if (point[0]!=vanderCorputSequenceModuloTwo[i]) {
            CPPUNIT_FAIL(IntegerFormatter::toOrdinal(i+1) +
                         " draw (" +
                         DoubleFormatter::toString(point[0]) +
                         ") in 1-D Sobol sequence is not in the "
                         "van der Corput sequence modulo two: " +
                         "it should have been " +
                         DoubleFormatter::toString(
                             vanderCorputSequenceModuloTwo[i]));
        }
    }
}


void LDSTest::testHalton() {

    Array point;
    // testing "high" dimensionality
    Size dimensionality = PPMT_MAX_DIM;
    HaltonRsg rsg(dimensionality);
    Size points = 100, i, k;
    for (i=0; i<points; i++) {
        point = rsg.nextSequence().value;
        if (point.size()!=dimensionality) {
            CPPUNIT_FAIL("Halton sequence generator returns "
                         " a sequence of wrong dimensionality: " +
                         IntegerFormatter::toString(point.size())
                         + " instead of  " +
                         IntegerFormatter::toString(dimensionality));
        }
    }


    // testing first and second dimension (van der Corput sequence)
    const double vanderCorputSequenceModuloTwo[] = {
        // first cycle (zero excluded)
        0.50000,
        // second cycle
        0.25000, 0.75000,
        // third cycle
        0.12500, 0.62500, 0.37500, 0.87500,
        // fourth cycle
        0.06250, 0.56250, 0.31250, 0.81250, 0.18750, 0.68750, 0.43750, 0.93750,
        // fifth cycle
        0.03125, 0.53125, 0.28125, 0.78125, 0.15625, 0.65625, 0.40625, 0.90625,
        0.09375, 0.59375, 0.34375, 0.84375, 0.21875, 0.71875, 0.46875, 0.96875,
    };


    dimensionality = 1;
    rsg = HaltonRsg(dimensionality);
    points = Size(QL_POW(2.0, 5))-1;  // five cycles
    for (i=0; i<points; i++) {
        point = rsg.nextSequence().value;
        if (point[0]!=vanderCorputSequenceModuloTwo[i]) {
            CPPUNIT_FAIL(IntegerFormatter::toOrdinal(i+1) +
                         " draw (" +
                         DoubleFormatter::toString(point[0]) +
                         ") in 1-D Halton sequence is not in the "
                         "van der Corput sequence modulo two: " +
                         "it should have been " +
                         DoubleFormatter::toString(
                             vanderCorputSequenceModuloTwo[i]));
        }
    }

    static const double vanderCorputSequenceModuloThree[] = {
        // first cycle (zero excluded)
        1.0/3,  2.0/3,
        // second cycle
        1.0/9,  4.0/9,  7.0/9,  2.0/9,  5.0/9,  8.0/9,
        // third cycle
        1.0/27, 10.0/27, 19.0/27,  4.0/27, 13.0/27, 22.0/27,
        7.0/27, 16.0/27, 25.0/27,  2.0/27, 11.0/27, 20.0/27,
        5.0/27, 14.0/27, 23.0/27,  8.0/27, 17.0/27, 26.0/27
    };

    dimensionality = 2;
    rsg = HaltonRsg(dimensionality);
    points = Size(QL_POW(3.0, 3))-1;  // three cycles of the higher dimension
    for (i=0; i<points; i++) {
        point = rsg .nextSequence().value;
        if (point[0]!=vanderCorputSequenceModuloTwo[i]) {
            CPPUNIT_FAIL("First component of " +
                         IntegerFormatter::toOrdinal(i+1) +
                         " draw (" +
                         DoubleFormatter::toString(point[0]) +
                         ") in 2-D Halton sequence is not in the "
                         "van der Corput sequence modulo two: " +
                         "it should have been " +
                         DoubleFormatter::toString(
                             vanderCorputSequenceModuloTwo[i]));
        }
        if (QL_FABS(point[1]-vanderCorputSequenceModuloThree[i])>1.0e-15) {
            CPPUNIT_FAIL("Second component of " +
                         IntegerFormatter::toOrdinal(i+1) +
                         " draw (" +
                         DoubleFormatter::toString(point[1]) +
                         ") in 2-D Halton sequence is not in the "
                         "van der Corput sequence modulo three: "
                         "it should have been " +
                         DoubleFormatter::toString(
                             vanderCorputSequenceModuloThree[i]));
        }
    }




    // testing homogeneity properties
    dimensionality = 33;
    rsg = HaltonRsg(dimensionality);
    SequenceStatistics<> stat(dimensionality);
    std::vector<double> mean, stdev, variance, skewness, kurtosis;
    k = 0;
    int j;
    for (j=1; j<5; j++) { // five cycle
        points = Size(QL_POW(2.0, j))-1; // base 2
        for (; k<points; k++) {
            point = rsg.nextSequence().value;
            stat.add(point);
        }
        mean = stat.mean();
        if (mean[0] != 0.5) {
            CPPUNIT_FAIL("First dimension mean (" +
                         DoubleFormatter::toString(mean[0]) +
                         ") at the end of the " +
                         IntegerFormatter::toOrdinal(j+1) +
                         " cycle in Halton sequence is not " +
                         DoubleFormatter::toString(0.5));
        }
    }

    // reset generator and gaussianstatistics
    rsg  = HaltonRsg(dimensionality);
    stat.reset(dimensionality);
    k = 0;
    for (j=1; j<3; j++) { // three cycle
        points = Size(QL_POW(3.0, j))-1; // base 3
        for (; k<points; k++) {
            point = rsg.nextSequence().value;
            stat.add(point);
        }
        mean = stat.mean();
        if (QL_FABS(mean[1]-0.5)>1e-16) {
            CPPUNIT_FAIL("Second dimension mean (" +
                         DoubleFormatter::toString(mean[1]) +
                         ") at the end of the " +
                         IntegerFormatter::toOrdinal(j+1) +
                         " cycle in Halton sequence is not " +
                         DoubleFormatter::toString(0.5));
        }
    }

}

void LDSTest::testDiscrepancy() {

    const double discrRandDim2[]   = {1.17e-002, 8.24e-003, 5.82e-003, 4.12e-003, 2.91e-003, 2.06e-003, 1.46e-003};
    const double discrMersDim2[]   = {8.84e-003, 5.42e-003, 5.23e-003, 4.47e-003, 4.75e-003, 3.11e-003, 2.97e-003};
    const double discrHaltDim2[]   = {1.26e-003, 6.73e-004, 3.35e-004, 1.91e-004, 1.11e-004, 5.05e-005, 2.42e-005};
    const double discrSoboDim2[]   = {8.33e-004, 4.32e-004, 2.24e-004, 1.12e-004, 5.69e-005, 2.14e-005}; // , null};
    const double discrUnSoDim2[]   = {8.33e-004, 4.32e-004, 2.24e-004, 1.12e-004, 5.69e-005, 2.14e-005}; // , null};

    const double discrRandDim3[]   = {9.27e-003, 6.56e-003, 4.63e-003, 3.28e-003, 2.32e-003, 1.64e-003, 1.16e-003};
    const double discrMersDim3[]   = {7.02e-003, 4.94e-003, 4.82e-003, 4.91e-003, 3.33e-003, 2.80e-003, 2.62e-003};
    const double discrHaltDim3[]   = {1.63e-003, 9.62e-004, 4.83e-004, 2.67e-004, 1.41e-004, 7.64e-005, 3.93e-005};
    const double discrSoboDim3[]   = {1.21e-003, 6.37e-004, 3.40e-004, 1.75e-004, 9.21e-005, 4.79e-005, 2.56e-005};
    const double discrUnSoDim3[]   = {1.21e-003, 6.37e-004, 3.40e-004, 1.75e-004, 9.21e-005, 4.79e-005, 2.56e-005};

    const double discrRandDim5[] = {5.15e-003, 3.64e-003, 2.57e-003, 1.82e-003, 1.29e-003, 9.10e-004, 6.43e-004};
    const double discrMersDim5[] = {4.28e-003, 3.48e-003, 2.48e-003, 1.98e-003, 1.57e-003, 1.39e-003, 6.33e-004};
    const double discrHaltDim5[] = {1.93e-003, 1.23e-003, 6.89e-004, 4.22e-004, 2.13e-004, 1.25e-004, 7.17e-005};
    const double discrSoboDim5[] = {1.59e-003, 9.55e-004, 5.33e-004, 3.22e-004, 1.63e-004, 9.41e-005, 5.19e-005};
    const double discrUnSoDim5[] = {1.85e-003, 9.39e-004, 5.19e-004, 2.99e-004, 1.75e-004, 9.51e-005, 5.55e-005};

    const double discrRandDim10[] = {9.69e-004, 6.85e-004, 4.84e-004, 3.42e-004, 2.42e-004, 1.71e-004, 1.21e-004};
    const double discrMersDim10[] = {8.83e-004, 6.56e-004, 4.87e-004, 3.37e-004, 3.06e-004, 1.73e-004, 1.43e-004};
    const double discrHaltDim10[] = {1.23e-003, 6.89e-004, 4.03e-004, 2.83e-004, 1.61e-004, 1.08e-004, 6.69e-005};
    const double discrSoboDim10[] = {7.08e-004, 5.31e-004, 3.60e-004, 2.18e-004, 1.57e-004, 1.12e-004, 6.39e-005};
    const double discrUnSoDim10[] = {7.67e-004, 4.92e-004, 3.47e-004, 2.34e-004, 1.39e-004, 9.47e-005, 5.72e-005};

    const double discrRandDim15[] = {1.73e-004, 1.22e-004, 8.62e-005, 6.10e-005, 4.31e-005, 3.05e-005, 2.16e-005};
    const double discrMersDim15[] = {1.63e-004, 1.12e-004, 8.36e-005, 6.09e-005, 4.34e-005, 2.95e-005, 2.10e-005};
    const double discrHaltDim15[] = {5.75e-004, 3.12e-004, 1.70e-004, 9.89e-005, 5.33e-005, 3.45e-005, 2.11e-005};
    const double discrSoboDim15[] = {1.59e-004, 1.23e-004, 7.73e-005, 5.51e-005, 3.91e-005, 2.73e-005, 1.96e-005};
    const double discrUnSoDim15[] = {2.24e-004, 1.39e-004, 9.86e-005, 6.02e-005, 4.39e-005, 3.06e-005, 2.32e-005};

    const double discrRandDim30[] = {9.54e-007, 6.75e-007, 4.77e-007, 3.37e-007, 2.38e-007, 1.69e-007, 1.19e-007};
    const double discrMersDim30[] = {4.38e-007, 3.25e-007, 4.47e-007, 2.85e-007, 2.03e-007, 1.50e-007, 1.17e-007};
    const double discrHaltDim30[] = {4.45e-004, 2.23e-004, 1.11e-004, 5.56e-005, 2.78e-005, 1.39e-005, 6.95e-006};
    const double discrSoboDim30[] = {6.43e-007, 5.28e-007, 3.88e-007, 2.49e-007, 2.09e-007, 1.55e-007, 1.07e-007};
    const double discrUnSoDim30[] = {4.35e-005, 2.17e-005, 1.09e-005, 5.43e-006, 2.73e-006, 1.37e-006, 6.90e-007};

    const double discrRandDim50[] = {9.32e-010, 6.59e-010, 4.66e-010, 3.29e-010, 2.33e-010, 1.65e-010, 1.16e-010};
    const double discrMersDim50[] = {3.27e-010, 2.42e-010, 1.47e-010, 1.98e-010, 2.31e-010, 1.30e-010, 8.09e-011};
    const double discrHaltDim50[] = {4.04e-004, 2.02e-004, 1.01e-004, 5.05e-005, 2.52e-005, 1.26e-005, 6.31e-006};
    const double discrSoboDim50[] = {2.98e-010, 2.91e-010, 2.62e-010, 1.53e-010, 1.48e-010, 1.15e-010, 8.41e-011};
    const double discrUnSoDim50[] = {1.63e-005, 8.14e-006, 4.07e-006, 2.04e-006, 1.02e-006, 5.09e-007, 2.54e-007};

    const double discrRandDim100[] = {2.78e-017, 1.96e-017, 1.39e-017, 9.81e-018, 6.94e-018, 4.91e-018, 3.47e-018};
    const double discrMersDim100[] = {5.30e-019, 7.29e-019, 3.71e-019, 3.33e-019, 1.33e-017, 6.70e-018, 3.36e-018};
    const double discrHaltDim100[] = {3.63e-004, 1.81e-004, 9.07e-005, 4.53e-005, 2.27e-005, 1.13e-005, 5.66e-006};
    const double discrSoboDim100[] = {1.26e-018};
    const double discrUnSoDim100[] = {4.97e-006};

    const double * const discrRand[8] = { discrRandDim2,  discrRandDim3,
        discrRandDim5,  discrRandDim10, discrRandDim15, discrRandDim30,
        discrRandDim50, discrRandDim100 };
    const double * const discrMers[8] = { discrMersDim2,  discrMersDim3,
        discrMersDim5,  discrMersDim10, discrMersDim15, discrMersDim30,
        discrMersDim50, discrMersDim100 };
    const double * const discrHalt[8] = { discrHaltDim2,  discrHaltDim3,
        discrHaltDim5,  discrHaltDim10, discrHaltDim15, discrHaltDim30,
        discrHaltDim50, discrHaltDim100 };
    const double * const discrSobo[8] = { discrSoboDim2,  discrSoboDim3,
        discrSoboDim5,  discrSoboDim10, discrSoboDim15, discrSoboDim30,
        discrSoboDim50, discrSoboDim100 };
    const double * const discrUnSo[8] = { discrUnSoDim2,  discrUnSoDim3,
        discrUnSoDim5,  discrUnSoDim10, discrUnSoDim15, discrUnSoDim30,
        discrUnSoDim50, discrUnSoDim100 };

    static const unsigned long dimensionality[] = {2, 3, 5, 10, 15, 30, 50, 100 };

    Array point;
    unsigned long dim;
    unsigned long seed = 123456;
    double trueRandomFactor, discr, tolerance=1e-2;
    bool printOnly = false;

    std::ofstream outStream("discrepancy.txt");
    for (int i = 0; i<8; i++) {
        if (printOnly)
            outStream << std::endl;

        dim = dimensionality[i];
        DiscrepancyStatistics stat(dim);

        trueRandomFactor = (1.0/QL_POW(2.0, int(dim))
            -1.0/QL_POW(3.0, int(dim)));
        MersenneTwisterUniformRsg mer(dim, seed);
        HaltonRsg                 hal(dim);
        SobolRsg                  sob(dim, seed);
        SobolRsg                  unS(dim, seed, true);

        Size j, k, jMin = 10;
        // 7 loops would take too long for usual/frequent test running
        // Size sampleLoops = 7;
        Size sampleLoops = 1;

        // true random numbers
        stat.reset();
        if (printOnly)
            outStream << "const double discrRandDim" << dim << "[] = {" ;
        for (j=jMin; j<jMin+sampleLoops; j++) {
            Size points = Size(QL_POW(2.0, int(j)))-1;

            discr = QL_SQRT(trueRandomFactor/points);

            if (printOnly) {
                if (j!=jMin)
                    outStream << ", ";
                outStream << DoubleFormatter::toExponential(discr, 2);
            } else {
                if(QL_FABS(discr-discrRand[i][j-jMin])>tolerance*discr) {
                  CPPUNIT_FAIL("True random discrepancy dimension " +
                    IntegerFormatter::toString(dimensionality[i]) +
                    " at " +
                    IntegerFormatter::toString(points) +
                    " samples is " +
                    DoubleFormatter::toExponential(discr, 2) +
                    " instead of " +
                    DoubleFormatter::toExponential(discrRand[i][j-jMin], 2));
                }
            }
        }
        if (printOnly)
            outStream << "};" << std::endl;


        // Mersenne sequences
        k = 0;
        stat.reset();
        if (printOnly)
            outStream << "const double discrMersDim" << dim << "[] = {" ;
        for (j=jMin; j<jMin+sampleLoops; j++) {
            Size points = Size(QL_POW(2.0, int(j)))-1;
            for (; k<points; k++) {
                point = mer.nextSequence().value;
                stat.add(point);
            }

            discr = stat.discrepancy();

            if (printOnly) {
                if (j!=jMin)
                    outStream << ", ";
                outStream << DoubleFormatter::toExponential(discr, 2);
            } else {
                if (QL_FABS(discr-discrMers[i][j-jMin])>tolerance*discr) {
                  CPPUNIT_FAIL("Mersenne discrepancy dimension " +
                    IntegerFormatter::toString(dimensionality[i]) +
                    " at " +
                    IntegerFormatter::toString(points) +
                    " samples is " +
                    DoubleFormatter::toExponential(discr, 2) +
                    " instead of " +
                    DoubleFormatter::toExponential(discrMers[i][j-jMin], 2));
                }
            }
        }
        if (printOnly)
            outStream << "};" << std::endl;

        // Halton sequences
        k = 0;
        stat.reset();
        if (printOnly)
            outStream << "const double discrHaltDim" << dim << "[] = {" ;
        for (j=jMin; j<jMin+sampleLoops; j++) {
            Size points = Size(QL_POW(2.0, int(j)))-1;
            for (; k<points; k++) {
                point = hal.nextSequence().value;
                stat.add(point);
            }

            discr = stat.discrepancy();

            if (printOnly) {
                if (j!=jMin)
                    outStream << ", ";
                outStream << DoubleFormatter::toExponential(discr, 2);
            } else {
                if (QL_FABS(discr-discrHalt[i][j-jMin])>tolerance*discr) {
                  CPPUNIT_FAIL("Halton discrepancy dimension " +
                    IntegerFormatter::toString(dimensionality[i]) +
                    " at " +
                    IntegerFormatter::toString(points) +
                    " samples is " +
                    DoubleFormatter::toExponential(discr, 2) +
                    " instead of " +
                    DoubleFormatter::toExponential(discrHalt[i][j-jMin], 2));
                }
            }
        }
        if (printOnly)
            outStream << "};" << std::endl;

        // Sobol sequences
        k = 0;
        stat.reset();
        if (printOnly)
            outStream << "const double discrSoboDim" << dim << "[] = {" ;
        for (j=jMin; j<jMin+sampleLoops; j++) {
            Size points = Size(QL_POW(2.0, int(j)))-1;
            for (; k<points; k++) {
                point = sob.nextSequence().value;
                stat.add(point);
            }

            discr = stat.discrepancy();

            if (printOnly) {
                if (j!=jMin)
                    outStream << ", ";
                outStream << DoubleFormatter::toExponential(discr, 2);
            } else {
                if (QL_FABS(discr-discrSobo[i][j-jMin])>tolerance*discr) {
                  CPPUNIT_FAIL("Sobol discrepancy dimension " +
                    IntegerFormatter::toString(dimensionality[i]) +
                    " at " +
                    IntegerFormatter::toString(points) +
                    " samples is " +
                    DoubleFormatter::toExponential(discr, 2) +
                    " instead of " +
                    DoubleFormatter::toExponential(discrSobo[i][j-jMin], 2));
                }
            }
        }
        if (printOnly)
            outStream << "};" << std::endl;

        // Unit Sobol sequences
        k = 0;
        stat.reset();
        if (printOnly)
            outStream << "const double discrUnSoDim" << dim << "[] = {" ;
        for (j=jMin; j<jMin+sampleLoops; j++) {
            Size points = Size(QL_POW(2.0, int(j)))-1;
            for (; k<points; k++) {
                point = unS.nextSequence().value;
                stat.add(point);
            }

            discr = stat.discrepancy();

            if (printOnly) {
                if (j!=jMin)
                    outStream << ", ";
                outStream << DoubleFormatter::toExponential(discr, 2);
            } else {
                if (QL_FABS(discr-discrUnSo[i][j-jMin])>tolerance*discr) {
                  CPPUNIT_FAIL("Unit Sobol discrepancy dimension " +
                    IntegerFormatter::toString(dimensionality[i]) +
                    " at " +
                    IntegerFormatter::toString(points) +
                    " samples is " +
                    DoubleFormatter::toExponential(discr, 2) +
                    " instead of " +
                    DoubleFormatter::toExponential(discrUnSo[i][j-jMin], 2));
                }
            }
        }
        if (printOnly)
            outStream << "};" << std::endl;

    }
    outStream.close();

}

