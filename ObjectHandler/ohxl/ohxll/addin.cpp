
/*
 Copyright (C) 2005 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <xlsdk/xlsdkdefines.hpp>
#include <ohxl/register.hpp>
#include <ohxl/export.hpp>

DLLEXPORT void xlAutoFree(XLOPER *px) {
    if (px->xltype & xltypeStr && px->val.str)
        delete [] px->val.str;
    else if (px->xltype & xltypeMulti && px->val.array.lparray) {
        int size = px->val.array.rows * px->val.array.columns;
        for (int i=0; i<size; i++)
            if (px->val.array.lparray[i].xltype & xltypeStr
            &&  px->val.array.lparray[i].val.str)
                delete [] px->val.array.lparray[i].val.str;
        delete [] px->val.array.lparray;
    }
}

DLLEXPORT int xlAutoOpen() {
    std::string xlErrorMessage;

    static XLOPER xDll;
    Excel(xlGetName, xlErrorMessage, &xDll, 0);

    ohRegisterFunctions(xDll);

    Excel(xlFree, xlErrorMessage, 0, 1, &xDll);
    return 1;
}

DLLEXPORT int xlAutoClose() {
    std::string xlErrorMessage;
    Excel(xlUDF, xlErrorMessage, 0, 1, TempStrNoSize("\x12""ohDeleteAllObjects"));
    return 1;
}

