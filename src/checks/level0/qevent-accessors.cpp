/*
    This file is part of the clazy static checker.

    Copyright (C) 2020 The Qt Company Ltd.
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
    m_replacements.insert({"QEnterEvent::pos", "position()"});
    m_replacements.insert({"QEnterEvent::globalPos", "globalPosition()"});
    m_replacements.insert({"QEnterEvent::x", "position().x()"});
    m_replacements.insert({"QEnterEvent::y", "position().y()"});
    m_replacements.insert({"QEnterEvent::globalX", "globalPosition().x()"});
    m_replacements.insert({"QEnterEvent::globalY", "globalPosition().y()"});

    m_replacements.insert({"QMouseEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QMouseEvent::globalPos", "globalPosition().toPoint()"});
    m_replacements.insert({"QMouseEvent::x", "position().toPoint().x()"});
    m_replacements.insert({"QMouseEvent::y", "position().toPoint().y()"});
    m_replacements.insert({"QMouseEvent::globalX", "globalPosition().toPoint().x()"});
    m_replacements.insert({"QMouseEvent::globalY", "globalPosition().toPoint().y()"});
    m_replacements.insert({"QMouseEvent::localPos", "position()"});
    m_replacements.insert({"QMouseEvent::windowPos", "scenePosition()"});
    m_replacements.insert({"QMouseEvent::screenPos", "globalPosition()"});

    m_replacements.insert({"QHoverEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QHoverEvent::posF", "position()"});

    m_replacements.insert({"QTabletEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QTabletEvent::globalPos", "globalPosition().toPoint()"});
    m_replacements.insert({"QTabletEvent::posF", "position()"});
    m_replacements.insert({"QTabletEvent::globalPosF", "globalPosition()"});
    m_replacements.insert({"QTabletEvent::x", "position().x()"});
    m_replacements.insert({"QTabletEvent::y", "position().y()"});
    m_replacements.insert({"QTabletEvent::globalX", "globalPosition().toPoint().x()"});
    m_replacements.insert({"QTabletEvent::globalY", "globalPosition().toPoint().y()"});
    m_replacements.insert({"QTabletEvent::hiResGlobalX", "globalPosition().x()"});
    m_replacements.insert({"QTabletEvent::hiResGlobalY", "globalPosition().y()"});

    m_replacements.insert({"QNativeGestureEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QNativeGestureEvent::globalPos", "globalPosition().toPoint()"});
    m_replacements.insert({"QNativeGestureEvent::localPos", "position()"});
    m_replacements.insert({"QNativeGestureEvent::windowPos", "scenePosition()"});
    m_replacements.insert({"QNativeGestureEvent::screenPos", "globalPosition()"});

    m_replacements.insert({"QDropEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QDropEvent::posF", "position()"});
    m_replacements.insert({"QDropEvent::mouseButtons", "buttons()"});
    m_replacements.insert({"QDropEvent::keyboardModifiers", "modifiers()"});

    m_replacements.insert({"QDragMoveEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QDragMoveEvent::posF", "position()"});
    m_replacements.insert({"QDragMoveEvent::mouseButtons", "buttons()"});
    m_replacements.insert({"QDragMoveEvent::keyboardModifiers", "modifiers()"});

    m_replacements.insert({"QDragEnterEvent::pos", "position().toPoint()"});
    m_replacements.insert({"QDragEnterEvent::posF", "position()"});
    m_replacements.insert({"QDragEnterEvent::mouseButtons", "buttons()"});
    m_replacements.insert({"QDragEnterEvent::keyboardModifiers", "modifiers()"});

    m_replacements.insert({"TouchPoint::pos", "position()"});
    m_replacements.insert({"TouchPoint::startPos", "pressPosition()"});
    m_replacements.insert({"TouchPoint::scenePos", "scenePosition()"});
    m_replacements.insert({"TouchPoint::startScenePos", "scenePressPosition()"});
    m_replacements.insert({"TouchPoint::screenPos", "globalPosition()"});
    m_replacements.insert({"TouchPoint::startScreenPos", "globalPressPosition()"});
}

void QEventAccessors::VisitStmt(clang::Stmt *stmt)
{
    auto *memberCall = dyn_cast<CXXMemberCallExpr>(stmt);
    if (!memberCall)
        return;

    auto search = m_replacements.find(clazy::qualifiedMethodName(memberCall));
    if (search != m_replacements.end()) {
        SourceLocation rangeStart = memberCall->getCallee()->getExprLoc();
        SourceLocation rangeEnd = clazy::getLocEnd(stmt);
        std::vector<FixItHint> fixits;
        if (fixitsEnabled())
            fixits.push_back(clazy::createReplacement({rangeStart, rangeEnd}, search->second));
        string errorMsg = "Replace the deprecated call " + search->first +
                "(): use " + search->second + " instead";
        emitWarning(rangeStart, errorMsg.c_str(), fixits);
    }
}
