/*
    This file is part of the clazy static checker.

    Copyright (C) 2020 Shawn Rutledge <shawn.rutledge@qt.io>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "qevent-accessors.h"
#include "FixItUtils.h"
#include "StringUtils.h"
#include "HierarchyUtils.h"
#include "SourceCompatibilityHelpers.h"

#include <clang/AST/DeclCXX.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/LLVM.h>
#include <llvm/Support/Casting.h>

class ClazyContext;

using namespace clang;
using namespace std;


QEventAccessors::QEventAccessors(const std::string &name, ClazyContext *context)
    : CheckBase(name, context, Option_CanIgnoreIncludes)
{
}

void QEventAccessors::VisitStmt(clang::Stmt *stmt)
{
    auto *memberCall = dyn_cast<CXXMemberCallExpr>(stmt);
    if (!memberCall)
        return;
    std::string methodName = clazy::qualifiedMethodName(memberCall);

    string errorMsg;
    std::string replacement;

    if (methodName == "QMouseEvent::pos" || methodName == "QTabletEvent::pos") {
        errorMsg = "Replace the deprecated call " + methodName;
        replacement = "position";
    }
    if (!errorMsg.empty()) {
        SourceLocation rangeStart = memberCall->getCallee()->getExprLoc();
        SourceLocation rangeEnd = Lexer::getLocForEndOfToken(rangeStart, -1, sm(), lo());
        std::vector<FixItHint> fixits;
        if (fixitsEnabled()) {
            replacement = "position";
            fixits.push_back(clazy::createReplacement({rangeStart, rangeEnd}, replacement));
        }

        errorMsg += ": use " + replacement + "() instead";
        emitWarning(rangeStart, errorMsg.c_str(), fixits);
    }
}
