# qevent-accessors

Finds and replaces calls to various obsolete accessors such as `QMouseEvent::pos()`,
`QTabletEvent::hiResGlobalY()` etc.  These have been deprecated and replaced
in Qt 6, because all QPointerEvents and QEventPoints (those that come from
pointing devices such as mouse or touchscreen) have a uniform set of accessor
functions.

